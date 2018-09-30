# Python CookBook

&nbsp;   
## Ch01 数据结构和算法

解包

    data = ['ACME', 50, 91.1, (2012, 12, 21)]
    name, _, price, date = data
    # date is (2012, 12, 21)

星号解包

    def foo_sum(_list: list) -> int:
        head, *rest = _list
        return head if not rest else head + foo_sum(rest)

星号解包

    record = ('ACME', 50, 123.45, (12, 18, 2012))
    name, *_, (*_, year) = record

`deque`：有限队列

    from collections import deque
    
    q = deque([1, 2, 3, 4, 5, 6, 7], maxlen=5)
    print(*q)  # output: 3 4 5 6 7

`deque`：无限队列

    q = deque([1, 2, 3, 4, 5, 6, 7])
    print(*q)  # output: 1 2 3 4 5 6 7
    
    q.append(8)
    q.appendleft(0)
    print(*q)  # output: 0 1 2 3 4 5 6 7 8
    
    q.pop()
    q.popleft()
    print(*q)  # output: 1 2 3 4 5 6 7

`heapq.nsmallest`, `heapq.nsmallest`：n个最值

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








