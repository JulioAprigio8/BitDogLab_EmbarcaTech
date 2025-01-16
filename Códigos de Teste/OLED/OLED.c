#include <stdio.h>
#include "pico/stdlib.h"
#include <string.h>
#include <stdlib.h>
#include "pico/binary_info.h"
#include "inc/ssd1306.h"
#include "hardware/i2c.h"

// I2C defines
#define I2C_PORT i2c0
#define I2C_SDA 14
#define I2C_SCL 15

int main() {
    stdio_init_all();

    // Inicialização do i2c
    i2c_init(I2C_PORT, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Processo de inicialização completo do OLED SSD1306
    ssd1306_init();

    // Preparar área de renderização para o display
    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area);

    // Zera o display inteiro
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);

    while (true) {
        // Limpa o display
        memset(ssd, 0, ssd1306_buffer_length);

        // Exibe texto
        char *text[] = {
            " Bem-vindos! ",
            " Embarcatech "
        };
        int y = 0;
        for (uint i = 0; i < count_of(text); i++) {
            ssd1306_draw_string(ssd, 5, y, text[i]);
            y += 16;
        }

        // Exibe uma linha
        ssd1306_draw_line(ssd, 10, 40, 100, 50, true);

        // Atualiza o display
        render_on_display(ssd, &frame_area);

        sleep_ms(2000);  // Espera 2 segundos antes de atualizar novamente
    }

    return 0;
}
