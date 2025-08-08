# 代码管理

## Git

!!! quote

    - 1 分钟上手 git：[git - 简明指南](https://rogerdudler.github.io/git-guide/index.zh.html)
    - 学习使用 GitHub：[GitHub Quickstart](https://docs.github.com/en/get-started/quickstart/hello-world) | [中文版：GitHub 快速入门](https://docs.github.com/zh/get-started/quickstart/hello-world)
    - 深入理解 Git：[Pro Git](https://git-scm.com/book/en/v2) | [中文版](https://www.progit.cn/)

### rebase

```bash
git rebase <target-branch>
git rebase --continue
```

### bisect

!!! quote

    - [Git - git-bisect Documentation](https://git-scm.com/docs/git-bisect)

### CRLF 问题

!!! quote

    - [git 多平台统一换行符](https://juejin.cn/post/6844903591258357773)
    - [Configuring Git to handle line endings - GitHub Docs](https://docs.github.com/en/get-started/getting-started-with-git/configuring-git-to-handle-line-endings)
    - [git - What is the purpose of `text=auto` in `.gitattributes` file? - Stack Overflow](https://stackoverflow.com/questions/21472971/what-is-the-purpose-of-text-auto-in-gitattributes-file)

创建仓库时，就应当添加 `.gitattributes`，强制所有人使用统一的换行符提交：

```text
# Set the default behavior, in case people don't have core.autocrlf set.
* text=auto

# Explicitly declare text files you want to always be normalized and converted
# to native line endings on checkout.
*.c text
*.h text

# Declare files that will always have CRLF line endings on checkout.
*.sln text eol=crlf

# Denote all files that are truly binary and should not be modified.
*.png binary
*.jpg binary
```

### 更换端口

!!! quote

    - [ssh: connect to host github.com port 22: Connection refused - Ask Ubuntu](https://askubuntu.com/questions/610940/ssh-connect-to-host-github-com-port-22-connection-refused)

某些情况下受网络限制无法访问服务器 22 端口，可以将 `git://` 链接改为 `ssh://` 链接，走其他端口，如下：

```shell
git remote set-url origin ssh://git@ssh.github.com:443/yourname/reponame.git
```

### commit

常见类型：

| 类型 | 描述 |
| --- | --- |
| **feat 功能** | 引入了新功能 |
| **fix 修复** | 修复了一个错误 |
| **chore 琐事** | 不涉及修复或功能的更改，不修改 src 或测试文件（例如更新依赖项） |
| **refactor 重构** | 重构代码，不修复错误也不添加功能 |
| **docs 文档** | 更新文档，例如 README 或其他 markdown 文件 |
| **style 风格** | 不影响代码含义的更改，可能与代码格式相关，例如空格、缺少分号等 |
| **test 测试** | 包括新的或更正以前的测试 |
| **perf 性能** | 性能改进 |
| **ci 持续集成** | 持续集成相关 |
| **build 生成** | 影响构建系统或外部依赖项的更改 |
| **revert 恢复** | 恢复以前的提交 |

[Magic Keywords](https://stackoverflow.com/questions/58525836/git-magic-keywords-in-commit-messages-signed-off-by-co-authored-by-fixes):

| 关键词 | 说明 |
| --- | --- |
| `Signed-off-by` | `git commit -s` 用于签署提交，以表明作者同意发布该提交 |
| `Co-authored-by` | 用于添加其他作者 |
| `Reviewed-by` | 用于添加审查者 |
| `Reported-by` | 用于添加报告者 |
| `Helped-by` | 用于添加帮助者 |
