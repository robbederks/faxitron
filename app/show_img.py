#!/usr/bin/env python3

from matplotlib import pyplot as plt
import numpy as np
from sympy import false

dat = []
with open("resp.dat", 'r') as f:
  for l in f:
    dat.append(int(l))

frame = []
with open("resp_hex.dat", "w") as f:
  for i in range(101, (len(dat)//2)*2, 2):
    f.write(f"{hex(dat[i+1])} {hex(dat[i])} \t {bin(dat[i+1] << 6 | dat[i])[2:].zfill(16)}\n")
    frame.append((dat[i+1] << 6 | dat[i]) & 0b1111111111)

with open("resp_frame.dat", "w") as f:
  for d in frame:
    f.write(f"{bin(d)[2:].zfill(16)} {hex(d)}\n")

COLS = 1026

# plt.hist(frame, bins=1000)
# plt.show()

# normalize
max_pixel = 800
min_pixel = 100
gamma = 2.2
print(f"Max: {max_pixel}")
frame = list(map(lambda n: max(0, min(1, ((n-min_pixel)/(max_pixel-min_pixel))))**(1/gamma), frame))

print(len(frame))
rows = len(frame) // COLS
print(rows)
plt.imshow(np.reshape(frame[:COLS*rows], (COLS, rows)), cmap="Greys")
#plt.imshow(np.reshape(frame[:COLS*rows], (COLS, rows)))
plt.show()


# rows = []
# saw_320 = False
# t = []
# for i in range(1, len(dat)-1, 2):
#   if dat[i] == 0x140:
#     saw_320 = True

#   if saw_320:
#     if dat[i] == 0x1c0:
#       rows.append(t[:])
#       t.clear()

#     t.append((dat[i], dat[i+1]))


# print("ROWS", len(rows))
# for i, r in enumerate(rows):
#   print(i, len(r))

# #dat = list(filter(lambda d: d < 480, dat))
# metadat = list(filter(lambda d: d > 260 and d != 480, dat))

# print(len(dat))
# rows = len(dat) // COLS
# print(rows)

# # plt.plot(dat)
# # plt.show()

# print(metadat, len(metadat))

# # plt.imshow(np.reshape(dat[:COLS*rows], (COLS, rows)), cmap="Greys")
# plt.show()