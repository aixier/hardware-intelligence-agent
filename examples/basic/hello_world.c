/*
 * CSK6 Hello World示例
 * 功能：基础串口输出和LED控制
 * 作者：CSK6开发团队
 * 时间：2025-08-27
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>

/* LED配置 */
#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

/* 延时时间 */
#define SLEEP_TIME_MS 1000

void main(void)
{
    int ret;
    bool led_state = true;

    printk("CSK6 Hello World示例启动!\n");
    printk("芯片型号: CSK6系列\n");
    printk("RTOS: Zephyr %s\n", KERNEL_VERSION_STRING);

    /* 初始化LED */
    if (!gpio_is_ready_dt(&led)) {
        printk("错误: LED设备未准备好\n");
        return;
    }

    ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        printk("错误: 无法配置LED GPIO\n");
        return;
    }

    printk("LED初始化成功\n");

    /* 主循环 */
    while (1) {
        /* 切换LED状态 */
        ret = gpio_pin_toggle_dt(&led);
        if (ret < 0) {
            printk("错误: 无法切换LED状态\n");
        }

        /* 打印状态信息 */
        printk("Hello CSK6! LED状态: %s (系统时钟: %lld ms)\n", 
               led_state ? "ON" : "OFF", 
               k_uptime_get());

        led_state = !led_state;

        /* 延时 */
        k_msleep(SLEEP_TIME_MS);
    }
}