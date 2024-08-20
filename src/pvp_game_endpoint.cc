/**
 * @file pvp_game_client.cc
 * @author Lei Peng (peng.lei@n-hop.com)
 * @brief Simulate the client of the pvp game by sending the request to the
 * server at any given rate and any packet size.
 * @version 3.2.0
 * @date 2024-04-23
 *
 * Copyright (c) 2023 The n-hop technologies Limited. All Rights Reserved.
 *
 */
#include <arpa/inet.h>
#include <fcntl.h>
#include <gflags/gflags.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h> /* See NOTES */
#include <unistd.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <memory>
#include <thread>
#include <unordered_map>
#include <vector>

#include "bats_epoll.h"

/**
 * @brief Define the header of the message.
 *
 */
struct PvpGameMessageHeader {
  uint32_t seq;
  uint32_t size;
  uint64_t timestamp;
} __attribute__((packed));

constexpr int max_message_size = 10240 + sizeof(PvpGameMessageHeader);
DEFINE_string(c, "", "Connect to the server with the given IP address.");
DEFINE_int32(p, 0, "Listen on the given port or connect to the given port.");
DEFINE_double(i, 1,
              "The interval of sending the request to the server. if the value "
              "is 0.1, it means sending the request 10 "
              "times per second.");
DEFINE_int32(l, 1024, "The packet size of the request. Default is 1024 bytes.");
DEFINE_int32(w, 1000, "The max request message to be sent.");

const std::string usage_str =
    "Usage: \nClient mode: pvp_game_endpoint -c <server_ip> -p <port> -i "
    "<interval> -l <packet_size>\nServer mode: "
    "pvp_game_endpoint -p <port>\n";

static inline uint64_t get_time_in_ns() {
  return static_cast<uint64_t>(
      std::chrono::high_resolution_clock::now().time_since_epoch().count());
}
static inline uint64_t get_time_in_ms() {
  return static_cast<uint64_t>(get_time_in_ns() / 1000000L);
}

/**
 * @brief Start a TCP connection to the server and send the message to the
 * server at the given rate.
 *
 * @param ip The IP address of the server.
 * @param port The port number of the server.
 * @param interval The message sending Rate. 0.1 means sending 10 messages per
 * second.
 * @param packet_size The length of each message.
 * @param max_request The max number of messages to be sent.
 * @return true
 * @return false
 */
bool StartTCPClient(const std::string& ip, int port, double interval,
                    int packet_size, int max_request);

/**
 * @brief Start a TCP server on port `port` to echo the message back to the
 * client.
 *
 * @param port The listening port of the server.
 * @return true
 * @return false
 */
bool StartSimpleEchoServer(int port);

/**
 * @brief The main function of the pvp_game_endpoint.
 *
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return int
 */
int main(int argc, char* argv[]) {
  gflags::SetUsageMessage(usage_str);
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  if (FLAGS_c.empty() && FLAGS_p == 0) {
    std::cout << gflags::ProgramInvocationShortName() << " "
              << gflags::GetArgv0() << " --help" << std::endl;
    return 0;
  }

  // ignore the SIGPIPE signal
  signal(SIGPIPE, SIG_IGN);
  bool is_server_mode = FLAGS_c.empty();
  if (is_server_mode) {
    StartSimpleEchoServer(FLAGS_p);
    return 0;
  }

  if (FLAGS_i < 0.001 || FLAGS_i > 1 || FLAGS_i == 0) {
    std::cerr << "Invalid interval value. The valid range is [0.001, 1]."
              << std::endl;
    return 1;
  }
  if (FLAGS_l <= static_cast<int>(sizeof(PvpGameMessageHeader)) ||
      FLAGS_l > max_message_size) {
    std::cerr << "Invalid packet size value " << FLAGS_l << std::endl;
    std::cerr << "The valid range is [" << sizeof(PvpGameMessageHeader) << ", "
              << max_message_size << "]" << std::endl;
    return 1;
  }
  if (FLAGS_p <= 1024 || FLAGS_p > 65535) {
    std::cerr << "Invalid port number." << std::endl;
    std::cerr << "The valid range is [1024, 65535]." << std::endl;
    return 1;
  }
  // dump input parameters
  std::cout << "Client uses the following parameters:\n"
            << "Server IP: " << FLAGS_c << "\n"
            << "Server Port: " << FLAGS_p << "\n"
            << "Interval: " << FLAGS_i << "\n"
            << "Packet Size: " << FLAGS_l << "\n"
            << "Max Request: " << FLAGS_w << std::endl;
  StartTCPClient(FLAGS_c, FLAGS_p, FLAGS_i, FLAGS_l, FLAGS_w);
  return 0;
}

