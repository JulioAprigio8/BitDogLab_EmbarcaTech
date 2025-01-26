#include <stdio.h>
#include "pico/stdlib.h"
#include <string.h>
#include <stdlib.h>
#include "pico/binary_info.h"
#include "inc/ssd1306.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"

// Definições dos LEDs
#define LED_VERMELHO 13
#define LED_AMARELO 12
#define LED_VERDE 11
#define Botao_A 5
#define Botao_B 6

// Definições I2C
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15

// Variáveis globais para o display
struct render_area frame_area;
uint8_t display_buffer[ssd1306_buffer_length];
volatile bool botao_pressionado = false;

void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == Botao_A || gpio == Botao_B) {
        botao_pressionado = true;
    }
}

void inicializar_botoes() {
    gpio_init(Botao_A);
    gpio_init(Botao_B);
    gpio_set_dir(Botao_A, GPIO_IN);
    gpio_set_dir(Botao_B, GPIO_IN);
    gpio_pull_up(Botao_A);
    gpio_pull_up(Botao_B);
    
    gpio_set_irq_enabled_with_callback(Botao_A, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled(Botao_B, GPIO_IRQ_EDGE_FALL, true);
}

void inicializar_gpio() {
    gpio_init(LED_VERMELHO);
    gpio_init(LED_AMARELO);
    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    gpio_set_dir(LED_AMARELO, GPIO_OUT);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
}

void inicializar_i2c() {
    i2c_init(I2C_PORT, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

void inicializar_display() {
    ssd1306_init();
    
    frame_area.start_column = 0;
    frame_area.end_column = ssd1306_width - 1;
    frame_area.start_page = 0;
    frame_area.end_page = ssd1306_n_pages - 1;
    
    calculate_render_area_buffer_length(&frame_area);
    
    memset(display_buffer, 0, ssd1306_buffer_length);
    render_on_display(display_buffer, &frame_area);
}

void exibir_mensagem(const char* linha1, const char* linha2) {
    memset(display_buffer, 0, ssd1306_buffer_length);
    ssd1306_draw_string(display_buffer, 5, 16, linha1);
    ssd1306_draw_string(display_buffer, 5, 32, linha2);
    render_on_display(display_buffer, &frame_area);
}

void SinalFechado() {
    gpio_put(LED_VERMELHO, 1);
    gpio_put(LED_AMARELO, 0);
    gpio_put(LED_VERDE, 0);
    exibir_mensagem("SINAL FECHADO", "AGUARDE");
    sleep_ms(5000);
}

void SinalAtencao() {
    gpio_put(LED_VERMELHO, 1);
    gpio_put(LED_AMARELO, 0);
    gpio_put(LED_VERDE, 1);
    exibir_mensagem("SINAL DE ATENCAO ", "PREPARE-SE");
    sleep_ms(2000);
}

void SinalAberto() {
    gpio_put(LED_VERMELHO, 0);
    gpio_put(LED_AMARELO, 0);
    gpio_put(LED_VERDE, 1);
    exibir_mensagem("SINAL ABERTO", "ATRAVESSE");
    sleep_ms(5000);
}

void modo_pedestre() {
    SinalAtencao();
    SinalAberto();
    botao_pressionado = false;
}

int main() {
    stdio_init_all();
    
    inicializar_gpio();
    inicializar_i2c();
    inicializar_display();
    inicializar_botoes();

    while (true) {
        if (botao_pressionado) {
            modo_pedestre();
        } else {
            SinalFechado();
            SinalAtencao();
            SinalAberto();
        }
    }

    return 0;
}
