# 倒排索引

主要是一些概念：

- Index：将 Term 映射到 Document 的数据结构，可以是列表、树和哈希
- Inverted File：倒排文件，存储 Document ID。
- 分布式索引：
    - Term-Partitioned：将 Term 分块
    - Document-Partitioned：将 Document 分块
- Thresholding：阈值，仅获取权重最高的 $x$ 份文档。
- 评估结果的相关度：
    - Recall：召回率，检索到的相关文档数目占总相关文档数目的比例。通俗理解就是尽可能多地找到相关文档。
    - Precision：精确率，检索到的相关文档数目占总结果文档数目的比例。通俗地说就是结果里面含有的垃圾少。
    - 需要会根据 query 数量结果计算上面两个指标。

一些例题：

- In distributed indexing, document-partitioned strategy is to store on each node all the documents that contain the terms in a certain range. 文档分块策略在每个节点上存储一定 ID 范围内的文档，而不是按 term 分块。错误。
- When evaluating the performance of data retrieval, it is important to measure the relevancy of the answer set. 这里 relevancy of the answer set 指的就是 precision，错误，data retrieval 应当通过 recall 来衡量。
- Precision is more important than recall when evaluating the explosive detection in airport security. 这里是机场的爆炸物检测，我们显然希望尽可能多地找到爆炸物，不管精确率。所以 recall 更重要。
- Which of the following is NOT concerned for measuring a search engine?
    - How fast does it index 需要考虑
    - How fast does it search 需要考虑
    - How relevant is the answer set 需要考虑
    - How friendly is the interface 不需要
