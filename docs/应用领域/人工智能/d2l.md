# Dive into Deep Learning

## 第一章：介绍

了解基本概念，建立整体认识。

- machine learning:

*Machine learning* is the study of algorithms that can learn from experience.

- parameters:

we do not attempt to design a system *explicitly* to recognize wake words. Instead, we define a flexible program whose behavior is determined by a number of *parameters*. Then we use the dataset to determine the best possible parameter values, i.e., those that improve the performance of our program with respect to a chosen performance measure.

- model, learning algorithm:

Once the parameters are fixed, we call the program a *model*. The set of all distinct programs (input–output mappings) that we can produce just by manipulating the parameters is called a *family* of models. And the “meta-program” that uses our dataset to choose the parameters is called a *learning algorithm*.

- supervised learning:

This sort of problem, where we try to predict a designated unknown label based on known inputs given a dataset consisting of examples for which the labels are known, is called *supervised learning*.

- data: example(data point, data instance, sample), features(covariates, inputs), label(target), dimensionality

Each *example* (or *data point*, *data instance*, *sample*) typically consists of a set of attributes called *features* (sometimes called *covariates* or *inputs*), based on which the model must make its predictions. In supervised learning problems, our goal is to predict the value of a special attribute, called the *label* (or *target*), that is not part of the model’s input.

- objective functions (loss functions): overfitting

By convention, we usually define objective functions so that lower is better.

When trying to predict numerical values, the most common loss function is *squared error*

During optimization, we think of the loss as a function of the model’s parameters, and treat the training dataset as a constant.

So we will typically want to split the available data into two partitions: the *training dataset* (or *training set*), for learning model parameters; and the *test dataset* (or *test set*), which is held out for evaluation. At the end of the day, we typically report how our models perform on both partitions.

When a model performs well on the training set but fails to generalize to unseen data, we say that it is *overfitting* to the training data.

- optimization algorithms: gradient descent

for each parameter, how that training set loss would change if you perturbed that parameter by just a small amount. It would then update the parameter in the direction that lowers the loss.

- machine learning problems: supervised learning (regression, classification, tagging, search, recommender system, sqeuence learning), unsupervised (clustering, componenet analysis, causality, probabilistic graphical model, deep generative model), self-supervised (leverage some aspect of the unlabeled data to provide supervision), environment (offline learning, intelligent agent), reinforcement

![supervised-learning.svg](d2l.assets/supervised-learning.svg)

The agent then receives a subsequent observation, and chooses asubsequent action, and so on. The behavior of a reinforcement learning agent is governedby a policy. In brief, a policy is just a function that maps from observations of the environmentto actions. **The goal of reinforcement learning is to produce good policies.**

- Markov decision process,, contextual bandit problem, multi-armed bandit problem
- Hardware perspective:

Note that random-access memory has not kept pace with the growth in data. At the same time, increases in computational power have outpaced the growth in datasets. This means that statistical models need to become more memory efficient, and so they are free to spend more computer cycles optimizing parameters, thanks to the increased compute budget. Consequently, the sweet spot in machine learning and statistics moved from (generalized) linear models and kernel methods to deep neural networks.

- attention

Rather than having to remember an entire text sequence, e.g., for machine translation in a fixed-dimensional representation, all that needed to be stored was a pointer to the intermediate state of the translation process.

- transformer, language model, GAN, diffusion model

- deep learning

Deep learning is the subset of machine learning concerned with models **based on many-layered neural networks**.

- end-to-end training

rather than assembling a system based on components that are individually tuned, one builds the system and then tunes their performance jointly

## 第二章：预备知识

!!! todo

- data manipulation

`tensor` support automatic differentiation. construction routines; indexing and slicing; basic mathematics operations; broadcasting; memory-efficient assignment; and conversion to and from other Python objects.

- data processing

padas

- linear algebra

Hadamard product $\odot$, `torch.mv(A, x)` `torch.mm(A, B)` `A@x`, norm $|| \cdot ||$ `torch.norm`

$l_p$ norm: $||\vec{x}||_p = (\sum_{i=1}^n|x_i|^p)^{1/p}$

*Frobenius norm*: square root of the sum of the squares of a matrix’s elements $||A||_F = \sqrt{\sum_{i,j}A_{i,j}^2}$​ `torch.norm()`

## 第三章：线性神经网络——回归

回归问题预测数值。模型为：

$$
\hat{y} = \mathbf{w}^\top \mathbf{x} + b
$$

几个概念：training set、example、label（要预测的目标）、feature（用于预测的变量）。

目标是找到最优参数 $\mathbf{w}$ 和 $b$，需要解决两个问题：

