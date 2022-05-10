#!/usr/bin/env python3

import os
import usb1
import time
import struct
import random
import subprocess

from binascii import hexlify

dir = os.path.dirname(os.path.realpath(__file__))
FW_PATH = f"{dir}/../../firmware/faxitron.img"

DEBUG = False

class Faxitron:
  DEBUG_EP = 0xA
  DATA_IN_EP = 0x1
  DATA_OUT_EP = 0x1

  def __init__(self):
    self.ctx = usb1.USBContext()

    # Check to see if we're in bootloader
    bootloader_dev = self.find_device(0x04b4, 0x00f3)
    if bootloader_dev is not None:
      # We are! Let's load the latest fw
      self.download_image(FW_PATH)
      time.sleep(2)

    # Find and connect to the device
    for _ in range(20):
      dev = self.find_device(0x1996, 0x0001)
      if dev != None:
        break
      time.sleep(0.1)
    assert dev != None, "Could not connect to the device!"

    # Open
    self.handle = dev.open()
    self.handle.claimInterface(0)
    print("Connected!")

  def find_device(self, vid, pid):
    for dev in self.ctx.getDeviceList(skip_on_error=True):
      if dev.getVendorID() == vid and dev.getProductID() == pid:
        return dev
    return None

  def download_image(self, img_path):
    return subprocess.call([f"{dir}/download_fx3", '-t', 'RAM', '-i', img_path])

  def reset(self):
    self.handle.controlWrite(usb1.ENDPOINT_IN | usb1.TYPE_VENDOR | usb1.RECIPIENT_DEVICE, 0xE0, 0, 0, b"", timeout=100)

  def read_logs(self):
    try:
      dat = self.handle.interruptRead(Faxitron.DEBUG_EP, 256, timeout=1)
    except usb1.USBErrorTimeout:
      return None

    while len(dat) > 0:
      priority, thread, id, param = struct.unpack('<BBHI', dat[:8])
      dat = dat[8:]
      if id <= 0xFFEE:
        print(f"Structured: Priority: {priority} Thread: {thread} Id: {hex(id)} Param: {hex(param)}")
      else:
        log_str = dat[:param].decode('utf-8', errors='ignore')
        dat = dat[param:]
        print(f"Debug: Priority: {priority} Thread: {thread} Id: {hex(id)} Log: {log_str}")

  def read_data(self, max=1024, timeout=50):
    try:
      dat = b""
      last_len = -1
      while last_len != len(dat) and len(dat) < max:
        last_len = len(dat)
        dat += self.handle.bulkRead(Faxitron.DATA_IN_EP, 512, timeout=timeout)

      assert len(dat) % 2 == 0
      DEBUG and print("READ", hexlify(dat, sep=' ', bytes_per_sep=2))

      res = []
      for i in range(len(dat) // 2):
        res.append(dat[2*i] + ((dat[2*i+1] & 0x0F) << 8))

      return res
    except usb1.USBErrorTimeout:
      return []

  def write_data(self, dat):
    assert len(dat) < 128, "TODO: loop over data"

    # we need to pad it since it's 16 bits
    arr = b"".join(map(lambda t: ((t & 0xFFF) | 0xE000).to_bytes(2, byteorder='little'), dat))

    DEBUG and print("WRITE", hexlify(arr, sep=' ', bytes_per_sep=2))

    return self.handle.bulkWrite(Faxitron.DATA_OUT_EP, arr, timeout=100)


if __name__ == '__main__':
  f = Faxitron()

  time.sleep(0.2)

  # Clear buffers
  f.read_data()

  dat = os.urandom(10000)
  tx_buf = dat[:]
  rx_buf = b""
  while len(tx_buf) > 0:
    l = random.randint(1, 127)
    f.write_data(tx_buf[:l])
    tx_buf = tx_buf[l:]

    print(len(tx_buf))
    t = f.read_data()
    if t:
      rx_buf += bytearray(t)

  for i, v in enumerate(zip(dat, rx_buf)):
    a, b = v
    if a != b:
      print("FIRST ERROR:", i, a, b)
      break

  print(dat[:200], "\n")
  print(rx_buf[:200])

  f.reset()

  print(len(dat), len(rx_buf))

