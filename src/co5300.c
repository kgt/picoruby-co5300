#include "../include/co5300.h"

static void
fill(int id, uint32_t color, size_t count, uint8_t *buf, size_t buf_len)
{
  size_t buf_px = buf_len / 3;
  if (count < buf_px) buf_px = count;
  buf_len = buf_px * 3;

  uint8_t r = (color >> 16) & 0xFF;
  uint8_t g = (color >> 8) & 0xFF;
  uint8_t b = color & 0xFF;
  for (size_t i = 0; i < buf_len; i+= 3) {
    buf[i]     = r;
    buf[i + 1] = g;
    buf[i + 2] = b;
  }

  for (size_t sent = 0; sent < count; sent += buf_px) {
    size_t remain = count - sent;
    CO5300_flush_buffer(id, buf, remain < buf_px ? remain * 3 : buf_len, true);
  }
}

#if defined(PICORB_VM_MRUBY)

#include "mruby/co5300.c"

#elif defined(PICORB_VM_MRUBYC)

#error "mruby/c is not supported yet"
#include "mrubyc/co5300.c"

#endif