1. 评估模型的质量。
2. 更新模型以提升质量。

评估模型质量使用损失函数（loss function），本章介绍了均方误差（mean squared error）。

### 小批量随机梯度下降

Minibatch Stochastic Gradient Descent 是本章最重要的话题。

- Gradient Descent：迭代地向使得损失函数下降的方向更新参数。
    - 每次要计算所有样本的梯度，计算量大。
- Stochastic Gradient Descent：每次只计算一个样本的梯度，计算量小，但是方向不稳定。
- Minibatch Stochastic Gradient Descent：上面两种方案的折中，每次计算一小批样本的梯度。

$$
(\mathbf{w}, b) \leftarrow (\mathbf{w}, b) - \frac\eta{|\mathcal{B}|} \sum_{i \in \mathcal{B}} \nabla_{(\mathbf{w}, b)} l^{(i)}(\mathbf{w}, b)
$$

其中 $\mathcal{B}$ 是小批量样本集合，$|\mathcal{B}|$ 是批量大小，$\eta$ 是学习率。这两个参数由用户定义，在训练过程中不被更新，称为**超参数**，有额外的优化方法。

### 正态分布与平方损失

本节从数学上证明了：均方误差能够抵抗样本中正态分布的噪声。

!!! todo "推导过程使用了概率论的极大似然估计，暂略。"

### 面向对象的实现设计

为了从更高的层次理解深度学习，本书使用面向对象的设计模式。有三类对象：

- `Module`：模型、损失和优化算法。
- `DataModule`：加载训练和验证集。
- `Trainer`：与硬件有关，将上面两个对象连接起来。

本节主要内容：

- 几个语言特性和函数包装，比如使用 Python decorator 向类中追加属性和方法等。

    ```python
    def add_to_class(Class):  #@save
        """Register functions as methods in created class."""
        def wrapper(obj):
            setattr(Class, obj.**name**, obj)
        return wrapper
    ```

- `Module`、`DataModule`、`Trainer` 的代码实现。

主要看 Trainer 实现，就可以了解另外两个模块是怎么使用的了：

```python
class Trainer(d2l.HyperParameters):  #@save
    """The base class for training models with data."""
    def __init__(self, max_epochs, num_gpus=0, gradient_clip_val=0):
        self.save_hyperparameters()
        assert num_gpus == 0, 'No GPU support yet'

    def prepare_data(self, data):
        self.train_dataloader = data.train_dataloader()
        self.val_dataloader = data.val_dataloader()
        self.num_train_batches = len(self.train_dataloader)
        self.num_val_batches = (len(self.val_dataloader)
                                if self.val_dataloader is not None else 0)

    def prepare_model(self, model):
        model.trainer = self
        model.board.xlim = [0, self.max_epochs]
        self.model = model

    def fit(self, model, data):
        self.prepare_data(data)
        self.prepare_model(model)
        self.optim = model.configure_optimizers()
        self.epoch = 0
        self.train_batch_idx = 0
        self.val_batch_idx = 0
        for self.epoch in range(self.max_epochs):
            self.fit_epoch()

    def fit_epoch(self):
        raise NotImplementedError
```

### 人造回归数据

本节主要是教学目的，编写了一个

### 从零开始实现线性回归

讲解模型、损失函数、优化算法、训练代码的实现。

- 模型：按预设的数学模型定义 `forward()` 方法，参数取随机值。

    ```python
    @d2l.add_to_class(LinearRegressionScratch)  #@save
    def forward(self, X):
        return torch.matmul(X, self.w) + self.b
    ```

- 损失函数：按均方误差定义，使用 `.mean()` 方法。
- 优化算法：给定一组参数和学习率，使用 Minibatch SGD 更新参数。

    ```python
    class SGD(d2l.HyperParameters):  #@save
        """Minibatch stochastic gradient descent."""
        def **init**(self, params, lr):
            self.save_hyperparameters()

        def step(self):
            for param in self.params:
                param -= self.lr * param.grad

        def zero_grad(self):
            for param in self.params:
                if param.grad is not None:
                    param.grad.zero_()
    ```

- 训练过程：
    - 初始化参数 -> 循环（计算梯度 -> 更新参数（优化器））。

    ```python
    @d2l.add_to_class(d2l.Trainer)  #@save
    def fit_epoch(self):
        self.model.train()
        for batch in self.train_dataloader:
            loss = self.model.training_step(self.prepare_batch(batch))
            self.optim.zero_grad()
            with torch.no_grad():
                loss.backward()
                if self.gradient_clip_val > 0:  # To be discussed later
                    self.clip_gradients(self.gradient_clip_val, self.model)
                self.optim.step()
            self.train_batch_idx += 1
        if self.val_dataloader is None:
            return
        self.model.eval()
        for batch in self.val_dataloader:
            with torch.no_grad():
                self.model.validation_step(self.prepare_batch(batch))
            self.val_batch_idx += 1
    ```

