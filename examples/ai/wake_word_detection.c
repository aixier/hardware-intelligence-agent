/*
 * CSK6 唤醒词检测示例
 * 功能：语音唤醒词检测和基础语音识别
 * 作者：CSK6开发团队
 * 时间：2025-08-27
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2s.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>
#include <string.h>
#include <math.h>

LOG_MODULE_REGISTER(wake_word, LOG_LEVEL_INF);

/* 音频配置 */
#define SAMPLE_RATE         16000
#define CHANNELS            1        /* 单声道用于语音识别 */
#define SAMPLE_BITS         16
#define FRAME_SIZE          512      /* 32ms帧 */
#define WINDOW_SIZE         400      /* 25ms窗口 */
#define HOP_SIZE            160      /* 10ms跳跃 */

/* 特征提取配置 */
#define MEL_FILTERS         26       /* Mel滤波器组数量 */
#define MFCC_COEFS          13       /* MFCC系数数量 */
#define FEATURE_FRAMES      100      /* 特征帧数量 (1秒) */

/* 唤醒词配置 */
#define WAKE_WORDS_COUNT    3
static const char* wake_words[WAKE_WORDS_COUNT] = {
    "小智",      /* 中文唤醒词 */
    "Hello",     /* 英文唤醒词 */
    "CSK6"       /* 设备名唤醒词 */
};

/* 检测阈值 */
#define DETECTION_THRESHOLD     0.7f
#define SILENCE_THRESHOLD       500
#define MIN_SPEECH_FRAMES       20

/* 音频缓冲区 */
static int16_t audio_buffer[FRAME_SIZE];
static float feature_buffer[FEATURE_FRAMES][MFCC_COEFS];
static int feature_index = 0;

/* 状态变量 */
static bool speech_detected = false;
static uint32_t speech_frame_count = 0;
static uint32_t total_detections = 0;

/**
 * 计算音频能量
 */
float calculate_energy(int16_t *buffer, size_t samples)
{
    float energy = 0.0f;
    for (size_t i = 0; i < samples; i++) {
        energy += (float)buffer[i] * buffer[i];
    }
    return energy / samples;
}

/**
 * 应用汉明窗
 */
void apply_hamming_window(float *signal, size_t length)
{
    for (size_t i = 0; i < length; i++) {
        float window = 0.54f - 0.46f * cosf(2.0f * M_PI * i / (length - 1));
        signal[i] *= window;
    }
}

/**
 * 简化的FFT实现 (仅用于演示)
 */
void simple_fft(float *real, float *imag, size_t N)
{
    /* 这里应该实现完整的FFT算法 */
    /* 为了演示，使用简化版本 */
    for (size_t i = 0; i < N; i++) {
        float re = 0, im = 0;
        for (size_t k = 0; k < N; k++) {
            float angle = -2.0f * M_PI * i * k / N;
            re += real[k] * cosf(angle) - imag[k] * sinf(angle);
            im += real[k] * sinf(angle) + imag[k] * cosf(angle);
        }
        real[i] = re;
        imag[i] = im;
    }
}

/**
 * 计算Mel频率
 */
float hz_to_mel(float hz)
{
    return 2595.0f * log10f(1.0f + hz / 700.0f);
}

float mel_to_hz(float mel)
{
    return 700.0f * (powf(10.0f, mel / 2595.0f) - 1.0f);
}

/**
 * 提取MFCC特征
 */
