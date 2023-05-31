# BATS Protocol Performance Test

BATS Protocol is a network communication protocol based on BATS codes, designed to provide high throughput and low latency for data transmission in lossy networks. In this project, we develop a testbed and compare the performance of BATS Protocol with other techniques/protocols. The performance metrics include throughput, latency and reliability.  

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
    - [Scenario 1-1: Reliable Communication with Feedback](#scenario-1-1-reliable-communication-with-feedback)
    - [Scenario 1-2: Latency and Reliability Bounded Communication](#scenario-1-2-latency-and-reliability-bounded-communication)
  - [4 Multi-hop Network Testing](#4-multi-hop-network-testing)
  - [5 More](#5-more)

## 1. Introduction

The BATS Protocol performance test here focus on the capability of the communication logic for various application scenarios. We care about the relative performance gain over other techniques. The gain we obtain here should be scalable with the network bandwidth.  

This is NOT a software performance test. In other words, we do not test how fast the BATS Protocol software can run a device, and we do not test performance for specific applications, like video streaming.  

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

This part describes the requirements of BATS protocol system testing. Various testing cases are defined.

### 2.1. Testbed Setup

One OpenWRT router and servals Raspberry Pis are used to build the testbed. The OpenWRT router is used to simulate the lossy network and define the topology of the network. The Raspberry Pis are nodes in the networks. The OpenWRT router and the Raspberry Pis are connected by Ethernet cables.

## 3 One-hop Network Testing

### Scenario 1-1: Reliable Communication with Feedback

### Scenario 1-2: Latency and Reliability Bounded Communication

## 4 Multi-hop Network Testing

## 5 More
