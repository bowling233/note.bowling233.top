# LLM

## 从 D2L 到 LLM

要点是：

MLP → 只能处理固定输入，缺乏序列建模能力

RNN/LSTM → 引入时序，但长程依赖差

Attention → 并行计算 + 全局依赖建模，核心突破

Transformer → 架构集成 Attention + FFN + 残差/归一化

Decoder-only Transformer → GPT 系列的核心结构

LLM → 在 Transformer 基础上加大规模 + 算力 + 对齐/推理优化，形成今天的大语言模型

## 阶段 1：复习 & 夯实基础（MLP → 序列建模）

* **MLP 局限性**

    * MLP 只能处理固定维度输入，缺乏处理序列数据的能力。
    * 复习一下为什么 RNN / CNN 曾经被用于序列建模。
* **重点**：理解 **为什么需要新架构**（上下文依赖、长程依赖问题）。

👉 学习方法

* 用 PyTorch 实现一个小 MLP 做分类任务，加深对“只能处理固定输入”的认识。
* 阅读 RNN/LSTM 的简单教程，理解它们解决什么问题，但为什么有局限。

---

## 阶段 2：Attention 机制（LLM 的核心思想）

* **核心动机**：RNN 难以建模长程依赖，Attention 提供了“全局依赖 + 并行计算”的方案。

* **关键知识点**

    * 点积注意力（Dot-Product Attention）
    * Q（Query）、K（Key）、V（Value）的定义与矩阵计算
    * Self-Attention（输入序列对自己做 Attention）
    * 多头注意力（Multi-Head Attention）的意义

* **重点**：

    * 数学公式会推导，但更要直观理解：**注意力就是相关性加权求和**。
    * 理解 Attention 的时间复杂度是 O(n²)，这也是 LLM 训练/推理的主要瓶颈。

👉 学习方法

* 推荐：*The Illustrated Transformer*（可视化讲解 Attention）。
* 自己手写实现一个 **Self-Attention 层**（不用库封装），输入几个向量看看输出效果。

---

## 阶段 3：Transformer 架构

* **基础结构**

    * Encoder/Decoder 结构（最初为机器翻译提出）
    * Decoder-only Transformer（GPT 系列 LLM 使用的简化版）

* **关键模块**

    * 位置编码（sinusoidal / RoPE）
    * 前馈网络（FFN / MLP）
    * 残差连接 + LayerNorm
    * 掩码机制（Mask Attention，用于语言建模）

* **重点**：

    * 搞懂 **解码器（Decoder-only）** 才是 GPT 类 LLM 的主干。
    * 位置编码为什么必须有，RoPE/ALiBi 之类改进解决什么问题。

👉 学习方法

* 阅读原始论文《Attention Is All You Need》但只抓主干图。
* 动手：用 PyTorch 实现一个 **迷你 Transformer**（几层、几百万参数），在小数据集上训练语言建模。

---

## 阶段 4：从 Transformer 到 LLM

* **Transformer → GPT（语言模型）**

    * 自回归训练目标（预测下一个 token）。
    * 大数据 + 大模型 + 大算力 → emergent abilities（涌现能力）。

* **LLM 的关键技术点**

    * 训练：大规模分布式（数据并行 / 模型并行）。
    * 推理：KV-Cache、连续批处理、量化。
    * 对齐：SFT（监督微调）、RLHF/DPO（人类反馈对齐）。

* **重点**：

    * Transformer 架构本身并不神奇，LLM 的关键是 **规模 + 算力 + 工程**。
    * 要明确区分 **架构（Transformer）** 与 **应用范式（LLM）**。

👉 学习方法

* 先看 **GPT-2 论文**（比 GPT-3 简单），理解解码式语言建模。
* 再看 **Chinchilla 论文**（数据量与模型大小的关系）。
* 了解 LLaMA 系列是如何在学术界/开源界复现 GPT 思路的。

---

# 📌 学习重点总结

1. **Attention**：这是 LLM 的灵魂，要透彻理解。
2. **Decoder-only Transformer**：这是 GPT 类 LLM 的核心架构。
3. **语言建模目标**：下一个 token 的预测，这点简单但核心。
4. **工程扩展**：LLM 的规模化训练 & 推理优化，是区别“会用 Transformer”和“会做 LLM”的分水岭。

---

# 🚀 学习方式建议

* **先概念后实现**：每学一个模块（Attention / Transformer），都手写一个小实现。
* **小步快跑**：从几百万参数的模型练手，再逐步理解大规模 LLM 的工程挑战。
* **边学边对比**：把 “原始 Transformer” 与 “LLM（GPT 系列）” 放在一起，思考差异。

---

要不要我帮你画一张 **从 MLP → Transformer → LLM 的知识关系图**，把关键概念和演进路线在一张图里理清？

