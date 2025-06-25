---
tags:
  - 个人笔记
---

# 📒 C 和 C++ 高级话题

## STL 容器

- https://lemire.me/blog/2016/09/15/the-memory-usage-of-stl-containers-can-be-surprising/

## STL 算法

!!! quote

    - [:simple-youtube: CppCon 2018: Jonathan Boccara “105 STL Algorithms in Less Than an Hour”](https://www.youtube.com/watch?v=2olsGf6JIkU)

分类：

```mermaid
mindmap
    root((STL Algorithm))
        排列
            堆
                make_heap
                push_heap
                pop_heap
            排序
                sort_heap
                sort
                partial_sort
                nth_element
                inplace_merge
            划分
                partition
                partition_point
            其他
                rotate
                shuffle
                prev_permutation, next_permutation
                reverse
        衍生版本
            stable_*
            is_*
            is_*_until
            *_copy
            *_if
            *_n
        查询
            数值
                count
                部分和
                    partial_sum
                    id2("`(transform_)inclusive_scan`")
                    id3("`(transform_)exclusive_scan`")
                折叠
                    accumulate
                    id4("`(transform_)reduce`")
                inner_product
                adjacent_difference
                sample
            性质
                all_of
                any_of
                none_of
            双范围性质
                equal
                is_permutation
                lexicographical_compare
                mismatch
            无序查询
                find
                adjacent_find
            有序查询
                equal_range
                lower_bound
                upper_bound
                binary_search
            查询范围
                search
                find_end
                find_first_of
            相对查询
                max_element
                min_element
                minmax_element
        集合
            set_difference
            set_intersection
            set_union
            set_symmetric_difference
            includes
            merge
        移动
            copy
            move
            swap_ranges
            copy_backward
            move_backward
        值修改
            fill
            generate
            iota
            replace
        结构变换
            remove
            unique
        其他
            for_each
            transform
        原始内存
            uninitialized_copy
            uninitialized_fill
            uninitialized_move
            destroy
            uninitialized_default_construct
            uninitialized_value_construct
```

经典用法：

```cpp
collection.erase(
    std::remove(begin(collection), end(collection), value),
    end(collection));
```

