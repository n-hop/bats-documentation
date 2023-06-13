# BATS Protocol Performance Test

BATS Protocol is a network communication protocol based on BATS codes, designed to provide high throughput and low latency data transmission in lossy networks. In this project, we evaluate and compare the performance of BATS Protocol with other techniques/protocols in terms of various performance metrics such as throughput, latency and reliability.  

## Contents

- [BATS Protocol Performance Test](#bats-protocol-performance-test)
  - [Contents](#contents)
  - [1. Introduction](#1-introduction)
    - [Network Topologies](#network-topologies)
      - [One-hop Network](#one-hop-network)
    - [Loss pattern](#loss-pattern)
    - [Performance metrics](#performance-metrics)
    - [Transmission Payload](#transmission-payload)
    - [Protocols to Compare with](#protocols-to-compare-with)
  - [2. Testbed Overview](#2-testbed-overview)
    - [2.1. Testbed Setup](#21-testbed-setup)
  - [3 One-hop Network Testing](#3-one-hop-network-testing)
  - [4 Multi-hop Network Testing](#4-multi-hop-network-testing)
  - [5 Multi-path Network Testing](#5-multi-path-network-testing)

## 1. Introduction

The BATS Protocol performance evaluation focuses on the capability of the communication logic, such as coding (including feedback), congestion control, routing, etc. Therefore, in the testbed, we try to reduce the effect of the implementation by limitting the network link bandwidth. The performance compared with other techniques is relative, and the gain should be scalable with the network bandwidth.

This is NOT a software performance test. In other words, we do not test how fast the BATS Protocol software can run a device. Tough the testbed considers various practical scenarios, we do not test performance for specific applications, like video streaming.  

### Network Topologies

Three kinds of network topologies will be mainly used in the tests:

- one-hop network
- multi-hop networks
- multi-path networks

#### One-hop Network

### Loss pattern

- **Loss pattern 1**: an independent loss pattern with 1% loss rate.
- **Loss pattern 2**: an robust loss pattern with 1% loss rate.
- **Loss pattern 3**: an combination of independent loss pattern and robust loss pattern.

### Performance metrics

- **Throughput**: the average throughput of the whole transmission.
- **Latency**: the average latency of the whole transmission.
- **Reliability**: the reliability of the system which is defined as the ratio of the number of successfully received(or decoded if we use BATS codes) packets to the number of sent(or encoded if we use BATS codes) packets. If we don't have restrictions on the latency of the feedback control, the reliability should be 1.0. If we have restrictions on the latency of the feedback, the reliability is less than 1.0.
- **Residual loss rate**: the residual loss rate over the BATS protocol transmission.

### Transmission Payload

we use the following payload to test the performance of BATS protocol.  

```plain

1. The regular files which have the size from 1MB to 1GB.
2. The video stream which has the fixed flow rate from 1Mbps to 10Mps.

```  

### Protocols to Compare with

We will compare with different existing techniques such as TCP, TCP-BBR, PEP, QUIC and UDP.

## 2. Testbed Overview

One router (running openWRT) and servals computers (e.g., Raspberry Pis) are used to build the testbed. The router and the computers are connected by Ethernet cables. We call this network formed by the computers and the router our *physical network*. In contrast, the network used for the testing is simulated by the physical network and is called the *test network*. 

The test network is formed by multiple network nodes simulated by the computers. Each computer can simulate one or multiple network nodes in the test network. The connection between the network nodes, called the network topology, is determined by the router. All the communication communications between different nodes will go through the router, which will simulate different network link behaviors such as packet loss, latency, wireless interference, etc. 

The detailed setup of the testbed is described in the following link:

- [Testbed Setup](testbed/testbed_setup.md)

## 3 One-hop Network Testing

We begin with a one-hop network that includes two nodes, connected directly by a network link. The two nodes can directly communicate with each over the network link with a limited bandwidth. The packets transmitted through the network link suffer from both packet loss and delay.

Though simple, this network enables us to study the outer code performance without worry about the inner code and congestion control. We focus on the evaluation of the end-to-end throughput, latency and reliability. 
 
Feedback is allowed to assist the communication, and the feedback messages also suffer from loss and delay. 

- **Scenario 1-1: Reliable Communication with Feedback**

    In this scenario, we test the performance of BATS protocol in a one-hop network. The feedback control is used to control the retransmission of the unsolvable file trunk. In other words, the protocol is reliable. we will repeat the testing with different loss patterns and different transmission payload.

- **Scenario 1-2: Latency and Reliability Bounded Communication**

    In this scenarios, we have restrictions on the latency of the feedback control. we can't use the feedback to do the retransmission of the unsolvable file trunk since the latency is restricted. In other words, the protocol is not 100% reliable. However, we can use the feedback to notify the sender about the link quality. The sender can adjust the coding redundancy according to the link quality. In this cases, we can ensure that a certain percentage of the file trunks can be successfully decoded at receiver side. we will repeat the testing with different loss patterns and different transmission payload.

## 4 Multi-hop Network Testing

In multi-hop testing, we will repeat the testing scenarios in one-hop network testing cases. The only difference is that the network topology is changed to multi-hop network and we introduce the recoding technique in the middle nodes. And the feedback is performed in hop-by-hop manner.

## 5 Multi-path Network Testing

In multi-path testing, we will introduce the multi-path transmission in the network.
