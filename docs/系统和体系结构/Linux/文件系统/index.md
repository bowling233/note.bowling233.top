# 文件系统

!!! quote

    - [陈硕：Linux 文件系统九讲 - Bilibili](https://space.bilibili.com/1356949475/channel/collectiondetail?sid=1211802)
    - [A Brief History of the BSD - FreeBSD Foundation](https://freebsdfoundation.org/wp-content/uploads/2016/04/A-Brief-History-of-the-BSD-Fast-Filesystem.pdf)

- 区块（Block）：
    - 文件系统分配空间的最小单位，文件系统不是按字节寻址的。例如：文件系统块大小为 4KB，文件大小为 1KB，那么文件占用 4KB 磁盘空间。
    - block 的分配采用 bitmap，信息保存在 superblock 中，获取 free block 的复杂度为 $O(1)$。
    - block 大小对文件系统性能有很大影响：大 block 减少元数据开销、提高吞吐，小 block 提高空间利用率。
    - block 大小受磁盘扇区和内存页面大小制约：
        - block 大小应该大于扇区大小，否则文件系统修改 1 个 block，磁盘需要把 sector 读到自己的缓存，修改其中的一小部分再写回。
        - block 大小应当小于内存页面大小，因为系统需要做内存映射。
    - 2011 年，因磁盘扇区大小从 512 字节变为 4KB，block 大小也增长到 4KB。受内存页面 4KB 限制，在可以预见的未来，block 大小都将固定在 4KB。
- 区块组（Block Group）：
    - 现代文件系统将 Block 分组，其中包含元数据，以提供更好的局部性（元数据和数据更近）。