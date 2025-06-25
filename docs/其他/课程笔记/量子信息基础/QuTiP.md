# QuTiP

## Python

- 常用库：

```python
import matplotlib.pyplot as plt
import numpy as np
from scipy.special import factorial
```

- 常用语句示例：

```python
n = 10
fib = [0, 1]
for i in range(2, n):
    fib.append(fib[i - 1] + fib[i - 2])
print(fib)

def sine_series(x, N):
    ans = 0.0
    for k in range(N + 1):
        ans = ans + (-1) ** k * x ** (1 + 2 * k) / factorial(1 + 2 * k)
    return ans
result = sine_series(x, N)
print("Series approximation:", result)
print("Error:", np.sin(x) - result)

xc, yc = random_coordinates(N)
max_dist, pnt1, pnt2 = max_dist(xc, yc)
plt.plot(xc, yc, "ro", markersize=8)
plt.plot(pnt1, pnt2, "b-", lw=2)
plt.show()
```

- NumPy:

```python
a = np.array([2.0, 4.0, 8.0, 16.0])
b = np.array([0, 1 + 0j, 1 + 1j, 2 - 2j])
c = np.array(["a", "b", "c", "d"])
output = [n for n in range(10)]
np.array(output)
np.linspace(-5, 5, 15)
np.array([1.0, 1 + 1j, "hello"])  # array data is upcast to strings

a = np.linspace(-10, 10, 20)
print(a[a <= -5]) # conditional statements return another array of boolean values
```

- Matplotlib:

```python
plt.plot(x, np.sin(x), "^", ms=8, color="0.8")
plt.xlim([-1, 1])  # change bounds on x-axis to [-1,1]
plt.ylim([-0.75, 0.75])  # change bounds on y-axis to [-0.75,0.75]
plt.show()

plt.figure(figsize=(12, 3))
plt.subplot(1, 3, 1)
plt.plot(x, np.sin(x), lw=2)
plt.subplot(1, 3, 2)
plt.plot(x, np.sin(x), lw=2, color="#740007")
plt.xlim([-np.pi, np.pi])
plt.subplot(1, 3, 3)

plt.savefig(
    "axes_example.png"
)  # Save the figure in PNG format in same directory as script
```

## Lecture 0 - Introduction to QuTiP

```python
from qutip import (Qobj, about, basis, coherent, coherent_dm, create, destroy,
                   expect, fock, fock_dm, mesolve, qeye, sigmax, sigmay,
                   sigmaz, tensor, thermal_dm)
```

## Lab 2 光子偏振

### 基础

- 用 `Qobj` 来表示矩阵或向量，来表示光子的偏振态
- `basis(N, i)` 表示 $N$ 维空间中的第 $i$ 个基矢量

```python title="圆偏振光"
from qutip import basis
import numpy as np
H = basis(2, 0)  # 水平偏振
V = basis(2, 1)  # 垂直偏振
R = (H - 1.j * V).unit()  # 右旋偏振
R = (H - 1.j * V) / np.sqrt(2)  # 右旋偏振
```

- 要得到一个态的共轭转置（或者说是对偶），可以使用 `.dag()` 方法。计算内积也可以直接使用 `.overlap()`。下面计算 $\braket{H|R}$

```python title="内积"
print (H.diag() * R)
H.overlap(R)
```

```python title="投影算子"
Ph = H * H.dag()
print(ph * R)
```

- 测量概率，就是求概率振幅的平方，而概率振幅就是两个态的内积

```python
def probability_amplitude(s1, s2):
    return s1.unit().overlap(s2.unit())

def measurement_probability(s1, s2):
    return abs(probability_amplitude(s1.unit(), s2.unit()))**2

print(probability_amplitude(V, R))
print(measurement_probability(V, R))
```

### 量子密钥分发

#### 理论

- 基本的密码学知识
    - 加密就是一个函数，$f(m)$，只有双方知道。但多次使用后，第三方可以通过统计分析得到 $f$ 的信息。
    - 经典的解决办法是：用一个经常变动的密钥进行加密 $f(m, k)$，但这个密钥也可能会被第三方分析得到。
    - 量子的解决办法是：第三方如果观测量子态，就会改变它的状态，这样就会被发现。上面的密钥使用量子分发。
