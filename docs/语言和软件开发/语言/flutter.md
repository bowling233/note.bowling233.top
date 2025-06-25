# Flutter

## Dart 语言

在阅读 Flutter 框架前，先了解 Dart 语言。最好的入门教程是官方教程。

个人还是不太熟悉偏函数式的语言，简单记一些笔记：

### 函数

语法糖 `=>` 用于单语句函数。

```dart
flybyObjects.where((name) => name.contains('turn')).forEach(print);
```

中间的函数参数为 `name`，返回值为 `name.contains('turn')`，均为类型推断。

### 变量

- `var` 由初值推断类型
- `Object` 和 `dynamic`，dynamic 将在运行时检查类型。
- 空安全：防止空指针异常，在访问数据和调用时都要考虑。
    - 声明类型时使用 `?` 允许为空。此后访问时使用 `?.` 避免空指针异常。
    - 不可空必须初始化，可空默认为 `null`。
    - `late` 延迟初始化，不可空，初次访问前必须初始化。用于**编程者确定**变量在访问前已经初始化。否则容易触发运行时异常。
    - `final` 只能赋值一次，`const` 是编译时常量。
- `enum` 枚举，可以当作类一样写，有构造函数，但成员全 `final`。

一些新的运算符用到再查。

### 类

所有类型都是对象，包括基本类型。

- getter 方法：用于只读但并不是不可变量的属性：`int get age => _age;`
- 命名构造函数 `Point.origin(this.x, this.y);`
- mixin 用于多继承共享代码，理解为公用接口。
- abstract 抽象类，需要 `implements` 实现。

最最最离谱的来了，`_` 开头的变量对于 library 是私有的，没有关键字。

内置类型，C++ 中的 Vector 在这里是 List，更类似数组。

- `record` 简单匿名聚合类，多个东西用括号括起就行，比如 `(int, int) swap ((int, int) record)`。有位置和命名参数。有趣的是名称是 Record 的一部分，名称不同类型不同。
- `List []`，`{}` 默认为 Map 如 `Map<String, String>{}`，`Set` 需要显式 `<String>{}`。
- Spread 操作符 `...`，`[...list1, ...list2]` 方便插入多个值。

类型：

- 成员称为 instance variable，非空的必须初始化。



### 异步

- `async`

### 控制流

- 异常与 C++ 类似。

### 库

每个 dart 文件就是一个库。

- 内置：`dart:`
- 第三方：`package:`
- 本地：URL。

## Flutter 框架

