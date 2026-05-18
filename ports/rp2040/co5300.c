#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/dma.h"

#include "../../include/co5300.h"
#include "co5300_qspi.pio.h"

void
CO5300_init(co5300_config_t *config)
{
  uint gpio_mask = 1 << config->pwr_pin | 1 << config->rst_pin | 1 << config->cs_pin;
  gpio_init_mask(gpio_mask);
  gpio_set_dir_out_masked(gpio_mask);

  gpio_put(config->cs_pin, 1);

  PIO pio;
  uint sm;
  uint offset;
  pio_claim_free_sm_and_add_program(&co5300_qspi_program, &pio, &sm, &offset);
  co5300_qspi_program_init(pio, sm, offset, config->sclk_pin, config->d0_pin, 75000000);

  config->pio_num = pio_get_index(pio);
  config->sm      = sm;
  config->offset  = offset;

  return;
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
  pio_sm_put_blocking(pio, config->sm, (uint32_t)(value & 0x80) << 21 | (uint32_t)(value & 0x40) << 18);
  pio_sm_put_blocking(pio, config->sm, (uint32_t)(value & 0x20) << 23 | (uint32_t)(value & 0x10) << 20);
  pio_sm_put_blocking(pio, config->sm, (uint32_t)(value & 0x08) << 25 | (uint32_t)(value & 0x04) << 22);
  pio_sm_put_blocking(pio, config->sm, (uint32_t)(value & 0x02) << 27 | (uint32_t)(value & 0x01) << 24);
}

void
CO5300_qspi4_write_byte(co5300_config_t *config, uint8_t value)
{
  pio_sm_put_blocking(pio_get_instance(config->pio_num), config->sm, (uint32_t)value << 24);
}

void
CO5300_deinit(co5300_config_t *config)
{
  pio_remove_program_and_unclaim_sm(&co5300_qspi_program, pio_get_instance(config->pio_num), config->sm, config->offset);
}
