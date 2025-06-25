# Linux 桌面环境

## 桌面环境

### GTK 与 Qt

### GNOME 与 KDE

## 输入法

### 输入法框架

!!! quote

    - [Fcitx or Ibus on KDE, especially in Wayland : r/kde](https://www.reddit.com/r/kde/comments/x3wg7d/fcitx_or_ibus_on_kde_especially_in_wayland/)

输入法框架应当根据桌面环境选择。根据个人使用经验，在 KDE 和 GNOME 下应选择 fcitx5。

### Rime 和 Rime-ice

!!! quote

    - [iDvel/rime-ice: Rime 配置：雾凇拼音 | 长期维护的简体词库](https://github.com/iDvel/rime-ice)

!!! TOOD

    配置 rime-ice

```shell
apt install fcitx5-rime
git clone https://github.com/iDvel/rime-ice.git Rime --depth 1
```

## 中文相关

### 字形显示问题

!!! quote

    - [Localization/Simplified Chinese - ArchWiki](https://wiki.archlinux.org/title/Localization/Simplified_Chinese)
    - [Linux 下的字体调校指南 - Leo's Field](https://szclsya.me/zh-cn/posts/fonts/linux-config-guide/)
    - [[Bug]: Some flatpak apps can't choose right font for specific language when using ttc or otc fonts · Issue #4865 · flatpak/flatpak](https://github.com/flatpak/flatpak/issues/4865)

- 原因：默认字体按照字典序排列，`ja-JP` 在 `zh_...` 前，所以显示为日文字形。
- 解决办法：

    - 用户级别：

        创建 `~/.config/fontconfig/fonts.conf` 文件，指定默认字体族。

    - Flatpak：

        为特定应用添加环境变量 `LC_ALL=zh_CN.UTF-8`

## Flatpak

GUI 应用程序无脑选择 Flatpak。

例外：

- Steam：应当使用发行版官方打包版本。Flatpak 的容器化会导致 Wine 出问题。

## 零散问题

- 切换是否启动图形界面：

    ```text
    systemctl set-default multi-user.target
    systemctl set-default graphical.target
    ```
