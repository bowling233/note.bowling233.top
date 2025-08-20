# LLM Infra

LLM Infra（大型语言模型基础设施）是支撑大模型训练、推理及部署的全栈技术体系，核心目标是高效利用硬件资源解决**大模型的计算、存储与通信瓶颈**。

## 概述

### 技术内涵：分布式并行与资源调度

LLM Infra 的核心是通过分布式技术将大模型拆解到多设备（GPU/TPU）上运行，主要包含以下关键技术：

#### 1. 并行策略（解决模型/数据切分问题）

- **张量并行（TP）**：将单个矩阵运算（如矩阵乘法）拆分到多卡，例如将 Transformer 层的注意力头分散在不同 GPU 上计算。
- **流水线并行（PP）**：按模型层切分（如将 Transformer 的 24 层分到 4 台机器，每台负责 6 层），通过微批次（micro-batch）减少设备空闲时间。
- **数据并行（DP）**：复制多份模型，每份处理不同数据子集，梯度聚合后同步更新（ZeRO 优化可减少显存冗余）。
- **专家并行（MoE）**：动态路由 token 到不同专家模块（如 Mixtral 模型），需专用负载均衡策略（如 MoE Router）。

#### 2. 显存与通信优化（解决资源瓶颈）

- **KV 缓存管理**：推理时缓存历史 Key/Value 向量加速解码，需分页存储（如 vLLM 的 PagedAttention）或 RDMA 传输。
- **混合精度训练**：用 FP16 计算前向/反向传播，FP32 更新参数，结合梯度缩放减少显存占用。
- **高性能通信库**：依赖 NCCL（NVIDIA 集体通信库）或 RDMA（远程直接内存访问）实现多机高速同步。

#### 3. 推理专用优化

- **PD 分离（Prefill-Decode 分离）**：拆解推理为预填充（计算密集型）和解码（内存密集型）两阶段，分别调度到不同设备提升吞吐。
- **连续批处理（Continuous Batching）**：动态合并不同长度的请求，提高 GPU 利用率。

### 核心挑战：Scaling, Sharding, Copying

LLM Infra 的本质是解决分布式系统的经典问题在新场景下的演化：

- **Scaling（扩展性）**  
    如何支持千亿参数模型和每秒数万次推理请求？  
    → 通过 3D 并行（TP+PP+DP）横向扩展算力，结合模型量化（INT8/INT4）压缩显存。

- **Sharding（分片）**  
    如何切分模型状态（参数/KV 缓存）？  
    → 张量并行切分计算图，KV Paging 切分缓存，类似数据库分片思想。

- **Copying（复制与同步）**  
    如何高效同步梯度或 KV 缓存？  
    → 使用 RDMA/NCCL 减少跨机通信延迟，ZeRO 技术避免全量参数复制。

### 行业定位：AI Infra vs. 传统 Infra

尽管技术栈不同（如用 vLLM 替代 Nginx，NCCL 替代 TCP），但底层问题本质一致：

- **传统 Infra 经验迁移**：  
    负载均衡 → MoE 路由，内存分页 → KV 缓存管理，数据库分片 → 模型并行。
- **新场景的特殊性**：  
    LLM 请求有状态（需维护 Session 上下文），延迟敏感（首 Token 延迟 TTFT 是关键指标），硬件依赖强（GPU 显存带宽成瓶颈）。

### LLM Infra 的技术栈全景

| 层级         | 核心组件/代表工具或框架                       |
| --------- | -------------------------------------------- |
| 硬件层       | GPU 集群（A100/H100）、RDMA 网络、存储系统<br>NVIDIA DGX、昇腾 910 |
| 分布式框架   | 并行策略调度、通信优化、显存管理<br>DeepSpeed、Megatron-LM、Alpa |
| 推理引擎     | 批处理、缓存管理、多节点部署<br>vLLM、TensorRT-LLM、sglang |
| 部署工具     | 容器化、API 服务、监控<br>Ray、Kubernetes、FastAPI |
| 优化工具     | 量化（FP16/INT4）、编译优化（CUDA 内核）<br>TensorRT、ONNX Runtime、FlashAttention |

### 为什么 LLM Infra 成为高薪方向？

- **技术壁垒高**：需同时掌握分布式系统、硬件架构、深度学习框架（如 PyTorch）。
- **业务价值直接**：优化推理速度（Token 延迟）可降低企业 GPU 成本（如 10% 优化 = 百万美元级节省）。
- **生态快速演进**：从训练框架（DeepSpeed）到推理引擎（vLLM）持续迭代，需持续学习。

## LLM 知识储备

### 生成式 AI 与自回归

**生成式 AI**：学习已有数据的分布规律，自动生成符合数据特征的新内容。

**自回归（Autoregressive）**指模型基于已生成的历史序列逐步预测下一个词（Token）的生成方式。其核心特点是当前输出的生成严格依赖于之前所有输出，形成链式依赖关系。

“自回归”一词来自数学中的**自回归模型（Autoregressive Model，AR）**，定义为：

$$
X_t = c + \phi_1 X_{t-1} + \phi_2 X_{t-2} + \cdots + \phi_p X_{t-p} + \epsilon_t
$$

- $X_t$ 是当前时刻的值；
- $\phi_i$ 是历史数据的权重系数；
- $\epsilon_t$ 是随机噪声（如高斯分布）

在 LLM 中，这一概念演变为：$P(\text{Token}_t | \text{Token}_1, \text{Token}_2, \ldots, \text{Token}_{t-1})$ 即第 $t$ 个 Token 的概率分布完全由前 $t-1$ 个 Token 决定，模型通过**迭代采样**逐步生成完整序列。

