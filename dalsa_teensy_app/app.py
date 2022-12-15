#!/usr/bin/env python3

import usb1
import time
import struct


class DalsaTeensy:
  DALSA_INTERFACE = 2
  CONTROL_OUT_ENDPOINT = 5
  CONTROL_IN_ENDPOINT = 6
  BULK_IN_ENDPOINT = 7

  STRUCT_STATE = struct.Struct("<IIB??")

  def __init__(self):
    self._handle = None
    self.connect()

  def connect(self):
    if self._handle is not None:
      self._handle.close()

    self._handle = usb1.USBContext().openByVendorIDAndProductID(0x16c0, 0x0483)
    assert self._handle is not None, "Could not find Dalsa Teensy. Make sure it's connected."

    self._handle.claimInterface(DalsaTeensy.DALSA_INTERFACE)

    print("Connected to Dalsa Teensy")

  def _control_out(self, data):
    return self._handle.bulkWrite(
      endpoint=DalsaTeensy.CONTROL_OUT_ENDPOINT,
      data=data,
    )

  def _control_in(self, size):
    return self._handle.bulkRead(
      endpoint=DalsaTeensy.CONTROL_IN_ENDPOINT,
      length=size,
    )

  def _bulk_in(self, size):
    return self._handle.bulkRead(
      endpoint=DalsaTeensy.BULK_IN_ENDPOINT,
      length=size,
    )

  def _command(self, cmd, data):
    self._control_out(struct.pack("<BI", cmd, len(data)) + data)
    resp = self._control_in(512)
    if len(resp) < 4:
      raise Exception("Invalid response length", len(resp))
    resp_len = struct.unpack("<I", resp[:4])[0]
    print(resp_len)
    resp = resp[4:]
    while len(resp) < resp_len:
      resp += self._control_in(resp_len - len(resp))
    return resp

  def ping(self):
    dat = self._command(0x00, b"")
    print(dat)

  def get_state(self):
    dat = self._command(0x01, b"")
    assert len(dat) == self.STRUCT_STATE.size, "Response does not match expected struct size"
    dat_unpacked = self.STRUCT_STATE.unpack(dat)
    return {
      'row': dat_unpacked[0],
      'col': dat_unpacked[1],
      'readout_pin': dat_unpacked[2],
      'busy': dat_unpacked[3],
      'done': dat_unpacked[4],
    }

  def get_frame(self):
    dat = self._command(0x02, b"")
    assert len(dat) == 4, "Response does not match expected size"
    frame_len = struct.unpack("<I", dat[:4])[0]
    return self._bulk_in(frame_len)

if __name__ == "__main__":
  dalsa_teensy = DalsaTeensy()

  st = time.monotonic()
  for _ in range(10):
    dalsa_teensy.ping()
    print(dalsa_teensy.get_state())

    print(len(dalsa_teensy.get_frame()))

  print(time.monotonic() - st)