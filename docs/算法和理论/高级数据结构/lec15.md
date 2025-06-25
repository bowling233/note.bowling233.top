# 外部排序 External Sorting



## 习题

??? note "[PTA 作业：判断题 1]"

    To merge 55 runs using 3 tapes for a 2-way merge, the original distribution (34, 21) is better than (27, 28).

    ??? success

        正确。根据 PPT 上的定理：If the number of runs is a Fibonacci number $F_N$, then the best way to distribute them is to split them into $F_{N-1}$ and $F_{N-2}$.

??? note "需要解析[PTA 作业：判断题 2]"

    If only one tape drive is available to perform the external sorting, then the tape access time for any algorithm will be $\Omega(N^2)$.

    ??? success

        正确。（来源：[If only one tape drive is available to perform the external sort - 题库 - 雨中笔记](https://150.158.156.12/post/1883.html)）。

??? note "[PTA 作业：判断题 3]"

    In external sorting, a $k$-way merging is usually used in order to reduce the number of passes and we will take the $k$ as large as possible as long as we have enough amount of tapes.

    ??? success

        错误。事实上 K 不是越大越好，因为如果 K 增大，就会导致 input buffer 的数量需求增加，导致 buffer size 减少，导致磁盘中一个 block 的 size 减少，导致访问磁盘的 seek time 增加，因此最优的 K 值取决于磁盘的参数和外部 memory 的规模。（来源：[External Sorting - 克瑞奇（Chritch）的笔记本](https://notes.zerokei.top/course/ads/external/)）。

??? note "[PTA 作业：判断题 4]"

    In general, for a 3-way merge we need 6 input buffers and 2 output buffers for parallel operations.

    ??? success

        正确。对于一个 K 路归并，需要 2k 个输入 buffer 和 2 个输出 buffer 来进行并行操作。为什么不是 k 和 1 ？因为是一个正式一个缓冲。（来源：[External Sorting - 克瑞奇（Chritch）的笔记本](https://notes.zerokei.top/course/ads/external/)）。

??? note "[PTA 作业：选择题 1]"

    Given 100,000,000 records of 256 bytes each, and the size of the internal memory is 128MB. If simple 2-way merges are used, how many passes do we have to do?

    - A. 10
    - B. 9
    - C. 8
    - D. 7

    ??? success

        选 B。$k$-way merge 的公式为 $passes = 1 + \lceil \log_k(\frac{N}{M}) \rceil$，其中 $N$ 为记录数，$M$ 为内存大小，$k$ 为归并路数。在上面的题目中，需要归并的记录数可以简单估计为 25.6GB 进行计算，数量级差异不大。

??? note "需要解析[PTA 作业：选择题 2]"

    In external sorting, suppose we have 5 runs of lengths 2, 8, 9, 5, and 3, respectively. Which of the following merging orders can obtain the minimum merge time?

    - A. merge runs of lengths 2 and 3 to obtain Run#1; merge Run#1 with the one of length 5 to obtain Run#2; merge Run#2 with the one of length 8 to obtain Run#3; merge Run#3 with the one of length 9
    - B. merge runs of lengths 2 and 3 to obtain Run#1; merge Run#1 with the one of length 5 to obtain Run#2; merge runs of lengths 8 and 9 to obtain Run#3; merge Run#2 and Run#3
    - C. merge runs of lengths 2 and 3 to obtain Run#1; merge runs of lengths 5 and 8 to obtain Run#2; merge Run#1 and Run#2 to obtain Run#3; merge Run#3 with the one of length 9
    - D. merge runs of lengths 2 and 3 to obtain Run#1; merge runs of lengths 5 and 8 to obtain Run#2; merge Run#2 with the one of length 9 to obtain Run#3; merge Run#1 and Run#3

    ??? success

        B。（来源：[Multiple choice:In external sorting, suppose we have 5 runs of lengths 2, 8, 9, - 题库 - 雨中笔记](https://150.158.156.12/post/36040.html)）。

??? note "[PTA 作业：选择题 3]"

    In external sorting, in order to reduce the number of passes, minimizing the initial number of runs (i.e. generating longer runs ) is a good idea. Suppose the input record keys are (25, 74, 56, 34, 21, 11, 29, 80, 38, 53) and the internal memery can hold only 3 records, the minimum number of initial runs obtained by replacement selection is__ 。

    - A. 1
    - B. 2
    - C. 3
    - D. 4

    ??? success

        选 B。容易得到两个 runs 分别为：{25, 34, 56, 74} 和 {11, 21, 29, 38, 53, 80}。

??? note "需要分析[PTA 作业：选择题 4]"

    Suppose we have the internal memory that can handle 12 numbers at a time, and the following two runs on the tapes:

    Run 1: 1, 3, 5, 7, 8, 9, 10, 12

    Run 2: 2, 4, 6, 15, 20, 25, 30, 32

    Use 2-way merge with 4 input buffers and 2 output buffers for parallel operations. Which of the following three operations are NOT done in parallel?

    - A. 1 and 2 are written onto the third tape; 3 and 4 are merged into an output buffer; 6 and 15 are read into an input buffer
    - B. 3 and 4 are written onto the third tape; 5 and 6 are merged into an output buffer; 8 and 9 are read into an input buffer
    - C. 5 and 6 are written onto the third tape; 7 and 8 are merged into an output buffer; 20 and 25 are read into an input buffer
    - D. 7 and 8 are written onto the third tape; 9 and 15 are merged into an output buffer; 10 and 12 are read into an input buffer

    ??? success

        选 D。（来源：[单选题：Suppose we have the internal memory that can handle 12 numbers a - 题库 - 雨中笔记](https://150.158.156.12/post/11814.html)）

??? note "[PTA 作业：选择题 5]"

    Suppose that the replacement selection is applied to generate longer runs with a priority queue of size 4. Given the sequence of numbers { 9, 75, 17, 12, 88, 91, 25, 22, 35, 41, 58, 96, 15 }. Which of the following gives the second output run?

    - A. 22 35 88 91
    - B. 22 35 41 58 88 91 96
    - C. 22 35 41 58 96
    - D. 22 35 41 58

    ??? success

        选 C。容易得到三个 runs 分别为：{9, 12, 17, 25, 75, 88, 91}，{22, 35, 41, 58, 96} 和 {15}。

??? note "[PTA 作业：编程题] Replacement Selection"

    When the input is much too large to fit into memory, we have to do external sorting instead of internal sorting. One of the key steps in external sorting is to generate sets of sorted records (also called runs) with limited internal memory. The simplest method is to read as many records as possible into the memory, and sort them internally, then write the resulting run back to some tape. The size of each run is the same as the capacity of the internal memory.

    Replacement Selection sorting algorithm was described in 1965 by Donald Knuth. Notice that as soon as the first record is written to an output tape, the memory it used becomes available for another record. Assume that we are sorting in ascending order, if the next record is not smaller than the record we have just output, then it can be included in the run.

    For example, suppose that we have a set of input { 81, 94, 11, 96, 12, 99, 35 }, and our memory can sort 3 records only. By the simplest method we will obtain three runs: { 11, 81, 94 }, { 12, 96, 99 } and { 35 }. According to the replacement selection algorithm, we would read and sort the first 3 records { 81, 94, 11 } and output 11 as the smallest one. Then one space is available so 96 is read in and will join the first run since it is larger than 11. Now we have { 81, 94, 96 }. After 81 is out, 12 comes in but it must belong to the next run since it is smaller than 81. Hence we have { 94, 96, 12 } where 12 will stay since it belongs to the next run. When 94 is out and 99 is in, since 99 is larger than 94, it must belong to the first run. Eventually we will obtain two runs: the first one contains { 11, 81, 94, 96, 99 } and the second one contains { 12, 35 }.

    Your job is to implement this replacement selection algorithm.

    - Input Specification:
    
    Each input file contains several test cases. The first line gives two positive integers N (≤10^5) and M (<N/2), which are the total number of records to be sorted, and the capacity of the internal memory. Then N numbers are given in the next line, all in the range of int. All the numbers in a line are separated by a space.

    - Output Specification:
    
    For each test case, print in each line a run (in ascending order) generated by the replacement selection algorithm. All the numbers in a line must be separated by exactly 1 space, and there must be no extra space at the beginning or the end of the line.

    - Sample Input:

        ```text
        13 3
        81 94 11 96 12 99 17 35 28 58 41 75 15
        ```

    - Sample Output:

        ```text
        11 81 94 96 99
        12 17 28 35 41 58 75
        15
        ```

    ??? success

        ```cpp
        #include <iostream>
        #include <queue>

        using namespace std;

        int main()
        {
            int N, M;
            cin >> N >> M;
            priority_queue<int, vector<int>, greater<int>> curRun;
            priority_queue<int, vector<int>, greater<int>> nextRun;
            vector<int> input;
            for (int i = 0; i < N; i++)
            {
                int tmp;
                cin >> tmp;
                input.push_back(tmp);
            }

            // create first run
            for (int i = 0; i < M; i++)
            {
                curRun.push(input[i]);
            }

            // output head and compare with next input

            int cur = M;
            int output = curRun.top();
            curRun.pop();
            cout << output;

            while (cur < N)
            {
                if (input[cur] < output)
                {
                    // input is smaller than output, put it in next run
                    nextRun.push(input[cur]);
                    cur++;
                }
                else
                {
                    // input is larger than output, put it in current run
                    curRun.push(input[cur]);
                    cur++;
                }

                // if current run is empty, break line
                if (curRun.empty())
                {
                    cout << endl;
                    // swap current run and next run
                    curRun.swap(nextRun);
                    // output head of current run
                    output = curRun.top();
                    curRun.pop();
                    cout << output;
                }
                else
                {
                    // output head of current run
                    output = curRun.top();
                    curRun.pop();
                    cout  << " " << output;
                }
            }

            // output last run
            while (!curRun.empty())
            {
                output = curRun.top();
                curRun.pop();
                cout  << " " << output;
            }

            // and the next run
            if(!nextRun.empty())
                cout << endl;
            while (!nextRun.empty())
            {
                output = nextRun.top();
                nextRun.pop();
                cout << output;
                if(nextRun.size() != 0)
                    cout << " ";
            }

            cout << endl;

            return 0;
        }
        ```

