#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#define Buzzer_A 21
#define Buzzer_B 10
#define Buzzer_Freq 2000

#define Botao_A 5
#define Botao_B 6

void pwm_init_buzzer(uint pin){
    gpio_set_function(pin,GPIO_FUNC_PWM);
     // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(pin);

    // Configurar o PWM com frequência desejada
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, clock_get_hz(clk_sys) / (Buzzer_Freq * 4096)); // Divisor de clock
    pwm_init(slice_num, &config, true);

    // Iniciar o PWM no nível baixo
    pwm_set_gpio_level(pin, 0);
}
void beep(uint pin, uint duration_ms) {
    // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(pin);

    // Configurar o duty cycle para 50% (ativo)
    pwm_set_gpio_level(pin, 1024);

    // Temporização
    sleep_ms(duration_ms);

    // Desativar o sinal PWM (duty cycle 0)
    pwm_set_gpio_level(pin, 0);

    // Pausa entre os beeps
    sleep_ms(100); // Pausa de 100ms
}


int main() {
    stdio_init_all();
    gpio_init(Botao_A);
    gpio_set_dir(Botao_A, GPIO_IN);
    gpio_pull_up(Botao_A);
    gpio_init(Botao_B);
    gpio_set_dir(Botao_B, GPIO_IN);
    gpio_pull_up(Botao_B);

    pwm_init_buzzer(Buzzer_A);
    pwm_init_buzzer(Buzzer_B);
    while (true) {
        if (gpio_get(Botao_A) == 0) {  // Botão pressionado (nível lógico baixo)
            printf("Botão A pressionado\n");
            beep(Buzzer_A, 1000); // Bipe de 500ms     // Liga o buzzer
                         // Aguarda 1 segundo
    }
   if (gpio_get(Botao_B) == 0) {  // Botão pressionado (nível lógico baixo)
            printf("Botão A pressionado\n");
            beep(Buzzer_B, 1000); // Bipe de 500ms     // Liga o buzzer
                         // Aguarda 1 segundo
    }
   
}
}
