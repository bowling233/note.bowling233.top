---
tags:
  - 个人笔记
---

# 数据库

!!! quote

    - [架构师之路，从「存储选型」起步 - 掘金](https://juejin.cn/post/7109647301370445832)
    - [数据库存储选型经验总结 - 阿里云开发者社区](https://developer.aliyun.com/article/870365)
    - [数据库分类详解：多样化的类型与应用场景 - 数字经济观察网](https://www.szw.org.cn/20230922/63447.html)

**数据库是大数据和分析领域的核心话题**。在课内，我们最多只接触过 MySQL 这样的传统关系型数据库。而随着互联网、物联网等场景发展，有**很多业务需求不是任何单一的数据库能解决的了**。下面对各类数据库的特点做简单概括：

- 按是否关系型分类：

    | **分类** | **特点** | **常见产品** |
    |----------|----------|--------------|
    | **关系型数据库** | - 数据以表格形式存储，有固定模式<br>- 使用主键、外键建立关系<br>- 支持事务处理，符合 ACID 特性<br>- 支持复杂查询和数据分析 | - Oracle<br>- MySQL<br>- PostgreSQL<br>- Microsoft SQL Server |
    | **非关系型数据库** | - 不使用固定模式，数据以键值对、文档、列族等形式存储<br>- 支持水平扩展和分布式计算<br>- 高性能、低延迟 | - Redis<br>- MongoDB<br>- Cassandra<br>- Neo4j |

- 按按特定场景分类：

    | **数据库类型** | **特点** | **常见产品** |
    |----------------|----------|--------------|
    | **键值数据库** | 数据以键值对形式存储，支持高性能读写 | Redis, Memcached, Riak |
    | **列式数据库** | 数据按列存储，适合大规模数据处理 | HBase, ClickHouse, Cassandra |
    | **文档数据库** | 数据以文档（如 JSON、XML）存储，灵活的模型 | MongoDB, Couchbase |
    | **图形数据库** | 处理图结构数据，适用于社交网络、知识图谱等 | Neo4j, Amazon Neptune |
    | **时序数据库** | 专用于时间序列数据，支持高效写入和查询 | InfluxDB, TimescaleDB |
    | **向量数据库** | 处理高维向量数据，适用于 AI 和推荐系统 | Milvus, Faiss |
    | **空间数据库** | 处理地理空间数据，支持地理查询 | PostGIS, Oracle Spatial |
    | **搜索引擎数据库** | 专为全文搜索设计，支持复杂查询 | Elasticsearch, Apache Lucene, Apache Solr |
    | **多模数据库** | 支持多种数据模型，提供灵活的数据处理 | ArangoDB, OrientDB |

接下来我们分析两个例子，从原理出发，了解这些数据库的特点和应用场景。

!!! example "搜索引擎数据库 Elasticsearch 与日志分析"

    观看 [:simple-bilibili: Elastic Search是什么？Lucene是什么？架构是怎么样的？- 小白 debug](https://www.bilibili.com/video/BV1yb421J7oX)，了解 Elasticsearch 的原理和架构。修读过《高级数据结构与算法分析》的同学应该很容易理解其中**倒排索引**的概念。
    
    Elasticsearch 将数据组织为文档，这些文档是基于 JSON 的信息单元，用于表示实体。文档根据其特征被归类到索引中。
    
    举一个具体的例子，这是一条存储在 Elasticsearch 中的日志数据。系统中各个组件每时每刻都在产生这样的海量的日志：
    
    ```json
    {
        "type":"liberty_message",
        "host":"9e1eceec70c1",
        "ibm_userDir":"\/opt\/ol\/wlp\/usr\/",
        "ibm_serverName":"defaultServer",
        "message":"BADAP0004W: BadApp Angry for test",
        "ibm_threadId":"0000009f",
        "ibm_datetime":"2020-05-04T12:33:26.064+0000",
        "ibm_messageId":"BADAP0004W",
        "module":"com.ibm.ws.lumberjack.badness.Angry",
        "loglevel":"WARNING",
    }
    ```
    
    运维人员如何审计这些日志？最基础的操作就是**过滤**。比如，审计用户的 SSH 登陆行为，就可以通过筛选 `sshd` 模块的日志，并查询 `message` 字段是否包含 `Invalid user` 来实现。
    
    你可能会觉得：这不就是关系型数据库中的一行数据嘛？使用 MySQL 不就能解决了？上面的数据太简单了，JSON 还可以嵌套，从各种地方收集的日志也具有不同的 schema。如果要在关系型数据库中存储这些数据，就需要设计很多表。**不仅设计复杂，效率也低。**而 Elasticsearch 能直接以文档的形式存储这些数据，并建立索引，方便快速查询。
    
    因为搜索引擎数据库**高效处理不同格式和类型的数据、提供灵活的搜索和检索功能**的特点，在可观测性领域，Elasticsearch 被广泛应用于**日志分析**等场景。

!!! example "时序数据库 InfluxDB 与指标监控"

    时序数据库的需求来源于指标监控场景。监控系统需要**实时收集、存储、查询和分析大量的时间序列数据**，比如 CPU 使用率、内存使用率、网络流量等。这些数据通常以**时间戳和值的形式存储，而且数据量巨大**。指标监控需要一个具备下列特点的数据库：
    
    - 极高的写吞吐量
    - 数据需要高度压缩
    - 大范围扫描记录
    - 原生支持聚合等操作
    
    上面这些是 MySQL 不擅长的。InfluxDB 针对时序数据的特点对存储引擎做了一系列优化，比如降低了事务的要求、从 LSM 演进成 TSM 作为底层数据的存储。这样的改变使得 InfluxDB 的查询性能比 MySQL 快 2.5~45 倍，数据压缩超过 4.5 倍（见 [Compare InfluxDB vs MySQL](https://www.influxdata.com/comparison/influxdb-vs-mysql/)）。
    
    关于 InfluxDB 原理，可以观看 InfluxData 的 CTO Paul Dix 在 2017 年做的分享 [:simple-bilibili: 【InfluxDB】InfluxDB 存储引擎技术内幕](https://www.bilibili.com/video/BV1jY41147AT)。

??? info "更多存储方面的知识"

    - [什么是对象存储？- 阿里云](https://www.aliyun.com/getting-started/what-is/what-is-object-storage)