void extract_mfcc(int16_t *audio_frame, float *mfcc_features)
{
    static float window_buffer[WINDOW_SIZE];
    static float fft_real[WINDOW_SIZE];
    static float fft_imag[WINDOW_SIZE];
    static float power_spectrum[WINDOW_SIZE / 2 + 1];
    static float mel_filters[MEL_FILTERS];
    
    /* 数据类型转换和预加重 */
    float prev_sample = 0.0f;
    for (int i = 0; i < WINDOW_SIZE && i < FRAME_SIZE; i++) {
        float current = (float)audio_frame[i] / 32768.0f;
        window_buffer[i] = current - 0.97f * prev_sample;  /* 预加重 */
        prev_sample = current;
    }
    
    /* 应用汉明窗 */
    apply_hamming_window(window_buffer, WINDOW_SIZE);
    
    /* 准备FFT输入 */
    for (int i = 0; i < WINDOW_SIZE; i++) {
        fft_real[i] = window_buffer[i];
        fft_imag[i] = 0.0f;
    }
    
    /* 计算FFT */
    simple_fft(fft_real, fft_imag, WINDOW_SIZE);
    
    /* 计算功率谱 */
    for (int i = 0; i <= WINDOW_SIZE / 2; i++) {
        power_spectrum[i] = fft_real[i] * fft_real[i] + fft_imag[i] * fft_imag[i];
    }
    
    /* Mel滤波器组 */
    float mel_low = hz_to_mel(300.0f);
    float mel_high = hz_to_mel(SAMPLE_RATE / 2.0f);
    
    for (int m = 0; m < MEL_FILTERS; m++) {
        float mel_freq = mel_low + (mel_high - mel_low) * m / (MEL_FILTERS - 1);
        float hz_freq = mel_to_hz(mel_freq);
        int bin = (int)(hz_freq * WINDOW_SIZE / SAMPLE_RATE);
        
        mel_filters[m] = 0.0f;
        if (bin < WINDOW_SIZE / 2) {
            mel_filters[m] = power_spectrum[bin];
        }
    }
    
    /* 计算对数Mel特征 */
    for (int m = 0; m < MEL_FILTERS; m++) {
        mel_filters[m] = log10f(mel_filters[m] + 1e-10f);
    }
    
    /* DCT变换得到MFCC */
    for (int c = 0; c < MFCC_COEFS; c++) {
        mfcc_features[c] = 0.0f;
        for (int m = 0; m < MEL_FILTERS; m++) {
            mfcc_features[c] += mel_filters[m] * cosf(M_PI * c * (2 * m + 1) / (2 * MEL_FILTERS));
        }
        mfcc_features[c] *= sqrtf(2.0f / MEL_FILTERS);
    }
}

/**
 * 简化的唤醒词检测算法
 */
float detect_wake_word(float features[][MFCC_COEFS], int num_frames, int wake_word_id)
{
    /* 这里应该使用训练好的神经网络或HMM模型 */
    /* 为了演示，使用简化的模板匹配方法 */
    
    static float templates[WAKE_WORDS_COUNT][MFCC_COEFS] = {
        /* 小智模板 (示例值) */
        {2.1f, -0.5f, 0.3f, -0.2f, 0.1f, -0.1f, 0.05f, -0.05f, 0.02f, -0.02f, 0.01f, -0.01f, 0.005f},
        /* Hello模板 (示例值) */
        {1.8f, -0.3f, 0.4f, -0.3f, 0.2f, -0.15f, 0.08f, -0.06f, 0.03f, -0.025f, 0.015f, -0.012f, 0.008f},
        /* CSK6模板 (示例值) */
        {2.3f, -0.4f, 0.25f, -0.15f, 0.12f, -0.08f, 0.06f, -0.04f, 0.025f, -0.02f, 0.012f, -0.01f, 0.006f}
    };
    
    if (wake_word_id >= WAKE_WORDS_COUNT || num_frames < MIN_SPEECH_FRAMES) {
        return 0.0f;
    }
    
    float max_similarity = 0.0f;
    
    /* 滑动窗口匹配 */
    for (int start = 0; start <= num_frames - MIN_SPEECH_FRAMES; start++) {
        float similarity = 0.0f;
        int template_frames = MIN_SPEECH_FRAMES;
        
        for (int f = 0; f < template_frames; f++) {
            float frame_sim = 0.0f;
            for (int c = 0; c < MFCC_COEFS; c++) {
                float diff = features[start + f][c] - templates[wake_word_id][c];
                frame_sim += expf(-diff * diff / 2.0f);  /* 高斯相似度 */
            }
            similarity += frame_sim / MFCC_COEFS;
        }
        
        similarity /= template_frames;
        if (similarity > max_similarity) {
            max_similarity = similarity;
        }
    }
    
    return max_similarity;
}

/**
 * 语音活动检测
 */
