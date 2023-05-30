# BATS test docs

This documentation describes the requirements of topologies, loss patterns and performance metrics for the BATS protocol system testing. Testing cases, which are defined in this docs, are used to compare the performance of BATS protocol with other protocols, such as TCP, TCP-BBR and UDP etc, in terms of throughput, latency, reliability and residual loss rate.

## Contents

- [1.Introduction](#1-introduction) 
- [2.Devices](#2-devices)
- [3.Topology](#3-topology)
- [4.Loss pattern](#4-loss-pattern)
- [5.Performance metrics](#5-performance-metrics)
- [6.Transmission payload](#6-transmission-payload)

## 1 Introduction

BATS protocol is a reliable transport protocol based on BATS codes. It is designed to provide high throughput and low latency for data transmission in lossy networks. The BATS protocol has implemented its own congestion control strategy, which is based on the feedback control. The feedback control can be used to adjust the transmission rate of the BATS protocol, report the link quality(such as the link loss rate, latency etc.) to the source and the request of retransmissions on un-decoded file blocks.

We will use different topologies, loss patterns to test the performance of BATS protocol. The topologies are defined in section [Topology](#3-topology). The loss patterns are defined in section [Loss pattern](#4-loss-pattern). The performance metrics are defined in section [Performance metrics](#5-performance-metrics).

## 2 Devices

One OpenWRT Router and servals Raspberry Pi are used to test the performance of BATS protocol. The OpenWRT Router is used to simulate the lossy network and define the topology of the network. The Raspberry Pi is used to simulate the nodes in the networks. The OpenWRT Router and the Raspberry Pi are connected by the Ethernet cable.

## 3 Topology

- **Topology 1**: a linear topology with 2 nodes.
- **Topology 2**: a linear topology with more than 2 nodes.
- **Topology 3**: a diamond topology with 4 nodes.

## 4 Loss pattern

- **Loss pattern 1**: an independent loss pattern with 1% loss rate.
- **Loss pattern 2**: an robust loss pattern with 1% loss rate.
- **Loss pattern 3**: an combination of independent loss pattern and robust loss pattern.

## 5 Performance metrics

- **Throughput**: the average throughput of the whole transmission.
- **Latency**: the average latency of the whole transmission.
- **Reliability**: the reliability of the system which is defined as the ratio of the number of successfully received(or decoded if we use BATS codes) packets to the number of sent(or encoded if we use BATS codes) packets. If we don't have restrictions on the latency of the feedback control, the reliability should be 1.0. If we have restrictions on the latency of the feedback, the reliability is less than 1.0.
- **Residual loss rate**: the residual loss rate over the BATS protocol transmission.

## 6 Transmission payload

we use the following payload to test the performance of BATS protocol.

```

1. The regular files which have the size from 1MB to 1GB.
2. The fixed video stream which has the flow rate from 1Mbps to 10Mps.

``` 