!!! note "迭代采样"

    - 步骤 1：基于当前 KV Cache，模型计算下一个 Token 的概率分布（如通过 Softmax）。
    - 步骤 2：从分布中采样一个 Token（例如选择概率最高的 Token）。
    - 步骤 3：将新 Token 加入序列，并更新 KV Cache（将新 Token 的 K/V 向量追加到缓存中）。
    - 重复：重复上述过程直至生成结束符（如 `<EOS>`）或达到最大长度。

!!! note "自回归的内存密集分析"

    - KV Cache 的显存占用：每个 Token 需存储其对应的 Key 和 Value 向量（维度通常为 d_model，如 4096 维）。生成 N 个 Token 需缓存 O(N \times d_model) 的数据量，长序列下显存压力显著。
    - 内存带宽瓶颈：每一步需读取整个 KV Cache 计算注意力（如 $Q \cdot K^T$），高频访问导致内存带宽成为瓶颈（尤其在大模型多机场景）。

!!! note "自回归的挑战与优化方向"

    1. 计算效率问题：

        - 生成长度为 N 的序列需 N 次串行前向计算，延迟高（如 1000 Token 需 1000 步）。
        - 优化手段：分块生成（如 vLLM 的 PagedAttention）、稀疏注意力（如 Longformer）。

    2. 误差累积（Exposure Bias）：

        - 训练时使用真实历史数据（Teacher Forcing），推理时依赖模型自身生成的历史，错误会逐步放大。
        - 优化手段：计划采样（Scheduled Sampling）、强化学习微调。

    3. 长程依赖建模：

        - 随着序列增长，模型可能遗忘早期信息（如对话开头）。
        - 优化手段：Transformer-XL 的循环记忆机制、位置编码改进（如 RoPE）。

### Transformer 和 Attention

!!! note "掩码注意力（Masked Self-Attention）"

### MoE

### 推理

分为两个阶段：

- Prefill（预填充）：处理输入 Prompt，生成 KV Cache（计算密集型）。
- Decode（解码）：基于 KV Cache 自回归生成 Toekn（内存密集型）。
    - 一次推理只输出一个 Token，与之前的 Token 拼接作为下次的输入。

## 推理框架

### vLLM

### sgLang

## 优化技术

### 并行策略

#### EP

专家并行（Expert Parallelism, EP）：将模型中的专家（Expert）分布到不同设备（GPU）上。

Token 路由：每个输入 Token 会被动态分配到最相关的专家（如 top-1 或 top-2）。

两个阶段：

- Dispatch（调度阶段）：将输入 Token 分发给对应的专家。
    - 路由计算：根据 Token 特征计算其与所有专家的匹配分数。
    - Top-K 选择：选出分数最高的 K 个专家（如 top-2）。
    - 跨设备发送：若目标专家在其他机器，需通过 RDMA/NVLink 发送 Token 数据。
- Combine（组合阶段）：聚合专家计算结果并输出。
    - 专家计算：各专家处理接收到的 Token。
    - 结果回传：将计算结果发回 Token 原属设备（可能跨机）。
    - 加权融合：按路由分数加权求和多个专家的输出。

### KV Cache

仅编码器模型的 Self Attention 中带 Masked，因此，在推理的时候，前面已经生成的 Token 不需要与后面的 Token 产生 Attention，从而使得前面已经计算的 K 和 V 可以缓存起来。因此，KV Cache 应运而生。

KV Cache 是一种典型的以空间换时间（或者叫以内存换计算）的优化技术提升推理速度从而降低延迟。

### ZeRO

### 批处理与迭代调度

- 原始论文：[Orca: A Distributed Serving System for Transformer-Based Generative Models | USENIX](https://www.usenix.org/conference/osdi22/presentation/yu)
- 相关博客：
    - [大模型推理服务调度优化技术-Continuous batching - 知乎](https://zhuanlan.zhihu.com/p/719610083)

批处理：

- 单处理：不组成 Batch，将单个 Prompt 传入 LLM 直接进行推理。虽然简单灵活，但 GPU 资源利用率低。
- 静态批处理：Client 侧多个 Prompt 打包为一个 Request 进行批处理，所有请求都完成才释放资源。**Batch 大小在推理完成前不变**。
- 动态批处理：Server 侧收集 Request 打包，动态创建 Batch。

核心观点：

- 传统方式使用**静态批处理**。但不同请求的生成长度不同，导致计算资源闲置，并增加新请求的排队时间。
-

### PD 分离

- [ ] 原始论文：[DistServe: Disaggregating Prefill and Decoding for Goodput-optimized Large Language Model Serving](https://arxiv.org/abs/2401.09670)
- [x] 原始博客：[Throughput is Not All You Need: Maximizing Goodput in LLM Serving using Prefill-Decode Disaggregation | Hao AI Lab @ UCSD](https://hao-ai-lab.github.io/blogs/distserve/)

核心观点：

- 吞吐（throughput）已经不再能准确衡量大模型的服务质量。

    !!! exmample

        - 聊天机器人（chatbot）需要快速的初始回复，而后续的生成只需要和人类阅读速度相当即可。
        - 代码补全需要快速的端到端响应

- 提出新的指标 Goodput，指符合服务等级目标（Service Level Objectives，SLO）的每秒完成的请求数。

博客要点：

- LLM 中常见的 SLO：
    - Time To First Token latency, TTFT
    - Time Per Output Token
- 由于 Prefill 和 Decode 的计算特征差异，将其并行会造成串扰
- 将 Prefill 和 Decode 分布在不同的 GPU 上，Prefill 完成后将 KV Cache 迁移到 Decode 卡上，请求结束后立刻离开
    - Prefill 和 Decode 可以根据计算特征各自选择最优的并行策略
    - 布局合理时，迁移耗时比 Decode 步长小
