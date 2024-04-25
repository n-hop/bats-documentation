/**
 * @file bats_epoll.h
 * @author Lei Peng (peng.lei@n-hop.com)
 * @brief
 * @version 3.2.0
 * @date 2023-12-15
 *
 * Copyright (c) 2023 The n-hop technologies Limited. All Rights Reserved.
 *
 */

#ifndef SRC_BATS_EPOLL_H_
#define SRC_BATS_EPOLL_H_

#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#include <array>
#include <iostream>
#include <list>
#include <tuple>

/**
 * @brief Each session hub has an epoll instance to monitor the state of the registered fd.
 *
 */
class EpollInstance {
 public:
  constexpr static int max_events = 1024;
  EpollInstance() = default;
  virtual ~EpollInstance() = default;
  // Delete copy constructor and assignment operator to prevent copying
  EpollInstance(const EpollInstance &) = delete;
  EpollInstance &operator=(const EpollInstance &) = delete;
  EpollInstance(EpollInstance &&) = delete;
  EpollInstance &operator=(EpollInstance &&) = delete;

  /// @brief Register the fd to the epoll instance. default events is EPOLLIN.
  /// @param fd
  /// @return
  bool Register(int fd, uint32_t events = EPOLLIN) {
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev) == -1) {
      std::cerr << "epoll_ctl: add error" << strerror(errno) << std::endl;
      return false;
    }
    std::cerr << "register event for " << fd << std::endl;
    return true;
  }
  /// @brief Unregister the fd from the epoll instance.
  /// @param fd
  /// @return
  bool Unregister(int fd) {
    struct epoll_event ev;
    ev.data.fd = fd;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, &ev) == -1) {
      std::cerr << "epoll_ctl: del error" << strerror(errno) << std::endl;
      return false;
    }
    std::cerr << "unregister event for " << fd << std::endl;
    return true;
  }
  /**
   * @brief Check the state of all the registered fd and return the ready fd list.
   *
   * @param timeout_ms The timeout for the select operation in milliseconds.
   * @return std::list<int>
   */
  std::tuple<int &, const std::array<struct epoll_event, max_events> &> Select(int timeout_ms) {
    int nfds = epoll_wait(epoll_fd_, events_, max_events, timeout_ms);
    if (nfds == -1) {
      std::cerr << "epoll_wait " << strerror(errno) << std::endl;
    }
    for (int n = 0; n < nfds; ++n) {
      ready_fd_[n] = events_[n];
    }
    ready_fd_num_ = nfds;
    return {ready_fd_num_, ready_fd_};
  }
  bool Init() {
    if (epoll_fd_ > 0) {
      return true;
    }
    // Initialize epoll instance
    epoll_fd_ = epoll_create(max_events);
    if (epoll_fd_ == -1) {
      std::cerr << "Initialize epoll instance error " << strerror(errno) << std::endl;
      return false;
    }
    std::cerr << "Initialize epoll instance success on fd " << epoll_fd_ << std::endl;
    return true;
  }
  bool Stop() {
    if (epoll_fd_ > 0) {
      close(epoll_fd_);
      epoll_fd_ = 0;
    }
    return true;
  }

 private:
  int epoll_fd_ = 0;
  struct epoll_event events_[max_events];
  std::array<struct epoll_event, max_events> ready_fd_;
  int ready_fd_num_ = 0;
};

#endif  // SRC_BATS_EPOLL_H_
