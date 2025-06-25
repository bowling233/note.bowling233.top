---
tags:
  - 读书笔记
  - 正在做
---

# 📖 PyTorch 深度学习实战

!!! abstract "书籍信息"

    - **书名**：PyTorch 深度学习实战/Deep Learning with PyTorch
    - **英文原书出版年份**：2020
    - **出版社**：人民邮电出版社

PyTorch 大部分使用 C++ 和 CUDA 编写。

## 概述

PyTorch 项目的基本流程如下所示：

为了训练一个模型，需要：数据源、优化器、把模型和数据传输到硬件的方法。

1. 需要将数据中的每个样本转换成 PyTorch 可以处理的张量。`torch.utils.data` 包下的 `Dataset` 类提供了标准张量，我们需要把数据预处理成这种格式。
2. 我们希望并行化数据加载，即将数据组装成具有多个样本的张量。使用 `DataLoader` 类实现。
3. 训练循环通常是标准的 `for` 循环。
4. 训练循环的每个步骤中，都要使用损失函数与目标进行比较。`torch.nn` 包中提供了一些损失函数。比较后，PyTorch 底层的自动求导引擎可以帮助我们修改模型。
5. 优化器可以帮助我们更新模型的参数，以最小化损失函数。`torch.optim` 包提供了优化器。
6. `torch.nn.parallel.DistributedDataParallel` 和 `torch.distributed` 模块可以帮助我们在多个 GPU 或服务器上上并行化训练。

## 深度学习

深度学习机的核心是一个复杂的数学函数，将输入映射到输出。PyTorch 提供了一个核心数据结构——张量，即一个多维数组，来帮助表达这个函数。PyTorch 张量可以存储数字、向量、矩阵和数组。

## 预训练网络

本节学习使用预训练的 AlexNet 和 ResNet。

> 两个数据集：ImageNet 和 WordNet。

### 创建网络

- 在 `torchvision.models` 中找到预训练的模型。
- 创建该模型的实例。

```python
from torchvision import models
alexnet = models.AlexNet()
```

现在我们已经可以直接调用 `alexnet` 来使用它了。但是，它并没有经过训练。

```python
resnet = models.resnet101(pretrained=True)
resnet
```

`pretrained=True` 会自动下载预训练模型的权重。它的输出描述了整个神经网络的结构，每一行是一层。

### 数据预处理

使用 `torchvision` 包中的 `transforms` 模块来预处理数据。定义一个预处理函数管道 `preprocess`。

数据预处理应当与训练期间向网络提供的内容相匹配。

### 推理（运行模型，inference）

```python
img_t = preprocess(img)
import torch
batch_t = torch.unsqueeze(img_t, 0)
resnet.eval()
out = resnet(batch_t)
out
#...
_, index = torch.max(out, 1)
labels[index[0]]
```

首先将图像预处理成了张量，然后按网络期望的方式对输入张量进行重塑、裁剪和归一化。`torch.unsqueeze` 扩充了 `batch` 的维度，使其符合网络的输入要求。

接下来将网络置于 `eval` 模式，开始推理。产生的结果 `out` 是一个拥有 1000 个分数的向量，每个 ImageNet 类对应一个分数。

接下来加载标签文件。

最后使用 `torch.max` 确定最大分数的索引。需要注意其返回的是一个一维张量，因此需要使用 `index[0]` 得到数字。

### 其他预训练网络

还介绍了 GAN、CycleGAN 和 NeuralTalk。大致流程一样，请参看模型的文档。

### Torch Hub

`torch.hub` 包含了一些预训练模型，可以直接使用。它们的 GitHub 根目录下都有一个 `hubconf.py`，我们需要查看该文件知道这个模型的入口点。

```python
import torch
from torch import hub
resnet18_model = hub.load('pytorch/vision:master', 'resnet18', pretrained=True)
```

Torch Hub 不会自动安装缺少的依赖项，但会报告。

## 张量

不论你在线性代数等数学课上学到了什么张量的概念，在 PyTorch 中，张量就是多维数组。它和 NumPy 可以无缝对接，接口上也很相似。与 NumPy 相比，张量能够在 GPU 上更快地执行运算、支持自动求导、支持分布式计算。

关于张量的创建、操作、视图等概念不再介绍，均与 NumPy 类似，需要时查看文档即可。

### 在 GPU 上操作张量

> PyTorch 现已支持 CUDA 和 ROCm 设备）

除了 `dtype`，张量还有 `device` 概念，即张量存储的位置。

