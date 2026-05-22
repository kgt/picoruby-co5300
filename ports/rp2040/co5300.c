#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/dma.h"

#include "../../include/co5300.h"
#include "co5300_qspi.pio.h"

static int dma_chan;
static dma_channel_config_t dma_config;

static void
dma_start(uint8_t *buf, uint32_t len, bool blocking)
{
  dma_channel_wait_for_finish_blocking(dma_chan);
  dma_channel_transfer_from_buffer_now(dma_chan, buf, len);
  if (blocking) dma_channel_wait_for_finish_blocking(dma_chan);
}

void
CO5300_init(co5300_config_t *config)
{
  uint gpio_mask = 1u << config->pwr_pin | 1u << config->rst_pin | 1u << config->cs_pin;
  gpio_init_mask(gpio_mask);
  gpio_set_dir_out_masked(gpio_mask);

  gpio_put(config->cs_pin, true);

  PIO pio;
  uint sm;
  uint offset;
  pio_claim_free_sm_and_add_program(&co5300_qspi_program, &pio, &sm, &offset);
  co5300_qspi_program_init(pio, sm, offset, config->sclk_pin, config->d0_pin, 75000000);

  config->pio_num = pio_get_index(pio);
  config->sm      = sm;
  config->offset  = offset;

  dma_chan = dma_claim_unused_channel(true);
  dma_config = dma_channel_get_default_config(dma_chan);
  channel_config_set_transfer_data_size(&dma_config, DMA_SIZE_8);
  channel_config_set_read_increment(&dma_config, true);
  channel_config_set_write_increment(&dma_config, false);
  channel_config_set_dreq(&dma_config, pio_get_dreq(pio, sm, true));
  dma_channel_set_write_addr(dma_chan, &pio->txf[sm], false);
  dma_channel_set_config(dma_chan, &dma_config, false);

  return;
}

void
CO5300_fill(co5300_config_t *config, uint32_t color, uint32_t count)
{
  uint32_t px_len = CO5300_BUF_LEN / 3;
  if (count < px_len) px_len = count;
  uint32_t buf_len = px_len * 3;

  uint8_t r = (color >> 16) & 0xFF;
  uint8_t g = (color >> 8) & 0xFF;
  uint8_t b = color & 0xFF;
  for (uint32_t i = 0; i < buf_len; i += 3) {
    config->buf[i]     = r;
    config->buf[i + 1] = g;
    config->buf[i + 2] = b;
  }

  uint32_t byte = count * 3;
  for (uint32_t sent = 0; sent < byte; sent += buf_len) {
    uint32_t remain = byte - sent;
    dma_start(config->buf, remain < buf_len ? remain : buf_len, true);
  }
}

void
CO5300_pwr_write(co5300_config_t *config, uint8_t value)
{
  gpio_put(config->pwr_pin, value > 0);
}

void
CO5300_rst_write(co5300_config_t *config, uint8_t value)
{
  gpio_put(config->rst_pin, value > 0);
}

void
CO5300_cs_write(co5300_config_t *config, uint8_t value)
{
  gpio_put(config->cs_pin, value > 0);
}

void
CO5300_qspi1_write_byte(co5300_config_t *config, uint8_t value)
{
  PIO pio = pio_get_instance(config->pio_num);
  uint32_t _value = (uint32_t)value;
  pio_sm_put_blocking(pio, config->sm, (_value & 0x80) << 21 | (_value & 0x40) << 18);
  pio_sm_put_blocking(pio, config->sm, (_value & 0x20) << 23 | (_value & 0x10) << 20);
  pio_sm_put_blocking(pio, config->sm, (_value & 0x08) << 25 | (_value & 0x04) << 22);
  pio_sm_put_blocking(pio, config->sm, (_value & 0x02) << 27 | (_value & 0x01) << 24);
}

void
CO5300_qspi4_write_byte(co5300_config_t *config, uint8_t value)
{
  pio_sm_put_blocking(pio_get_instance(config->pio_num), config->sm, (uint32_t)value << 24);
}

void
CO5300_deinit(co5300_config_t *config)
{
  dma_channel_unclaim(dma_chan);
  pio_remove_program_and_unclaim_sm(&co5300_qspi_program, pio_get_instance(config->pio_num), config->sm, config->offset);
}
