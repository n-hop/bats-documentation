# Openvswitch配置

ubuntu 工控机上安装openvswitch，该工控机有三张物理网卡eth0 eth1 eth2 ：

(1)创建  ovs-br，并将 eth0 eth1 eth2添加到  ovs-br
```bash
sudo ovs-vsctl add-br ovs-br
sudo ovs-vsctl add-port ovs-br eth0
sudo ovs-vsctl add-port ovs-br eth1
sudo ovs-vsctl add-port ovs-br eth2
```

(2)配置 ovs-br的IP地址：

```bash
sudo ip addr add 10.0.0.1/24 dev ovs-br
sudo ip link set dev ovs-br up
```

(3)将PC1的网线插入eth0, PC1的配置如下：

```bash
ip addr add 10.0.0.2/24 dev eth0
ip route add 10.0.0.0/24 via 10.0.0.1
```

(4)将PC2的网线插入eth1, PC2的配置如下：

```bash
ip addr add 10.0.0.3/24 dev eth0
ip route add 10.0.0.0/24 via 10.0.0.1
```

(5)在ubuntu工控机上配置流量转发方向：

```bash
sudo ovs-ofctl add-flow ovs-br in_port=1,actions=output:2
sudo ovs-ofctl add-flow ovs-br in_port=2,actions=output:1,output:3
sudo ovs-ofctl add-flow ovs-br in_port=3,actions=output:2
```

(6)ubuntu工控机上设置网卡 eth0 eth1 eth2：

```bash
sudo ip link set eth0 promisc on
sudo ip link set eth1 promisc on
sudo ip link set eth2 promisc on
```

(7)其他命令：

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

# 测试网络连通性
    
```bash
# PC1 ping PC2
# PC2 ping PC1
```