#!/usr/bin/env python3

from matplotlib import pyplot as plt
import numpy as np
from sympy import false

dat = []
with open("resp.dat", 'r') as f:
  for l in f:
    dat.append(int(l))

COLS = 1025

rows = []
saw_320 = False
t = []
for i in range(1, len(dat)-1, 2):
  if dat[i] == 0x140:
    saw_320 = True

  if saw_320:
    if dat[i] == 0x1c0:
      rows.append(t[:])
      t.clear()

    t.append((dat[i], dat[i+1]))


print("ROWS", len(rows))
for i, r in enumerate(rows):
  print(i, len(r))

#dat = list(filter(lambda d: d < 480, dat))
metadat = list(filter(lambda d: d > 260 and d != 480, dat))

print(len(dat))
rows = len(dat) // COLS
print(rows)

# plt.plot(dat)
# plt.show()

print(metadat, len(metadat))

# plt.imshow(np.reshape(dat[:COLS*rows], (COLS, rows)), cmap="Greys")
# plt.show()