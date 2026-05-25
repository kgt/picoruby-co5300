#ifndef CO5300_DEFINED_H_
#define CO5300_DEFINED_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CO5300_BUF_LEN (48 * 1024)

typedef struct {
  uint8_t pwr_pin;
  uint8_t rst_pin;
  uint8_t cs_pin;
  uint8_t sclk_pin;
  uint8_t d0_pin;
} co5300_config_t;

int CO5300_init(const co5300_config_t *c);
void CO5300_flush_buffer(int id, const uint8_t *buf, size_t buf_len, bool blocking);
void CO5300_pwr_write(int id, uint8_t value);
void CO5300_rst_write(int id, uint8_t value);
void CO5300_cs_write(int id, uint8_t value);
void CO5300_qspi1_write_byte(int id, uint8_t value);
void CO5300_qspi4_write_byte(int id, uint8_t value);
void CO5300_deinit(int id);

#ifdef __cplusplus
}
#endif

#endif /* CO5300_DEFINED_H_ */
