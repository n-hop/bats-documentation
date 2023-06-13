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

```

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

| ID  | code schema | loss rate | throughput | latency | reliability | feedback     |
| --- | ----------- | --------- | ---------- | ------- | ----------- | ------------ |
| 1   | systematic  | 0         |            |         |             | restricted   |
| 2   | systematic  | 5         |            |         |             | restricted   |
| 3   | systematic  | 10        |            |         |             | restricted   |
| 4   | systematic  | 15        |            |         |             | restricted   |
| 5   | systematic  | 20        |            |         |             | restricted   |
| 6   | systematic  | 0         |            |         | 100%        | unrestricted |
| 7   | systematic  | 5         |            |         | 100%        | unrestricted |
| 8   | systematic  | 10        |            |         | 100%        | unrestricted |
| 9   | systematic  | 15        |            |         | 100%        | unrestricted |
| 10  | systematic  | 20        |            |         | 100%        | unrestricted |


| ID  | code schema   | loss rate | throughput | latency | reliability | feedback     |
| --- | ------------- | --------- | ---------- | ------- | ----------- | ------------ |
| 1   | random binary | 0         |            |         |             | restricted   |
| 2   | random binary | 5         |            |         |             | restricted   |
| 3   | random binary | 10        |            |         |             | restricted   |
| 4   | random binary | 15        |            |         |             | restricted   |
| 5   | random binary | 20        |            |         |             | restricted   |
| 6   | random binary | 0         |            |         | 100%        | unrestricted |
| 2   | random binary | 5         |            |         | 100%        | unrestricted |
| 3   | random binary | 10        |            |         | 100%        | unrestricted |
| 4   | random binary | 15        |            |         | 100%        | unrestricted |
| 5   | random binary | 20        |            |         | 100%        | unrestricted |


**combination of independent loss pattern and robust loss pattern**

| ID  | code schema | loss rate | throughput | latency | reliability | feedback     |
| --- | ----------- | --------- | ---------- | ------- | ----------- | ------------ |
| 1   | systematic  | 0         |            |         |             | restricted   |
| 2   | systematic  | 5         |            |         |             | restricted   |
| 3   | systematic  | 10        |            |         |             | restricted   |
| 4   | systematic  | 15        |            |         |             | restricted   |
| 5   | systematic  | 20        |            |         |             | restricted   |
| 6   | systematic  | 0         |            |         | 100%        | unrestricted |
| 7   | systematic  | 5         |            |         | 100%        | unrestricted |
| 8   | systematic  | 10        |            |         | 100%        | unrestricted |
| 9   | systematic  | 15        |            |         | 100%        | unrestricted |
| 10  | systematic  | 20        |            |         | 100%        | unrestricted |