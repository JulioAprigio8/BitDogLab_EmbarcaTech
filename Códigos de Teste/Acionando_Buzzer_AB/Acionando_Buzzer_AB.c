//Esse código aciona o Buzzer A e B da placa BitDogLab
//Referencia: https://wokwi.com/projects/417289794886393857
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#define Buzzer_A 21
#define Buzzer_B 10
#define Buzzer_Freq 2000

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


int main()
{
    stdio_init_all();
    pwm_init_buzzer(Buzzer_A);
    pwm_init_buzzer(Buzzer_B);
    while (true) {
        beep(Buzzer_A,200);
        sleep_ms(250);
        beep(Buzzer_B,200);
        sleep_ms(250);
    }
}
