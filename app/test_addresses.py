#!/usr/bin/env python3

import time
from faxitron.faxitron import Faxitron

if __name__ == "__main__":
  f = Faxitron()

  for i in range(1023, 1024):
    print("write", i)
    f.write_data([i])

    time.sleep(0.1)

    d = []
    st = time.monotonic()
    while time.monotonic() - st < 4:
      d.extend(map(lambda b: (b >> 10, b & 0x3FF), f.read_data(timeout=100)))

    print('TOTAL', len(d))
    for c, dat in d[:5]:
      print(bin(c), hex(dat), bin(dat))

    with open(f"response_{i}.dat", 'w') as out:
      for _, dat in d:
        out.write(f"{dat}\n")

    time.sleep(0.2)
