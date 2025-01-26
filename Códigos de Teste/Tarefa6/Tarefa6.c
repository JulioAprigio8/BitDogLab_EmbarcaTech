// Inclusão das bibliotecas necessárias
#include <stdio.h>          // Biblioteca padrão de entrada/saída
#include "pico/stdlib.h"    // Biblioteca principal do Pico
#include <string.h>         // Para manipulação de strings
#include <stdlib.h>         // Biblioteca padrão
#include "pico/binary_info.h" // Informações binárias do Pico
#include "inc/ssd1306.h"    // Biblioteca do display OLED
#include "hardware/i2c.h"   // Biblioteca para comunicação I2C
#include "hardware/gpio.h"  // Biblioteca para controle de GPIO

// Definição dos pinos dos LEDs e botões
#define LED_VERMELHO 13     // LED vermelho no GPIO 13
#define LED_AMARELO 12      // LED amarelo no GPIO 12
#define LED_VERDE 11        // LED verde no GPIO 11
#define Botao_A 5          // Botão A no GPIO 5
#define Botao_B 6          // Botão B no GPIO 6

// Configurações do barramento I2C
#define I2C_PORT i2c1       // Usando I2C1
#define I2C_SDA 14         // Pino de dados I2C (SDA)
#define I2C_SCL 15         // Pino de clock I2C (SCL)

// Variáveis globais
struct render_area frame_area;  // Área de renderização do display
uint8_t display_buffer[ssd1306_buffer_length];  // Buffer do display
volatile bool botao_pressionado = false;  // Flag para estado do botão

// Função de callback para interrupção dos botões
void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == Botao_A || gpio == Botao_B) {
        botao_pressionado = true;  // Ativa a flag quando qualquer botão é pressionado
    }
}

// Inicialização dos botões
void inicializar_botoes() {
    // Configura os pinos dos botões
    gpio_init(Botao_A);
    gpio_init(Botao_B);
    gpio_set_dir(Botao_A, GPIO_IN);  // Define como entrada
    gpio_set_dir(Botao_B, GPIO_IN);
    gpio_pull_up(Botao_A);  // Ativa pull-up interno
    gpio_pull_up(Botao_B);
    
    // Configura as interrupções dos botões
    gpio_set_irq_enabled_with_callback(Botao_A, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled(Botao_B, GPIO_IRQ_EDGE_FALL, true);
}

// Inicialização dos LEDs
void inicializar_gpio() {
    // Configura os pinos dos LEDs
    gpio_init(LED_VERMELHO);
    gpio_init(LED_AMARELO);
    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);  // Define como saída
    gpio_set_dir(LED_AMARELO, GPIO_OUT);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
}

// Inicialização da comunicação I2C
void inicializar_i2c() {
    i2c_init(I2C_PORT, ssd1306_i2c_clock * 1000);  // Inicializa I2C
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);     // Configura pinos para I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);  // Ativa pull-up nos pinos I2C
    gpio_pull_up(I2C_SCL);
}

// Inicialização do display OLED
void inicializar_display() {
    ssd1306_init();  // Inicializa o display
    
    // Configura a área de renderização
    frame_area.start_column = 0;
    frame_area.end_column = ssd1306_width - 1;
    frame_area.start_page = 0;
    frame_area.end_page = ssd1306_n_pages - 1;
    
    calculate_render_area_buffer_length(&frame_area);
    
    // Limpa o display
    memset(display_buffer, 0, ssd1306_buffer_length);
    render_on_display(display_buffer, &frame_area);
}

// Função para exibir mensagens no display
void exibir_mensagem(const char* linha1, const char* linha2) {
    memset(display_buffer, 0, ssd1306_buffer_length);  // Limpa o buffer
    
    // Define dimensões dos caracteres
    const int char_width = 6;   // Largura de cada caractere
    const int char_height = 8;  // Altura de cada caractere
    
    // Calcula posições para centralização
    int x1 = (96 - (strlen(linha1) * char_width)) / 2;   // Centraliza linha 1
    int x2 = (110 - (strlen(linha2) * char_width)) / 2;  // Centraliza linha 2
    int y1 = 20;  // Posição vertical linha 1
    int y2 = 32;  // Posição vertical linha 2
    
    // Desenha as strings e atualiza o display
    ssd1306_draw_string(display_buffer, x1, y1, linha1);
    ssd1306_draw_string(display_buffer, x2, y2, linha2);
    render_on_display(display_buffer, &frame_area);
}

// Funções de controle do semáforo
void SinalFechado() {
    gpio_put(LED_VERMELHO, 1);  // Acende vermelho
    gpio_put(LED_AMARELO, 0);   // Apaga amarelo
    gpio_put(LED_VERDE, 0);     // Apaga verde
    exibir_mensagem("SINAL FECHADO", "AGUARDE");
    sleep_ms(5000);  // Espera 5 segundos
}

void SinalAtencao() {
    gpio_put(LED_VERMELHO, 1);
    gpio_put(LED_AMARELO, 0);
    gpio_put(LED_VERDE, 1);
    exibir_mensagem("SINAL DE ATENCAO", "PREPARE-SE");
    sleep_ms(2000);  // Espera 2 segundos
}

void SinalAberto() {
    gpio_put(LED_VERMELHO, 0);
    gpio_put(LED_AMARELO, 0);
    gpio_put(LED_VERDE, 1);
    exibir_mensagem("SINAL ABERTO", "ATRAVESSE");
    sleep_ms(5000);  // Espera 5 segundos
}

// Função para modo pedestre quando botão é pressionado
void modo_pedestre() {
    SinalAtencao();
    SinalAberto();
    botao_pressionado = false;  // Reset da flag
}

// Função principal
int main() {
    stdio_init_all();  // Inicializa E/S padrão
    
    // Inicializa todos os componentes
    inicializar_gpio();
    inicializar_i2c();
    inicializar_display();
    inicializar_botoes();

    // Loop principal
    while (true) {
        if (botao_pressionado) {
            modo_pedestre();  // Executa modo pedestre se botão pressionado
        } else {
            // Ciclo normal do semáforo
            SinalFechado();
            SinalAtencao();
            SinalAberto();
        }
    }

    return 0;
}
