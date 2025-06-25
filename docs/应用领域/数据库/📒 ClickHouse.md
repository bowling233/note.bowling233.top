---
tags:
  - 个人笔记
---

# ClickHouse

## ClickHouse 基本概念

- 列式数据库：适合过滤和聚合操作。
- 完整性：异步多主复制（asynchronous multi-master replication），数据写入任意可用的副本（replica）后，其余的副本会在后台自动获取并同步这份数据拷贝。大多数故障情况下能够自动恢复。

## ClickHouse 运维

### [Docker 部署](https://github.com/ClickHouse/ClickHouse/blob/master/docker/server/README.md)

Docker 下用户配置应当使用环境变量，配置文件 `users.xml` 可能被覆盖修改（比如服务器检测到无密码，会自动限制本地访问），至少应当使用 `users.d`。

- **不安全模式：**`CLICKHOUSE_SKIP_USER_SETUP=1` 允许从任意 IP 访问，并给予所有权限。
- **设置初始用户：**`CLICKHOUSE_PASSWORD` 设置密码，`CLICKHOUSE_DEFAULT_ACCESS_MANAGEMENT=1` 启用账户管理。

### 配置文件

<https://clickhouse.com/docs/en/operations/configuration-files>

### [系统表格](https://clickhouse.com/docs/operations/system-tables)

存放于 `system` 数据库，常用：

```text
user
```

### [访问控制](https://clickhouse.com/docs/operations/access-rights)

- **默认用户：** `default`，密码为空，只允许从 `::1` 访问。
- **管理方式：** SQL 和配置文件两种，默认不允许 SQL 管理。

!!! warning "坑点"

    - 如果默认用户设置了密码，`DICTIONARY` 即使查询本地表也需要指定用户名密码：

        ```sql
        SOURCE(CLICKHOUSE(TABLE 'source_table' USER 'default' PASSWORD 'password' DB 'db' ))
        ```

        远程数据库则添加 `HOST` 和 `PORT` 关键字。

### 生命周期

<https://clickhouse.com/blog/using-ttl-to-manage-data-lifecycles-in-clickhouse>

## ClickHouse 表

### MergeTree

!!! quote

    - [](https://clickhouse.com/docs/engines/table-engines/mergetree-family/mergetree)

ClickHouse 默认的表引擎。插入数据时生成表的片段，后台进程自动合并这些片段。特点是插入速度快。

```sql
ENGINE = MergeTree
```

## ClickHouse 查询优化

列式数据库在实践时有一些需要注意的特点：

- 写入慢，因此要 batch。
- 数据压缩和聚合效果好。
- 多列查询效果不好。

下面的文章涵盖了使用 ClickHouse 和 OpenTelemetry 处理日志的方方面面，从输入到查询和可视化。

- [Integrating OpenTelemetry](https://clickhouse.com/docs/en/observability/integrating-opentelemetry)
- [Building an Observability Solution with ClickHouse](https://clickhouse.com/blog/storing-log-data-in-clickhouse-fluent-bit-vector-open-telemetry)。

ClickHouse 仍在快速发展中，可以关注一下它的[博客](https://clickhouse.com/blog)，有版本更新、性能优化等内容。比如，JSON 数据格式于 2024 年 9 月才得到实验性支持。

阅读 [Overview of ClickHouse Architecture](https://clickhouse.com/docs/en/development/architecture) 了解 ClickHouse 的架构。

## [Materialized View](https://clickhouse.com/docs/en/materialized-view)

Materialized View 通过在插入数据时计算并存储查询结果，将查询的时间开销转移到写入时。

上面我们已经知道列式数据库的多列查询效果不好。比如，下面的查询涉及四个列，运行时间非常慢。但如果把高亮的部分去掉，只涉及两个列，查询时间可以缩短 10 倍。

```sql hl_lines="4-8"
select toIPv4OrDefault(LogAttributes ['client.address']) as ip,
        COUNT(*) as request_count
from otel_logs
where ResourceAttributes ['cloud.region'] = 'zjusct-falcon'
        AND ResourceAttributes ['host.name'] = 'zjumirror'
        AND ResourceAttributes ['service.name'] = 'nginx'
        AND LogAttributes ['client.address'] != ''
        AND LogAttributes ['log.file.name'] = 'otel.log'
        AND (
                Timestamp >= $__fromTime
                AND Timestamp <= $__toTime
        )
group by ip
```

但是我们需要这些条件。为了避免每次查询都要计算这些条件，我们可以使用 Materialized View。

```sql
CREATE TABLE zjumirror_nginx_rps_ipv4
(
    timestamp DateTime,
    ip IPv4,
    request_count UInt32
)
ENGINE = SummingMergeTree()
ORDER BY (timestamp, ip);

CREATE MATERIALIZED VIEW zjumirror_nginx_rps_ipv4_mv TO zjumirror_nginx_rps_ipv4 AS
SELECT
    toStartOfMinute(Timestamp) as timestamp,
    toIPv4(LogAttributes ['client.address']) as ip,
    COUNT(*) as request_count
FROM otel_logs
WHERE ResourceAttributes ['cloud.region'] = 'zjusct-falcon'
    AND ResourceAttributes ['host.name'] = 'zjumirror'
    AND ResourceAttributes ['service.name'] = 'nginx'
    AND isIPv4String(LogAttributes ['client.address'])
    AND LogAttributes ['log.file.name'] = 'otel.log'
GROUP BY timestamp, ip;
```

现在我们的查询可以改为：

```sql
SELECT ip, sum(request_count) as request_count
FROM zjumirror_nginx_rps_ipv4
WHERE timestamp >= $__fromTime AND timestamp <= $__toTime
GROUP BY ip
```
