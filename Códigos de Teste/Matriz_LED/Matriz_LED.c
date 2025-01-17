#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812b.pio.h"

#define LED_COUNT 25
#define LED_PIN 7

struct pixel_t
{
  uint8_t G, R, B;
};
typedef struct pixel_t pixel_t;
typedef pixel_t npLED_t;

npLED_t leds[LED_COUNT];
PIO np_pio;
uint sm;

// Funções originais
void npInit(uint pin)
{
  uint offset = pio_add_program(pio0, &ws2812b_program);
  np_pio = pio0;
  sm = pio_claim_unused_sm(np_pio, false);
  if (sm < 0)
  {
    np_pio = pio1;
    sm = pio_claim_unused_sm(np_pio, true);
  }
  ws2812b_program_init(np_pio, sm, offset, pin, 800000.f);
  for (uint i = 0; i < LED_COUNT; ++i)
  {
    leds[i].R = 0;
    leds[i].G = 0;
    leds[i].B = 0;
  }
}

void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b)
{
  leds[index].R = r;
  leds[index].G = g;
  leds[index].B = b;
}

void npClear()
{
  for (uint i = 0; i < LED_COUNT; ++i)
    npSetLED(i, 0, 0, 0);
}

void npWrite()
{
  for (uint i = 0; i < LED_COUNT; ++i)
  {
    pio_sm_put_blocking(np_pio, sm, leds[i].G);
    pio_sm_put_blocking(np_pio, sm, leds[i].R);
    pio_sm_put_blocking(np_pio, sm, leds[i].B);
  }
  sleep_us(100);
}

// Novas funções
void hueToRGB(float hue, uint8_t *r, uint8_t *g, uint8_t *b)
{
  float h = fmod(hue, 360.0f) / 60.0f;
  float x = 1 - fabs(fmod(h, 2) - 1);
  float c = 1;
  float m = 0;

  if (h < 1)
  {
    *r = (uint8_t)((c + m) * 255);
    *g = (uint8_t)((x + m) * 255);
    *b = (uint8_t)(m * 255);
  }
  else if (h < 2)
  {
    *r = (uint8_t)((x + m) * 255);
    *g = (uint8_t)((c + m) * 255);
    *b = (uint8_t)(m * 255);
  }
  else if (h < 3)
  {
    *r = (uint8_t)(m * 255);
    *g = (uint8_t)((c + m) * 255);
    *b = (uint8_t)((x + m) * 255);
  }
  else if (h < 4)
  {
    *r = (uint8_t)(m * 255);
    *g = (uint8_t)((x + m) * 255);
    *b = (uint8_t)((c + m) * 255);
  }
  else if (h < 5)
  {
    *r = (uint8_t)((x + m) * 255);
    *g = (uint8_t)(m * 255);
    *b = (uint8_t)((c + m) * 255);
  }
  else
  {
    *r = (uint8_t)((c + m) * 255);
    *g = (uint8_t)(m * 255);
    *b = (uint8_t)((x + m) * 255);
  }
}

void fadingEffect(float startHue, float endHue, int steps, int delayMs)
{
  float hueStep = (endHue - startHue) / steps;

  for (int step = 0; step <= steps; step++)
  {
    float currentHue = startHue + hueStep * step;
    uint8_t r, g, b;
    hueToRGB(currentHue, &r, &g, &b);

    for (int i = 0; i < LED_COUNT; i++)
    {
      npSetLED(i, r, g, b);
    }
    npWrite();
    sleep_ms(delayMs);
  }
}
// Função para gerar o efeito rainbow
void rainbowEffect(int cycles, int delayMs)
{
  float baseHue = 0;
  const float hueStepPerLED = 360.0f / LED_COUNT;

  for (int cycle = 0; cycle < cycles; cycle++)
  {
    for (int i = 0; i < LED_COUNT; i++)
    {
      float hue = fmod(baseHue + i * hueStepPerLED, 360.0f);
      uint8_t r, g, b;
      hueToRGB(hue, &r, &g, &b);
      npSetLED(i, r, g, b);
    }

    npWrite();
    baseHue += 5;
    if (baseHue >= 360.0f)
      baseHue -= 360.0f;

    sleep_ms(delayMs);
  }
}
// Função para criar o efeito de onda de cores
void colorWaveEffect(int cycles, int delayMs)
{
  float time = 0;
  const float frequency = 0.1; // Ajuste para mudar a "largura" da onda
  const float speed = 0.1;     // Ajuste para mudar a velocidade da onda

  for (int cycle = 0; cycle < cycles; cycle++)
  {
    for (int i = 0; i < LED_COUNT; i++)
    {
      // Calcula a posição na onda para cada LED
      float wave = sin(frequency * i + time) * 0.5 + 0.5;

      // Mapeia a posição da onda para um valor de matiz (0-360)
      float hue = wave * 360;

      uint8_t r, g, b;
      hueToRGB(hue, &r, &g, &b);
      npSetLED(i, r, g, b);
    }

    npWrite();
    time += speed;
    sleep_ms(delayMs);
  }
}
int main()
{
  stdio_init_all();
  npInit(LED_PIN);
  npClear();

  while (true)
  {
    for (int i = 1; i < 26; i++)
    {
      npSetLED(i - 1, 50, 0, 0);
      npWrite();
      sleep_ms(1000 / i);
    }
    sleep_ms(500);
    npClear();
    fadingEffect(0, 240, 100, 20);
    fadingEffect(240, 120, 100, 20);
    fadingEffect(120, 0, 100, 20);
    sleep_ms(500);
    npClear();
    rainbowEffect(100, 50); // 100 ciclos, 50ms de atraso
    sleep_ms(500);
    npClear();
    colorWaveEffect(300, 30); // 300 ciclos, 30ms de atraso

    npClear();
    sleep_ms(500);
  }
}
