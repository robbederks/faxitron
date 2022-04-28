#!/usr/bin/env python3

import os
import usb1
import time
import struct
import subprocess

dir = os.path.dirname(os.path.realpath(__file__))
FW_PATH = f"{dir}/../firmware/faxitron.img"

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

  def read_data(self):
    try:
      return self.handle.bulkRead(Faxitron.DATA_IN_EP, 256, timeout=100)
    except usb1.USBErrorTimeout:
      return None

  def write_data(self, dat):
    assert len(dat) <= 256, "TODO: loop over data"
    return self.handle.bulkWrite(Faxitron.DATA_OUT_EP, dat, timeout=100)

if __name__ == '__main__':
  f = Faxitron()

  time.sleep(0.2)

  i=0
  while 1:
    f.read_logs()

    try:
      dat = f.read_data()
      if dat is not None:
        print('read', dat)
        print(len(dat))
    except Exception as e:
      print('re', e)

    try:
      f.write_data(b"ABCDEFGH"*16)
      i += 1
      if i%1000 == 0:
        print(i)
    except Exception as e:
      print('we', e)
    time.sleep(0.01)

