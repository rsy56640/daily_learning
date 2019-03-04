# [Distributed systems for fun and profit](http://book.mixu.net/distsys/index.html) 阅读笔记

躺在收藏夹几年了，最近大概要做点分布式的东西，，拿出来看一看。   
**这里面有很多 Further reading，之后还得好好研究。**

- [1. Distributed systems at a high level](#1)
- [2. Up and down the level of abstraction](#2)
- [](#3)
- [](#4)
- [](#5)

&nbsp;   
<a id="1"></a>
## [1. Distributed systems at a high level](http://book.mixu.net/distsys/intro.html)

- Scalability：当task的规模不断上涨时可以 1. 增加node；2. 增加异地数据中心（处理延迟）
- Performance
  - 低延迟：受限于物理层面
  - 高吞吐
  - 低消耗
- Latency：
- Availability：技术上主要考虑 fault-tolerance

### Abstractions and Models

- System Model (asynchronous / synchronous)
- Failure Model (crash-fail, partitions, Byzantine)
- Consistency Model (strong / eventual)

### Partition and Replicate

- Partition: 将数据集分割成独立的子集
  - performance: 限制处理数据大小，同一分区内数据相关
  - availability: 分区允许fail，降低了对整体可用性造成影响的概率
  - *Partitioning is mostly about defining your partitions based on what you think the primary access pattern will be, and dealing with the limitations that come from having independent partitions*
- Replicate: 同样的数据复制到多台机器上
  - performance: 额外的计算能力和带宽
  - availability: 某些结点fail了也没事
  - *Replication is about providing extra bandwidth, and caching where it counts. It is also about maintaining consistency in some way according to some consistency model*
  - 要维护 **consistency**，**trade-off**


&nbsp;   
<a id="2"></a>
##[2. Up and down the level of abstraction](http://book.mixu.net/distsys/abstractions.html)

### 关于系统模型的若干假设

#### Nodes
- 功能：1. 执行程序；2. 存储数据到mem和disk；3. （并不准确的）时钟
- 一个node 在接受 msg 和接受msg时的 state 决定了：本地 computation，完成后的 state，以及发送的 msg
- 通常认为 node 只有在 crash 才 fail，并且之后的某个时间点会 recover
- 另一种假设是 node 可能 misbehave，也就是拜占庭将军问题，这里不讨论

#### Communication Links
- 通常认为是 unreliable, msg 可能 lost 或 delayed
- 很多情况很具体，视情况做优化和trade-off

#### Time and Order
同步模型：假定 msg delay 有上界，并且 clock 保证同步   
异步模型：node 只有 crash 才 fail，msg 可以无限期 delay

consensus：
1. Agreement
2. Integrity
3. Termination
4. Validity

#### FLP
即使最多一个 node fail，也没有算法保证在异步模型中的 consensus

#### CAP
- **Consistency**: 同一时刻，所有 nodes 看到的数据是一致的（即强一致性）
- **Availability**: 某些 node fail 不影响其他 nodes 继续运行（即服务始终可用）
- **Partition tolerance**: *网络分区* 不影响整个系统继续运行（即协议可以容忍任何网络分区异常）

最多只能同时满足两点

![](assets/CAP.png)

`CA` 和 `CP` 都提供了 strong consistency，区别在于：`CA` 不允许 node fail；`CP` 有 `2f+1` 个 node，那么允许 `f` 个 node fail

- `CA` 不区分 node fail 还是 network fail，所以 stop accepting writes，防止 multiple copies
- `CP` 在两个分区上采取强制非对称的行为来保证 single-copy consistency
- `CA` 没有分区，常用2PC，多用于分布式关系数据库；`CP` 有分区，用 Paxos/Raft 等区分，并且允许网络故障

在强一致性和性能之间做trade-off，提高可用性

### Consistency Model

- Strong Consistency Model
  - Linearizable Consistency：顺序可以对外公布
  - Sequential Consistency：每个 node 看到的顺序一致
- Weak Consistency Model
  - Client-Centric Consistency Model：保证用户不会访问old data（实现上通常在本地保留快照缓存）
  - Causal Consistency: Strongest Model Available
  - Eventual Consistency Model

参考：

- [FLP Impossibility](https://blog.csdn.net/chen77716/article/details/27963079)


&nbsp;   
<a id="3"></a>
##[]()



&nbsp;   
<a id="4"></a>
##[]()



&nbsp;   
<a id="5"></a>
##[]()

