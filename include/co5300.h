#ifndef CO5300_DEFINED_H_
#define CO5300_DEFINED_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct co5300_config {
  uint pwr_pin;
  uint rst_pin;
  uint cs_pin;
  uint sclk_pin;
  uint d0_pin;
  uint pio_num;
  uint sm;
  uint offset;
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
