#!/usr/bin/env python3

from distutils import log
import os
from requests import head
import usb1
import time
import struct
import subprocess

dir = os.path.dirname(os.path.realpath(__file__))
FW_PATH = f"{dir}/../firmware/faxitron.img"

class Faxitron:
  DEBUG_EP = 0x81

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
      dat = self.handle.interruptRead(Faxitron.DEBUG_EP, 256, timeout=100)
    except usb1.USBErrorTimeout:
      return None

    while len(dat) > 0:
      priority, id, param = struct.unpack('<BxHI', dat[:8])
      dat = dat[8:]
      if id <= 0xFFEE:
        print(f"Structured: Priority: {priority} Id: {hex(id)} Param: {hex(param)}")
      else:
        log_str = dat[:param].decode('utf-8', errors='ignore')
        dat = dat[param:]
        print(f"Debug: Priority: {priority} Id: {hex(id)} Log: {log_str}")

if __name__ == '__main__':
  f = Faxitron()
  while 1:
    f.read_logs()
