#include <mruby.h>
#include <mruby/presym.h>
#include <mruby/variable.h>
#include <mruby/data.h>
#include <mruby/array.h>
#include <mruby/class.h>

#include "../include/co5300.h"

static void
mrb_co5300_free(mrb_state *mrb, void *ptr)
{
  mrb_free(mrb, ptr);
}

struct mrb_data_type mrb_co5300_type = {
  "CO5300", mrb_co5300_free
};

static mrb_value
mrb_co5300_s_init(mrb_state *mrb, mrb_value klass)
{
  mrb_int pwr_pin, rst_pin, cs_pin, sclk_pin, d0_pin;
  mrb_get_args(mrb, "iiiii", &pwr_pin, &rst_pin, &cs_pin, &sclk_pin, &d0_pin);

  co5300_config_t *config = (co5300_config_t *)mrb_malloc(mrb, sizeof(co5300_config_t));
  config->pwr_pin  = (uint)pwr_pin;
  config->rst_pin  = (uint)rst_pin;
  config->cs_pin   = (uint)cs_pin;
  config->sclk_pin = (uint)sclk_pin;
  config->d0_pin   = (uint)d0_pin;

  mrb_value self = mrb_obj_value(Data_Wrap_Struct(mrb, mrb_class_ptr(klass), &mrb_co5300_type, config));

  CO5300_init(config);

  return self;
}

static mrb_value
mrb_co5300_pwr_write(mrb_state *mrb, mrb_value self)
{
  co5300_config_t *config = (co5300_config_t *)mrb_data_get_ptr(mrb, self, &mrb_co5300_type);
  mrb_int value;
  mrb_get_args(mrb, "i", &value);
  CO5300_pwr_write(config, value);
  return mrb_nil_value();
}

static mrb_value
mrb_co5300_rst_write(mrb_state *mrb, mrb_value self)
{
  co5300_config_t *config = (co5300_config_t *)mrb_data_get_ptr(mrb, self, &mrb_co5300_type);
  mrb_int value;
  mrb_get_args(mrb, "i", &value);
  CO5300_rst_write(config, value);
  return mrb_nil_value();
}

static mrb_value
mrb_co5300_cs_write(mrb_state *mrb, mrb_value self)
{
  co5300_config_t *config = (co5300_config_t *)mrb_data_get_ptr(mrb, self, &mrb_co5300_type);
  mrb_int value;
  mrb_get_args(mrb, "i", &value);
  CO5300_cs_write(config, value);
  return mrb_nil_value();
}

static mrb_value
mrb_co5300_qspi1_write_byte(mrb_state *mrb, mrb_value self)
{
  co5300_config_t *config = (co5300_config_t *)mrb_data_get_ptr(mrb, self, &mrb_co5300_type);
  mrb_int value;
  mrb_get_args(mrb, "i", &value);
  CO5300_qspi1_write_byte(config, value);
  return mrb_nil_value();
}

static mrb_value
mrb_co5300_qspi4_write_byte(mrb_state *mrb, mrb_value self)
{
  co5300_config_t *config = (co5300_config_t *)mrb_data_get_ptr(mrb, self, &mrb_co5300_type);
  mrb_int value;
  mrb_get_args(mrb, "i", &value);
  CO5300_qspi4_write_byte(config, value);
  return mrb_nil_value();
}

void
mrb_picoruby_co5300_gem_init(mrb_state* mrb)
{
  struct RClass *class_CO5300 = mrb_define_class_id(mrb, MRB_SYM(CO5300), mrb->object_class);

  MRB_SET_INSTANCE_TT(class_CO5300, MRB_TT_CDATA);

  mrb_define_class_method_id(mrb, class_CO5300, MRB_SYM(init), mrb_co5300_s_init, MRB_ARGS_REQ(5));
  mrb_define_method_id(mrb, class_CO5300, MRB_SYM(pwr_write), mrb_co5300_pwr_write, MRB_ARGS_REQ(1));
  mrb_define_method_id(mrb, class_CO5300, MRB_SYM(rst_write), mrb_co5300_rst_write, MRB_ARGS_REQ(1));
  mrb_define_method_id(mrb, class_CO5300, MRB_SYM(cs_write), mrb_co5300_cs_write, MRB_ARGS_REQ(1));
  mrb_define_method_id(mrb, class_CO5300, MRB_SYM(qspi1_write_byte), mrb_co5300_qspi1_write_byte, MRB_ARGS_REQ(1));
  mrb_define_method_id(mrb, class_CO5300, MRB_SYM(qspi4_write_byte), mrb_co5300_qspi4_write_byte, MRB_ARGS_REQ(1));
}

void
mrb_picoruby_co5300_gem_final(mrb_state* mrb)
{
}
