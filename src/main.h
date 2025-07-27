#include <time.h>
#include <stdio.h>
#include <string.h>
#include "lwip/dns.h"
#include "lwip/tcp.h"
#include "lwip/pbuf.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "lwip/apps/mqtt.h"
#include "lwip/altcp_tcp.h"
#include "lwip/altcp_tls.h"
#include "pico/cyw43_arch.h"
#include "lwip/apps/mqtt_priv.h"
#include "hardware/structs/rosc.h"
#include "env.h"
#include "inc/ssd1306.h" 
#include "hardware/i2c.h"
/* MACROS PI PICO */
#define BUTTON5_PIN 5
#define BUTTON6_PIN 6

#define LED_PIN_G 11
#define LED_PIN_B 12
#define LED_PIN_R 13

#define PWM_STEPS 2000
#define DEBOUNCE_DELAY_MS 700
#define FADE_STEP_DELAY (100)

// Adicionado para depuração e tamanho do buffer
#define DEBUG_printf printf
#define BUFFER_SIZE 256

// Pinos I2C para o OLED (verificado no esquemático da BitDogLab)
#define I2C_SDA_PIN 14
#define I2C_SCL_PIN 15
/* END */

/*VARIAVEIS*/
extern bool ledverdestatus;

typedef struct MQTT_CLIENT_T_ {
    ip_addr_t remote_addr;
    mqtt_client_t *mqtt_client;
    u32_t received;
    u32_t counter;
    u32_t reconnect;
} MQTT_CLIENT_T;

// Variável global para o estado do cliente MQTT
extern MQTT_CLIENT_T *global_mqtt_state;

// Variável global para o display OLED
extern ssd1306_t disp;
/* END */

/* FUNÇOES */
void pinos_start();
void gpio5_callback(uint gpio, uint32_t events);
static MQTT_CLIENT_T* mqtt_client_init(void);
void run_dns_lookup(MQTT_CLIENT_T *state);
void mqtt_run_test(MQTT_CLIENT_T *state);
void gpio_event_string(char *buf, uint32_t events);
void setup_pwm(uint gpio_pin);
void update_pwm(uint gpio_pin);
void pwm_led(uint gpio_pin, uint brilho);

// Declarado aqui para ser acessível globalmente
void mqtt_pub_request_cb(void *arg, err_t err);

// Funções para o display OLED
void oled_init_display();
void oled_display_send(const char *message);
/* END */
