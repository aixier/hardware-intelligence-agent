/*
 * CSK6 WiFi连接示例
 * 功能：WiFi网络连接和基础网络通信
 * 作者：CSK6开发团队
 * 时间：2025-08-27
 */

#include <zephyr/kernel.h>
#include <zephyr/net/wifi.h>
#include <zephyr/net/wifi_mgmt.h>
#include <zephyr/net/net_mgmt.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_context.h>
#include <zephyr/net/socket.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(wifi_connect, LOG_LEVEL_INF);

/* WiFi配置 */
#define WIFI_SSID        "YourWiFiSSID"      /* 修改为你的WiFi名称 */
#define WIFI_PSK         "YourWiFiPassword"  /* 修改为你的WiFi密码 */
#define WIFI_SECURITY    WIFI_SECURITY_TYPE_PSK
#define WIFI_MFP         WIFI_MFP_OPTIONAL

/* 网络状态 */
static struct net_mgmt_event_callback wifi_cb;
static struct net_mgmt_event_callback ipv4_cb;
static K_SEM_DEFINE(wifi_connected, 0, 1);
static K_SEM_DEFINE(ipv4_address_obtained, 0, 1);

static bool is_wifi_connected = false;
static struct in_addr my_ip_addr;

/**
 * WiFi事件处理函数
 */
static void wifi_mgmt_event_handler(struct net_mgmt_event_callback *cb,
                                   uint32_t mgmt_event,
                                   struct net_if *iface)
{
    switch (mgmt_event) {
        case NET_EVENT_WIFI_CONNECT_RESULT:
            {
                const struct wifi_status *status =
                    (const struct wifi_status *)cb->info;
                
                if (status->status) {
                    LOG_ERR("WiFi连接失败 (%d)", status->status);
                } else {
                    LOG_INF("WiFi连接成功");
                    is_wifi_connected = true;
                    k_sem_give(&wifi_connected);
                }
                break;
            }
            
        case NET_EVENT_WIFI_DISCONNECT_RESULT:
            LOG_INF("WiFi断开连接");
            is_wifi_connected = false;
            break;
            
        case NET_EVENT_WIFI_SCAN_RESULT:
            {
                const struct wifi_scan_result *entry =
                    (const struct wifi_scan_result *)cb->info;
                
                if (entry) {
                    LOG_INF("发现WiFi: %s (信号强度: %d dBm)", 
                           entry->ssid, entry->rssi);
                }
                break;
            }
            
        case NET_EVENT_WIFI_SCAN_DONE:
            LOG_INF("WiFi扫描完成");
            break;
            
        default:
            break;
    }
}

/**
 * IPv4事件处理函数
 */
static void ipv4_mgmt_event_handler(struct net_mgmt_event_callback *cb,
                                   uint32_t mgmt_event,
                                   struct net_if *iface)
{
    switch (mgmt_event) {
        case NET_EVENT_IPV4_ADDR_ADD:
            {
                char buf[INET_ADDRSTRLEN];
                
                for (int i = 0; i < NET_IF_MAX_IPV4_ADDR; i++) {
                    struct net_if_addr *if_addr = &iface->config.ip.ipv4->unicast[i];
                    
                    if (if_addr->addr_type != NET_ADDR_DHCP) {
                        continue;
                    }
                    
                    if (if_addr->is_used) {
                        my_ip_addr = if_addr->address.in_addr;
                        inet_ntop(AF_INET, &my_ip_addr, buf, sizeof(buf));
                        LOG_INF("获得IPv4地址: %s", buf);
                        k_sem_give(&ipv4_address_obtained);
                        return;
                    }
                }
                break;
            }
            
        default:
            break;
    }
}

/**
 * 扫描可用WiFi网络
 */
int scan_wifi_networks(void)
{
    struct net_if *iface = net_if_get_default();
    
    if (!iface) {
        LOG_ERR("无法获取默认网络接口");
        return -ENODEV;
    }
    
    LOG_INF("开始扫描WiFi网络...");
    
    int ret = net_mgmt(NET_REQUEST_WIFI_SCAN, iface, NULL, 0);
    if (ret) {
        LOG_ERR("WiFi扫描请求失败 (%d)", ret);
        return ret;
    }
    
    /* 等待扫描完成 */
    k_msleep(5000);
    
    return 0;
}

/**
 * 连接到WiFi网络
 */
