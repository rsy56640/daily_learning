# [CMU 15-445/645 Intro to Database Systems (Fall 2017)](https://www.youtube.com/playlist?list=PLSE8ODhjZXjYutVzTeAds8xUt1rcmyT7x)

这个课程的slides和notes都很全，，我不太想记笔记了。

- [](#)
- [06 Database Storage](#06)
- [07 Buffer Pools](#07)
- [](#08)
- [](#09)


&nbsp;   
<a id="06"></a>
## [06 Database Storage](https://www.youtube.com/watch?v=VLagoZ90ABk)

#### Do not use OS, DBMS wants to control things itself
- Specialized prefetching
- Buffer replacement policy
- Scheduling
- Flushing data to disk

> Of course, `mmap` is ok if you **only read data**.

- File Storage: 虽然文件系统无法对DB文件做优化，但是重写一个太麻烦了，效率提升一些，也不可移植。
- Page
  - Heap File Organization
  - Sequential/Sorted File Organization
  - Hashing File Organization
  - Log-Structured File Organization
- Tuple: each tuple in the database is assigned a unique identifier (page id + offset/slot)
  - Slotted Pages:
      - Header
      - Tuples' Offsets
      - ...
      - Tuple
  - Tuple Layout:
      - Header
      - Attr Data
- Storage Model


&nbsp;   
<a id="07"></a>
## [07 Buffer Pools](https://www.youtube.com/watch?v=sp_vhhgKQQg)

#### Row based storage model
- Advantages
  - Fast inserts, updates and deletes
  - Entire tuple query
- Disadvantages
  - Scan subsets of attributes

#### Column based storage model
- Advantages
  - Query processing
- Disadvantages
  - Slow for point query, insert, update adnd delete

#### Buffer Pool is an in-memory cache of pages read from disk.

#### Lock vs Latch(mutex)
**Lock** is a concept of logically protecting DB contents from transactions, while **Latch** is used to protect critical sections.

### Buffer Pool
- Buffer Pool Manager
  - race condition
  - prefetch
  - scan sharing: share the page with multiple queries
- Replacement Policy
  - LRU (using hash to speed up)
- Allocation Policy
- Other Memory Pools: DBMS needs memory other than tuples and indexes
  - Sorting, Join Buffer
  - Query Cache
  - Maintanence Buffer
  - Log Buffer
  - Dictionary Cache 


&nbsp;   
<a id="08"></a>
## []()



&nbsp;   
<a id="09"></a>
## []()