```python
points_gpu = torch.tensor([[4, 1], [5, 3], [2, 1]], device='cuda')
points_gpu = points.to(device='cuda')
```

上面两种方式都可以在 GPU 上创建张量。如果有多个 GPU，可以使用 `cuda:0`、`cuda:1` 等指定设备。

要输出或访问得到的张量时，应当将张量移回 CPU。

对于上面的方式，有简写的 `cpu()` 和 `cuda()` 方法。

```python
points_gpu = points.cuda()
points_gpu = points.cuda(0)
points_cpu = points_gpu.cpu()
```

### 与 NumPy 互操作

只需要调用 `numpy()` 和 `from_numpy()` 方法即可。

```python
points_np = points.numpy()
points = torch.from_numpy(points_np)
```

NumPy 数组都存储在 CPU 上，也就是说 GPU 上的张量会被拷贝。

修改 NumPy 数组会导致原始张量被修改，反之亦然。

值得注意的是：PyTorch 默认数字类型为 32 位浮点数，NumPy 默认为 64 位。我们需要确保张量是 `torch.float` 类型的。

## 构建一个线性模型

**学习就是参数估计。**本节使用线性模型的拟合过程展示 PyTorch 的自动求导机制。

- 首先准备数据
- 然后选择模型，我们的模型是 $y=wx+b$，需要学习 $w$ 和 $b$。
- 选择一个合适的损失函数。对于线性模型，可以选用误差的绝对值或平方。

上面步骤写成 Python 函数：

```python
def model(t_u, w, b):
    return w * t_u + b
def loss_fn(t_p, t_c):
    squared_diffs = (t_p - t_c)**2
    return squared_diffs.mean()
```

- 接下来就是计算损失函数对参数的导数，即梯度。需要注意的是，这一步我们使用了链式法则：先计算损失函数对其输入（模型的输出）的导数，再乘模型对参数的导数。

> 为什么要用链式法则？这涉及到了计算图的概念。

- PyTorch 提供了 `torch.autograd` 模块来自动计算梯度。我们将模型的参数配置为具有 `requires_grad=True` 属性的张量，即可利用自动求导引擎。

```python
params = torch.tensor([1.0, 0.0], requires_grad=True)
loss = loss_fn(model(t_u, *params), t_c)
loss.backward()
params.grad
```

> 如何理解上面的 `backward()` 方法？它将从 `loss` 张量开始，沿着计算图反向移动，计算其中 `requires_grad=True` 的张量的梯度。这些梯度累加到张量的 `grad` 属性中。

- 每次迭代时，应当将梯度归零，使用 `grad.zero_()` 方法。

现在我们可以写出一个完整的自动求导训练代码：

```python
def training_loop(n_epochs, learning_rate, params, t_u, t_c):
    for epoch in range(1, n_epochs + 1):
        if params.grad is not None:
            params.grad.zero_()
        t_p = model(t_u, *params)
        loss = loss_fn(t_p, t_c)
        loss.backward()
        with torch.no_grad():
            params -= learning_rate * params.grad
        if epoch % 500 == 0:
            print('Epoch %d, Loss %f' % (epoch, float(loss)))
    return params

training_loop(
    n_epochs = 5000,
    learning_rate = 1e-2,
    params = torch.tensor([1.0, 0.0], requires_grad=True),
    t_u = t_u,
    t_c = t_c)
```

> - 在 `with` 块中，使用 `torch.no_grad()` 来关闭自动求导引擎，不在更新参数时让 PyTorch 为计算图添加前向边。


## 优化器

优化器将从用户代码中提取优化策略，不需要我们自己更新模型中的每个参数。

### 构造优化器

SGD 是梯度下降的一种变体，它使用了动量（momentum）的概念。接下来创建一个 SGD 优化器。

每个优化器构造函数都接收一个参数列表作为第一个输入。

```python
import torch.optim as optim
optimizer = optim.SGD([params], lr=learning_rate)
```

### 使用优化器

上面的训练代码部分改写为：

```python
optimizer.zero_grad()
loss.backward()
optimizer.step()
```

在调用 `step()` 时，参数的值会被更新。需要注意的是，每次迭代时应当将梯度归零，可以使用 `optimizer.zero_grad()` 和 `optimizer.step.zero_grad()` 方法。

## 验证

训练模型时，我们不会使用全部的数据，而是分为训练集和验证集。我们在训练集上对模型的参数进行优化（训练），将训练后的模型在验证集上进行评估。

### 过拟合

