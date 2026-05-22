class CO5300
  def self.new(pwr:, rst:, cs:, sclk:, d0:)
    co5300 = init(pwr, rst, cs, sclk, d0)
    co5300.power_on
    co5300.init

    co5300
  end

  def power_on
    pwr_write 1
    rst_write 0
    sleep_ms 100
    rst_write 1
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
    start_qspi do
      qspi1_write_bytes 0x32, 0x00, 0x2c, 0x00
      fill color, w * h
    end
  end

  def send_command(command, *params)
    start_qspi do
      qspi1_write_bytes 0x02, 0x00, command, 0x00, *params
    end
  end

  private

  def start_qspi
    cs_write 0
    yield self
  ensure
    cs_write 1
  end

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

  def qspi1_write_bytes(*bytes)
    i = 0
    while i < bytes.length
      qspi1_write_byte bytes[i]
      i += 1
    end
  end

  def qspi4_write_bytes(*bytes)
    i = 0
    while i < bytes.length
      qspi4_write_byte bytes[i]
      i += 1
    end
  end
end
