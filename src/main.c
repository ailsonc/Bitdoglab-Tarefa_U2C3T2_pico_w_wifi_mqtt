#include "main.h"

bool alarme = false;
bool posicao_js = false;
bool ledverdestatus = false;

MQTT_CLIENT_T *global_mqtt_state;

static MQTT_CLIENT_T* mqtt_client_init(void) {
    MQTT_CLIENT_T *state = calloc(1, sizeof(MQTT_CLIENT_T));
    if (!state) {
        DEBUG_printf("Failed to allocate state\n");
        return NULL;
    }
    return state;
}

int main() {
    stdio_init_all();
    pinos_start();

    if (cyw43_arch_init()) {
        DEBUG_printf("Failed to initialize WiFi\n");
        return 1;
    }
    cyw43_arch_enable_sta_mode();

    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        DEBUG_printf("Failed to connect to WiFi\n");
        return 1;
    }

    global_mqtt_state = mqtt_client_init();
    if (!global_mqtt_state) {
        return 1;
    }
    
    run_dns_lookup(global_mqtt_state);
    mqtt_run_test(global_mqtt_state);

    cyw43_arch_deinit();
    return 0;
}
