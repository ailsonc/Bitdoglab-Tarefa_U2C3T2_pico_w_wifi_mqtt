#include "main.h"

// Variável global para o estado do cliente MQTT
MQTT_CLIENT_T *global_mqtt_state;

static MQTT_CLIENT_T* mqtt_client_init(void) {
    MQTT_CLIENT_T *state = calloc(1, sizeof(MQTT_CLIENT_T));
    if (!state) {
        DEBUG_printf("Failed to allocate state\n");
        return NULL;
    }
    return state;
}

uint8_t ssd[ssd1306_buffer_length];

struct render_area frame_area = {
    start_column : 0,
    end_column : ssd1306_width - 1,
    start_page : 0,
    end_page : ssd1306_n_pages - 1
};

// Funções para o display OLED
void oled_init_display() {
    // Inicialização do i2c
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    // Processo de inicialização completo do OLED SSD1306
    ssd1306_init();

    calculate_render_area_buffer_length(&frame_area);

    ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);
}

void oled_display_send(const char *message) {
    ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);

    ssd1306_draw_string(ssd, 5, 0, message);
    render_on_display(ssd, &frame_area);
}

int main() {
    stdio_init_all();
    pinos_start();
    oled_init_display(); 

    oled_display_send("Bem-vindos!");

    if (cyw43_arch_init()) {
        DEBUG_printf("Failed to initialize WiFi\n");
        oled_display_send("Falha ao ini o WiFi!"); 
        return 1;
    }
    cyw43_arch_enable_sta_mode();
    
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        DEBUG_printf("Failed to connect to WiFi\n");
        oled_display_send("Falha ao con o WiFi!"); 
        return 1;
    }

    // Inicializa o estado MQTT e atribui à variável global
    global_mqtt_state = mqtt_client_init();
    if (!global_mqtt_state) {
        oled_display_send("Falha ao con MQTT!"); 
        return 1; // Erro na inicialização
    }
    
    run_dns_lookup(global_mqtt_state);
    mqtt_run_test(global_mqtt_state);

    cyw43_arch_deinit();
    return 0;
}