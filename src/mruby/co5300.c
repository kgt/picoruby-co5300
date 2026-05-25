#include <mruby.h>
#include <mruby/presym.h>
#include <mruby/variable.h>
#include <mruby/data.h>
#include <mruby/array.h>
#include <mruby/class.h>

typedef struct {
  int     id;
  uint8_t *buf;
} mrb_co5300_t;

static void
mrb_co5300_free(mrb_state *mrb, void *p)
{
  mrb_co5300_t *co5300 = (mrb_co5300_t *)p;
  CO5300_deinit(co5300->id);
  mrb_free(mrb, co5300->buf);
  mrb_free(mrb, co5300);
}

struct mrb_data_type mrb_co5300_type = {
  "CO5300", mrb_co5300_free
};

static mrb_value
mrb_co5300_s_init(mrb_state *mrb, mrb_value klass)
{
  mrb_int pwr_pin, rst_pin, cs_pin, sclk_pin, d0_pin;
  mrb_get_args(mrb, "iiiii", &pwr_pin, &rst_pin, &cs_pin, &sclk_pin, &d0_pin);

  co5300_config_t config;
  config.pwr_pin  = (uint8_t)pwr_pin;
  config.rst_pin  = (uint8_t)rst_pin;
  config.cs_pin   = (uint8_t)cs_pin;
  config.sclk_pin = (uint8_t)sclk_pin;
  config.d0_pin   = (uint8_t)d0_pin;
  config.buf      = (uint8_t *)mrb_malloc(mrb, CO5300_BUF_LEN);

  mrb_co5300_t *co5300 = (mrb_co5300_t *)mrb_malloc(mrb, sizeof(mrb_co5300_t));
  co5300->id  = CO5300_init(&config);
  co5300->buf = config.buf;

  mrb_value self = mrb_obj_value(Data_Wrap_Struct(mrb, mrb_class_ptr(klass), &mrb_co5300_type, co5300));

  return self;
}

static mrb_value
mrb_co5300_fill(mrb_state *mrb, mrb_value self)
{
  mrb_co5300_t *co5300 = (mrb_co5300_t *)mrb_data_get_ptr(mrb, self, &mrb_co5300_type);
  mrb_value color_ary;
  mrb_int count;
  mrb_get_args(mrb, "Ai", &color_ary, &count);

  uint8_t r = (uint8_t)mrb_integer(mrb_ary_ref(mrb, color_ary, 0));
  uint8_t g = (uint8_t)mrb_integer(mrb_ary_ref(mrb, color_ary, 1));
  uint8_t b = (uint8_t)mrb_integer(mrb_ary_ref(mrb, color_ary, 2));
  uint32_t color = (r << 16) | (g << 8) | b;

  CO5300_fill(co5300->id, color, (uint32_t)count);

  return mrb_nil_value();
}

static mrb_value
mrb_co5300_pwr_write(mrb_state *mrb, mrb_value self)
{
  mrb_co5300_t *co5300 = (mrb_co5300_t *)mrb_data_get_ptr(mrb, self, &mrb_co5300_type);
  mrb_int value;
  mrb_get_args(mrb, "i", &value);
  CO5300_pwr_write(co5300->id, (uint8_t)value);
  return mrb_nil_value();
}

static mrb_value
mrb_co5300_rst_write(mrb_state *mrb, mrb_value self)
{
  mrb_co5300_t *co5300 = (mrb_co5300_t *)mrb_data_get_ptr(mrb, self, &mrb_co5300_type);
  mrb_int value;
  mrb_get_args(mrb, "i", &value);
  CO5300_rst_write(co5300->id, (uint8_t)value);
  return mrb_nil_value();
}

static mrb_value
mrb_co5300_cs_write(mrb_state *mrb, mrb_value self)
{
  mrb_co5300_t *co5300 = (mrb_co5300_t *)mrb_data_get_ptr(mrb, self, &mrb_co5300_type);
  mrb_int value;
  mrb_get_args(mrb, "i", &value);
  CO5300_cs_write(co5300->id, (uint8_t)value);
  return mrb_nil_value();
}

static mrb_value
mrb_co5300_qspi1_write_byte(mrb_state *mrb, mrb_value self)
{
  mrb_co5300_t *co5300 = (mrb_co5300_t *)mrb_data_get_ptr(mrb, self, &mrb_co5300_type);
  mrb_int value;
  mrb_get_args(mrb, "i", &value);
  CO5300_qspi1_write_byte(co5300->id, (uint8_t)value);
  return mrb_nil_value();
}

static mrb_value
mrb_co5300_qspi4_write_byte(mrb_state *mrb, mrb_value self)
{
  mrb_co5300_t *co5300 = (mrb_co5300_t *)mrb_data_get_ptr(mrb, self, &mrb_co5300_type);
  mrb_int value;
  mrb_get_args(mrb, "i", &value);
  CO5300_qspi4_write_byte(co5300->id, (uint8_t)value);
  return mrb_nil_value();
}

void
mrb_picoruby_co5300_gem_init(mrb_state* mrb)
{
  struct RClass *class_CO5300 = mrb_define_class_id(mrb, MRB_SYM(CO5300), mrb->object_class);

  MRB_SET_INSTANCE_TT(class_CO5300, MRB_TT_CDATA);

  mrb_define_class_method_id(mrb, class_CO5300, MRB_SYM(init), mrb_co5300_s_init, MRB_ARGS_REQ(5));
  mrb_define_method_id(mrb, class_CO5300, MRB_SYM(fill), mrb_co5300_fill, MRB_ARGS_REQ(2));
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
