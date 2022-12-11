#!/usr/bin/env python3

import usb1
import time


class DalsaTeensy:
  DALSA_INTERFACE = 2
  CONTROL_OUT_ENDPOINT = 5
  BULK_IN_ENDPOINT = 6

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

  def send_control_out(self, data):
    return self._handle.bulkWrite(
      endpoint=DalsaTeensy.CONTROL_OUT_ENDPOINT,
      data=data,
    )

  def read_bulk(self, size):
    return self._handle.bulkRead(
      endpoint=DalsaTeensy.BULK_IN_ENDPOINT,
      length=size,
    )

if __name__ == "__main__":
  dalsa_teensy = DalsaTeensy()

  st = time.monotonic()
  for _ in range(2000):
    # dalsa_teensy.send_control_out(b"A"*64)
    # print()
    dalsa_teensy.read_bulk(512)
  print(time.monotonic() - st)