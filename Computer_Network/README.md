# 计算机网络学习笔记

&nbsp;   
### 参考书籍：

- [TCP/IP详解 卷1：协议]()
- [UNIX网络编程 卷1：套接字联网API]()
- [计算机网络 自顶向下方法（英文第6版）]()
- [Effective TCP/IP Programming（TCPIP高效编程：改善网络程序的44个技巧）]()


### 视频资料：

- [CS144 Introduction to Computer Networking Fall 2016 Stanford University](https://www.youtube.com/playlist?list=PLvFG2xYBrYAQCyz4Wx3NPoYJOFjvU7g2Z)
- [Computer Networking: A Top-Down Approach Sixth Edition](https://wps.pearsoned.com/ecs_kurose_compnetw_6/216/55463/14198700.cw/index.html)
- [计算机网络 - 哈尔滨工业大学](http://www.icourse163.org/course/HIT-154005)
- [IT Support: Networking Essentials - Microsoft: CLD260x](https://courses.edx.org/courses/course-v1:Microsoft+CLD260x+3T2018/course/)



### 笔记资料：

- [Computer Network Tutorials - GeeksforGeeks](https://www.geeksforgeeks.org/computer-network-tutorials/)
- [Data Communication & Computer Network - tutorialspoint](https://www.tutorialspoint.com/data_communication_computer_network/)
- [note/计算机网络/计算机网络.md](https://github.com/arkingc/note/blob/master/%E8%AE%A1%E7%AE%97%E6%9C%BA%E7%BD%91%E7%BB%9C/%E8%AE%A1%E7%AE%97%E6%9C%BA%E7%BD%91%E7%BB%9C.md)
- [CS-Notes/notes/计算机网络.md](https://github.com/CyC2018/CS-Notes/blob/master/notes/%E8%AE%A1%E7%AE%97%E6%9C%BA%E7%BD%91%E7%BB%9C.md)
- [InterviewMap - Network](https://yuchengkai.cn/docs/cs/)
- [Skill-Tree/计算机网络.md](https://github.com/linw7/Skill-Tree/blob/master/%E8%AE%A1%E7%AE%97%E6%9C%BA%E7%BD%91%E7%BB%9C.md)
- [Skill-Tree/网络编程.md](https://github.com/linw7/Skill-Tree/blob/master/%E7%BD%91%E7%BB%9C%E7%BC%96%E7%A8%8B.md)
- [笔试面试知识整理 - 计算机网络](https://hit-alibaba.github.io/interview/basic/network/)
- [搞定计算机网络面试，看这篇就够了 - 掘金](https://juejin.im/post/5b5f20686fb9a04f844adbdd)


### 实验资料：

- [Beej's Guide to Network Programming - Using Internet Sockets](https://beej.us/guide/bgnet/html/single/bgnet.html)
- [《计算机网络：自顶向下方法》配套实验：TCP模拟器 - 知乎](https://zhuanlan.zhihu.com/p/35390933)
- [Socket 编程实战](https://liujiacai.net/blog/2016/10/31/socket-programming/)


&nbsp;   
## 目录

- [CS144 学习笔记](#cs144)
  - [](#)
  - []()
  - []()
- [计算机网络 - 哈尔滨工业大学 学习笔记](#computer_network_hit)
  - []()
  - []()
  - []()
- []()
  - []()
- []()
  - []()



&nbsp;   
&nbsp;   
<a id="cs144"></a>
## CS144 学习笔记

### 1-1. A day in the life of an application
**Bidirectional, reliable byte stream** communication model  
Abstract away entire network -- just a pipe between 2 programs  
Application Level:

- WWW (HTTP)
- Skype
- BitTorrent

### 1-2. The four layer Internet model

Early 4 layer internet model:

- **Application**: Bi-directional reliable byte stream between 2 applications, using application-specific semantics(e.g. HTTP, BitTorrent)
- **Transport**: Guarantees correct, in-order delivery of data end-to-end. Controls congestion.
- **Network**: Delivers datagrams end-to-end. Best-effort delivery - no guarantees. Must use the Internet Protocol(IP)
- **Link**: Delivers data over a single link between an end host and router, or between routers.

![](assets/cs144_2_1.png)

**IP**:

- IP makes a best-effort attempt to deliver our datagrams to the other end. But it makes no promises.
- IP datagrams can get lost, can be delivered out of order, and can be corrupted. There are no guarantees.

![](./assets/cs144_2_2.png)

### 1-3. The IP service model

![](./assets/cs144_3_1.png)

**Datagram**: Individually routed packets. Hop-by-hop routing.

- The IP service model provides a service which includes the routing to the destination.   
- The forwarding decision at each router is based on the **IP DA**( destination address).   
- Each router contains a **forwarding table** tells **where to send the packet next** after matching a given destination address. **The router doesnot know the whole path**.   
- Neither the sender nor the receiver know the whole path.

**Unreliable**: Packets might be dropped.

- dropped
- duplicated
- wrong destination

**Best effort**: but onlt if necessary.

**Connectionless**: No per-flow state. Packets might be mis-sequenced.

The reason IP is **simple**:

- Simple, dumb, minimal: faster, more streamlined, lower cost to build and maintain.
- The end-to-end principle: where possible, implement features in the end hosts.
- Allows a variety of reliable (or unreliable) services to be built on top.
- Works over any link layer: IP makes very few assumptions about the link layer below.

**The IP service model**   
1. Tries to prevent packets looping forever. (Because forwarding table might change. IP simply adds a **hop-count** (TTL) field in the header of each datagram)   
2. Will fragment packets if they are too long.   
3. Uses a header checksum to reduce chances of delivering datagram to wrong destination.   
4. Allows for new versions of IP: IPV4 and IPV6   
5. Allows for new options (fields) to be added to header.   

![](./assets/cs144_3_2.png)

![](./assets/IP_datagram_format_2018_11_07.jpg)

- **版本** : 有 4（IPv4）和 6（IPv6）两个值；
- **首部长度** : 占 4 位，因此最大值为 15。值为 1 表示的是 1 个 32 位字的长度，也就是 4 字节。因为首部固定长度为 20 字节，因此该值最小为 5。如果可选字段的长度不是 4 字节的整数倍，就用尾部的填充部分来填充。
- **区分服务** : 用来获得更好的服务，一般情况下不使用。
- **总长度** : 包括首部长度和数据部分长度。
- **生存时间** ：TTL，它的存在是为了防止无法交付的数据报在互联网中不断兜圈子。以路由器跳数为单位，当 TTL 为 0 时就丢弃数据报。
- **协议** ：指出携带的数据应该上交给哪个协议进行处理，例如 ICMP、TCP、UDP 等。
- **首部检验和** ：因为数据报每经过一个路由器，都要重新计算检验和，因此检验和不包含数据部分可以减少计算的工作量。
- **标识** : 在数据报长度过长从而发生分片的情况下，相同数据报的不同分片具有相同的标识符。
- **片偏移** : 和标识符一起，用于发生分片的情况。片偏移的单位为 8 字节。

![](./assets/datagram_shard_eg_2018_11_07.png)

### 1-4. A Day in the Life of a Packet

TCP 3 handshake

- [SYN] &nbsp;&nbsp;&nbsp; =>
- &emsp;&emsp;&emsp;&nbsp;&nbsp; <= &nbsp;&nbsp;&nbsp; [SYN, ACK]
- [ACK] &nbsp;&nbsp;&nbsp; =>

### 1-5. Packet switching principle

**Packet**: A self-contained unit of data that carries information necessary for it to reach its destination.

**Packet switching**: Independently for each arriving packet, pick its outgoing link. If the link is free, send it. Else hold the packet for later.

**Self-routing** is usually **turned off** because of the **security issues**: people owning routers donot want you telling them how to send packets. (trick to send packets to secret computer)

2 properties of packet switching:

- Simple packet forwarding
- Efficient sharing of links

#### No per-flow state required
**Flow**: A collection of datagrams belonging to the same end-to-end communication, e.g. a TCP connection.

- No per-flow state to be added/removed
- No per-flow state to be stored
- No per-flow state to be changed upon failure

Summary

- **Packet switches are simple**: they forward packets independently, and don't need to know about flows.
- **Packet switching is efficient**: it lets us efficiently share the capacity among many flows sharing the link.

### 1-6. Layering principle

Layers communicate sequentially with the layers above and below.

This separation of concerns allows each layer in the hierarchy to focus on doing its job well, and provide a well-defined service to the layer above.

The lower layers abstract away the details for higher layers.

Reasons for layering:

- Modularity
- Well defined service
- Reuse
- Separation of concerns
- Continuous improvement

### 1-7. Encapslation principle

### 1-8. Byte order

- Little Endian
- Big Endian


&nbsp;   
&nbsp;   
<a id="computer_network_hit"></a>
## 计算机网络 - 哈尔滨工业大学 学习笔记

