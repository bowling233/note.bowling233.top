# 多量子比特和量子纠缠

## 张量积

## 纠缠态和 EPR 佯谬

## Bell 态中的信息

- PPT：信息分布在两个量子比特之间（not local），任意单个量子比特不提供任何信息。

要理解上面这一描述，课上采用计算期望值 $\braket{L}$ 的方法。

1. $L$ 的特征值 $\lambda$，特征向量 $\ket{\lambda}$
2. 把量子态分解到特征向量上 $\ket{\psi} = \sum_\lambda \ket{\lambda}\braket{\lambda|\psi}$，再应用 $L$
3. 再测量，得到期待值 $\braket{L} = \sum_\lambda \lambda \mathrm{P}(\lambda)$

!!! note "牢记：$\mathrm{P}(\lambda) = \braket{\psi|\lambda}\braket{\lambda|\psi}$"

本质上就是带权（概率）的特征值求和。

测量 Bell 态 $\ket{\psi} = \frac1{\sqrt{2}}(\ket{01}+\ket{10})$ 的第一个比特，我们对第一个比特应用由泡利矩阵生成的任意酉矩阵：

$$
(\vec{\sigma}^{(1)} \cdot \vec{n}) \otimes I
$$

计算它的期望值：



## 密度矩阵
