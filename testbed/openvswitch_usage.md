# Openvswitch 配置

## 硬件

工控机

- Ubuntu 22.04 LTS

树莓派 4b+ \*3

- Debian GNU/Linux 11 (bullseye)

将三个树莓派（A,B 和 C）直接连接通过网线至工控机三个网口, 不妨认为它们分别连接上工控机对应网卡 eth0, eth1 和 eth2.

## 基础环境配置

目标网络结构如下：

```text
          A=(10.0.0.11) -- (10.0.0.12)=B=(10.0.1.1) -- (10.0.1.12)=C
```

树莓派 B 只有一张物理网卡，实际配置中需添加一个虚拟网卡。

### 安装 ovs 并创建网桥

1. 在工控机上安装 openvswitch

在 Ubuntu 上可以通过源安装

```
 apt install openvswitch-switch
```

2. 创建一个新的网桥 br0，并将 eth0 eth1 eth2 添加到 ovs-br

```bash
sudo ovs-vsctl add-br br0
sudo ovs-vsctl add-port br0 eth0
sudo ovs-vsctl add-port br0 eth1
sudo ovs-vsctl add-port br0 eth2
```

将本机的网卡加入到网桥之中后机器的 ip 地址失效了，不能 ssh，不能 ping 通。这是因为当网卡加入网桥之后，网卡就是交换机上的一个端口，交换机作为二层设备，其端口是不可能有 IP 地址的，所以网卡的 IP 地址失效。

此时，我们也不需要在 network layer 操作这 3 张网卡，因此可以清除这三张网卡的 ip 相关信息和相关的路由信息，避免有额外影响。

### 配置 3 个树莓派的 ip 地址及路由策略

为树莓派 A、C 配置静态 ip 地址,并将树莓派 B 设定为网关。

```bash
vim /etc/dhcpcd.conf
```

在该配置文件中添加静态 ip 信息

```
interface eth0
static ip_address=10.0.0.11/24
static routers=10.0.0.12/24
```

对于树莓派 B 来说，需要对网卡配置两个 ip 地址, 同时将 A、C 设定为网关。

- 配置静态 ip

```bash
vim /etc/network/interfaces
```

```
auto eth0
iface eth0 inet static
address 10.0.0.12
netmask 255.255.255.0

auto eth0:0
iface eth0 inet static
address 10.0.1.12
netmask 255.255.255.0

```

- 添加路由

```bash
route add default gw 10.0.0.11
route add default gw 10.0.1.11
```

网络配置完成。

### 路由配置结果

```bash
ip route
```

A:

```
root@raspberrypi:/home/nhop/bats-protocol-framework# ip route
default via 10.0.0.12 dev eth0 src 10.0.0.11 metric 202
default via 10.53.1.1 dev wlan0 proto dhcp src 10.53.1.189 metric 303
10.0.0.0/24 dev eth0 proto dhcp scope link src 10.0.0.11 metric 202
10.53.1.0/24 dev wlan0 proto dhcp scope link src 10.53.1.189 metric 303
```

B:

```
root@raspberrypi:/home/pi# ip route
default via 10.0.1.11 dev eth0
default via 10.0.0.11 dev eth0
default via 10.53.1.1 dev wlan0 proto dhcp src 10.53.1.186 metric 303
10.0.0.0/24 dev eth0 proto kernel scope link src 10.0.0.12
10.0.1.0/24 dev eth0 proto kernel scope link src 10.0.1.12
10.53.1.0/24 dev wlan0 proto dhcp scope link src 10.53.1.186 metric 303
```

C:

```
root@raspberrypi:/home/nhop/bats-protocol-framework# ip route
default via 10.0.1.12 dev eth0 src 10.0.1.11 metric 202
default via 10.53.1.1 dev wlan0 proto dhcp src 10.53.1.39 metric 303
10.0.1.0/24 dev eth0 proto dhcp scope link src 10.0.1.11 metric 202
10.53.1.0/24 dev wlan0 proto dhcp scope link src 10.53.1.39 metric 303
```

#### 一跳连通性测试

A(10.0.0.11) ping B(10.0.0.12) 或 C(10.0.1.11) ping B(10.0.0.12)

```bash
ping 10.0.0.12
```

能 ping 通

```
root@raspberrypi:~# ping 10.0.0.12
PING 10.0.0.12 (10.0.0.12) 56(84) bytes of data.
64 bytes from 10.0.0.12: icmp_seq=1 ttl=64 time=1.59 ms
64 bytes from 10.0.0.12: icmp_seq=2 ttl=64 time=0.879 ms
^C
--- 10.0.0.12 ping statistics ---
2 packets transmitted, 2 received, 0% packet loss, time 1002ms
rtt min/avg/max/mdev = 0.879/1.232/1.586/0.353 ms

```

