/*
 * CSK6 音频采集示例
 * 功能：麦克风数据采集和基础音频处理
 * 作者：CSK6开发团队
 * 时间：2025-08-27
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2s.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(audio_capture, LOG_LEVEL_INF);

/* 音频配置参数 */
#define SAMPLE_RATE         16000    /* 采样率 16kHz */
#define CHANNELS            2        /* 双声道 */
#define SAMPLE_BITS         16       /* 16位采样 */
#define FRAME_SIZE          1024     /* 帧大小 */
#define BUFFER_SIZE         (FRAME_SIZE * CHANNELS * sizeof(int16_t))

/* I2S设备 */
#define I2S_DEV_NODE DT_ALIAS(i2s0)
static const struct device *i2s_dev = DEVICE_DT_GET(I2S_DEV_NODE);

/* 音频缓冲区 */
static int16_t audio_buffer[2][FRAME_SIZE * CHANNELS];
static uint8_t current_buffer = 0;

/* 音频统计信息 */
static uint32_t frames_captured = 0;
static uint32_t total_samples = 0;

/**
 * 音频数据处理函数
 */
void process_audio_frame(int16_t *buffer, size_t samples)
{
    int32_t sum_left = 0, sum_right = 0;
    int16_t max_left = 0, max_right = 0;
    
    /* 计算音频统计信息 */
    for (size_t i = 0; i < samples; i += 2) {
        int16_t left = buffer[i];
        int16_t right = buffer[i + 1];
        
        sum_left += abs(left);
        sum_right += abs(right);
        
        if (abs(left) > max_left) max_left = abs(left);
        if (abs(right) > max_right) max_right = abs(right);
    }
    
    /* 计算平均幅度 */
    int16_t avg_left = sum_left / (samples / 2);
    int16_t avg_right = sum_right / (samples / 2);
    
    /* 输出统计信息 */
    if (frames_captured % 100 == 0) {  /* 每100帧输出一次 */
        LOG_INF("音频统计 [帧:%d] L-平均:%d,峰值:%d | R-平均:%d,峰值:%d", 
                frames_captured, avg_left, max_left, avg_right, max_right);
    }
    
    /* 检测音频活动 */
    if (max_left > 1000 || max_right > 1000) {
        LOG_INF("检测到音频活动! 峰值: L=%d, R=%d", max_left, max_right);
    }
}

/**
 * I2S配置函数
 */
int configure_i2s(void)
{
    struct i2s_config config;
    int ret;

    if (!device_is_ready(i2s_dev)) {
        LOG_ERR("I2S设备未准备好");
        return -ENODEV;
    }

    /* 配置I2S参数 */
    config.word_size = SAMPLE_BITS;
    config.channels = CHANNELS;
    config.format = I2S_FMT_DATA_FORMAT_I2S;
    config.options = I2S_OPT_BIT_CLK_SLAVE | I2S_OPT_FRAME_CLK_SLAVE;
    config.frame_clk_freq = SAMPLE_RATE;
    config.mem_slab = NULL;  /* 使用应用程序缓冲区 */
    config.block_size = BUFFER_SIZE;
    config.timeout = SYS_FOREVER_MS;

    ret = i2s_configure(i2s_dev, I2S_DIR_RX, &config);
    if (ret < 0) {
        LOG_ERR("I2S配置失败: %d", ret);
        return ret;
    }

    LOG_INF("I2S配置成功 - 采样率:%dHz, 通道:%d, 位深:%d位", 
            SAMPLE_RATE, CHANNELS, SAMPLE_BITS);

    return 0;
}

/**
 * 音频采集任务
 */
void audio_capture_task(void)
{
    void *mem_block;
    size_t block_size;
    int ret;

    LOG_INF("开始音频采集任务");

    /* 启动I2S接收 */
    ret = i2s_trigger(i2s_dev, I2S_DIR_RX, I2S_TRIGGER_START);
    if (ret < 0) {
        LOG_ERR("启动I2S接收失败: %d", ret);
        return;
    }

    while (1) {
        /* 读取音频数据 */
        ret = i2s_read(i2s_dev, &mem_block, &block_size);
        if (ret < 0) {
            LOG_ERR("I2S读取失败: %d", ret);
            k_msleep(100);
            continue;
        }

        /* 处理音频数据 */
        if (mem_block && block_size >= sizeof(int16_t)) {
            size_t samples = block_size / sizeof(int16_t);
            process_audio_frame((int16_t *)mem_block, samples);
            
            frames_captured++;
            total_samples += samples;
        }

        /* 释放缓冲区 */
        ret = i2s_buf_release(i2s_dev, I2S_DIR_RX, mem_block);
        if (ret < 0) {
            LOG_ERR("释放I2S缓冲区失败: %d", ret);
        }

        /* 让出CPU时间 */
        k_yield();
    }
}

/**
 * 主函数
 */
void main(void)
{
    int ret;

    LOG_INF("CSK6 音频采集示例启动");
    LOG_INF("配置: %dHz, %d通道, %d位", SAMPLE_RATE, CHANNELS, SAMPLE_BITS);

    /* 配置I2S */
    ret = configure_i2s();
    if (ret < 0) {
        LOG_ERR("I2S配置失败，程序退出");
        return;
    }

    /* 等待系统稳定 */
    k_msleep(1000);

    LOG_INF("开始音频采集，按Ctrl+C停止");

    /* 启动音频采集 */
    audio_capture_task();
}

/**
 * 音频采集线程
 */
K_THREAD_DEFINE(audio_capture_tid, 2048, audio_capture_task, NULL, NULL, NULL,
                K_PRIO_PREEMPT(5), 0, 0);