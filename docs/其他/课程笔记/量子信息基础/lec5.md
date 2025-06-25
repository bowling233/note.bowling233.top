# 量子系统随时间的演化

本节课在线性代数上加强了难度。让我们再次复习相关概念。

???+ note "Hermitian Operators"

    定义：
    
    - 自伴（共轭转置等于本身）：$A^\dagger = A$。
        - 伴随矩阵 $A^\dagger$ 定义为：$\braket{A^\dagger|g}= \braket{f|Ag}$。

    性质：

    - 本征值都是实数。
        - 位置、动量和能量等物理量是实数，因为它们都是 Hermitian 算子的本征值。
        - 因此期待值也是实数。
            - 结合内积的共轭对称性质，可以得到：$\braket{A} = \braket{\psi|A\psi} = \braket{A\psi|\psi} = \braket{\psi|A\psi}^* = \braket{A}^*$。
    - 本征向量正交。
    
