# BATS Protocol Performance Test

BATS Protocol is a network communication protocol based on BATS codes, designed to provide high throughput and low latency for data transmission in lossy networks. In this project, we develop a testbed to compare the performance of BATS Protocol with other protocols, such as TCP, TCP-BBR, QUIC and UDP. The performance metrics include throughput, latency and reliability. 

## Contents

- [1. Introduction](#1-introduction) 
- [2. Testbed Overview](#2-Testbed-Overview)
- [3.Topology](#3-topology)
- [4.Loss pattern](#4-loss-pattern)
- [5.Performance metrics](#5-performance-metrics)
- [6.Transmission payload](#6-transmission-payload)

## 1. Introduction

BATS Protocol is a network communication protocol based on BATS codes. It is designed to provide high throughput and low latency for data transmission in lossy networks. BATS Protocol has implemented its own congestion control strategy that involves the intermediate network nodes. The feedback control can be used to adjust the transmission rate of the BATS protocol, report the link quality(such as the link loss rate, latency etc.) to the source and the request of retransmissions on un-decoded file blocks.

We will use different topologies, loss patterns to test the performance of BATS protocol. The topologies are defined in section [Topology](#3-topology). The loss patterns are defined in section [Loss pattern](#4-loss-pattern). The performance metrics are defined in section [Performance metrics](#5-performance-metrics).

## 2. Testbed Overview

This part describes the requirements of BATS protocol system testing. Various testing cases are defined.

### 2.1. Testbed Setup

One OpenWRT router and servals Raspberry Pis are used to build the testbed. The OpenWRT router is used to simulate the lossy network and define the topology of the network. The Raspberry Pis are nodes in the networks. The OpenWRT router and the Raspberry Pis are connected by Ethernet cables.

### 2.2. Network Topologies
Three kinds of network topologies will be mainly used in the tests:
- one-hop network
- multi-hop networks
- multi-path networks
#### 2.2.1. One-hop Network



### 4 Loss pattern

- **Loss pattern 1**: an independent loss pattern with 1% loss rate.
- **Loss pattern 2**: an robust loss pattern with 1% loss rate.
- **Loss pattern 3**: an combination of independent loss pattern and robust loss pattern.

### 5 Performance metrics

- **Throughput**: the average throughput of the whole transmission.
- **Latency**: the average latency of the whole transmission.
- **Reliability**: the reliability of the system which is defined as the ratio of the number of successfully received(or decoded if we use BATS codes) packets to the number of sent(or encoded if we use BATS codes) packets. If we don't have restrictions on the latency of the feedback control, the reliability should be 1.0. If we have restrictions on the latency of the feedback, the reliability is less than 1.0.
- **Residual loss rate**: the residual loss rate over the BATS protocol transmission.

### 6 Transmission payload

we use the following payload to test the performance of BATS protocol.

```

1. The regular files which have the size from 1MB to 1GB.
2. The fixed video stream which has the flow rate from 1Mbps to 10Mps.

``` 
## 3 One-hop Network Testing

## 4 Multi-hop Network Testing

## 5 More
