#!/usr/bin/env python3

import os
import time
import random
from binascii import hexlify
from faxitron.faxitron import Faxitron

# DLB needs to be set to true in the firmware

if __name__ == "__main__":
  f = Faxitron()

  time.sleep(0.2)

  # Clear buffers
  f.read_data()

  #dat = os.urandom(10000)

  dat = bytearray(list(range(255, 0, -1)) * 1000)
  #dat = bytearray(list(range(255, 0, -1)) * 10)

  tx_buf = dat[:]
  rx_buf = b""
  while len(tx_buf) > 0:
    l = random.randint(1, 127)
    f.write_data(tx_buf[:l])
    tx_buf = tx_buf[l:]

    print(len(tx_buf))
    t = f.read_data(timeout=1)
    if t:
      rx_buf += bytearray(t)

  for i, v in enumerate(zip(dat, rx_buf)):
    a, b = v
    if a != b:
      print("FIRST ERROR:", i, a, b)
      break

  print(hexlify(dat[:200], sep=' '), "\n")
  print(hexlify(rx_buf[:200], sep=' '))

  f.reset()

  print(len(dat), len(rx_buf))
