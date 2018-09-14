 # 《代数组合论》自学笔记

Algebraic Combinatorics

Walks, Trees, Tableaux, and More     --  Richard P. Stanley

代数组合论

游动、树、表及其他                     --  辛国策 周岳 译

-----



## Ch01 图中的游动







### Ex01

<a id="ex1_1"></a>

1. (巧题) 给出推论1.6的组合证明，即 $K_p$（完全图）中某个顶点到其自身的长为 $l$ 的闭游动的条数是 $\frac{1}{p}((p-1)^l+(p-1)(-1)^l).$

proof:

记从某个顶点出发的 $l$ 游动到其他顶点的条数为 $A_l$，闭游动的条数为 $B_l$.

考察从某顶点开始的 $l$ 游动的所有条数：$(p-1)^l=(p-1)A_l+B_l.$

另，注意到：$B_{l+1}-A_{l+1}=(p-1)A_l-((p-2)A_l+B_l)=A_l-B_l.$

于是 $B_l-A_l=(-1)^l.$ 

即得：$B_l=\frac{1}{p}((p-1)^l+(p-1)(-1)^l).$



2.



