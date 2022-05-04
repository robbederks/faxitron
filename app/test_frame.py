#!/usr/bin/env python3

import time
import numpy as np
from matplotlib import pyplot as plt
from faxitron.faxitron import Faxitron

def start_exposure(f):
  f.write_data([0x9, 0x1])

def stop_exposure(f):
  f.write_data([0x9, 0x2])


if __name__ == "__main__":
  f = Faxitron()

  time.sleep(0.1)

  start_exposure(f)
  print("Start XRAY")
  #time.sleep(10)
  stop_exposure(f)

  d = []
  st = time.monotonic()
  while time.monotonic() - st < 3:
    d.extend(map(lambda b: (b >> 10, b & 0x3FF), f.read_data(timeout=100)))

  print('TOTAL', len(d))

  filtered = list(filter(lambda t: t[1] <= 255, d))
  print('SMALL', len(filtered))
  for c, dat in d[1000:1006]:
    print(bin(c), hex(dat), bin(dat))

  with open(f"resp.dat", 'w') as out:
    for _, dat in d:
      out.write(f"{dat}\n")

  # plt.plot(list(map(lambda k: k[1], filtered)))
  # plt.show()

  dat = list(map(lambda k: k[1], filtered))

  COLS = 1024
  print(len(dat))
  rows = len(dat) // COLS
  print(rows)

  plt.imshow(np.reshape(dat[:COLS*rows], (COLS, rows)), cmap="Greys")
  plt.show()

  f.reset()