bool detect_speech_activity(int16_t *buffer, size_t samples)
{
    float energy = calculate_energy(buffer, samples);
    return energy > SILENCE_THRESHOLD;
}

/**
 * 音频处理主函数
 */
void process_audio_buffer(int16_t *buffer, size_t samples)
{
    /* 语音活动检测 */
    bool current_speech = detect_speech_activity(buffer, samples);
    
    if (current_speech) {
        if (!speech_detected) {
            LOG_INF("检测到语音活动开始");
            speech_detected = true;
            speech_frame_count = 0;
            feature_index = 0;
        }
        
        speech_frame_count++;
        
        /* 提取MFCC特征 */
        if (feature_index < FEATURE_FRAMES) {
            extract_mfcc(buffer, feature_buffer[feature_index]);
            feature_index++;
        }
        
    } else if (speech_detected) {
        /* 语音结束，进行唤醒词检测 */
        LOG_INF("语音活动结束，开始唤醒词检测 (特征帧数: %d)", feature_index);
        
        if (feature_index >= MIN_SPEECH_FRAMES) {
            /* 检测所有唤醒词 */
            for (int i = 0; i < WAKE_WORDS_COUNT; i++) {
                float confidence = detect_wake_word(feature_buffer, feature_index, i);
                
                LOG_INF("唤醒词 '%s' 置信度: %.3f", wake_words[i], confidence);
                
                if (confidence > DETECTION_THRESHOLD) {
                    total_detections++;
                    LOG_INF("*** 检测到唤醒词: '%s' (置信度: %.3f) ***", 
                           wake_words[i], confidence);
                    LOG_INF("总检测次数: %d", total_detections);
                    
                    /* 这里可以触发后续的语音识别或设备唤醒逻辑 */
                    break;
                }
            }
        }
        
        speech_detected = false;
        speech_frame_count = 0;
    }
}

/**
 * 音频采集和处理任务
 */
void audio_processing_task(void)
{
    LOG_INF("开始音频处理任务");
    LOG_INF("支持的唤醒词: %s, %s, %s", wake_words[0], wake_words[1], wake_words[2]);
    LOG_INF("检测阈值: %.2f", DETECTION_THRESHOLD);
    
    /* 模拟音频数据输入 */
    uint32_t sample_count = 0;
    
    while (1) {
        /* 模拟从I2S读取音频数据 */
        /* 在实际应用中，这里应该是从I2S设备读取 */
        for (int i = 0; i < FRAME_SIZE; i++) {
            /* 生成模拟音频数据（可以替换为实际的I2S读取） */
            audio_buffer[i] = (int16_t)(sin(2.0 * M_PI * 440.0 * sample_count / SAMPLE_RATE) * 1000);
            sample_count++;
        }
        
        /* 处理音频缓冲区 */
        process_audio_buffer(audio_buffer, FRAME_SIZE);
        
        /* 模拟实时处理延时 */
        k_msleep(32);  /* 32ms = 512 samples at 16kHz */
        
        /* 定期输出状态 */
        static uint32_t status_counter = 0;
        status_counter++;
        if (status_counter % 100 == 0) {
            LOG_INF("处理状态 - 帧数: %d, 总检测次数: %d, 当前语音活动: %s", 
                   status_counter, total_detections, speech_detected ? "是" : "否");
        }
    }
}

void main(void)
{
    LOG_INF("CSK6 唤醒词检测示例启动");
    LOG_INF("音频配置: %dHz, %d通道, %d位", SAMPLE_RATE, CHANNELS, SAMPLE_BITS);
    LOG_INF("特征配置: %d MFCC系数, %d Mel滤波器", MFCC_COEFS, MEL_FILTERS);
    
    /* 等待系统初始化 */
    k_msleep(1000);
    
    LOG_INF("开始唤醒词检测，请说出唤醒词...");
    
    /* 启动音频处理 */
    audio_processing_task();
}

/* 音频处理线程 */
K_THREAD_DEFINE(audio_processing_tid, 4096, audio_processing_task, NULL, NULL, NULL,
                K_PRIO_PREEMPT(3), 0, 0);