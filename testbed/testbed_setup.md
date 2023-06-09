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

### 1.3. Test

#### 1.3.1. Test with ICMP

```bash
# on n-hop slave1
ping 10.0.0.3
ping 10.0.0.2
ping 10.0.0.12

# on n-hop slave2
ping 10.0.0.2
ping 10.0.0.3
ping 10.0.0.11
```

All the ping should be successful.

#### 1.3.2. Test with TCP

```bash
```

#### 1.3.32. Test with BATS protocol

```bash
```