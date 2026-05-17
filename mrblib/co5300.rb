require "pio"

class CO5300
  def initialize(pwr:, rst:, cs:, sclk:, d0:, pio: PIO::PIO0, sm: 0)
    @pin_pwr = GPIO.new(pwr, GPIO::OUT)
    @pin_rst = GPIO.new(rst, GPIO::OUT)
    @pin_cs = GPIO.new(cs, GPIO::OUT)

    prog = PIO.asm(side_set: 1) do
      wrap_target
      out :pins, 28, side: 0
      nop            side: 1
      out :pins,  4, side: 0
      nop            side: 1
      wrap
    end
    @sm = PIO::StateMachine.new(
      pio: pio,
      sm: sm,
      program: prog,
      freq: 75_000_000,
      out_pins: d0,
      out_pin_count: 4,
      sideset_pins: sclk,
      out_shift_right: false,
      out_shift_autopull: true,
      out_shift_threshold: 32,
      fifo_join: PIO::FIFO_JOIN_TX
    )
    @sm.start
  end

  def on
    @pin_pwr.write 1
    @pin_rst.write 0
    sleep_ms 100
    @pin_rst.write 1
    sleep_ms 200
  end

  def init
    send_command 0xFE
    send_command 0xC4, 0x80
    send_command 0x35, 0x00
    sleep_ms 10
    send_command 0x53, 0x20
    sleep_ms 10
    send_command 0x51, 0xFF
    sleep_ms 10
    send_command 0x63, 0xFF
    sleep_ms 10
    send_command 0x11
    sleep_ms 120
    send_command 0x29
    sleep_ms 10
  end

  def draw_rect(x:, y:, w:, h:, color:)
    set_window x, y, w, h
    @pin_cs.write 0
    write_1line 0x32, 0x00, 0x2c, 0x00
    write_4line *color * w * h
    @pin_cs.write 1
  end

  def send_command(command, *params)
    @pin_cs.write 0
    write_1line 0x02, 0x00, command, 0x00, *params
    @pin_cs.write 1
  end

  private

  def set_window(x, y, w, h)
    x_data = Array.new(4)
    x_data[0] = (x >> 8) & 0x03
    x_data[1] = x & 0xFF
    x_data[2] = ((x + w - 1) >> 8) & 0x03
    x_data[3] = (x + w - 1) & 0xFF

    y_data = Array.new(4)
    y_data[0] = (y >> 8) & 0x03
    y_data[1] = y & 0xFF
    y_data[2] = ((y + h - 1) >> 8) & 0x03
    y_data[3] = (y + h - 1) & 0xFF

    send_command 0x2A, *x_data
    send_command 0x2B, *y_data
  end

  def write_1line(*bytes)
    new_bytes = Array.new(bytes.length * 4)
    i = 0

    while i < bytes.length
      byte = bytes[i]

      # 0bABCDEFGH -> 0b000A000B, 0b000C000D, 0b000E000F, 0b000G000H
      new_bytes[i * 4]     = (byte & 0x80) >> 3 | (byte & 0x40) >> 6
      new_bytes[i * 4 + 1] = (byte & 0x20) >> 1 | (byte & 0x10) >> 4
      new_bytes[i * 4 + 2] = (byte & 0x08) << 1 | (byte & 0x04) >> 2
      new_bytes[i * 4 + 3] = (byte & 0x02) << 3 | byte & 0x01

      i += 1
    end

    @sm.put_buffer new_bytes
  end

  def write_4line(*bytes)
    @sm.put_buffer bytes
  end
end
