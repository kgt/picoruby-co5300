#ifndef CO5300_DEFINED_H_
#define CO5300_DEFINED_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct co5300_config {
  uint8_t pwr_pin;
  uint8_t rst_pin;
  uint8_t cs_pin;
  uint8_t sclk_pin;
  uint8_t d0_pin;
  uint8_t pio_num;
  uint8_t sm;
  uint8_t offset;
} co5300_config_t;

void CO5300_init(co5300_config_t *config);
void CO5300_pwr_write(co5300_config_t *config, uint8_t value);
void CO5300_rst_write(co5300_config_t *config, uint8_t value);
void CO5300_cs_write(co5300_config_t *config, uint8_t value);
void CO5300_qspi1_write_byte(co5300_config_t *config, uint8_t value);
void CO5300_qspi4_write_byte(co5300_config_t *config, uint8_t value);
void CO5300_deinit(co5300_config_t *config);

#ifdef __cplusplus
}
#endif

#endif /* CO5300_DEFINED_H_ */
