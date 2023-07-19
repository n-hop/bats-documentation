# Testbed Result

## One-hop network testing

### 1. Topology

```text
n-hop router | (enp5s0: 10.0.0.3) --> (eth0: 10.0.0.11) n-hop slave1
             |_(enp4s0: 10.0.0.2) --> (eth0: 10.0.0.12) n-hop slave2
```

The objective of this experimental study is to investigate the impact of different loss rates and loss distributions on network traffic.

### 2. TCP-cubic

| ID  | cc algorithm | loss model | loss rate | throughput(Mbits/sec) | latency | reliability |
| --- | ------------ | ---------- | --------- | --------------------- | ------- | ----------- |
| 1   | TCP_cubic    | random     | 0         | 897                   | 1ms     | 100%        |
| 2   | TCP_cubic    | random     | 5         | 120                   | 1ms     | 100%        |
| 3   | TCP_cubic    | random     | 10        | 11.3                  | 1ms     | 100%        |
| 4   | TCP_cubic    | random     | 15        | 2.15                  | 1ms     | 100%        |
| 5   | TCP_cubic    | random     | 20        | 1.17                  | 1ms     | 100%        |
| 6   | TCP_BBR      | random     | 0         | 888                   | 1ms     | 100%        |
| 7   | TCP_BBR      | random     | 5         | 66.4                  | 1ms     | 100%        |
| 8   | TCP_BBR      | random     | 10        | 4.22                  | 1ms     | 100%        |
| 9   | TCP_BBR      | random     | 15        | 2.28                  | 1ms     | 100%        |
| 10  | TCP_BBR      | random     | 20        | 1.42                  | 1ms     | 100%        |

### 3. UDP

| ID  | loss model | loss rate | throughput(Mbits/sec) | Jitter  | latency | reliability |
| --- | ---------- | --------- | --------------------- | ------- | ------- | ----------- |
| 1   | random     | 0         | 1.05                  | 0.097ms | 1ms     | 100%        |
| 2   | random     | 5         | 1                     | 0.049ms | 1ms     | 100%        |
| 3   | random     | 10        | 0.937                 | 0.055ms | 1ms     | 100%        |
| 4   | random     | 15        | 0.88                  | 0.077ms | 1ms     | 100%        |
| 5   | random     | 20        | 0.851                 | 0.129ms | 1ms     | 100%        |

### 4. BATS

| ID  | Delay distribution | loss rate | throughput | link rate | latency |
| --- | ------------------ | --------- | ---------- | --------- | ------- |
| 1   | uniform            | 0         |            |           |         |
| 2   | uniform            | 5         |            |           |         |
| 3   | uniform            | 10        |            |           |         |
| 4   | uniform            | 15        |            |           |         |
| 5   | uniform            | 20        |            |           |         |
| 6   | normal             | 0         |            |           |         |
| 7   | normal             | 5         |            |           |         |
| 8   | normal             | 10        |            |           |         |
| 9   | normal             | 15        |            |           |         |
| 10  | normal             | 20        |            |           |         |
| 11  | pareto             | 0         |            |           |         |
| 12  | pareto             | 5         |            |           |         |
| 13  | pareto             | 10        |            |           |         |
| 14  | pareto             | 15        |            |           |         |
| 15  | pareto             | 20        |            |           |         |
| 16  | paretonormal       | 0         |            |           |         |
| 17  | paretonormal       | 5         |            |           |         |
| 18  | paretonormal       | 10        |            |           |         |
| 19  | paretonormal       | 15        |            |           |         |
| 20  | paretonormal       | 20        |            |           |         |

## Two-hop network testing

### 1.1. Topology

```text
n-hop router | (enp5s0: 10.0.0.1) --> (eth0: 10.0.0.2) n-hop slave1
             |_(enp4s0: 10.0.1.1) --> (eth0: 10.0.1.2) n-hop slave2
             |_(enp4s0: 10.0.2.1) --> (eth0: 10.0.2.2) n-hop slave3
```
