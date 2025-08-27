# CSK6官方文档获取指南

## 📚 本地文档

本项目已包含完整的CSK6官方文档，位于：
- `resources/references/docs-csk6/` - 完整文档目录（439个文件）

## 🔄 获取最新文档

要获取最新的官方文档，请使用以下方法：

### 方法1：直接克隆官方仓库
```bash
# 删除现有文档
rm -rf resources/references/docs-csk6

# 重新克隆最新文档
git clone --depth 1 https://github.com/LISTENAI/docs-csk6.git resources/references/docs-csk6

# 移除Git信息以避免嵌套仓库
rm -rf resources/references/docs-csk6/.git
```

### 方法2：作为子模块（推荐）
如果您想将文档作为子模块管理：

```bash
# 删除现有目录
rm -rf resources/references/docs-csk6

# 添加为子模块
git submodule add https://github.com/LISTENAI/docs-csk6.git resources/references/docs-csk6

# 初始化和更新子模块
git submodule init
git submodule update
```

### 方法3：用户克隆时自动获取
用户克隆项目时，使用以下命令自动获取所有子模块：

```bash
# 克隆项目并获取子模块
git clone --recursive https://github.com/aixier/hardware-intelligence-agent.git

# 或者先克隆再更新子模块
git clone https://github.com/aixier/hardware-intelligence-agent.git
cd hardware-intelligence-agent
git submodule update --init --recursive
```

## 📖 文档结构

包含的主要文档模块：

### 核心文档
- `overview/` - 系统概览和芯片介绍
- `quick_start/` - 快速入门指南
- `application/` - 应用开发文档

### 开发工具
- `tool/lisa_plugin_zephyr/` - lisa-zep工具文档
- `tool/vscode_plugin/` - VS Code插件文档

### AI功能
- `ai_usage/` - AI功能使用指南
  - `fd/` - 人脸检测
  - `hsgd/` - 语音识别
  - `xtts/` - 语音合成

### 硬件资源
- `hardware/` - 硬件配置和外设
- `gdbdebug/` - 调试和烧录
- `build/` - 构建系统和配置

### 系统服务
- `kernel/` - 内核服务
- `service/` - 系统服务

### FAQ和帮助
- `FAQ/` - 常见问题解答

## 🔗 官方链接

- **官方仓库**: https://github.com/LISTENAI/docs-csk6
- **聆思科技**: https://www.listenai.com
- **开发者社区**: https://open.listenai.com

## ⚠️ 注意事项

1. 文档会定期更新，建议定期同步最新版本
2. 如果遇到网络问题，可以使用镜像站点或VPN
3. 本地文档版本信息可查看 `resources/references/docs-csk6/` 目录的最后修改时间

---

*最后更新: 2025-08-27*