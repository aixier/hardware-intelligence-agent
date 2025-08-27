# CSK6 API参考文档收集

## 收集时间
2025-08-27

## API文档需求

### 1. 系统API
- **系统初始化**
  - system_init()
  - clock_config()
  - power_management()

- **内存管理**
  - malloc/free函数
  - 内存池管理
  - DMA缓冲区

### 2. 硬件驱动API
- **GPIO API**
  - gpio_init()
  - gpio_set/get()
  - gpio_interrupt()

- **UART API**
  - uart_init()
  - uart_send/recv()
  - uart_config()

- **I2C/SPI API**
  - i2c_init()
  - spi_transfer()
  - 错误处理

### 3. 音频API
- **音频采集**
  - audio_capture_init()
  - audio_read_data()
  - audio_set_params()

- **音频播放**
  - audio_play_init()
  - audio_write_data()
  - audio_volume_control()

### 4. AI功能API
- **语音识别**
  - speech_recognition_init()
  - speech_process()
  - speech_result_get()

- **唤醒词检测**
  - wakeword_init()
  - wakeword_detect()
  - wakeword_config()

## 文档格式要求
- 函数原型定义
- 参数说明
- 返回值说明
- 使用示例
- 错误码定义

## 获取状态
**状态**: 待收集
**来源**: 官方SDK文档