查看跳数

```bash
traceroute 10.0.0.12
```

```
root@raspberrypi:~# traceroute 10.0.0.12
traceroute to 10.0.0.12 (10.0.0.12), 30 hops max, 60 byte packets
 1  10.0.0.12 (10.0.0.12)  1.866 ms  1.655 ms  1.590 ms

```

#### 两跳连通性测试

A(10.0.0.11) ping C(10.0.1.11)

```bash
ping 10.0.1.11
```

```
root@raspberrypi:/home/nhop# ping 10.0.1.11
PING 10.0.1.11 (10.0.1.11) 56(84) bytes of data.
From 10.0.0.12 icmp_seq=1 Redirect Host(New nexthop: 10.0.1.11)
64 bytes from 10.0.1.11: icmp_seq=1 ttl=63 time=2.27 ms
From 10.0.0.12 icmp_seq=2 Redirect Host(New nexthop: 10.0.1.11)
64 bytes from 10.0.1.11: icmp_seq=2 ttl=63 time=1.17 ms
^C
--- 10.0.1.11 ping statistics ---
2 packets transmitted, 2 received, +2 errors, 0% packet loss, time 1002ms
rtt min/avg/max/mdev = 1.173/1.722/2.272/0.549 ms
root@raspberrypi:/home/nhop#
```

查看跳数

```bash
traceroute 10.0.1.11
```

```
root@raspberrypi:/home/nhop# traceroute 10.0.1.11
traceroute to 10.0.1.11 (10.0.1.11), 30 hops max, 60 byte packets
 1  10.0.0.12 (10.0.0.12)  0.825 ms  0.656 ms  0.581 ms
 2  10.0.1.11 (10.0.1.11)  0.809 ms  0.687 ms  0.743 ms
root@raspberrypi:/home/nhop#
```

# BATS 通信测试

## 编译 BATS Protocol

略

## 使用 IP Routing 的方式进行两跳通信

编译完成后，首先运行 bats protocol ip-routing 通信的 daemon 进程。三台树莓派都要运行。

在项目根目录下：

```bash
./buildrasp/src/bats-protocol/test/end_to_end/ip_routing_protocol
```

本次测试从 A 通过 bats 通信 C。

在 A,另外打开一个终端, 运行 source App, 不需等待输出：

```bash
root@raspberrypi:/home/nhop/bats-protocol-framework# ./buildrspi/src/bats-protocol/test/end_to_end/source_app 10.0.1.11
```

这之后，在 C，另外打开一个终端，运行 destination App：

```bash
root@raspberrypi:/home/nhop/bats-protocol-framework# ./buildrspi/src/bats-protocol/test/end_to_end/destination_app 10.0.1.11
```

等待一段时间后，两边获得输出如下：

```bash
root@raspberrypi:/home/nhop/bats-protocol-framework# ./buildrspi/src/bats-protocol/test/end_to_end/source_app 10.0.1.11
WARNING: Logging before InitGoogleLogging() is written to STDERR
I20230711 07:17:14.436903 62198 batspro_uds_bats_chunk_socket.h:88] BATS Protocol process: 58871 is running, file: bats_uds_socket_bind_point.58871
I20230711 07:17:14.437103 62198 batspro_uds_bats_chunk_socket.h:94] uds_socket_bind_path: /tmp/bats_uds_socket_bind_point.58871
[source] package size: 4096
[source] Send Throughputs: 711 Mbps
```

```bash
root@raspberrypi:/home/nhop/bats-protocol-framework# ./buildrspi/src/bats-protocol/test/end_to_end/destination_app 10.0.1.11
WARNING: Logging before InitGoogleLogging() is written to STDERR
I20230711 02:17:16.339803 55933 batspro_uds_bats_chunk_socket.h:88] BATS Protocol process: 52686 is running, file: bats_uds_socket_bind_point.52686
I20230711 02:17:16.340078 55933 batspro_uds_bats_chunk_socket.h:94] uds_socket_bind_path: /tmp/bats_uds_socket_bind_point.52686
[Destination] last package size: 4096 address: 10.0.0.11 port: 1025
[Destination] Throughputs: 450 Mbps
```

# 其他命令：

```bash
# openvswitch配置展示
sudo ovs-vsctl show
# openvswitch流量方向展示
sudo ovs-ofctl dump-flows ovs-br
# 删除已配置的ovs-br
sudo ovs-vsctl del-br ovs-br
# 网桥信息展示
sudo brctl show
```