- BB84 协议
    - 个人认为这一协议的核心在于：用相同的基测偏振光，才能得到必定一致的结果。
    - 两个信道：一个是量子信道，一个是经典信道
    - 第一步 量子信道：
        - 发送：Alice 想向 Bob 传递一串比特。对于每一位比特，她随机用对角基或水平基编码，把光子传给 Bob。
        - 接收：Bob 也随机选择对角基或水平基测量光子。
        - 结果：显然，如果 Bob 选对了基，那么就能得到正确的比特。但如果 Bob 选错了基，那么他得到的结果就是随机的。
    - 第二步 经典信道：
        - 发送：Bob 告诉 Alice 他所用的基
        - 回复：Alice 告诉 Bob 哪些基是错的
        - 结果：现在，双方都知道哪些比特传错了。丢弃掉这些比特，剩下的就是密钥，称为 **Shifted Key**。
    - 检测 Eve 的存在：
        - Eve 也只能猜，随机测量光子。她的测量会随机改变光子的状态。因此，**即使 Bob 使用了正确的基，也可能得到错误结果**。
        - 为了检测 Eve 的存在，Alice 和 Bob 会一起随机核对一部分比特。如果有错误，就说明有人监听了。

        !!! bug "Eve 有机会逃逸！"

            Eve 可能选择到正确的基。即使选择了错误的基，Bob 也可能测得正确的结果。只不过这概率很小：如果 Eve 检测了 $n$ 个比特，她逃逸成功的概率为 $\frac34^n$。

        !!! bug "存在中间人攻击！"

    - 缺陷：测量仪器、信道噪声、不可靠的光源……

#### 编程

让我们看看样例代码是怎么实现一次 BB84 协议的。

```python title="BB84 协议"
# 模式和基的准备
basisHV = [H, V] # 水平基的 0 和 1
basisPM45 = [P45, M45] # 对角基的 0 和 1
modes = [basisHV, basisPM45] # 水平还是对角基？
# 第一次：Alice 编码
Alice_bits = bit_generate(nbits)
for i in range(len(Alice_bits)):
    mode = mode_select()
    Alice_modes.append(mode)
    Alice_prepared.append(Photon(modes[mode][Alice_bits[i]])) # 选择基，编码生成光子
# 第一次：Bob 解码
for p in Bob_received:
    mode = mode_select()
    Bob_modes.append(mode)
    Bob_bits.append(p.measure(modes[mode]))
# 第二次：Alice 和 Bob 交流，互换 modes 和 bits
keys = []
for am,bm,ab,bb in zip(Alice_modes,Bob_modes,Alice_bits,Bob_bits):
    if (am == bm): # compared modes through a classical channel
        assert ab == bb
        keys.append(ab)
```

接下来是我们的编程任务：

- 中间人攻击：
    - 方法：Eve 拦截 Alice 发送给 Bob 的光子，将测量结果重新发送给 Bob。
    - 概率：测量时，Eve 有 50% 的概率选择正确的基，并将正确的结果发送给 Bob。如果选择错误了，她发送错误基的光子，如果 Bob 选对了基，测对的结果为 50%。总的来说，这些比特中有 25% 会出现错误。
    - 结果：当 Alice 和 Bob 交流基的信息时，Eve 也会知道，但这太晚了。
    - 编程任务：展示 Alice 和 Bob 的密钥中有 25% 的比特不一致。


## Appendix 术语表和数学知识

| Eng | 中文 |
| - | - |
| Bra-ket(Dirac) notation | 狄拉克符号 |

- 狄拉克符号：
    - $\ket{v}$ 表示向量（量子系统的状态）
    - $\bra{f}$ 表示线性泛函
    - 任何一个态都可以写为基在复数空间上的线性组合 $\ket{\phi} = \alpha\ket0 + \beta e^{i\theta}\ket1$
    - 内积：$\braket{a|b} = \braket{b|a}^*$

> 在量子力学中使用狄拉克符号因为这些向量与一般的向量所在的空间有很大不同。一般的向量在普通的三维空间，而量子力学中的向量在更加抽象、复杂的希尔伯特空间，比如所有可能的波函数组成的无限维空间。

!!! note "回顾知识：线性映射"

    - 线性泛函：从向量空间到标量域的映射
    - 对偶空间 $V'=L(V, F)$：向量空间上所有线性泛函构成
    - 对偶基：一组线性泛函，将向量空间中的每个基分别映射到 $1$
    - 对偶映射：对 $T\in L(V,W)$，对偶映射是7. $T'\in L(W', V')$

- 复数

- 欧拉公式：$e^{ix} = \cos x + i\sin x$
    - 其实平常使用复指数我们都只关心展开后的实部，这可以看作简单表示三角函数。把指数上面除 $i$ 以外的部分套进 $\cos$ 函数即可。
    - 更进一步的理解是：$\cos$ 和 $\sin$ 是线性独立的，它们一起构成一个二维向量空间。

- 张量积 $\otime$ 两个向量空间形成一个更大的向量空间，用于生成复合系统。
    - 从线性代数角度来看，张量积是一种双线性运算。在线性代数中，外积就是张量积。
    - 张量积的秩是所需指标数，维数是两个空间维数相乘
    - 对于矩阵，通常称为克罗内科积
