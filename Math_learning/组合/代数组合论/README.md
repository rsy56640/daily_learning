# 《代数组合论》自学笔记

Algebraic Combinatorics

Walks, Trees, Tableaux, and More     --  Richard P. Stanley

代数组合论

游动、树、表及其他                     --  辛国策 周岳 译

-----



- [第一章 图中的游动](#1)
- [第二章 立方体和 Radon 变换](#2)
- [第三章 随机游动](#3)
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
& = \sum_{k=1}^l (-1)^{l-k}\binom{l}{k}p^{k-1}J + (-1)^l I \\
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



<a id="2"></a>

# Ch02 立方体和 Radon 变换

用 $\mathbb{Z_2}$ 表示 2 阶循环群，$\mathbb{Z_2^n}$ 表示本身的 $n$ 次直积，即为 $0$ 和 $1$ 组成的 $n$ 元组 $(a_1,a_2,...,a_n)$. 定义如下一个称为 $n$ 维立方体的图 $C_n$：$C_n$ 的顶点集为 $V=\mathbb{Z_2^n}$，两顶点 $u$ 和 $v$ 之间有一条边当且仅当它们有一个坐标不同，也就是说，$u+v$ 恰有一个坐标非 $0$.

为了明确地得到 $C_n$ 的特征值和特征向量，采用有限Radon变换。用 $\mathcal{V}$ 表示所有函数 $f:\mathbb{Z_2^n}\to\mathbb{R}$ 的集合，其中 $\mathbb{R}$ 表示实数域。（注意 $\mathcal{V}$ 是 $\mathbb{R}$ 上的 $2^n$ 维向量空间）

定义向量空间 $\mathcal{V}$ 中两组重要的基。对应于每一个 $u\in\mathbb{Z_2^n}$ 在每组基中都有一个基元。

第一组基 $B_1$ 基元 $f_u$ 定义如下：
$$
f_u(v) = \delta_{uv}
\tag{2.2}
$$

其中 $\delta_{ij}$ 为 Kronecker delta.

$B_1$ 是一组基，因为对 $\forall g\in \mathcal{V}$ 都满足
$$
\begin{equation}\label{2.3}
g = \sum_{u\in\mathbb{Z_2^n} }g(u) f_u
\end{equation}
\tag{2.3}
$$
第二组基 $B_2$ 基元 $\mathcal{X_u}$ 定义如下：
$$
\mathcal{X_u}(v) = (-1)^{u \cdot v}
$$
<a id="2.1"></a>

### 2.1 引理

集合 $B_2 = \{\mathcal{X_u}:u\in\mathbb{X_2^n}\}$ 构成了 $\mathcal{V}$ 的一组基。

**Proof**:

证明正交：
$$
\begin{aligned}
<\mathcal{X_u}, \mathcal{X_v}>
& = \sum_{w\in\mathbb{Z_2^n}} \mathcal{X_u}(w)\mathcal{X_v}(w) \\
& = \sum_{w\in\mathbb{Z_2^n}} (-1)^{(u+v)\cdot w} \\
& = \begin{equation}
\left\{
        \begin{array}{}
            2^n, & when\ u+v=0  \\
            0,   & ow.          \\
        \end{array}
\right.
\end{equation}
\end{aligned}
$$
$u+v=0$ 当且仅当 $u=v$，于是正交。



定义 Radon 变换：

给定 $\mathbb{Z_2^n}$ 的子集 $\Gamma$ 和函数 $f\in\mathcal{V}$，定义一个新函数 $\Phi_{\Gamma}f\in \mathcal{V}$ 为：
$$
\Phi_{\Gamma}f(v) = \sum_{w\in\Gamma}f(v+w)
$$
函数 $\Phi_{\Gamma}f$ 就称为 $f$（在群 $\mathbb{Z_2^n}$ 上关于其子集 $\Gamma$）的（离散或有限）Radon 变换。

易知 $\Phi_{\Gamma}:\mathcal{V}\to\mathcal{V}$ 是一个线性变换。

<a id="2.2"></a>

### 2.2 定理

$\Phi_{\Gamma}$ 的特征向量是函数组 $\mathcal{X_u}$，其中 $u\in\mathbb{Z_2^n}$，对应于 $\mathcal{X_u}$ 的特征值 $\lambda_u$（即 $\Phi_{\Gamma}\mathcal{X_u} = \lambda_u\mathcal{X_u}$）是
$$
\lambda_u = \sum_{w\in\Gamma} (-1)^{u \cdot w}
$$
**Proof**:
$$
\begin{aligned}
\Phi_{\Gamma}\mathcal{X_u}(v)
& = \sum_{w\in\Gamma} \mathcal{X_u}(v+w)     \\
& = \sum_{w\in\Gamma} (-1)^{u \cdot (v+w)}   \\
& = (\sum_{w\in\Gamma} (-1)^{u \cdot w}) \mathcal{X_u}(v) \\
\end{aligned}
$$
因此
$$
\Phi_{\Gamma}\mathcal{X_u} = (\sum_{w\in\Gamma} (-1)^{u \cdot w}) \mathcal{X_u}
$$
注意到 $\Phi_{\Gamma}$ 的特征向量 $\mathcal{X_u}$ 与 $\Gamma$ 无关；仅特征值依赖于 $\Gamma$.



现在可以得到主要结果了。设 $\Delta = \{\delta_1,...\delta_n\}$，其中 $\delta_i$ 是第 $i$ 个单位坐标向量。$\delta_i$ 的第 $j$ 个坐标正好是 $\delta_{ij}$（Kronecker delta）. 用 $[\Phi_\Delta]$ 表示线性变换 $\Phi_\Delta:\mathcal{V}\to\mathcal{V}$ （关于由(2.2)给出的 $\mathcal{V}$ 的基 $B_1$）的矩阵。

<a id="2.3"></a>

### 2.3 引理

若 $\pmb{A}(C_n)$ 为 $n$ 维立方体的邻接矩阵，则 $[\Phi_\Delta] = \pmb A(C_n)$.

**Proof**:
$$
\begin{aligned}
\Phi_\Delta f_u(v)
& = \sum_{w\in\Delta} f_u(v+w) \\
& = \sum_{w\in\Delta} f_{u+w}(v)
\end{aligned}
$$
上式是因为 $u=v+w$ 当且仅当 $u+w=v$（*没看懂？？？*）. 因此
$$
\Phi_\Delta f_u = \sum_{w\in\Delta} f_{u+w}
\tag{2.4}
$$
于是
$$
\begin{aligned}
(\Phi_\Delta)_{uv} =
\begin{equation}
\left\{
	\begin{array}{}
		1,\quad if\ u+v\in\Delta \\
		0,\quad ow.              \\
	\end{array}
\right.
\end{equation}
\end{aligned}
$$
$u+v\in\Delta$ 当且仅当 $u$ 和 $v$ 仅有一个坐标不同。这正好是 $uv$ 作为 $C_n$ 的边的条件，证毕。



<a id="2.4"></a>

### 2.4 推论

$\pmb A(C_n)$ 的特征向量（看作是 $C_n$ 的顶点的线性组合）$E_u$（$u\in\mathbb{Z_2^n}$）由下式给出
$$
\begin{equation}\label{2.5}
E_u = \sum_{v\in\mathbb{Z_2^n}} (-1)^{u \cdot v} v
\end{equation}
\tag{2.5}
$$
对应于 $E_u$ 的特征值是
$$
\begin{equation}\label{2.6}
\lambda_u = n - 2\omega(n)
\end{equation}
\tag{2.6}
$$
其中 $\omega(n)$ 是 $u$ 中 $1$ 的数目（即 Hamming权），因此 $\pmb A(C_n)$ 有 $\binom{n}{i}$ 个特征值等于 $n-2i$.

**Proof**:

根据 $\eqref{2.3}$ 对 $\forall g\in\mathcal{V}$
$$
g = \sum_\mathcal{V} g(v) f_v
$$

对 $g = \mathcal{X_u}$ 应用上式，可得
$$
\begin{equation}\label{2.7}
\mathcal{X_u} = \sum_v \mathcal{X_u}(v) f_v = \sum_v (-1)^{u \cdot v} f_v
\end{equation}
\tag{2.7}
$$


根据[定理2.2](#2.2)，对应于 $\Phi_\Delta$ 的特征向量 $\mathcal{X_u}$（或等价地，$\pmb A(C_n)$ 的特征向量 $E_u$）的特征值为
$$
\begin{equation}\label{2.8}
\lambda_u = \sum_{w\in\Delta} (-1)^{u \cdot w} = n - 2\omega(n)
\end{equation}
\tag{2.8}
$$
至此，我们得到了计算 $C_n$ 中游动条数所需的全部信息。


<a id="2.5"></a>

### 2.5 推论

设 $u,v\in\mathbb{Z_2^n}$，且 $\omega(u+v)=k$（即 $u$ 和 $v$ 恰有 $k$ 个坐标不同）。则在 $C_n$ 中 $u$ 与 $v$ 之间长为 $l$ 的游动的条数为

$$
\begin{equation}\label{2.9}
(\pmb A^l)_{uv} = \frac{1}{2^n} \sum_{i=0}^n \sum_{j=0}^k (-1)^j \binom{k}{j} \binom{n-k}{i-j} (n-2i)^l
\end{equation}
\tag{2.9}
$$
注意到如果 $j \gt i$，则 $\binom{n-k}{i-j} = 0$

特别地，
$$
\begin{equation}\label{2.10}
(\pmb A^l)_{uu} = \frac{1}{2^n} \sum_{i=0}^n \binom{n}{i} (n-2i)^l
\end{equation}
\tag{2.10}
$$
**Proof**:

令 $E_u^{'} = \frac{1}{2^{\frac{n}{2}}} E_u$ 得到一组标准正交基，根据[推论1.2](#1.2)，可得
$$
(\pmb A^l)_{uv} = \frac{1}{2^n} \sum_{w\in\mathbb{Z_2^n}} E_{uw} E_{vw} \lambda_w^l
$$
$E_{uw}$ 是展开式 $\eqref{2.5}$ 中 $f_w$ 的系数，即 $E_{uw} = (-1)^{u \cdot w}$，而 $\lambda_w = n-2\omega(w)$. 因此
$$
\begin{equation}\label{2.11}
(\pmb A^l)_{uv} = \frac{1}{2^n} \sum_{w\in\mathbb{Z_2^n}} (-1)^{(u+v) \cdot w} (n - 2\omega(n))^l
\end{equation}
\tag{2.11}
$$

$\omega(w) = i$，且与 $u+v$ 有 $j$ 个公共 $1$ 的向量 $w$ 的个数为 $\binom{k}{j}\binom{n-k}{i-j}$，$\eqref{2.11}$ 就化简为 $\eqref{2.9}$，证毕。





<a id="3"></a>

# Ch03 随机游动



