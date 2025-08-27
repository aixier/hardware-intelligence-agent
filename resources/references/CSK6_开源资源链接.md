# CSK6 开源资源链接收集

## 收集时间
2025-08-27

## GitHub 仓库

### 官方仓库
1. **LISTENAI/docs-csk6**
   - 链接：https://github.com/LISTENAI/docs-csk6
   - 描述：聆思科技官方CSK6文档仓库
   - 状态：3 stars, 5 forks
   - 内容：官方开发文档和指南

2. **aibittek/csk6_apps**
   - 链接：https://github.com/aibittek/csk6_apps
   - 描述：CSK6应用示例代码
   - 内容：不同CSK6开发板的应用示例
   - 开发板支持：包含网络模块、WiFi AP演示等

## 官方文档中心

### Zephyr 项目文档
基于聆思定制的Zephyr文档：https://zephyr-docs.listenai.com/

#### 示例应用
1. **网络示例**
   - CSK6 WiFi SoftAP：https://zephyr-docs.listenai.com/samples/boards/csk6/network/wifi_softap/README.html
   - CSK6 WiFi Station：https://zephyr-docs.listenai.com/samples/boards/csk6/network/wifi_scan_ap/README.html
   - CSK6002 WiFi Manager：https://zephyr-docs.listenai.com/samples/boards/csk6/network/wifi_mgr/README.html

2. **驱动示例**
   - SOC Flash示例：https://zephyr-docs.listenai.com/samples/boards/csk6/driver/soc_flash_listenai/README.html
   - DMA内存复制：https://zephyr-docs.listenai.com/samples/boards/csk6/driver/dma_memcpy/README.html

3. **应用示例**
   - 启动示例：https://zephyr-docs.listenai.com/samples/boards/csk6/apps/startup/README.html

## 技术社区资源

### 论坛讨论
1. **硬汉嵌入式论坛**
   - 标题：聆思CSK6大模型开发板英语评测类开源SDK详解
   - 链接：https://forum.anfulai.cn/forum.php?mod=viewthread&tid=124493

2. **知乎技术文章**
   - 标题：【聆思CSK6视觉AI开发套件试用】从部署工程师和测试工程师角度体验CSK6
   - 链接：https://zhuanlan.zhihu.com/p/599715123

3. **极术社区**
   - 标题：【聆思CSK6 视觉AI开发套件试用】csk6简单使用小记
   - 链接：https://aijishu.com/a/1060000000429746

## 开发工具和SDK

### 官方下载中心
- 聆思下载中心：https://www.listenai.com/download/
- 一键安装包：csk6_integration_installer_v1.6.5.exe
- 支持系统：Windows, Linux, macOS

### 开发环境要求
- git（需添加到环境变量）
- CSK6一键安装包
- Zephyr RTOS开发环境

### 创建示例项目命令
```bash
lisa zep create --from-git https://cloud.listenai.com/zephyr/applications/app_algo_hsd_sample_for_csk6.git
```

## 硬件特性

### CSK6-MIX开发套件
- 芯片：CSK6011A，板载16MB Flash
- 外设：摄像头、麦克风、扬声器、屏幕等
- 调试器：板载DAPLink调试器
- 接口：SPI、UART、USB、SDIO、I2C、I2S

### 核心特性
- 多核异构架构：ARM Star MCU + HiFi4 DSP + NPU
- AI计算能力：128 GOPS
- 操作系统：Zephyr RTOS
- 双固件架构：AP+CP

## 开发支持

### 官方支持渠道
- 文档中心：https://docs2.listenai.com/
- API文档：原 https://open.listenai.com/csksdk_api (已重定向)
- 技术支持：官方邮箱和论坛

### 社区支持
- GitHub Issues：项目问题反馈
- 技术论坛：开发者交流
- 知乎专栏：技术文章分享

## 学习路径建议

### 初学者
1. 下载安装CSK6开发工具
2. 阅读官方文档和入门指南
3. 运行基础示例项目
4. 学习Zephyr RTOS开发

### 进阶开发
1. 研究WiFi和网络功能
2. 学习音频处理和AI算法
3. 开发自定义应用
4. 参与开源项目贡献

## 注意事项
1. 部分仓库可能需要特殊网络访问
2. 官方资源可能需要注册账号
3. 开发板和工具包需要单独购买
4. 建议关注官方更新和社区动态

---
**最后更新时间**: 2025-08-27
**状态**: 持续更新中