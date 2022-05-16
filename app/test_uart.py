#!/usr/bin/env python3

import time
from faxitron.faxitron import Faxitron


if __name__ == "__main__":
  f = Faxitron()

  time.sleep(0.1)

  i = 0
  try:
    f.write_uart("?D\r".encode('ascii'))

    while True:
      f.read_logs()

      i += 1
      if i % 50 == 0:
        f.write_uart("?D\r".encode('ascii'))

      d = f.read_uart()
      # if len(d) > 0:
      #   print(d)
  except Exception as e:
    print(e)
    pass

  f.reset()
