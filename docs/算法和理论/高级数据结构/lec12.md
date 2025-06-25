# Local Search 局部搜索

=== "Local Search"

    用于解决**最优化问题**的一种启发式算法。
    
    - 步骤：
        - 从一个初始解/可行解（Feasible solution，FS）开始，搜索解的邻域。
        - 如果有更优的解，移动到该解继续搜索。
    
    局部搜索容易陷入局部最优解。

=== "Metropolis–Hastings algorithm"

    这是一种马尔可夫链蒙特卡洛（Markov Chain Monte Carlo，MCMC）算法，使用马尔可夫链从随机分布中取样。

    - 步骤：
        - 初始化：选定初始状态 $x_0$，令 $t=0$；
        - 迭代：
            1. 生成：从某一容易抽样的分布 $Q(x'|x_t)$ 中随机生成候选状态 $x'$；[注 1]
            2. 计算：计算是否采纳候选状态的概率 $A(x'|x)=\min \left(1,{\frac {P(x')}{P(x)}}{\frac {Q(x|x')}{Q(x'|x)}}\right)$；
            3. 接受或拒绝：
                - 从 $[0,1]$ 的均匀分布中生成随机数 $u$；
                - 如 $u\leq A(x'|x)$，则接受该状态，并令 $x_{t+1}=x'$；
                - 如 $u>A(x'|x)$，则拒绝该状态，并令 $x_{t+1}=x_t$（复制原状态）；
            4. 增量：令 $t=t+1$；

=== "Simulated Annealing"

    模拟退火（Simulated Annealing，SA）基于 MH 算法，能够依概率收敛到全局最优解。

    ```
    s := s0; e := E(s)                  // 设定目前状态为 s0，其能量 E(s0)
    k := 0                              // 评估次数 k
    while k < kmax and e > emin         // 若还有时间（评估次数 k 还不到 kmax）且结果还不够好（能量 e 不够低）则：
        sn := neighbour(s)              // 随机选取一邻近状态 sn
        en := E(sn)                     // sn 的能量为 E(sn)
        if random() < P(e, en, temp(k/kmax)) // 决定是否移至邻近状态 sn
            s := sn; e := en            // 移至邻近状态 sn
        k := k + 1                      // 评估完成，次数 k 加一
    return s                            // 返回状态 s
    ```

## Vertex Cover Problem 顶点覆盖问题
  
??? question "问题描述"

    - 给定无向图 $G = (V, E)$
    - 顶点覆盖：$C \subseteq V$，对于每条边 $(u, v) \in E$，至少有一个端点在 $C$ 中
    - 最小顶点覆盖：$|C|$ 最小

??? tip "问题分析"

    - NP-hard
    - 无多项式时间算法
    - 近似算法：贪心算法

??? success "解法"

    === "贪心算法"
        
        - 初始解：$C = V$
        - 邻域：从 $C$ 中随机选择一个顶点 $v$，将 $v$ 从 $C$ 中移除

    === "Metropolis–Hastings 算法"

        - 初始解：$C = V$
        - 邻域：从 $C$ 的领域中随机选择一个覆盖 $C'$
        - 如果 $C'$ 更优，则接受 $C'$，否则以一定概率接受 $C'$

## Hopfield Network 霍普菲尔德网络

??? question "问题描述"

    - 二值 Hopfield 网络：$N$ 个神经元，每个神经元的状态为 $\oplus 1$
    - 边的权重： $w_{e}$：当 $w_{e} > 0$ 时，该边两端的神经元 $i$ 和 $j$ 状态应当不同；当 $w_{e} < 0$ 时，神经元 $i$ 和 $j$ 状态应当相同。权重的绝对值表示约束的强度。显然，当 $w_{ij}s_i s_j < 0$ 时，约束被满足。
    - 点的满足度：$E = \sum_{e} w_{e} s_i s_j$。当其 $\leq 0$ 时，约束被满足。
    - 网络的稳定性：当所有点的约束都被满足时，网络达到稳定状态。
    - 目标：找到一个稳定状态。

??? tip "问题分析"

??? success "解法"

    === "State-flipping 算法"

        - 初始解：随机生成一个状态
        - 邻域：随机选择一个未被满足的神经元，翻转其状态
        - 直到满足所有约束

        该算法在最多 $W=\sum_e |w_e|$ 步内收敛到一个稳定状态。

        ??? tip "证明"

## Maxium Cut Problem 最大割问题

??? question "问题描述"

    - 给定无向图 $G = (V, E)$
    - 割：将 $V$ 分为两个不相交的集合 $S$ 和 $T$，$S \cup T = V$
    - 割的大小：$|E(S, T)|$，其中 $E(S, T)$ 表示 $S$ 和 $T$ 之间的边
    - 最大割：割的大小最大
  
??? tip "问题分析"

    这是 Hopfield 网络的一个变种，其中 $w_{ij} = 1$。``````
