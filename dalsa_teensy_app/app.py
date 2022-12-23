#!/usr/bin/env python3

import sys
import usb1
import time
import struct

import numpy as np
import matplotlib.pyplot as plt

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
    resp = resp[4:]
    while len(resp) < resp_len:
      resp += self._control_in(resp_len - len(resp))
    return resp

  def ping(self):
    dat = self._command(0x00, b"")
    assert len(dat) == 1, "Response does not match expected size"
    assert dat[0] == 0xA5, "Invalid ping response"

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

  def start_readout(self, high_gain=False):
    dat = self._command(0x03, b"\x01" if high_gain else b"\x00")
    assert len(dat) == 1, "Response does not match expected size"
    if dat[0] != 0:
      raise Exception("Failed to start readout, is another readout in progress?")

if __name__ == "__main__":
  dalsa_teensy = DalsaTeensy()
  dalsa_teensy.ping()

  # for hg in [False, True]:
  img = None
  plt.figure()
  while True:
    st = time.monotonic()
    dalsa_teensy.start_readout(True)
    print("Started readout")

    done = False
    while not done:
      time.sleep(0.1)
      state = dalsa_teensy.get_state()
      done = state['done']
    print(f"Readout done in {time.monotonic() - st:.2f}s")

    frame = dalsa_teensy.get_frame()
    assert len(frame) == 2150688, "Frame does not match expected size"

    frame = np.frombuffer(frame, dtype=np.uint16)
    frame = frame.reshape((1024 + 8, 1024 + 18))

    if img is None:
      img = plt.imshow(np.max(frame) - frame, cmap='gray')
    else:
      img.set_data(np.max(frame) - frame)
    plt.draw()
    plt.pause(5)

    if len(sys.argv) > 1:
      plt.show()
      break
