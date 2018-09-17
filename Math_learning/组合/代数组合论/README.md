# 《代数组合论》自学笔记

Algebraic Combinatorics

Walks, Trees, Tableaux, and More     --  Richard P. Stanley

代数组合论

游动、树、表及其他                     --  辛国策 周岳 译

-----



- [第一章 图中的游动](#1)
- [](#)
- [](#)
- [](#)





<a id="1"></a>

# Ch01 图中的游动

图 $G$ 的邻接矩阵(adjacency matrix) 是复数域上的 $p\times p$ 阶矩阵 $\pmb A= \pmb A(G)$，其 $(i,j)$ 元 $a_{ij}$ 等于与 $v_i$ 和 $v_j$ 相关联的边的条数，故 $\pmb A$ 是一个实对称矩阵（从而有实特征值）。

<a id="1.1"></a>

### 1.1 定理

对任意整数 $l\ge 1$，矩阵 $A(G)^l$ 的 $(i,j)$ 元等于从 $G$ 中从 $v_i$ 到 $v_j$ 的长为 $l$ 的游动的条数。



<a id="1.2"></a>

### 1.2 推论

在图 $G$ 中固定两个顶点 $v_i$ 和 $v_j$.设邻接矩阵 $\pmb A(G)$ 的特征值为 $\lambda_1,...,\lambda_p$.则存在实数 $c_1,...,c_p$ 使得对任意 $l$ 都有
$$
\pmb A(G)^l_{ij}=c_1\lambda_1^l+...+c_p\lambda_p^l
$$
事实上，如果 $U$ 是满足 $U^{-1}\pmb AU=diag(\lambda_1,...,\lambda_p)$ 的实正交矩阵，那么
$$
c_k=u_{ik}u_{jk}
$$
**Proof**:
$$
\begin{aligned}
\pmb A^l
& = U\cdot diag(\lambda_1^l,...,\lambda_p^l)U^{-1} \\
& = U\cdot diag(\lambda_1^l,...,\lambda_p^l)U^T    \\
\pmb A_{ij}^l & = \sum_k u_{ik} \lambda_k^l u_{jk} \\
\end{aligned}
$$
**$Q.E.D.$**



定义：图 $G$ 的一个闭游动是一个起点与终点相同的游动。

因此长度为 $l$ 的闭游动的*总*(total)条数 $f_G(l)$ 由下式给出
$$
f_G(l)=\sum_{i=1}^p \pmb A_{ii}^l=tr(\pmb A^l)
$$


<a id="1.3"></a>

### 1.3 推论

如果 $\pmb A(G)$ 有特征值 $\lambda_1,...,\lambda_p$，那么 $G$ 中长为 $l$ 的闭游动的条数为
$$
f_G(l)=\lambda_1^l+...+\lambda_p^l
$$


于是，我们可以通过算两次的方法来计算特征值：

用组合推理来对图中的游动计数，而结果反过来可以确定 $G$ 的特征值。



<a id="1.4"></a>

### 1.4 引理

令 $\pmb J$ 表示 $p\times p$ 阶全 $1$ 矩阵，则 $\pmb J$ 的特征值为 $p$（$1$ 重）和 $0$（$p-1$ 重）。

**Proof**:

$rank(\pmb J)=1$，于是有 $p-1$ 个特征值为 $0$，再由 $tr(\pmb J)=p$ 所以唯一非零特征值为 $p$.

**$Q.E.D.$**



<a id="1.5"></a>

### 1.5 命题

完全图 $K_p$ 的特征值有：重数为 $p-1$ 的特征值 $-1$，重数为 $1$ 的特征值 $p-1$.



<a id="1.6"></a>

### 1.6 推论

完全图 $K_p$ 中从顶点 $v_i$ 出发的长为 $l$ 的闭游动的总条数是
$$
(\pmb A(K_p)^l)_{ii}=\frac{1}{p}((p-1)^l + (p-1)(-1)^l).
$$
**Proof**:

[练习 1.1](#ex1.1)中给出了另一种巧妙的组合证明。
$$
\begin{aligned}
\pmb A^l
& = (J-I)^l \\
& = \sum_{k=0}^l (-1)^{l-k}\binom{l}{k}J^k \qquad\qquad (note\ J^k=p^{k-1}J,\ J^0=I) \\
& = \sum_{k=1}^ (-1)^{l-k}\binom{l}{k}p^{k-1}J + (-1)^l I \\
for\ i\neq j,\quad \pmb A_{ij}^l & = \frac{1}{p}((p-1)^l - (-1)^l)
\end{aligned}
$$
**$Q.E.D.$**

注意到一个有趣的事实：如果 $i \neq j$，那么
$$
(\pmb A(K_p)^l)_{ii} - (\pmb A(K_p)^l)_{ij} = (-1)^l
$$
另外，$K_p$ 中长为 $l$ 的游动的总条数为
$$
\sum_{i=1}^p \sum_{j=1}^p (\pmb A(K_p)^l)_{ij} = p(p-1)^l
$$
<a id="1.7"></a>

### 1.7 引理

设 $\alpha_1,...,\alpha_r$ 和 $\beta_1,...,\beta_s$ 是非零复数，并使得对任意正整数 $l$ 都有
$$
\alpha_1^l+...+\alpha_r^l=\beta_1^l+...+\beta_s^l
$$
则 $r=s$ 且 $\{\alpha\}$ 恰是 $\{\beta\}$ 的置换。

**Proof**:

（生成函数），取 $|x|<1$，将上式乘以 $x^l$ 并对所有的 $l\ge 1$ 求和，级数收敛，且
$$
\frac{\alpha_1 x}{1-\alpha_1 x}+...+\frac{\alpha_r x}{1-\alpha_r x} = \frac{\beta_1 x}{1-\beta_1 x}+...+\frac{\beta_s x}{1-\beta_s x}  \tag{1.7}
$$
于是得到一个恒等多项式，且有无限的零点，于是系数相等，

因此式 $(1.7)$ 对所有复数 $x$ （除了分母为 $0$）成立.

固定一个 $\gamma\neq 0$，将式 $(1.7)$ 乘以 $1-\gamma x$ 并令 $x\to 1/\gamma$，左边为等于 $\gamma$ 的 $\alpha_i$ 的个数，而右边为等于 $\gamma$ 的 $\beta_j$ 的个数。

**$Q.E.D.$**



<a id="ex1.1"></a>

### Ex01

#### 1.

(巧题) 给出推论1.6的组合证明，即 $K_p$（完全图）中某个顶点到其自身的长为 $l$ 的闭游动的条数是 $\frac{1}{p}((p-1)^l+(p-1)(-1)^l).$

**Proof**:

记从某个顶点出发的 $l$ 游动到其他顶点的条数为 $A_l$，闭游动的条数为 $B_l$.

考察从某顶点开始的 $l$ 游动的所有条数：$(p-1)^l=(p-1)A_l+B_l.$

另，注意到：$B_{l+1}-A_{l+1}=(p-1)A_l-((p-2)A_l+B_l)=A_l-B_l.$

于是 $B_l-A_l=(-1)^l.$ 

即得：$B_l=\frac{1}{p}((p-1)^l+(p-1)(-1)^l).$

**$Q.E.D.$**



#### 2.





