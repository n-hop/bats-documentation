# Testbed Setup
## Setup for one-hop network testing

### 1.1. Topology

```text
n-hop router | (enp5s0: 10.0.0.3) --> (eth0: 10.0.0.11) n-hop slave1
             |_(enp4s0: 10.0.0.2) --> (eth0: 10.0.0.12) n-hop slave2
```

### 1.2. Configuration

On n-hop slave1

```bash
sudo ip link set eth0 down
sudo ip addr flush dev eth0
sudo ip addr add 10.0.0.11/24 dev eth0
sudo ip route add 10.0.0.12/32 via 10.0.0.3 dev eth0
sudo ip link set eth0 up
```

On n-hop slave2

```bash
sudo ip link set eth0 down
sudo ip addr flush dev eth0
sudo ip addr add 10.0.0.12/24 dev eth0
sudo ip route add 10.0.0.11/32 via 10.0.0.2 dev eth0
sudo ip link set eth0 up
```

On n-hop router

Prepare the configuration file `testbed/01-network-manager-all.yaml` for the router, copy it to `/etc/netplan/01-network-manager-all.yaml`,
then apply the configuration with the following command:

```bash
sudo netplan apply
```

Set the bandwidth of the link between n-hop router and n-hop slave1 to 100Mbps:

```bash
sudo tc qdisc add dev enp4s0 root tbf rate 100mbit burst 100kbit latency 1ms
sudo tc qdisc add dev enp5s0 root tbf rate 100mbit burst 100kbit latency 1ms
```

### 1.3. Test

#### 1.3.1. Test with ICMP

```bash
# on n-hop slave1
ping 10.0.0.3
ping 10.0.0.2
ping 10.0.0.12 -c 10

# on n-hop slave2
ping 10.0.0.2
ping 10.0.0.3
ping 10.0.0.11 -c 10
```

All the ping should be successful.

#### 1.3.2. Test with TCP

```bash
# on client side, use `CUBIC` to do the test
iperf -c 10.0.0.12 --linux-congestion=cubic -i1 -t10

# on client side, use `BBR` to do the test
iperf -c 10.0.0.12 --linux-congestion=bbr -i1 -t10

# on server side
iperf -s -i1

```
add link rate results to the table

| ID  | cc algorithm | loss rate | throughput | latency | reliability |
| --- | ------------ | --------- | ---------- | ------- | ----------- |
| 1   | TCP_cubic    | 0         |            |         | 100%        |
| 2   | TCP_cubic    | 5         |            |         | 100%        |
| 3   | TCP_cubic    | 10        |            |         | 100%        |
| 4   | TCP_cubic    | 15        |            |         | 100%        |
| 5   | TCP_cubic    | 20        |            |         | 100%        |
| 6   | TCP_BBR      | 0         |            |         | 100%        |
| 7   | TCP_BBR      | 5         |            |         | 100%        |
| 8   | TCP_BBR      | 10        |            |         | 100%        |
| 9   | TCP_BBR      | 15        |            |         | 100%        |
| 10  | TCP_BBR      | 20        |            |         | 100%        |

#### 1.3.32. Test with BATS protocol

If the bandwidth of the link between n-hop slave2 and n-hop slave1 is 100Mbps, the actual bandwidth of BATS protocol between n-hop slave2 and n-hop slave1 is 90Mbps, this is because the BATS protocol introduces 10% protocol overhead. For TCP, the overhead is 5%.

On n-hop slave1, perform the following command:

```bash
python3 -u ./bperf.py -c 10.0.0.12 -p 12345 -t 10 -i 1 -b 90
```

On n-hop slave2, perform the following command:

```bash
python3 -u ./bperf.py -s 10.0.0.12 -p 12345
```

`restricted feedback` means system has relatively high latency, and `unrestricted feedback` means system has relatively low latency. In unrestricted feedback mode, the receiver will use feedback to request retransmission of lost packets or file blocks.

**independent loss pattern**

Please check in readme document of `BATS protocol` to learn how to set code schema through `BATS socket` interface.

| ID  | code schema     | loss rate | throughput | link rate of each node | latency | reliability | feedback     |
| --- | --------------- | --------- | ---------- | ---------------------- | ------- | ----------- | ------------ |
| 1   | ordinary random | 0         |            |                        |         |             | restricted   |
| 2   | ordinary random | 5         |            |                        |         |             | restricted   |
| 3   | ordinary random | 10        |            |                        |         |             | restricted   |
| 4   | ordinary random | 15        |            |                        |         |             | restricted   |
| 5   | ordinary random | 20        |            |                        |         |             | restricted   |
| 6   | ordinary random | 0         |            |                        |         | 100%        | unrestricted |
| 7   | ordinary random | 5         |            |                        |         | 100%        | unrestricted |
| 8   | ordinary random | 10        |            |                        |         | 100%        | unrestricted |
| 9   | ordinary random | 15        |            |                        |         | 100%        | unrestricted |
| 10  | ordinary random | 20        |            |                        |         | 100%        | unrestricted |



| ID  | code schema | loss rate | throughput | link rate of each node | latency | reliability | feedback     |
| --- | ----------- | --------- | ---------- | ---------------------- | ------- | ----------- | ------------ |
| 1   | identity    | 0         |            |                        |         |             | restricted   |
| 2   | identity    | 5         |            |                        |         |             | restricted   |
| 3   | identity    | 10        |            |                        |         |             | restricted   |
| 4   | identity    | 15        |            |                        |         |             | restricted   |
| 5   | identity    | 20        |            |                        |         |             | restricted   |
| 6   | identity    | 0         |            |                        |         | 100%        | unrestricted |
| 2   | identity    | 5         |            |                        |         | 100%        | unrestricted |
| 3   | identity    | 10        |            |                        |         | 100%        | unrestricted |
| 4   | identity    | 15        |            |                        |         | 100%        | unrestricted |
| 5   | identity    | 20        |            |                        |         | 100%        | unrestricted |

**combination of independent loss pattern and robust loss pattern**

TODO: add the results
