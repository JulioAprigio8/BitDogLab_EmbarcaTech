//Esse código acende o LED RGB que fica entre o JoyStick e o Display OLED da BitDogLab
//Referencia: https://wokwi.com/projects/417301067520179201
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define LEDR 13
#define LEDG 11
#define LEDB 12
void led_rgb_put(bool r, bool g, bool b) { //Função que controla o estado de cada cor do LED
    gpio_put(LEDR, r);
    gpio_put(LEDG, g);
    gpio_put(LEDB, b);
}
int main()
{
    stdio_init_all();//Inicializa todas as funções de entrada/saida
    gpio_init(LEDR);//Inicializando GPIO do LED RGB
    gpio_init(LEDG);
    gpio_init(LEDB);

    gpio_set_slew_rate(LEDR, GPIO_SLEW_RATE_SLOW);//configura a taxa de transição de velocidade com que o sinal muda de alto para baixo como lenta, ajudando a diminuir interferencias eletromagneticas
    gpio_set_slew_rate(LEDG, GPIO_SLEW_RATE_SLOW);//mas pode limitar a velocidade maxima de operação do pino
    gpio_set_slew_rate(LEDB, GPIO_SLEW_RATE_SLOW);

    gpio_set_dir(LEDR, true);//define o pino como entrada ou saida (true para entrada e false para saida)
    gpio_set_dir(LEDG, true);
    gpio_set_dir(LEDB, true);


    while (true) {
        led_rgb_put(true, false, false);//Define a cor como vermelho
        sleep_ms(500);
        led_rgb_put(false, true, false);//Define a cor como verde
        sleep_ms(500);
        led_rgb_put(false, false, true);//Define a cor como azul
        sleep_ms(500);
        led_rgb_put(true, true, true);//Define a cor como branco
        sleep_ms(500);
    }
}