### 线性回归的具体实现

本节使用现代深度学习框架改写上面的模型。

- 定义模型：PyTorch 提供 `Linear` 和 `LazyLinear` 两种线性层。后者只需要定义输出维度。

    ```python
    self.net = nn.LazyLinear(1)
    self.net.weight.data.normal_(0, 0.01)
    self.net.bias.data.fill_(0)
    ```

    PyTorch 层的 `__call__` 方法就是前向传播，所以 `forward()` 直接调用 `self.net(X)` 即可。

- 损失函数：PyTorch 提供了 `MSELoss` 类。

    ```python
    fn = nn.MSELoss()
    return fn(y_hat, y)
    ```

- 优化算法：PyTorch 提供了 `optim` 模块。实例化 `SGD` 时，指定需要优化的参数和学习率。

    ```python
    return torch.optim.SGD(self.parameters(), self.lr)
    ```

### 泛化

!!! todo "暂略。"

### 权重衰减 Weight Decay

### 其他

- 使用向量化计算取代循环能够极大地提高计算效率。
- 本章介绍的线性回归可以看成一层全连接的神经网络。

## 第四章：线性神经网络——分类

上一节的回归问题解决“有多少”的问题，本节的分类问题解决“哪一类”的问题。

分类问题常用独热码（one-hot encoding）表示标签。分类问题的模型描述如下：

$$
\mathbf{o} = \mathbf{W} \mathbf{x} + \mathbf{b}
$$

- 输入是特征向量 $\mathbf{x}$。
- 输出是各标签的概率分布 $\mathbf{o}$。每个标签的概率（输出的每个分量）都取决于输入的所有特征（输入的每个分量）。因此这是全连接层。

因为输出是概率分布，因此 $0 \leq o_i \leq 1$ 且 $\sum_i o_i = 1$，使用均方误差函数进行评估不能产生令人满意的效果。

### Softmax

借助指数函数，定义了 softmax 激活函数，它用于将输入转换成离散的概率分布：

$$
\hat{\mathbf{y}} = \text{softmax}(\mathbf{o}) \quad \text{where} \quad \hat{y}_i = \frac{\exp(o_i)}{\sum_j \exp(o_j)}
$$

对于 Softmax 函数，需要使用最大似然估计进行误差分析。内容暂略。总之就是这么个东西：

$$
l(\mathbf{y}, \hat{\mathbf{y}}) = \log \sum_{k=1}^q \exp(o_k) - \sum_{j=1}^q y_j o_j
$$

!!! todo "最大似然估计的推导过程"

### 图像分类数据集

- MNIST 手写数字识别
    - LeNet-5
- ImageNet
- Fashion-MNIST

