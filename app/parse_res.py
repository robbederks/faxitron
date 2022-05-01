#!/usr/bin/env python3

import os

ids = []
responses = {}

for fn in os.listdir('fast/'):
  if "response_" in fn:
    with open('fast/' + fn, 'r') as f:
      id = int(fn.split('_')[1].split('.')[0])
      ids.append(id)
      responses[id] = list(map(lambda d: int(d.strip()), f.readlines()))

ids.sort()

for id in ids:
  print(hex(id), '\t', len(responses[id]), '\t', " ".join(map(lambda d: hex(d), responses[id][:10])))