参考 [《Flutter 实战·第二版》](https://book.flutterchina.club/)

- Widget
    - 在 widget 树中获取 State 对象：如果 StatefulWidget 的状态是希望暴露出的，应当在 StatefulWidget 中提供一个 of 静态方法来获取其 State 对象。`ScaffoldState _state=Scaffold.of(context);`
    - 生命周期中的回调函数：
        - `build` 在调用 initState() 之后，在调用 didUpdateWidget() 之后。，在调用 setState() 之后。，在调用 didChangeDependencies() 之后。，在 State 对象从树中一个位置移除后（会调用 deactivate）又重新插入到树的其他位置之后。
- 状态
    - 父 Widget 管理子 Widget 的状态：通过回调将其状态导出到其父组件，状态由父组件管理父 widget 指定 `onChanged: _handleTapboxChanged,`，子 widget 在对应事件调用 `onChanged`，如 `onTap: _handleTap => onChanged(),`。
    - 全局状态管理：实现一个全局的事件总线，将语言状态改变对应为一个事件，然后在 APP 中依赖应用语言的组件的 initState 方法中订阅语言改变的事件。收到通知后调用 setState(...) 方法重新 build 一下自身即可。
- 资源管理
    - 直接使用 package:flutter/services.dart 中全局静态的 rootBundle 对象来加载 asset 即可。`return await rootBundle.loadString('assets/config.json');`

## Android

### Gradle

命令行：

```bash
./gradlew clean
./gradlew assembleDebug
```

Android 开发中遇到各种奇奇怪怪的问题都可能是 Gradle 版本和 Java 版本不兼容导致的，因此每到一个环境下首先检查 Java 和 Gradle 版本：

- 使用 `flutter doctor --verbose` 查看 Flutter 所使用的 Java 版本。
- 根据 Java 版本查看 Gradle 兼容性矩阵，确定 Gradle 版本。
- 修改 `gradle-wrapper.properties` 中的 Gradle 版本，即更改 `distributionUrl`。
- 如果遇到奇怪的问题，可以尝试先升级 Gradle 版本，反正高版本 Gradle 兼容比较旧的 Java，而旧的 Gradle 不兼容新的 Java。而且新的 Gradle 报错和提示信息都更好。

### 编译 `flutter_gl` Demo 遇到过的问题

- 首先升级 Flutter 依赖。
- 修复 [How do I fix 'namespace not specified' error in Android Studio?](https://stackoverflow.com/questions/76108428/how-do-i-fix-namespace-not-specified-error-in-android-studio)，参考 <https://developer.android.com/build/configure-app-module?hl=zh-cn#kts>
- 将 Gradle Wrapper 升级到 8.9。
- 升级 Gradle 插件和 Kotlin 版本到 Flutter 最小支持的版本。
- 依赖的最小 SDK 需要升级，升级 AGP 到 8.1.1，更改 `compileSdkVersion` 到 34.
- 然后遇到 Merge Manifest 的问题，[java - android:exported needs to be explicitly specified for <activity>. Apps targeting Android 12 and higher are required to specify - Stack Overflow](https://stackoverflow.com/questions/68554294/androidexported-needs-to-be-explicitly-specified-for-activity-apps-targeting)
- 按照 [Deprecated imperative apply of Flutter's Gradle plugins | Flutter](https://docs.flutter.dev/release/breaking-changes/flutter-gradle-plugin-apply) 迁移 gradle plugin。
- [java.lang.SecurityException: Permission Denial: starting Intent { act=android.intent.action.MAIN cat=[android.intent.category.LAUNCHER] - Stack Overflow](https://stackoverflow.com/questions/26492301/java-lang-securityexception-permission-denial-starting-intent-act-android-in)
- [Inconsistent JVM-target compatibility detected for tasks despite already defining jvm ver](https://stackoverflow.com/questions/77520506/inconsistent-jvm-target-compatibility-detected-for-tasks-despite-already-definin)

## iOS

### 初次构建

参考 [Make iOS apps - Flutter](https://docs.flutter.dev/get-started/install/macos/mobile-ios#configure-your-target-ios-device)。

- 首先账户需要加入开发者计划。我已经加入，但之前看舍友用国内手机号注册而没法加入开发者计划，不知道现在有没有放宽。
- 在设备上开启开发者模式：参考 [No Developer Mode option on iOS 16… | Apple Developer Forums](https://forums.developer.apple.com/forums/thread/716697)。**注意需要连接到启动了 Xcode 的 Mac 上，否则不会显示开发者模式。**
    - 在 Mac 上启动 Xcode。
    - 连接设备。信任 Mac。
    - 在设备上点击设置 -> 隐私与安全性，在安全性中找到开发者模式。
    - 点击开启，输入密码，重启设备。
    - 重启后再次确认开启开发者模式。
- 在 Xcode 中设置设备：
    - 菜单栏 -> Window -> Devices and Simulators。设备显示在 Devices 选项卡中。
    - 点击左下角的加号，选择设备进行设置。
    - 选择开发者账户。
    - 设备同步开发信息，等待同步完成即可。
- 按照 Flutter 教程中的步骤，开启开发者签名证书，并在项目中启用。
- 第一次部署应用到设备时，会遇到设备不信任该开发者的问题。参考 [Untrusted Enterprise Apps (Developer) in iOS - Applivery](https://www.applivery.com/docs/mobile-app-distribution/troubleshooting/mobile-app-distribution-all/untrusted-enterprise-developer/#:~:text=Testers%20will%20see%20the%20message,Press%20Trust%20%5BDeveloper%5D.)，在设备上点击设置 -> 通用 -> 设备管理，信任开发者即可。
