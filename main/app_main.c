#include <stdio.h>
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_spp_api.h"

#define BT_NAME "ESP32-WROOM-UE-SPP"

static void spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    switch (event)
    {
    case ESP_SPP_SRV_OPEN_EVT:
        printf("SPP客户端已连接\n");
        break;
    case ESP_SPP_CLOSE_EVT:
        printf("蓝牙断开\n");
        break;
    case ESP_SPP_DATA_IND_EVT:
        printf("收到：%.*s\n", param->data_ind.len, param->data_ind.data);
        esp_spp_write(param->data_ind.handle, param->data_ind.len, param->data_ind.data);
        break;
    default:
        break;
    }
}

void bt_init(void)
{
    esp_err_t ret;
    // 释放BLE专用内存，给经典蓝牙腾RAM
    esp_bt_controller_mem_release(ESP_BT_MODE_BLE);

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret != ESP_OK) return;

    // BTDM双模：BLE + 经典蓝牙
    esp_bt_controller_enable(ESP_BT_MODE_BTDM);
    esp_bluedroid_init();
    esp_bluedroid_enable();

    // 设置蓝牙名、可搜索
    esp_bt_dev_set_device_name(BT_NAME);
    esp_bt_gap_set_scan_mode(ESP_BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE);

    // 启动SPP服务端
    esp_spp_register_callback(spp_cb);
    esp_spp_init(ESP_SPP_MODE_CB);
    esp_spp_create_server(0, ESP_SPP_SEC_NONE, ESP_SPP_ROLE_SLAVE);
}

void app_main(void)
{
    bt_init();
}