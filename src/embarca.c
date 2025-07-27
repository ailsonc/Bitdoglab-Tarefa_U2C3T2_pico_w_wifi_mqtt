#include "main.h"

extern MQTT_CLIENT_T *global_mqtt_state;

static uint32_t last_debounce_time[3] = {0, 0, 0};
uint adc_x_raw;
uint adc_y_raw;
uint brightness = 0;
bool increasing = true; 

static const char *gpio_irq_str[] = {
    "LEVEL_LOW",  // 0x1
    "LEVEL_HIGH", // 0x2
    "EDGE_FALL",  // 0x4
    "EDGE_RISE"   // 0x8
};

void pinos_start()
{
    gpio_init(LED_PIN_R);
    gpio_init(LED_PIN_B);
    gpio_init(LED_PIN_G);
    adc_init();
    adc_gpio_init(26);
    adc_gpio_init(27);
    gpio_set_function(LED_PIN_R, GPIO_FUNC_PWM);
    gpio_set_function(LED_PIN_G, GPIO_FUNC_PWM);
    gpio_set_function(LED_PIN_B, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(LED_PIN_R);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 4.0f);
    pwm_init(slice_num, &config, true);
    slice_num = pwm_gpio_to_slice_num(LED_PIN_B);
    pwm_init(slice_num, &config, true);
    slice_num = pwm_gpio_to_slice_num(LED_PIN_G);
    pwm_init(slice_num, &config, true);

    gpio_init(BUTTON6_PIN);
    gpio_set_dir(BUTTON6_PIN, GPIO_IN);
    gpio_pull_up(BUTTON6_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON6_PIN,GPIO_IRQ_EDGE_FALL, true, &gpio5_callback);
    
    gpio_init(BUTTON5_PIN);
    gpio_set_dir(BUTTON5_PIN, GPIO_IN);
    gpio_pull_up(BUTTON5_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON5_PIN,GPIO_IRQ_EDGE_FALL, true, &gpio5_callback);

    gpio_init(BUTTONJS_PIN);
    gpio_set_dir(BUTTONJS_PIN, GPIO_IN);
    gpio_pull_up(BUTTONJS_PIN);
    gpio_set_irq_enabled_with_callback(BUTTONJS_PIN,GPIO_IRQ_EDGE_FALL, true, &gpio5_callback);
}

void gpio_event_string(char *buf, uint32_t events) {
    for (uint i = 0; i < 4; i++) {
        uint mask = (1 << i);
        if (events & mask) {
            // Copy this event string into the user string
            const char *event_str = gpio_irq_str[i];
            while (*event_str != '\0') {
                *buf++ = *event_str++;
            }
            events &= ~mask;

            // If more events add ", "
            if (events) {
                *buf++ = ',';
                *buf++ = ' ';
            }
        }
    }
    *buf++ = '\0';
}

void gpio5_callback(uint gpio, uint32_t events) {
    uint32_t now = to_ms_since_boot(get_absolute_time());
    char message_buffer[BUFFER_SIZE]; // Buffer para a mensagem MQTT

    if (gpio == BUTTON5_PIN && (now - last_debounce_time[0] > DEBOUNCE_DELAY_MS)) 
    {
        last_debounce_time[0] = now;
        ledverdestatus  = !ledverdestatus;
        DEBUG_printf("Botao A pressionado!\n");
        snprintf(message_buffer, BUFFER_SIZE, "Botao A pressionado");
        if (global_mqtt_state && global_mqtt_state->mqtt_client) {
            mqtt_publish(global_mqtt_state->mqtt_client, "pico_w/button", message_buffer, strlen(message_buffer), 0, 0, mqtt_pub_request_cb, global_mqtt_state);
        } else {
            DEBUG_printf("Erro: Cliente MQTT nao inicializado para Botao A.\n");
        }
    }
    if (gpio == BUTTON6_PIN && (now - last_debounce_time[1] > DEBOUNCE_DELAY_MS)) 
    {
        last_debounce_time[1] = now;
        DEBUG_printf("Botao B pressionado!\n");
        snprintf(message_buffer, BUFFER_SIZE, "Botao B pressionado");
        if (global_mqtt_state && global_mqtt_state->mqtt_client) {
            mqtt_publish(global_mqtt_state->mqtt_client, "pico_w/button", message_buffer, strlen(message_buffer), 0, 0, mqtt_pub_request_cb, global_mqtt_state);
        } else {
            DEBUG_printf("Erro: Cliente MQTT nao inicializado para Botao B.\n");
        }
    }
    if (gpio == BUTTONJS_PIN && (now - last_debounce_time[2] > DEBOUNCE_DELAY_MS)) 
    {
        last_debounce_time[2] = now;
    }
}

void setup_pwm(uint gpio_pin) {
    gpio_set_function(gpio_pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(gpio_pin);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 4.0f);  
    pwm_init(slice_num, &config, true);
}

void update_pwm(uint gpio_pin) {
    pwm_set_gpio_level(LED_PIN_R, brightness);
    printf("brilho_led_vermelho: %u\n",brightness);
    if (increasing) 
    {
        brightness = brightness+400;
        if (brightness >= PWM_STEPS) 
        {
            increasing = false; 
        }
    }
    else 
    {
        brightness = brightness-400;
        if (brightness == 0) {
            increasing = true; 
        }
    }
}

void pwm_led(uint gpio_pin, uint brilho)
{
    if(gpio_pin == LED_PIN_B)
    {
        pwm_set_gpio_level(LED_PIN_B, brilho);
    }
    else if (gpio_pin == LED_PIN_G)
    {
        pwm_set_gpio_level(LED_PIN_G, brilho);
    }
}
