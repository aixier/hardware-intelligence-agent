# CSK6基础示例CMake配置文件

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(csk6_basic_examples)

# Hello World示例
zephyr_library()
zephyr_library_sources(hello_world.c)

# GPIO控制示例
add_executable(gpio_control gpio_control.c)
target_link_libraries(gpio_control zephyr)

# 编译配置
target_compile_definitions(zephyr_interface
    INTERFACE
    -DCONFIG_LOG_DEFAULT_LEVEL=3
    -DCONFIG_GPIO=y
    -DCONFIG_UART_CONSOLE=y
)

# 设置编译选项
target_compile_options(zephyr_interface
    INTERFACE
    -Wall
    -Wextra
    -O2
)

# 包含头文件目录
target_include_directories(zephyr_interface
    INTERFACE
    ${ZEPHYR_BASE}/include
    ${ZEPHYR_BASE}/drivers
)