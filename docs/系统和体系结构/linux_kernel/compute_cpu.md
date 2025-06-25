# 计算：CPU

## policy

https://docs.kernel.org/admin-guide/pm/cpufreq.html

https://manpages.debian.org/experimental/linux-cpupower/x86_energy_perf_policy.8.en.html

<<<AI 生成，需检查

从架构的角度来看，Intel 处理器的能效与性能策略管理涉及多个层次的控制机制：

- P 状态（Performance States）：定义处理器在运行时的不同频率和电压。较低的 P 状态通常意味着更低的频率和电压，从而降低功耗。
- C 状态（C-States）：定义处理器在空闲时可以进入的不同级别的低功耗状态。每个 C 状态都代表一个不同的待机级别，C0 表示全速运行，C1、C2、C3 等则表示逐渐更低的功耗状态。

- MSR（Model-Specific Registers）：这些是 Intel 处理器提供的专用寄存器，用于直接访问和配置硬件的各项设置。x86_energy_perf_policy 就是通过读写 MSR 来配置 EPB 和 HWP 相关设置。
- EPB（Energy Performance Bias）：该字段告诉硬件偏向于哪一类策略，影响 P 状态的选择。EPB 影响着 CPU 如何选择合适的 P 状态，是否偏向于降低功耗，还是保持较高的性能。
- HWP（Hardware P-States）：这是一个更精细的控制机制，它将硬件对 CPU 频率的调节进一步细化。HWP 使得硬件根据负载动态调节频率，从而优化功耗和性能之间的平衡。操作系统不再直接控制 P 状态，而是通过设置目标频率范围，让硬件自主调整。

>>>AI 生成，需检查

## Intel

### Uncore Performance Monitoring

Intel Uncore Performance Monitoring
