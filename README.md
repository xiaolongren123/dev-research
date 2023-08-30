# 方块连连研发测试题
这是一份针对嵌入式工程师的研发测试题，是一个基于`ESP32`平台的简单`DEMO`研发任务。
可能需要花费你1小时到数天的时间来解决问题，花费的具体时间取决于你对前置知识的熟悉程度。

这是一份**开放式**试题，你可以寻求互联网的帮助，但请不要让别人代劳。

# 目标
+ 这份试题并非要为难你，也不是要获取廉价解决方案，希望你能通过这份工作了解我们的工作习惯和常用工具，也测试一下你对基本知识的了解

# 做题的硬件要求
## 要求
+ 一台PC，里面安装`Windows`或者`Linux`系统，`MacOS`也可
+ 一个`ESP32S2`或`ESP32S3`开发模组,以便实现试题要求，模组或开发板需要你自行购买，如果成功入职，公司会为你报销模组或开发板的购买费用。

## 参考购买链接

+ 淘宝：[ESP32S3-WROOM-N8-R8](https://item.taobao.com/item.htm?spm=a21n57.1.0.0.792d7e4cr8fqAx&id=669443108979&ns=1&abbucket=2#detail)


# 一些前置知识
+ 我们使用[Git](https://git-scm.com/)和[Github](https://github.com)来管理代码，你需要简单的学习 `Git`的使用
  + 中文教程，可以参考[这里](https://www.liaoxuefeng.com/wiki/896043488029600)
  + Git分支：可以参考这个[互动教程](https://learngitbranching.js.org/)
+ [ESP32S3环境配置文档](https://docs.espressif.com/projects/esp-idf/zh_CN/v4.4/esp32s3/get-started/index.html)

# 题目基本要求
每个题目都有一些独立的小需求，请你逐个实现，并放置到指定文件夹，每个题目请你都生成一个单独的提交。

基本步骤：
+ `clone` 本项目到你的本地计算机
+ 实现每个题目的要求，并逐个提交
+ 将提交推送你的github项目内，并发送链接给我

每个项目都必须是一个ESP-IDF标准的`main-compoent`，请参考 `ESP-IDF` 内的 `hello_world`项目

# 题目
## 题目一
+ 文件夹：puzzle1
+ 考点：WIFI模块基本网络功能，基本的TCP/IP和HTTP

实现功能:

+ 启动后连接WIFI，WIFI账号密码分别为"UPGRADE_AP","TEST1234"，你可以用手机热点的方式模拟WIFI账号密码
+ 连接上WIFI后，访问URL：`https://dummyjson.com/products/1` 获取其中字段 "brand" 的值，并使用 `ESP_LOGI` 或者其他输出函数输出到调试终端（串口）
+ 关闭WIFI

额外要求：

使用一个动态分配的内存来存储URL获取的内容，并妥善处理善后。

## 题目二
+ 文件夹：puzzle2
+ 考点：FLASH访问和文件系统使用

在问题一的基础上实现功能：

+ 将URL获取的到的数据，存储到FLASH文件系统内，存储文件名为`hello_this_is_long_name_products.json`


## 题目三（选做）
+ 文件夹：puzzle3
+ 多线程和事件驱动
+ 额外硬件需求：三根杜邦线和一个额外的串口工具，用于连接ESP32的额外串口，用于输入输出

在问题二的基础上实现功能：

+ 实现一个额外的串口通信功能：在串口接收输入，如果用户输入字符串`fetch`，则连接WIFI，并将题目一URL的内容取回，存储到文件系统，并将所有内容显示到串口输出。

# 致谢

谢谢你的耐心，期待与你共事


+ 将调试串口改造为不输出log




