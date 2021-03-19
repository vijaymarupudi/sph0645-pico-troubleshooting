#include "pico/time.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <inttypes.h>
#include <vector>
#include "hardware/clocks.h"
#include "hardware/pio.h"
#include "hardware/spi.h"
#include "hardware/uart.h"
#include "sph0645.pio.h"
#include <algorithm>
#include "hardware/dma.h"

#define PIN_DATA_OUT 2
#define PIN_SCLK 3
#define PIN_WS 4

#define printu(var) printf("%s: %lu\n", (#var), (size_t) (var))

#define bswap(x) \
  ((((x) & 0xff000000u) >> 24) | (((x) & 0x00ff0000u) >> 8)	\
   | (((x) & 0x0000ff00u) << 8) | (((x) & 0x000000ffu) << 24))

size_t clk;
PIO pio = pio0;
uint sm;
uint dma_chan;

#define BLOCK_SIZE (48000)

void init() {
  stdio_uart_init_full(uart0, 921600, 0, 1);
  /* stdio_init_all(); */
  clk = clock_get_hz(clk_sys);
  dma_chan = dma_claim_unused_channel(true);
}

static void start_dma(int32_t* buf, size_t len) {
  dma_channel_config c = dma_channel_get_default_config(dma_chan);
  channel_config_set_read_increment(&c, false);
  channel_config_set_write_increment(&c, true);
  channel_config_set_dreq(&c, pio_get_dreq(pio, sm, false));
  dma_channel_configure(dma_chan, &c, buf, &pio->rxf[sm], len, true);
}

static void finalize_dma() {
  dma_channel_wait_for_finish_blocking(dma_chan);
}

static void print_samples(int32_t* samples, size_t len) {
    for (size_t i = 0; i < len; i++) {
      auto val = samples[i];
      printf("%d\t%X\n", val, val);
    }
    /* printf("("); */
    /* for (size_t i = 0; i < len; i++) { */
    /*   printf("%d, ", samples[i]); */
    /*   /1* printf("%08X, ", samples[i]); *1/ */
    /* } */
    /* printf(")\n"); */
}

static void normalize(int32_t* samples, size_t len) {
  for (int i = 0; i < 10; i++) {
    start_dma(samples, len);
    finalize_dma();
  }
}


int main() {

  init();

  auto offset = pio_add_program(pio, &i2s_program);
  sm = pio_claim_unused_sm(pio, true);
  i2s_program_init(pio, sm, offset, PIN_DATA_OUT, PIN_SCLK);
  /* start_dma(samples, BLOCK_SIZE); */
  /* finalize_dma(); */
  /* normalize(samples, BLOCK_SIZE); */

  int32_t samples[BLOCK_SIZE] = {0};

  auto start_time = time_us_32();
  for (size_t i = 0; i < BLOCK_SIZE; i++) {
      uint32_t val = pio_sm_get_blocking(pio, sm);
      samples[i] = *((int32_t *) &val);
  }

  /* printf("%lu\n", time_us_32() - start_time); */

  /* for (size_t i = 0; i < BLOCK_SIZE; i++) { */
  /*   samples[i] = pio_sm_get_blocking(pio, sm); */
  /* } */

  print_samples(samples, BLOCK_SIZE);
  return 0;
}