bool StartSimpleEchoServer(int port) {
  EpollInstance epoll_instance;
  epoll_instance.Init();
  // create a TCP socket
  int listen_fd = 0;
  if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    std::cerr << "Failed to create a socket." << std::endl;
    return false;
  }
  // bind the socket to the port
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(port);
  if (bind(listen_fd, reinterpret_cast<struct sockaddr*>(&serv_addr),
           sizeof(serv_addr)) < 0) {
    std::cerr << "Failed to bind the socket to the port." << std::endl;
    return false;
  }

  // listen on the port
  if (listen(listen_fd, 5) < 0) {
    std::cerr << "Failed to listen on the port." << std::endl;
    return false;
  }

  if (!epoll_instance.Register(listen_fd)) {
    std::cerr << "Failed to register the listener to the epoll instance."
              << std::endl;
    return false;
  }

  std::unordered_map<int, std::string> client_map;
  std::vector<std::thread> message_echo_threads;
  // listen and receive thread
  while (true) {
    const auto& [num, ready] = epoll_instance.Select(10);
    for (int i = 0; i < num; i++) {
      auto fd = ready[i].data.fd;
      if (fd == listen_fd) {
        struct sockaddr_in cli_addr;
        socklen_t cli_len = sizeof(cli_addr);
        int new_fd = accept(
            listen_fd, reinterpret_cast<struct sockaddr*>(&cli_addr), &cli_len);
        if (new_fd < 0) {
          std::cerr << "Failed to accept the new connection." << std::endl;
          continue;
        }
        std::string ip = inet_ntoa(cli_addr.sin_addr);
        ip += ":";
        ip += std::to_string(cli_addr.sin_port);
        std::cout << "Accepted a new connection from " << ip << std::endl;
        client_map[new_fd] = ip;
        message_echo_threads.emplace_back([&new_fd, &client_map]() {
          char buf[2000];
          while (true) {
            int n = read(new_fd, buf, 2000);
            if (n == 0) {
              break;
            }
            if (n > 0) {
              n = write(new_fd, buf, n);
              if (n == 0) {
                break;
              }
            }
          }
          std::cout << "Connection closed." << client_map[new_fd] << std::endl;
        });
      }
    }
  }

  for (auto& t : message_echo_threads) {
    t.join();
  }
  return true;
}

