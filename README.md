# BATS Protocol Performance Test

BATS Protocol is a network communication protocol based on [BATS codes](https://shhyang.github.io/research/bats/), designed to provide high throughput and low latency data transmission in lossy networks. In this project, we evaluate and compare the performance of BATS Protocol with other techniques/protocols in terms of various performance metrics such as throughput, latency and reliability.

To see performance test results directly, please jump to the section [BTP performance](#921-multi-hop-network-testing-with-btp), [BRTP performance](#922-multi-hop-network-testing-with-brtp).

For further contact, please visit [n-hop technologies Limited](https://www.n-hop.com/). Or email to **peng.lei@n-hop.com**.

## Objective

- To demonstrate the benefits/gain of BATS Protocol in different network scenarios.

## Contents

- [BATS Protocol Performance Test](#bats-protocol-performance-test)
  - [Objective](#objective)
  - [Contents](#contents)
  - [1.Introduction](#1introduction)
  - [2.Network Topologies](#2network-topologies)
  - [3.Network Tools](#3network-tools)
  - [4.Networking Parameters](#4networking-parameters)
  - [5.Packet Loss Pattern](#5packet-loss-pattern)
  - [6.Protocol Metrics](#6protocol-metrics)
  - [7.Protocols to Compare with](#7protocols-to-compare-with)
  - [8.BATS Protocol](#8bats-protocol)
    - [8.1 BATS Transmission Protocol (BTP)](#81-bats-transmission-protocol-btp)
    - [8.2 BATS Reliable Transmission Protocol (BRTP)](#82-bats-reliable-transmission-protocol-brtp)
    - [8.3 Acceleration for TCP over BATS protocol](#83-acceleration-for-tcp-over-bats-protocol)
  - [9. Testing Cases](#9-testing-cases)
    - [9.1 Overall Setup](#91-overall-setup)
    - [9.1 One-hop Network Testing](#91-one-hop-network-testing)
    - [9.2 Multi-hop Network Testing](#92-multi-hop-network-testing)
      - [9.2.1 Multi-hop Network Testing with BTP, BRTP](#921-multi-hop-network-testing-with-btp-brtp)

## 1.Introduction

The BATS Protocol performance evaluation focuses on the capability of the communication logic, such as coding (including feedback), congestion control, routing, etc. Therefore, in the testbed, we try to reduce the effect of the implementation by limiting the network link bandwidth. The performance compared with other techniques is relative, and the gain should be scalable with the network bandwidth.

This is NOT a software performance test. In other words, we do not test how fast the BATS Protocol software can run a device. Tough the testbed considers various practical scenarios, we do not test performance for specific applications, like video streaming.  

## 2.Network Topologies

Three kinds of network topologies will be mainly used in the tests:

- one-hop network
- multi-hop networks
- multi-path networks (in progress)

## 3.Network Tools

In this section, we will introduce the network tools used in the test framework.

- **Iperf3** is an open-source tool used for measuring network performance. It provides a straightforward way to assess the bandwidth, latency, and other parameters of a network link. iPerf operates in client-server mode, where one system acts as the server and another as the client. The client sends a controlled amount of data to the server, and the tool calculates metrics like throughput, packet loss, and jitter. iPerf is commonly used to measure TCP and UDP performance on networks.

- **Bperf** is a specialized network performance testing tool developed by n-hop technologies Limited. It is designed to measure and assess the performance of the BATS protocol.

- **Linux Traffic Control** is a powerful framework built into the Linux kernel that offers advanced traffic management and Quality of Service (QoS) capabilities. It allows administrators to shape and control network traffic by configuring various parameters like bandwidth, latency, packet scheduling, and prioritization. With tc, you can implement policies that prioritize certain types of traffic, limit bandwidth for specific applications, and manage congestion to ensure fair allocation of network resources.

- **Customized application** is used to measure the end-to-end latency by sending messages of any fixed length at a given rate, and the source code is under the directory `src/app`. 

## 4.Networking Parameters

- **Link bandwidth**:
  - The bandwidth of the link is the maximum rate at which data can be transmitted over the link. The bandwidth of a link is a critical parameter that determines the maximum data rate that can be achieved between two nodes. The bandwidth of a link is determined by the physical characteristics of the link, such as the cable type, signal strength, and interference.
- **Link latency**:
  - The latency of a link is the time it takes for a packet to travel from the source node to the destination node. It represents the propagation delay of the signal over the link.
- **Link loss rate**:
  - The loss rate of a link is the possibility that a packet transmitted over the link will be lost. Packet loss can occur due to various reasons, such as network congestion, buffer overflow, or link errors. In terms of the behavior of the loss, we can classify the loss into two types: independent loss and burst loss. In the section [packet loss pattern](#5packet-loss-pattern), we will introduce the loss pattern in detail.
- **Link jitter**:
  - In the context of computer networks, packet jitter or packet delay variation (PDV) is the variation in latency as measured in the variability over time of the end-to-end delay across a network. A network with constant delay has no packet jitter. Link jitter is the latency variation of the link.

## 5.Packet Loss Pattern

In assessing the efficacy of network protocols like TCP, UDP, and BATS, it's crucial to scrutinize their performances under varying degrees of packet loss. We will explore 3 principal packet loss models for the evaluation.

- **Random Loss Model**: The random loss model involves dropping packets independently based on a given percentage p. Each packet loss event is unrelated to others, making it a simple yet unpredictable scenario. This model reflects scenarios where packet loss occurs randomly and does not follow any specific pattern.
- **State Loss Model**: The state loss model utilizes a 4-state Markov chain to depict packet loss behaviors. The Markov chain states are as follows:
  - State 1: Good Packet Reception (no loss)
  - State 2: Good Reception within a Burst (no loss)
  - State 3: Burst Losses (loss probability is 100%)
  - State 4: Independent Losses (loss probability is 100%)
  
Utilizing transition probabilities such as p<sub>13</sub>, p<sub>31</sub>, p<sub>23</sub>, p<sub>32</sub>, and p<sub>14</sub> , the Markov chain's transition matrix is described as:

$$
A = \begin{bmatrix}
1-p_{13}-p_{14} & 0         & p_{13}          & p_{14} \\
0               & 1-p_{23}  & p_{23}          & 0 \\
p_{31}          & p_{32}    & 1-p_{31}-p_{32} & 0 \\
p_{41}          & 0         & 0               & 1-p_{41} \\
\end{bmatrix}
$$

Furthermore, we derive the probabilities of each state <i>π</i><sub>i</sub>,i=1...4 in terms of the aforementioned transition probabilities. Notably, State 4 is associated to isolated packet losses, after that the system must come back to State 1, so <i>p<sub>41</sub></i> is assumed to be 1. Solving the system yields:

$$
\left\{ 
\begin{aligned}
\pi_{1}(p_{13}+p_{14}) & = & \pi_{3}p_{31}+\pi_{4}\\
\pi_{3}(p_{32}) & = & \pi_{2}p_{23}\\
\pi_{1}(p_{14}) & = & \pi_{4}\\
\pi_{1}+\pi_{2}+\pi_{3}+\pi_{4}+ & = & 1\\
\end{aligned}
\right.
\Rightarrow\left\{ 
\begin{aligned}
\pi_{1} = \frac{p_{23}p_{31}}{p_{13}p_{23} + p_{23}p_{31} + p_{14}p_{23}p_{31} + p_{13}p_{32}} \\
\pi_{2} = \frac{p_{13}p_{32}}{p_{13}p_{23} + p_{23}p_{31} + p_{14}p_{23}p_{31} + p_{13}p_{32}} \\
\pi_{3} = \frac{p_{13}p_{23}}{p_{13}p_{23} + p_{23}p_{31} + p_{14}p_{23}p_{31} + p_{13}p_{32}} \\
\pi_{4} = \frac{p_{14}p_{23}p_{31}}{p_{13}p_{23} + p_{23}p_{31} + p_{14}p_{23}p_{31} + p_{13}p_{32}} \\
\end{aligned}
\right.
$$

Consequently, the theoretical packet loss probability is given by:

$$
P_{E}=\pi_{1}\cdot0\%+\pi_{2}\cdot0\%+\pi_{3}\cdot100\%+\pi_{4}\cdot100\%=\pi_{3}+\pi_{4}
$$

This model effectively captures scenarios involving structured packet losses, like bursts or isolated instances.

- **Gilbert-Elliot (GeModel) Loss Model**: The Gilbert-Elliot model, synonymous with the burst loss model, is characterized by distinctive states and transition probabilities:
  - Probability of transitioning to a bad (lossy) state.
  - Probability of transitioning from a bad state to a good state.
  - Loss probability in the bad state.
  - Loss probability in the good state.

The Gilbert-Elliott loss model is a Markov chain with two states (G and B, G is in the good state, and B is in the bad state. π<sub>G</sub> is the probability of being in the good state, and π<sub>B</sub> is the probability of being in the bad state.). The transition matrix of the Markov chain is as follows:

$$
A = \begin{bmatrix}
1-p & p \\
r   & 1-r \\
\end{bmatrix}
$$

Markov chains can achieve a stationary distribution independent of the initial state. Solving the system, we get the states probabilities:

$$
\left\{ 
\begin{aligned}
\pi_{G}(1-p)+\pi_{B}r & = & \pi_{G}\\
\pi_{G}p+\pi_{B}(1-r) & = & \pi_{G}\\
\pi_{G}+\pi_{B} & = & 1\\
\end{aligned}
\right.
\Rightarrow\left\{ 
\begin{aligned}
\pi_{G} = \frac{r}{p+r} \\
\pi_{B} = \frac{p}{p+r} \\
\end{aligned}
\right.
$$

As a result, the theoretical packet loss probability is:

$$
P_{E}=\pi_{G}(1-k)+\pi_{B}(1-h)=\frac{r(1-k)}{p+r}+\frac{p(1-h)}{p+r}
$$

The Gilbert-Elliott model is particularly useful for simulating bursty packet loss behaviors, where packets are more likely to be lost during periods of degraded network conditions (bad state) compared to periods of normal operation (good state). This model is suitable for scenarios with intermittent but extended periods of packet loss.

In conclusion, these loss models provide a structured way to simulate different types of packet loss patterns in network environments. Understanding how network protocols perform under these loss models helps in designing and optimizing protocols for real-world scenarios with varying degrees of packet loss. Please refer to [Definition of a general and intuitive loss model for packet networks and its implementation in the Netem module in the Linux kernel](https://citeseerx.ist.psu.edu/document?repid=rep1&type=pdf&doi=667a752ce7e577a31acf9907a0f133d538330205) for more details.

And all the supported loss pattern in the tool tc-netem can be found in https://www.man7.org/linux/man-pages/man8/tc-netem.8.html.

## 6.Protocol Metrics

In terms of performance evaluation, we care about the following metrics:

- **Throughput**: the average throughput during the whole transmission. `Throughput` here means how many payload data can be transmitted per second at the application layer. For example, for TCP transmission, the throughput is the number of TCP payload bytes transmitted per second. 
- **Latency**: the average latency during the whole transmission, it is an end-to-end measurement at the application layer.
- **Reliability**: the reliability of the system which is defined as the ratio of the number of successfully received(or packets after decoding if we use BATS codes) packets to the number of sent(or packets before encoding if we use BATS codes) packets. If we don't have restrictions on the latency of the feedback control, the reliability should be 1.0. If we have restrictions on the latency of the feedback, the reliability is less than 1.0.
- **Residual loss rate**: the residual loss rate over the BATS protocol transmission. For the BRTP(BATS Reliable Transmission Protocol), the residual loss rate should be 0.0. For the BTP(BATS Transmission Protocol, none reliable version), the residual loss rate may be slightly larger than 0.0.

To evaluate the efficiency of the BATS protocol, we need to monitor the following metrics:

- **Transmission rate of the link**: `Throughput` is not equal to the link bandwidth due to the overhead of the protocol. So in the test, we need to know the Link layer transmit/receive rate. And Link layer transmit/receive rate refers to the real networking load of the link. This can be used to evaluate the redundancy introduced by the BATS protocol.
- **Link loss statistics**: number of packet loss of each batch. Recoding number statistics is used to evaluate the coding efficiency of the BATS protocol in those middle nodes.
- **Recoding number statistics**: recoding number of each batch.

## 7.Protocols to Compare with

We will compare with different existing techniques such as TCP, TCP-BBR, UDP, KCP, etc. In future, we will also compare with PEP, QUIC and other protocols.

- **[TCP](https://en.wikipedia.org/wiki/Transmission_Control_Protocol)**: Linux implementation version of 6.5.0-14-generic.
- **[KCP](https://github.com/skywind3000/kcp)**: A Fast and Reliable ARQ Protocol. We use the KCP instance provided by [kcptun](https://github.com/xtaci/kcptun).
- **[TCP-bbr](https://en.wikipedia.org/wiki/TCP_congestion_control#TCP_BBR)**: The latest network congestion control algorithm in TCP.

## 8.BATS Protocol

In order to be compatible with the existing network infrastructure, BATS protocol is usually deployed as an overlay network protocol, it runs on top of the existing network protocol stack TCP/IP. The following diagram shows how the BATS protocol works in the network.
<div align="center" style="text-align:center">
<img src="imgs/bats_new_arch.svg" alt=""></div>
<div align="center">Fig 1.1 BATS protocol</div>

All the nodes in the network which run the BATS protocol are called BATS nodes. All BATS nodes form a BATS network; the following diagram shows the structure of a classic BATS network.

<div align="center" style="text-align:center">
<img src="imgs/bats-networking.svg" alt=""></div>
<div align="center">Fig 1.2 BATS networking</div>

### 8.1 BATS Transmission Protocol (BTP)

BTP is a none reliable transmission protocol based on [BATS codes](https://shhyang.github.io/research/bats/). It is designed to provide high throughput and low latency data transmission in lossy networks. The BTP employed restricted feedback, utilizing link loss rate and coding statistics to adjust the coding redundancy.

This enabled acceptable reliability while not relying on exhaustive feedback mechanisms. The BTP is suitable for scenarios where the feedback latency is high, and the reliability is not the primary concern.

Even though BTP is not a reliable protocol, but it can still provide a high reliability(99%) in most cases due to our advanced coding technology.

### 8.2 BATS Reliable Transmission Protocol (BRTP)

BRTP is a reliable transmission protocol based on [BATS codes](https://shhyang.github.io/research/bats/). As an enhancement of BTP, BRTP ensures 100% reliable data transmission by utilizing feedback to control the retransmission of unsolvable file trunks. Here retransmission is performed in network coding way, which is more efficient than transmitting the original data.

BRTP is suitable for scenarios where the reliability is the primary concern, and can endure increased latency.

Compared to the traditional TCP protocol, BRTP can be more efficient in lossy networks in terms of throughput and average latency, especially in the case of high loss rate.

### 8.3 Acceleration for TCP over BATS protocol

In order to improve the performance of TCP over BATS protocol, BATS protocol has developed an transparent proxy mode for TCP flows. It takes the idea from [On-board satellite "split TCP" proxy](https://ieeexplore.ieee.org/document/1264081).

## 9. Testing Cases

### 9.1 Overall Setup

- **Link bandwidth limit**: We set the bandwidth limit of the link to a sufficiently small value that the BATS protocol can easily fullfil, so that we can eliminate the uncertainty brought by software performance.
- **Source rate limit**: We set the data source rate to be smaller than the bandwidth limit in each test scenario, so that there is no congestion, and we can focus on the specific targets of each test scenario. Then, for each scenario, we can gradually increase the source rate towards the bandwidth limit, until exceeds it, to see how the communication behavior changes. In this case, we need to turn off the BATS protocol's congestion control mechanism.
- **Testing Platform**: Ubuntu 22.04 LTS, Linux kernel 6.5.0-14-generic.
- **Fixed parameters in the test**:
  - Link bandwidth: 300Mbps
  - Link jitter: 0

### 9.1 One-hop Network Testing

- **Purpose**: Study the outer code performance without worry about the inner code and congestion control. We focus on the evaluation of the end-to-end throughput, latency and reliability.
- **Scenery**: A one-hop network that includes two nodes, connected directly by a network link. The packets transmitted through the network link suffer from both packet loss and delay.

### 9.2 Multi-hop Network Testing

- **Purpose**: Study the inner code performance and the congestion control mechanism. We focus on the evaluation of the throughput, latency and reliability of different BATS protocols/mode in a multi-hop network.
- **Scenery**: A multi-hop chain network that includes multiple nodes, connected by network links. The packets transmitted through the network links suffer from both packet loss, delay and congestion.
- **Accumulated loss rate**: When the length of the network path is long, the accumulated loss rate will be high. The following diagram shows the accumulated loss rate of a 6-hop network with different link loss rates.

<div align="center" style="text-align:center">
<img src="imgs/6-hops-loss.png" alt="" style="zoom:60%;"></div>
<div align="center">Fig 9.1 Accumulated loss rate of a 6-hop network</div>

- **Topology**:

<div align="center" style="text-align:center">
<img src="imgs/6-hops.svg" alt=""></div>
<div align="center">Fig 9.2 Test network topology</div>

- **Parameters**:
  - Hop number: 6
  - Link latency: < 1ms
  - Link loss rate: 0% to 10%

#### 9.2.1 Multi-hop Network Testing with BTP, BRTP

<div align="center" style="text-align:center">
<img src="imgs/iperf_tcp_throughput_test.svg" alt=""></div>
<div align="center">Fig 9.3 Iperf3 throughput&reliability evaluation</div>

Customized application latency evaluation: