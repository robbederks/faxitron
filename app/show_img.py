#!/usr/bin/env python3

from matplotlib import pyplot as plt
import numpy as np

dat = []
with open("esp32.dat", 'r') as f:
  for l in f:
    dat.append(int(l))

COLS = 1024

print(len(dat))
rows = len(dat) // COLS
print(rows)

# plt.plot(dat)
# plt.show()

plt.imshow(np.reshape(dat[:COLS*rows], (COLS, rows)), cmap="Greys")
plt.show()