# 性能监控与分析

性能的监控与分析可以分为很多层面，比如应用层面会考虑监控应用特定部分代码执行期间的性能指标。本文不考虑这一场景，而是考虑系统级别的性能监控，即运行一个类似 [intel/pcm: Intel® Performance Counter Monitor (Intel® PCM)](https://github.com/intel/pcm) 的守护进程。

本文我们将学习 PAPI、Linux Perf 子系统、Intel PCM 和 opentelemetry-ebpf，通过代码实践了解如何监控微体系结构级别的性能指标。

## PAPI



## Linux Perf

!!! quote

    - [perf: Linux profiling with performance counters](https://perfwiki.github.io/main/)
    - [Wiki - Linux 效能分析工具：Perf](https://wiki.csie.ncku.edu.tw/embedded/perf-tutorial)

Perf 有多种运行模式，比如 per-thread、per-process、per-cpu 和 system-wide，这里考虑 system-wide。

https://www.intel.com/content/www/us/en/developer/articles/technical/timed-process-event-based-sampling-tpebs.html

## Intel PCM


## OpenTelemetry eBPF

前置条件：掌握 eBPF 的基本概念和编程方法。

---

Great! As a kernel developer learning Go and eBPF for an OpenTelemetry collector, here are the key source code areas you should focus on in this project:

## **1. eBPF Program Architecture & Loading (Critical for your use case)**

The project uses **libbpf** with CO-RE (Compile Once, Run Everywhere). Start with these files:

**Core eBPF Program:**
- ```c name=collector/kernel/bpf_src/render_bpf.c url=https://github.com/open-telemetry/opentelemetry-network/blob/6b7a3e998ba3d5c6b259b6e1eb77c9c3c5952dc1/collector/kernel/bpf_src/render_bpf.c#L1-L25```
  - Main eBPF program with global configuration variables
  - Shows how to use vmlinux.h for BTF/CO-RE support
  - Includes networking macros and kernel version handling

**Go-style C++ eBPF Management (you can adapt this pattern to pure Go):**
- ```c++ name=collector/kernel/probe_handler.cc url=https://github.com/open-telemetry/opentelemetry-network/blob/6b7a3e998ba3d5c6b259b6e1eb77c9c3c5952dc1/collector/kernel/probe_handler.cc#L166-L189```
  - Shows how to load eBPF skeleton using libbpf
  - Pattern: `open_bpf_skeleton()` → `configure_bpf_skeleton()` → `load_bpf_skeleton()`
  - Getting map file descriptors from loaded programs

- ```c++ name=collector/kernel/bpf_handler.cc url=https://github.com/open-telemetry/opentelemetry-network/blob/6b7a3e998ba3d5c6b259b6e1eb77c9c3c5952dc1/collector/kernel/bpf_handler.cc#L14-L50```
  - Complete lifecycle: open → configure → load → attach probes
  - Configuring global eBPF variables before loading

## **2. Perf Ring Buffer Communication (Essential for PMU data collection)**

**C-based Perf Ring Implementation:**
- ```c name=util/perf_ring.h url=https://github.com/open-telemetry/opentelemetry-network/blob/6b7a3e998ba3d5c6b259b6e1eb77c9c3c5952dc1/util/perf_ring.h#L1-L67```
  - Data structure for perf event communication
  - Memory layout with `perf_event_mmap_page`

- ```c name=util/perf_ring.c url=https://github.com/open-telemetry/opentelemetry-network/blob/6b7a3e998ba3d5c6b259b6e1eb77c9c3c5952dc1/util/perf_ring.c#L148-L207```
  - Reading from perf rings: `pr_peek()`, `pr_read()`, `pr_pop()`
  - Handling ring buffer wrap-around

**C++ Wrapper (study this pattern for Go):**
- ```c++ name=util/perf_ring_cpp.h url=https://github.com/open-telemetry/opentelemetry-network/blob/6b7a3e998ba3d5c6b259b6e1eb77c9c3c5952dc1/util/perf_ring_cpp.h#L312-L377```
  - Object-oriented wrapper around C perf ring
  - Methods: `peek()`, `read()`, `pop()`, `bytes_remaining()`

**Multiple CPU Ring Management:**
- ```c++ name=collector/kernel/perf_reader.cc url=https://github.com/open-telemetry/opentelemetry-network/blob/6b7a3e998ba3d5c6b259b6e1eb77c9c3c5952dc1/collector/kernel/perf_reader.cc#L20-L47```
  - Managing per-CPU perf rings
  - Setting callbacks for asynchronous event handling

## **3. Attaching eBPF Probes (kprobes, kretprobes, tail calls)**

- ```c++ name=collector/kernel/bpf_handler.cc url=https://github.com/open-telemetry/opentelemetry-network/blob/6b7a3e998ba3d5c6b259b6e1eb77c9c3c5952dc1/collector/kernel/bpf_handler.cc#L216-L228```
  - Attaching kprobes to kernel functions
  - Registering tail calls for complex eBPF programs

- ```c++ name=collector/kernel/probe_handler.cc url=https://github.com/open-telemetry/opentelemetry-network/blob/6b7a3e998ba3d5c6b259b6e1eb77c9c3c5952dc1/collector/kernel/probe_handler.cc#L254-L272```
  - Tail call registration into BPF prog arrays
  - Error handling for probe attachment

## **4. OpenTelemetry Data Encoding & Export**

For your performance counter collector, you'll need to transform raw data to OpenTelemetry format:

**Encoder Pattern (Rust FFI, but shows wire format):**
- ```rust name=crates/render/ebpf_net/ingest/src/encoder.rs url=https://github.com/open-telemetry/opentelemetry-network/blob/6b7a3e998ba3d5c6b259b6e1eb77c9c3c5952dc1/crates/render/ebpf_net/ingest/src/encoder.rs#L3487-L3513```
  - Wire format encoding for telemetry
  - Timestamp + message struct serialization

**OTLP gRPC Export:**
- ```c++ name=reducer/otlp_grpc_formatter.cc url=https://github.com/open-telemetry/opentelemetry-network/blob/6b7a3e998ba3d5c6b259b6e1eb77c9c3c5952dc1/reducer/otlp_grpc_formatter.cc#L1-L121```
  - Formatting metrics for OpenTelemetry Protocol

## **5. Recommended Learning Path for Your PMU Collector**

### **Phase 1: Understand eBPF Loading**
1. Study `probe_handler.cc` - learn the skeleton open/configure/load pattern
2. Examine `bpf_handler.cc` constructor - see how to set global eBPF variables
3. Review `render_bpf.c` - understand eBPF program structure

### **Phase 2: Master Perf Buffers**
1. Read `util/perf_ring.h` and `.c` - fundamental ring buffer operations
2. Study `perf_reader.cc` - per-CPU ring management
3. Look at `buffered_poller.cc` (mentioned in searches) - polling and event processing

### **Phase 3: Apply to PMU Counters**
For micro-architecture performance counters, you'll specifically need:
- Attach to perf event file descriptors (similar to how this project attaches kprobes)
- Use `BPF_PERF_EVENT_ARRAY` maps to collect PMU data
- Read from perf rings in userspace (use the patterns in `perf_reader.cc`)

### **Phase 4: Export to OpenTelemetry**
1. Study the encoder patterns in `crates/render/ebpf_net/ingest/src/encoder.rs`
2. Check `otlp_grpc_formatter.cc` for OTLP export
3. Map PMU metrics to OpenTelemetry metric types (Counter, Gauge, Histogram)
