# [Distributed systems for fun and profit](http://book.mixu.net/distsys/index.html) 阅读笔记

躺在收藏夹几年了，最近大概要做点分布式的东西，，拿出来看一看。

- [1. Distributed systems at a high level](#1)
- [](#2)
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
##[]()




&nbsp;   
<a id="3"></a>
##[]()



&nbsp;   
<a id="4"></a>
##[]()



&nbsp;   
<a id="5"></a>
##[]()

