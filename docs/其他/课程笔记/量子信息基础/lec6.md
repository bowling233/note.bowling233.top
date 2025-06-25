# 量子电路

## 量子态的可复制性

我们希望从量子态中复制信息，可以想象有两种方式：

- 拷贝：输入一个 $\ket{\psi}$，输出 $\ket{\psi}\ket{\psi}=\alpha^2\ket{00} + \alpha\beta\ket{01} + \alpha\beta\ket{10} + \beta^2\ket{11}$
- 纠缠：输入一个 $\ket{\psi}$ 和另一个态如 $\ket{0}$，使它们纠缠在一起，输出 $\alpha\ket{00} + \beta\ket{11}$

上面两种情况用数学描述，就分别是复制和张量积。接下来我们证明，第一种情况是不可能的。

!!! note "不可克隆定理"

    无法完美复制一个未知的量子态。

<div class="grid cards" markdown>

-   __假设存在__

    ---

    设这个克隆器是 $U_c$ 作用在：

    - 输入态 $\ket{\psi}$
    - 将被复制的态 $\ket{0}$
    - 辅助态 $\ket{a_0}$

- __复制过程__

    ---

    尝试对两个非正交的态 $\ket{\psi}$ 和 $\ket{\phi}$ 进行复制：

    - $U_c(\ket{\psi}\ket{0}\ket{a_0}) = \ket{\psi}\ket{\psi}\ket{a_\psi}$
    - $U_c(\ket{\phi}\ket{0}\ket{a_0}) = \ket{\phi}\ket{\phi}\ket{a_\phi}$

- __矛盾__

    ---

    对上面两式做内积，得到：$\braket{\psi|\phi} = (\braket{\psi|\phi})^2\braket{a_\psi|a_\phi}$，即 $\braket{\psi|\phi} = \frac1{\braket{a_\psi|a_\phi}}$。右边分母 $\leq 1$，左边 $\geq 1$，得 $\braket{\psi|\phi} = 1$，与非正交的假设矛盾。

</div>

## 量子电路中的计算

在经典数字电路中我们使用一个 $n\rightarrow m$ 的函数 $f$，它将 $n$ 位输入映射到 $m$ 位输出。要在量子电路中实现它，我们要找到一个酉变换 $U_f$ 作用于 $n+m$ 个量子比特，使得：

$$
U_f\ket{x}\ket{y} = \ket{x}\ket{y\oplus f(x)}
$$

其中 $\oplus$ 是异或运算，这使得 $U_f$ 是一个可逆变换：

$$
U_fU_f(\ket{x}\ket{y}) = U_f(\ket{x}\ket{y\oplus f(x)}) = \ket{x}\ket{y\oplus f(x)\oplus f(x)} = \ket{x}\ket{y}
$$

并且当 $y=0$ 时，$U_f(\ket{x}\ket{0}) = \ket{x}\ket{f(x)}$。

### 量子门

| 函数 | 说明 | 矩阵 |
| --- | --- | --- |
| `snot()` | Hadamard门，将基态变为叠加态 | $\frac{1}{\sqrt{2}}\begin{bmatrix}1&1\\1&-1\end{bmatrix}$ |
| `rx(theta)` | 旋转门 $R_x(\theta)=e^{-i\frac{\theta}2X}$ | $\begin{bmatrix}\cos(\theta/2)&-i\sin(\theta/2)\\-i\sin(\theta/2)&\cos(\theta/2)\end{bmatrix}$ |
| `ry(theta)` | 旋转门 $R_y(\theta)=e^{-i\frac{\theta}2Y}$ | $\begin{bmatrix}\cos(\theta/2)&-\sin(\theta/2)\\\sin(\theta/2)&\cos(\theta/2)\end{bmatrix}$ |
| `rz(theta)` | 旋转门 $R_z(\theta)=e^{-i\frac{\theta}2Z}$ | $\begin{bmatrix}e^{-i\theta/2}&0\\0&e^{i\theta/2}\end{bmatrix}$ |
| `phasegate(theta)` | 相位门 $P(\theta)=\begin{bmatrix}1&0\\0&e^{i\theta}\end{bmatrix}$ | $\begin{bmatrix}1&0\\0&e^{i\theta}\end{bmatrix}$ |
| `cnot()` | CNOT门，控制位为第一个 | $\begin{bmatrix}1&0&0&0\\0&1&0&0\\0&0&0&1\\0&0&1&0\end{bmatrix}$ |
| `csign()` | CSIGN门 | $\begin{bmatrix}1&0&0&0\\0&1&0&0\\0&0&1&0\\0&0&0&-1\end{bmatrix}$ |

### 并行性

可以把量子比特看作是经典比特（基矢）的复合。对量子态进行 $U_f$ 变换时，相当于对每个基矢计算 $f(x)$，然后把结果叠加起来。这就是量子并行性。

但结果也是叠加态，测量后会坍缩，我们如何获得其中蕴含的结果呢？

