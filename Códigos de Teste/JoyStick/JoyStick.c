#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#define BUTTON_PIN 22

int main() {
    stdio_init_all();
    adc_init();
    
    // Configuração dos pinos ADC
    adc_gpio_init(26);
    adc_gpio_init(27);
    
    // Configuração do pino do botão
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
    
    uint32_t button_counter = 0;
    bool last_button_state = true;

    while (1) {
        adc_select_input(0);
        uint adc_y_raw = adc_read();
        adc_select_input(1);
        uint adc_x_raw = adc_read();

        // Leitura do estado do botão
        bool current_button_state = gpio_get(BUTTON_PIN);
        
        // Detecção de borda de descida (botão pressionado)
        if (last_button_state && !current_button_state) {
            button_counter++;
        }
        last_button_state = current_button_state;

        // Display do joystick e contador
        const uint bar_width = 40;
        const uint adc_max = (1 << 12) - 2;
        uint bar_x_pos = adc_x_raw * bar_width / adc_max;
        uint bar_y_pos = adc_y_raw * bar_width / adc_max;
        printf("\rX: [");
        for (uint i = 0; i <= bar_width; ++i)
            putchar(i == bar_x_pos ? 'o' : ' ');
        printf("]  Y: [");
        for (uint i = 0; i <= bar_width; ++i)
            putchar(i == bar_y_pos ? 'o' : ' ');
            printf("  ");
        printf("]C: %lu", button_counter);
        
        sleep_ms(50);
    }
}
