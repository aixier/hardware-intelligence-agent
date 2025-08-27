/*
 * CSK6 GPIO控制示例
 * 功能：按键检测和LED控制
 * 作者：CSK6开发团队
 * 时间：2025-08-27
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

/* LED配置 */
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET_OR(LED1_NODE, gpios, {0});

/* 按键配置 */
#define SW0_NODE DT_ALIAS(sw0)
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios, {0});

/* 中断回调数据 */
static struct gpio_callback button_cb_data;

/* 按键状态 */
static volatile bool button_pressed = false;
static volatile uint32_t button_press_count = 0;

/**
 * 按键中断回调函数
 */
void button_pressed_handler(const struct device *dev, 
                           struct gpio_callback *cb, 
                           uint32_t pins)
{
    printk("按键按下! 引脚: 0x%x\n", pins);
    button_pressed = true;
    button_press_count++;
}

/**
 * LED初始化函数
 */
int init_leds(void)
{
    int ret;

    /* 初始化LED0 */
    if (!gpio_is_ready_dt(&led0)) {
        printk("错误: LED0设备未准备好\n");
        return -1;
    }

    ret = gpio_pin_configure_dt(&led0, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        printk("错误: 无法配置LED0\n");
        return ret;
    }

    /* 初始化LED1（如果存在） */
    if (led1.port != NULL) {
        if (!gpio_is_ready_dt(&led1)) {
            printk("错误: LED1设备未准备好\n");
            return -1;
        }

        ret = gpio_pin_configure_dt(&led1, GPIO_OUTPUT_ACTIVE);
        if (ret < 0) {
            printk("错误: 无法配置LED1\n");
            return ret;
        }
    }

    printk("LED初始化成功\n");
    return 0;
}

/**
 * 按键初始化函数
 */
int init_button(void)
{
    int ret;

    if (button.port == NULL) {
        printk("警告: 未发现按键设备\n");
        return 0;
    }

    if (!gpio_is_ready_dt(&button)) {
        printk("错误: 按键设备未准备好\n");
        return -1;
    }

    ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
    if (ret < 0) {
        printk("错误: 无法配置按键GPIO\n");
        return ret;
    }

    ret = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
    if (ret < 0) {
        printk("错误: 无法配置按键中断\n");
        return ret;
    }

    gpio_init_callback(&button_cb_data, button_pressed_handler, BIT(button.pin));
    gpio_add_callback(button.port, &button_cb_data);

    printk("按键初始化成功\n");
    return 0;
}

/**
 * LED模式控制
 */
void led_pattern_control(uint32_t pattern)
{
    switch (pattern % 4) {
        case 0: /* 所有LED关闭 */
            gpio_pin_set_dt(&led0, 0);
            if (led1.port) gpio_pin_set_dt(&led1, 0);
            printk("LED模式: 全部关闭\n");
            break;
            
        case 1: /* LED0闪烁 */
            gpio_pin_toggle_dt(&led0);
            if (led1.port) gpio_pin_set_dt(&led1, 0);
            printk("LED模式: LED0闪烁\n");
            break;
            
        case 2: /* LED1闪烁 */
            gpio_pin_set_dt(&led0, 0);
            if (led1.port) gpio_pin_toggle_dt(&led1);
            printk("LED模式: LED1闪烁\n");
            break;
            
        case 3: /* 交替闪烁 */
            gpio_pin_toggle_dt(&led0);
            if (led1.port) gpio_pin_set_dt(&led1, !gpio_pin_get_dt(&led0));
            printk("LED模式: 交替闪烁\n");
            break;
    }
}

void main(void)
{
    uint32_t loop_count = 0;
    uint32_t last_button_count = 0;

    printk("CSK6 GPIO控制示例启动!\n");

    /* 初始化硬件 */
    if (init_leds() < 0) {
        printk("LED初始化失败!\n");
        return;
    }

    if (init_button() < 0) {
        printk("按键初始化失败!\n");
        return;
    }

    printk("GPIO初始化完成，开始主循环\n");
    printk("按下按键可以切换LED模式\n");

    /* 主循环 */
    while (1) {
        /* 检查按键状态 */
        if (button_pressed) {
            button_pressed = false;
            printk("检测到按键按下事件! 总次数: %d\n", button_press_count);
        }

        /* 检查按键计数变化 */
        if (button_press_count != last_button_count) {
            last_button_count = button_press_count;
            printk("切换到LED模式 %d\n", button_press_count % 4);
        }

        /* LED控制 */
        led_pattern_control(button_press_count);

        /* 状态输出 */
        if (loop_count % 10 == 0) {
            printk("运行状态 - 循环: %d, 按键次数: %d, 系统时钟: %lld ms\n", 
                   loop_count, button_press_count, k_uptime_get());
        }

        loop_count++;
        k_msleep(500);
    }
}