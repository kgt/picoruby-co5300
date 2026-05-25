#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/dma.h"

#include "../../include/co5300.h"
#include "co5300_qspi.pio.h"

#define MAX_CO5300_HANDLERS 1

typedef struct {
  co5300_config_t config;
  PIO             pio;
  uint            sm;
  uint            offset;
  uint            dma_chan;
  bool            initialized;
} co5300_handler_t;

static co5300_handler_t handlers[MAX_CO5300_HANDLERS];

static void
dma_start(uint dma_chan, uint8_t *buf, uint32_t len, bool blocking)
{
  dma_channel_wait_for_finish_blocking(dma_chan);
  dma_channel_transfer_from_buffer_now(dma_chan, buf, len);
  if (blocking) dma_channel_wait_for_finish_blocking(dma_chan);
}

int
CO5300_init(const co5300_config_t *config)
{
  int id = -1;
  for (int i = 0; i < MAX_CO5300_HANDLERS; i++) {
    if (!handlers[i].initialized) {
      id = i;
      break;
    }
  }
  if (id < 0) return -1;

  co5300_handler_t *handler = handlers + id;

  handler->config = *config;

  uint gpio_mask = 1u << config->pwr_pin | 1u << config->rst_pin | 1u << config->cs_pin;
  gpio_init_mask(gpio_mask);
  gpio_set_dir_out_masked(gpio_mask);

  gpio_put(config->cs_pin, true);

  pio_claim_free_sm_and_add_program(&co5300_qspi_program, &handler->pio, &handler->sm, &handler->offset);
  co5300_qspi_program_init(handler->pio, handler->sm, handler->offset, config->sclk_pin, config->d0_pin, 75000000);

  handler->dma_chan = dma_claim_unused_channel(true);
  dma_channel_config_t dma_config = dma_channel_get_default_config(handler->dma_chan);
  channel_config_set_transfer_data_size(&dma_config, DMA_SIZE_8);
  channel_config_set_read_increment(&dma_config, true);
  channel_config_set_write_increment(&dma_config, false);
  channel_config_set_dreq(&dma_config, pio_get_dreq(handler->pio, handler->sm, true));
  dma_channel_set_write_addr(handler->dma_chan, &handler->pio->txf[handler->sm], false);
  dma_channel_set_config(handler->dma_chan, &dma_config, false);

  handler->initialized = true;

  return id;
}

void
CO5300_fill(int id, uint32_t color, uint32_t count)
{
  uint32_t px_len = CO5300_BUF_LEN / 3;
  if (count < px_len) px_len = count;
  uint32_t buf_len = px_len * 3;

  uint8_t r = (color >> 16) & 0xFF;
  uint8_t g = (color >> 8) & 0xFF;
  uint8_t b = color & 0xFF;
  for (uint32_t i = 0; i < buf_len; i += 3) {
    handlers[id].config.buf[i]     = r;
    handlers[id].config.buf[i + 1] = g;
    handlers[id].config.buf[i + 2] = b;
  }

  uint32_t byte = count * 3;
  for (uint32_t sent = 0; sent < byte; sent += buf_len) {
    uint32_t remain = byte - sent;
    dma_start(handlers[id].dma_chan, handlers[id].config.buf, remain < buf_len ? remain : buf_len, true);
  }
}

void
CO5300_pwr_write(int id, uint8_t value)
{
  gpio_put(handlers[id].config.pwr_pin, value > 0);
}

void
CO5300_rst_write(int id, uint8_t value)
{
  gpio_put(handlers[id].config.rst_pin, value > 0);
}

void
CO5300_cs_write(int id, uint8_t value)
{
  gpio_put(handlers[id].config.cs_pin, value > 0);
}

void
CO5300_qspi1_write_byte(int id, uint8_t value)
{
  uint32_t _value = (uint32_t)value;
  pio_sm_put_blocking(handlers[id].pio, handlers[id].sm, (_value & 0x80) << 21 | (_value & 0x40) << 18);
  pio_sm_put_blocking(handlers[id].pio, handlers[id].sm, (_value & 0x20) << 23 | (_value & 0x10) << 20);
  pio_sm_put_blocking(handlers[id].pio, handlers[id].sm, (_value & 0x08) << 25 | (_value & 0x04) << 22);
  pio_sm_put_blocking(handlers[id].pio, handlers[id].sm, (_value & 0x02) << 27 | (_value & 0x01) << 24);
}

void
CO5300_qspi4_write_byte(int id, uint8_t value)
{
  pio_sm_put_blocking(handlers[id].pio, handlers[id].sm, (uint32_t)value << 24);
}

void
CO5300_deinit(int id)
{
  dma_channel_unclaim(handlers[id].dma_chan);
  pio_remove_program_and_unclaim_sm(&co5300_qspi_program, handlers[id].pio, handlers[id].sm, handlers[id].offset);
  handlers[id].initialized = false;
}