bool StartTCPClient(const std::string& ip, int port, double interval,
                    int packet_size, int max_request) {
  bool is_active = true;
  // create a TCP socket
  int client_fd = 0;
  if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    std::cerr << "Failed to create a socket." << std::endl;
    return false;
  }
  // connect to the server
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0) {
    std::cerr << "Invalid IP address." << std::endl;
    return false;
  }
  int max_retry = 3;
  while (max_retry-- > 0) {
    if (connect(client_fd, reinterpret_cast<struct sockaddr*>(&serv_addr),
                sizeof(serv_addr)) < 0) {
      std::cerr << "Failed to connect to the server. Retry in 1 second."
                << std::endl;
      std::this_thread::sleep_for(std::chrono::seconds(1));
      is_active = false;
    } else {
      std::cout << "Connected to the server at " << ip << ":" << port
                << std::endl;
      is_active = true;
      break;
    }
  }

  if (!is_active) {
    // sending_thread will finish first.
    std::cerr << "Failed to connect to the server after 3 retries."
              << std::endl;
    shutdown(client_fd, SHUT_RDWR);
    close(client_fd);
    return true;
  }

  std::atomic<bool> is_ready = false;
  // sending thread
  std::thread sending_thread([&max_request, &is_ready, packet_size, interval,
                              client_fd]() {
    // thread of sending the request to the server
    while (!is_ready.load()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    uint32_t seq = 1;
    while (max_request > 0) {
      std::vector<char> buf;
      buf.resize(packet_size);
      std::this_thread::sleep_for(
          std::chrono::milliseconds(static_cast<int>(interval * 1000)));
      auto message_header = reinterpret_cast<PvpGameMessageHeader*>(buf.data());
      message_header->seq = htonl(seq++);
      message_header->size = htonl(packet_size - sizeof(PvpGameMessageHeader));
      // no need to do bytes order conversion for timestamp since it will be
      // processed by the sender.
      message_header->timestamp = get_time_in_ms();
      int n = write(client_fd, buf.data(), buf.size());
      if (n == 0) {
        std::cerr << "Failed to send the request to the server." << std::endl;
        break;
      }
      max_request--;
      // std::cout << "Sent a request to the server. seq = " <<
      // ntohl(message_header->seq) << " max_request "
      //          << max_request << std::endl;
    }
    std::cout << "Sending thread is done." << std::endl;
  });
  sending_thread.detach();
  std::cout << "Start to receive the response from the server." << std::endl;

  int buf_offset = 0;
  std::vector<char> buf;
  buf.resize(packet_size);

  /// @notes: using the idea from
  /// https://github.com/libinzhangyuan/udt_patch_for_epoll/blob/master/client_epoll/main.cpp
  std::vector<int> recv_package_interval;
  std::vector<int> recv_package_interval10;
  std::vector<int> recv_package_jitter10;
  uint64_t static_last_refresh_time = 0;
  double static_received_bytes = 0;
  uint64_t total_rtt = 0;
  uint64_t recv_cnt = 0;
  int static_last_rtt = 0;
  auto max_response = max_request;

  is_ready.store(true);
  auto now = std::chrono::system_clock::now();
  auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
  auto timestamp = seconds.time_since_epoch().count();
  std::cout << "Start timestamp: " << timestamp << "\n";
  while (max_response > 0) {
    int n = read(client_fd, buf.data() + buf_offset, packet_size - buf_offset);
    if (n == 0) {
      std::cerr << "Remote shutdown the connection." << std::endl;
      break;
    }
    if (n < 0) {
      // convert errno to string
      std::string err_msg = strerror(errno);
      std::cerr << "Failed to read the response from the server. " << (err_msg)
                << std::endl;
      continue;
    }
    if (n < packet_size - buf_offset) {
      buf_offset += n;
      continue;
    }
    // receive done.
    buf_offset = 0;
    static_received_bytes += packet_size;
    uint64_t cur_time = get_time_in_ms();
    auto response_header = reinterpret_cast<PvpGameMessageHeader*>(buf.data());
    auto seq = ntohl(response_header->seq);
    auto new_rtt = cur_time - (response_header->timestamp);
    if (static_last_refresh_time == 0) {
      static_last_refresh_time = (response_header->timestamp);
    }
    // calculate the jitter
    if (recv_cnt > 0) {
      auto last_rtt = recv_package_interval10.empty()
                          ? static_last_rtt
                          : recv_package_interval10.back();
      int cur_rtt_jitter = std::abs(static_cast<int>(new_rtt - last_rtt));
      recv_package_jitter10.push_back(cur_rtt_jitter);
    }
    if (recv_cnt == 0) {
      std::cout << "First response from the server, rtt = " << new_rtt << "\n";
    }
    recv_package_interval.push_back(new_rtt);
    recv_package_interval10.push_back(new_rtt);
    recv_cnt++;

    if (seq % 10 == 0 && seq > 0) {
      int average10 = 0;
      for (int x : recv_package_interval10) {
        average10 += x;
      }
      average10 = (average10 / recv_package_interval10.size());

      // calculate the jitter for the last 10 packets
      int jitter10 = 0;
      for (int x : recv_package_jitter10) {
        jitter10 += x;
      }
      jitter10 = (jitter10 / recv_package_jitter10.size());

      for (int x : recv_package_interval) {
        total_rtt += x;
      }
      std::cout << "max: "
                << *std::max_element(recv_package_interval10.begin(),
                                     recv_package_interval10.end())
                << "  min: "
                << *std::min_element(recv_package_interval10.begin(),
                                     recv_package_interval10.end())
                << "  average 10: " << average10 << " jitter10: " << jitter10
                << "  average total: " << total_rtt / recv_cnt << " ";
      if (cur_time - static_last_refresh_time > 0) {
        // calculate the throughput
        static_received_bytes = static_received_bytes / 1024;
        double second =
            static_cast<double>(cur_time - static_last_refresh_time) / 1000;
        int throughput = static_received_bytes / second;
        // std::cout << " " << throughput << " KB/s " << static_received_bytes
        // << " KB " << second << " s \n";
        std::cout << " " << throughput << " KB/s\n";
      } else {
        std::cout << "\n";
      }
      static_last_rtt = recv_package_interval10.back();
      std::cout.flush();
      recv_package_interval10.clear();
      recv_package_interval.clear();
      recv_package_jitter10.clear();
      static_last_refresh_time = cur_time;
      static_received_bytes = 0;
    }
    // std::cout << "Received a response from the server. seq = " << seq << "
    // new_rtt = " << new_rtt << "\n";
    max_response--;
  }
  // sending_thread will finish first.
  std::cout << "Stop to receive the response from the server."
            << "\n";
  {
    auto now = std::chrono::system_clock::now();
    auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
    auto timestamp = seconds.time_since_epoch().count();
    std::cout << "Receive thread is done."
              << "\n";
    std::cout << "End timestamp: " << timestamp << "\n";
  }
  shutdown(client_fd, SHUT_RDWR);
  close(client_fd);
  return true;
}