!!! note "技能：如何加载数据集"

    - [Datasets & DataLoaders — PyTorch Tutorials documentation](https://pytorch.org/tutorials/beginner/basics/data_tutorial.html)

    PyTorch 提供两种数据类型：`torch.utils.data.DataLoader` 和 `torch.utils.data.Dataset`。Dataset 存储样本（sample）及其标签（label），dataloader 在其上实现了一个迭代器，方便访问数据。

    对于 PyTorch 自身提供的数据集，加载很简单，直接实例化即可：

    ```python
    self.train = torchvision.datasets.FashionMNIST(root="../data", train=True, transform=transform, download=True)
    ```

    最原始的方法是手动索引数据集：

    ```python
    img, label = self.train[i]
    ```

    要手动创建一个数据集，需要实现 `__init__`、`__len__` 和 `__getitem__` 方法：

    ```python
    def __getitem__(self, idx):
        img_path = os.path.join(self.img_dir, self.img_labels.iloc[idx, 0])
        image = read_image(img_path)
        label = self.img_labels.iloc[idx, 1]
        if self.transform:
            image = self.transform(image)
        if self.target_transform:
            label = self.target_transform(label)
        return image, label
    ```

    在训练过程中，使用 `DataLoader` 类从数据集中加载数据**到内存**，一般是加载小批量（batch）数据：

    ```python
    return torch.utils.data.DataLoader(dataset, batch_size, shuffle=shuffle)

    for X, y in data.train_dataloader():
        continue
    ```

!!! example "实践：从 CIFAR 数据集中提取文件"

    CIFAR 数据集可以在 [](https://www.cs.toronto.edu/~kriz/cifar.html) 下载。其 Python 版本有 6 个 batch 文件，这里我们尝试将其提取为 `torchvision.datasets.ImageFolder` 所要求的布局：

    ```text
    root/dog/xxx.png
    root/dog/xxy.png
    root/dog/[...]/xxz.png

    root/cat/123.png
    root/cat/nsdf3.png
    root/cat/[...]/asd932_.png
    ```

    阅读 CIFAR 数据集说明，使用 `pickle` 载入得到一个 `dict`。

### 待办

## 第五章：多层感知机 Multilayer Perceptrons

多层感知机是最简单的神经网络，由多个全连接层组成。本章关注：

- 深度网络中梯度是如何计算的
- 数值稳定性和参数初始化问题

### 多层感知机

在上一章，我们的模型是：输入通过仿射变换连接到输出，再经过一个 softmax。然而，**线性性（仿射变换）是一个很强的假设**，具有很大的局限性。其中之一就是单调性（monotonicity）：数据的变化总是会导致输出的变化。这在很多情况下是不适用的。

解决方式是在中间加入更多隐藏层。但仅仅添加层是不够的：

$$
\mathbf{H} = \mathbf{X}\mathbf{W}^{(1)} + \mathbf{b}^{(1)} \\
\mathbf{O} = \mathbf{H}\mathbf{W}^{(2)} + \mathbf{b}^{(2)}
$$

这样的模型展开后还是仿射变换，因为仿射变换的复合还是仿射变换。为了解决这个问题，我们需要在仿射变换之后加入**非线性函数 $\sigma$**，这个函数称为激活函数（activation function）。

$$
\mathbf{H} = \sigma(\mathbf{X}\mathbf{W}^{(1)} + \mathbf{b}^{(1)}) \\
\mathbf{O} = \mathbf{H}\mathbf{W}^{(2)} + \mathbf{b}^{(2)}
$$

相关研究表明，即使只有一层隐藏层，只要给够节点和正确的权重，多层感知机就能拟合任意函数。这个结论称为**万能近似定理**（universal approximation theorem）。

常见激活函数：

- Rectified Linear Unit (ReLU):$\mathrm{ReLU}(x) = \max(x, 0)$
    - 它的导数表现得很好，不会出现梯度消失的问题，使得优化更加简单。
- Sigmoid：$\sigma(x) = \frac{1}{1 + \exp(-x)}$
    - 当输入过大或过小时，梯度会消失。
- 双曲正切函数 hyperbolic tangent：$\tanh(x) = \frac{1 - \exp(-2x)}{1 + \exp(-2x)}$
    - 同样有梯度消失问题。

### 多层感知机实现

层数及其宽度是超参数。宽度一般选择 2 的幂的倍数，有利于提高计算效率。暂不考虑空间结构，故输入层展平图像。

```python
self.net = nn.Sequential(nn.Flatten(), nn.LazyLinear(num_hiddens),
                            nn.ReLU(), nn.LazyLinear(num_outputs))
```

### 前向、反向传播和计算图



## 第七章：卷积神经网络 Convolutional Neural Networks

到现在为止，处理图像时我们将其展平，忽略了图像的空间结构。对于这种网络，改变特征的顺序不会改变其行为，即使打乱像素也能得到相似的结果。

为了利用图像的空间结构，引入 CNN。需要的参数比全连接少，且计算效率高，因为卷积运算在 GPU 上很容易并行。

### 从全连接层到卷积

到目前为止，我们只考虑了表状数据，并假设我们能找到的模式与特征之间的作用有关。但在考虑它们如何相互作用前，我们忽略了数据的空间结构。对于**高维数据**，这种**无结构**的网络会野蛮生长。我们需要使用数据的空间结构指导我们构建更好的架构。

- 不变性 invariance
    - 在图像中寻找物体时，我们并不过度关心其精确位置。CNN 从物体识别和分割算法中总结了空间不变性的特点，如下所述。
    - 翻译不变性 translation invariance：在早期的层，应当对相同的 patch 有类似的响应，不管它出现在图像的什么位置。
    - 局部性 locality：在早期的层，网络应当只关注局部区域，不需要考虑较远的距离。
    - 随着处理，更深的层次应当识别图像中更大范围的特征。

接下来从限制 MLP 开始着手，应用上面的几个不变性，MLP 最终被化简为：

## 第九章：循环神经网络 Recurrent Neural Networks

到目前为止，我们处理的数据都是定长的。但如果我们面对的是图像序列（比如视频），或者需要生成序列的的结构化的预测，那么我们就需要使用循环神经网络（RNN）。
