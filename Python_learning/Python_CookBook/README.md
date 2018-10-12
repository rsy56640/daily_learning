# Python CookBook

&nbsp;   
## Ch01 数据结构和算法

#### 解包

    data = ['ACME', 50, 91.1, (2012, 12, 21)]
    name, _, price, date = data
    # date is (2012, 12, 21)

#### 星号解包

    def foo_sum(_list: list) -> int:
        head, *rest = _list
        return head if not rest else head + foo_sum(rest)

#### 星号解包

    record = ('ACME', 50, 123.45, (12, 18, 2012))
    name, *_, (*_, year) = record

#### `deque`：有限队列

    from collections import deque
    
    q = deque([1, 2, 3, 4, 5, 6, 7], maxlen=5)
    print(*q)  # output: 3 4 5 6 7

#### `deque`：无限队列

    q = deque([1, 2, 3, 4, 5, 6, 7])
    print(*q)  # output: 1 2 3 4 5 6 7
    
    q.append(8)
    q.appendleft(0)
    print(*q)  # output: 0 1 2 3 4 5 6 7 8
    
    q.pop()
    q.popleft()
    print(*q)  # output: 1 2 3 4 5 6 7

#### `heapq.nsmallest()`, `heapq.nsmallest()`：n个最值

    import heapq
    
    portfolio = [
        {'name': 'IBM', 'shares': 100, 'price': 91.1},
        {'name': 'AAPL', 'shares': 50, 'price': 543.22},
        {'name': 'FB', 'shares': 200, 'price': 21.09},
        {'name': 'HPQ', 'shares': 35, 'price': 31.75},
        {'name': 'YHOO', 'shares': 45, 'price': 16.35},
        {'name': 'ACME', 'shares': 75, 'price': 115.65}
    ]
    cheap = heapq.nsmallest(3, portfolio, key=lambda s: s['price'])       # 3个 price 最小的
    expensive = heapq.nlargest(3, portfolio, key=lambda s: s['price'])    # 3个 price 最大的
    
    print(cheap)
    print(expensive)

#### `heapq.heappush()`, `heappop()`：最小堆

    import heapq
    
    class PriorityQueue:
        def __init__(self):
            self._queue = []
            self._index = 0
    
        def push(self, item, priority):
            heapq.heappush(self._queue, (priority, self._index, item))
            self._index += 1
    
        def pop(self):
            return heapq.heappop(self._queue)[-1]

#### 元组的比较

    print((11, Item()) < (2, Item()))   # False，尽管 Item 类型无法比较
    print([2, 3, Item()] < [2, 5, ])    # True，尽管个数不一样
不管个数与类型，比较的方式为：按顺序逐个比较，直到有一位能确定大小。

#### `defaultdict`

    from collections import defaultdict
    
    d = defaultdict(list)
    d['a'].append(1)
    d['a'].append(2)
    d['b'].append(4)
    for k, v in d.items():
        print(k, v)
    
    
    d = defaultdict(set)
    d['a'].add(1)
    d['a'].add(2)
    d['b'].add(4)
    for k, v in d.items():
        print(k, v)

#### `OrderedDict`：保证 key 只出现一次，并且顺序是第一次插入的顺序

    from collections import OrderedDict

    o = OrderedDict()
    o["qwe"] = 1
    o['a'] = 2
    o[3] = 3
    o['a'] = o[3]
    for k, v in o.items():
        print(k, v)

#### `zip()`, `dict.keys()`, `dict.values()`

    dict = { 'a': 2, 'b': 3.7 }
    max(zip(dict.values(), dict.keys())) # (3.7, 'b')

####

