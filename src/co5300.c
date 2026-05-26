#include "../include/co5300.h"

extern const uint8_t terminus_16x32[6080];

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

static void
draw_text(int id, const char *text, uint32_t fg_color, uint32_t bg_color, uint8_t *buf, size_t buf_len)
{
  buf_len = buf_len / 3 * 3;

  uint8_t fg_r = (fg_color >> 16) & 0xFF;
  uint8_t fg_g = (fg_color >> 8) & 0xFF;
  uint8_t fg_b = fg_color & 0xFF;
  uint8_t bg_r = (bg_color >> 16) & 0xFF;
  uint8_t bg_g = (bg_color >> 8) & 0xFF;
  uint8_t bg_b = bg_color & 0xFF;

  size_t j = 0;
  for (size_t y = 0; y < 32; y++) {
    for (const char *p = text; *p; p++) {
      uint8_t c = (uint8_t)*p;
      if (c < 0x20 || 0x7E < c) c = 0x20;

      const uint8_t *data = terminus_16x32 + (c - 0x20) * 64 + y * 2;
      for (size_t i = 0; i < 16; i++, j += 3) {
        if (j >= buf_len) {
          CO5300_flush_buffer(id, buf, buf_len, true);
          j -= buf_len;
        }

        if (data[i / 8] & (0x80 >> (i % 8))) {
          buf[j]     = fg_r;
          buf[j + 1] = fg_g;
          buf[j + 2] = fg_b;
        } else {
          buf[j]     = bg_r;
          buf[j + 1] = bg_g;
          buf[j + 2] = bg_b;
        }
      }
    }
  }
  CO5300_flush_buffer(id, buf, j, true);
}

#if defined(PICORB_VM_MRUBY)

#include "mruby/co5300.c"

#elif defined(PICORB_VM_MRUBYC)

#error "mruby/c is not supported yet"
#include "mrubyc/co5300.c"

#endif