**如果训练损失和验证损失发散，就表明出现了过拟合。**

解决过拟合有一些方法：

- 在损失函数中添加惩罚项，使其表现更平稳。
- 在输入样本中添加噪声。

### 分割数据集

`randperm()` 函数可以用于打乱张量的元素：

```python
shuffled_indices = torch.randperm(n_samples)
val_indices = shuffled_indices[:n_val]
```

它生成随机索引，然后我们可以使用切片操作将数据集分割为训练集和验证集。

### 训练和验证

现在我们可以在训练循环内加上验证步骤，同时，记得关闭验证步骤的自动求导引擎，创建计算图会带来开销。

```python
with torch.no_grad():
    val_loss = loss_fn(model(t_u_val, *params), t_c_val)
    assert val_loss.requires_grad == False
```

## 构建一个神经网络

上面我们构建了一个简单的线性模型，了解了训练的基本流程。接下来我们终于可以构建一个神经网络。与线性模型相比，神经网络的优势在于激活函数是非线性的，因此可以拟合非线性函数。


**不管基础模型是什么，流程都是一样的。**

### 使用 `torch.nn`

使用 `torch.nn` 包中的 `Module` 类替换掉上面的线性模型即可。

```python
import torch.nn as nn
import torch.optim as optim
linear_model = nn.Linear(1, 1)
x = torch.tensor([1.0])
y = linear_model(x)
print(y)

optimizer = optim.SGD(linear_model.parameters(), lr=0.01)

def training_loop(n_epochs, optimizer, model, loss_fn, x, y):
    for epoch in range(1, n_epochs + 1):
        y_pred = model(x)
        loss = loss_fn(y_pred, y)
        optimizer.zero_grad()
        loss.backward()
        optimizer.step()
        if epoch % 10 == 0:
            print('Epoch %d, Loss %f' % (epoch, float(loss)))
    return y_pred
```

它有一些方便之处：

- 可以处理批次，默认第 0 维是批次维。
- 可以通过 `parameters()` 方法访问模型的参数。

### 连接模型构建神经网络

可以使用 `nn.Sequential` 容器连接模型来构建一个新模型：

```python
seq_model = nn.Sequential(
            nn.Linear(1, 13),
            nn.Tanh(),
            nn.Linear(13, 1))
```

检查一下它的 `parameters()` 看看优化器会得到哪些张量？还可以通过名称识别参数。

```python
[param.shape for param in seq_model.parameters()]
for name, param in seq_model.named_parameters():
    print(name, param.shape)
```

## 数据准备

本节使用 CIFAR-10 为例。

### 下载数据集

使用 `datasets` 模块下载数据集。

```python
from torchvision import datasets
data_path = '../data-unversioned/p1ch7/'
cifar10 = datasets.CIFAR10(data_path, train=True, download=True)
cifar10_val = datasets.CIFAR10(data_path, train=False, download=True)
```

> 上面创建了两个数据集，但只执行了一次下载。也就是说，下载时都会下载完整的数据集。

数据集都是从 `torch.utils.data.Dataset` 类继承的。

### Dataset 类

`Dataset` 类是一个抽象类，我们需要继承它并实现 `__len__` 和 `__getitem__` 方法，分别返回数据集的大小和索引对应的数据。

### 变换

`torchvision` 包中的 `transforms` 模块提供了一些常用的变换。把它传递给数据集，数据集就会在 `__getitem__` 方法返回前先应用变换。

以 CIFAR-10 中图片到标准张量的变换为例：

```python
from torchvision import transforms
tensor_cifar10 = datasets.CIFAR10(
    data_path, train=True, download=False,
    transform=transforms.ToTensor())
```

### 归一化

在前面的训练优化过程中提及过归一化的操作。归一化的原因是：如果数据的尺度不同，那么损失函数的梯度也会不同，这会导致训练过程变得困难。比如对于图像数据，其**三个通道**应当进行归一化，使其具有相同分布，在学习率相同的情况下，实现**通道信息的混合和更新**。

对数据离线计算均值和标准差后，可以使用 `transforms.Normalize` 变换来归一化数据。

```python
transformed_ciifar10 = datasets.CIFAR10(
    data_path, train=True, download=False,
    transform=transforms.Compose([
        transforms.ToTensor(),
        transforms.Normalize((0.4915, 0.4823, 0.4468),
                             (0.2470, 0.2435, 0.2616))
    ]))
```

## 书本外的知识

### 分布式训练

https://docs.pytorch.org/tutorials/beginner/dist_overview.html