int connect_to_wifi(void)
{
    struct net_if *iface = net_if_get_default();
    struct wifi_connect_req_params params;
    
    if (!iface) {
        LOG_ERR("无法获取默认网络接口");
        return -ENODEV;
    }
    
    /* 配置WiFi连接参数 */
    memset(&params, 0, sizeof(params));
    params.ssid = WIFI_SSID;
    params.ssid_length = strlen(WIFI_SSID);
    params.psk = WIFI_PSK;
    params.psk_length = strlen(WIFI_PSK);
    params.channel = WIFI_CHANNEL_ANY;
    params.security = WIFI_SECURITY;
    params.mfp = WIFI_MFP;
    
    LOG_INF("正在连接到WiFi: %s", WIFI_SSID);
    
    int ret = net_mgmt(NET_REQUEST_WIFI_CONNECT, iface,
                      &params, sizeof(params));
    if (ret) {
        LOG_ERR("WiFi连接请求失败 (%d)", ret);
        return ret;
    }
    
    /* 等待连接完成 */
    ret = k_sem_take(&wifi_connected, K_SECONDS(30));
    if (ret) {
        LOG_ERR("WiFi连接超时");
        return ret;
    }
    
    /* 等待获取IP地址 */
    ret = k_sem_take(&ipv4_address_obtained, K_SECONDS(30));
    if (ret) {
        LOG_ERR("获取IP地址超时");
        return ret;
    }
    
    return 0;
}

/**
 * HTTP GET请求示例
 */
int http_get_test(void)
{
    int sock;
    struct sockaddr_in server_addr;
    char request[] = "GET / HTTP/1.1\r\n"
                    "Host: httpbin.org\r\n"
                    "Connection: close\r\n"
                    "\r\n";
    char response[1024];
    int ret;
    
    if (!is_wifi_connected) {
        LOG_ERR("WiFi未连接，无法进行HTTP请求");
        return -ENOTCONN;
    }
    
    /* 创建socket */
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {
        LOG_ERR("创建socket失败");
        return -errno;
    }
    
    /* 配置服务器地址 */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80);
    ret = inet_pton(AF_INET, "34.205.167.146", &server_addr.sin_addr);  /* httpbin.org */
    if (ret != 1) {
        LOG_ERR("无效的服务器地址");
        close(sock);
        return -EINVAL;
    }
    
    /* 连接到服务器 */
    LOG_INF("正在连接到HTTP服务器...");
    ret = connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret < 0) {
        LOG_ERR("连接服务器失败 (%d)", errno);
        close(sock);
        return -errno;
    }
    
    /* 发送HTTP请求 */
    ret = send(sock, request, strlen(request), 0);
    if (ret < 0) {
        LOG_ERR("发送HTTP请求失败");
        close(sock);
        return -errno;
    }
    
    LOG_INF("HTTP请求已发送，等待响应...");
    
    /* 接收响应 */
    ret = recv(sock, response, sizeof(response) - 1, 0);
    if (ret > 0) {
        response[ret] = '\0';
        LOG_INF("收到HTTP响应 (%d字节):", ret);
        printk("%s\n", response);
    } else {
        LOG_ERR("接收响应失败");
    }
    
    close(sock);
    return 0;
}

/**
 * 网络状态监控任务
 */
void network_monitor_task(void)
{
    while (1) {
        if (is_wifi_connected) {
            char ip_str[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &my_ip_addr, ip_str, sizeof(ip_str));
            LOG_INF("网络状态: 已连接 (IP: %s)", ip_str);
        } else {
            LOG_INF("网络状态: 未连接");
        }
        
        k_msleep(10000);  /* 每10秒检查一次 */
    }
}

void main(void)
{
    int ret;

    LOG_INF("CSK6 WiFi连接示例启动");

    /* 注册WiFi事件回调 */
    net_mgmt_init_event_callback(&wifi_cb, wifi_mgmt_event_handler,
                                NET_EVENT_WIFI_CONNECT_RESULT |
                                NET_EVENT_WIFI_DISCONNECT_RESULT |
                                NET_EVENT_WIFI_SCAN_RESULT |
                                NET_EVENT_WIFI_SCAN_DONE);
    net_mgmt_add_event_callback(&wifi_cb);

    /* 注册IPv4事件回调 */
    net_mgmt_init_event_callback(&ipv4_cb, ipv4_mgmt_event_handler,
                                NET_EVENT_IPV4_ADDR_ADD);
    net_mgmt_add_event_callback(&ipv4_cb);

    /* 等待网络接口准备好 */
    k_msleep(2000);

    /* 扫描WiFi网络 */
    ret = scan_wifi_networks();
    if (ret) {
        LOG_WRN("WiFi扫描失败，继续连接...");
    }

    /* 连接WiFi */
    ret = connect_to_wifi();
    if (ret) {
        LOG_ERR("WiFi连接失败，程序退出");
        return;
    }

    LOG_INF("WiFi连接成功！");

    /* 等待网络稳定 */
    k_msleep(2000);

    /* 测试HTTP连接 */
    LOG_INF("开始HTTP连接测试...");
    ret = http_get_test();
    if (ret) {
        LOG_ERR("HTTP测试失败 (%d)", ret);
    } else {
        LOG_INF("HTTP测试成功！");
    }

    /* 启动网络监控任务 */
    LOG_INF("启动网络状态监控...");
    network_monitor_task();
}

/* 网络监控线程 */
K_THREAD_DEFINE(network_monitor_tid, 1024, network_monitor_task, NULL, NULL, NULL,
                K_PRIO_COOP(7), 0, 0);