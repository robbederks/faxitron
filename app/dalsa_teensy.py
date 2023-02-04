#!/usr/bin/env python3

import usb1
import struct

class DalsaTeensy:
  DALSA_INTERFACE = 2
  CONTROL_OUT_ENDPOINT = 5
  CONTROL_IN_ENDPOINT = 6
  BULK_IN_ENDPOINT = 7

  STRUCT_STATE = struct.Struct("<IIIB???")

  FAXITRON_STATE_WARMING_UP = "warming_up"
  FAXITRON_STATE_DOOR_OPEN = "door_open"
  FAXITRON_STATE_READY = "ready"

  FAXITRON_MODE_FRONT_PANEL = "front_panel"
  FAXITRON_MODE_REMOTE = "remote"

  def __init__(self):
    self._handle = None
    self.connect()

  def connect(self):
    if self._handle is not None:
      self._handle.close()

    self._handle = usb1.USBContext().openByVendorIDAndProductID(0x16c0, 0x0483)
    assert self._handle is not None, "Could not find Dalsa Teensy. Make sure it's connected."

    self._handle.claimInterface(DalsaTeensy.DALSA_INTERFACE)

    print("Connected to Dalsa Teensy")

  def _control_out(self, data):
    return self._handle.bulkWrite(
      endpoint=DalsaTeensy.CONTROL_OUT_ENDPOINT,
      data=data,
    )

  def _control_in(self, size):
    return self._handle.bulkRead(
      endpoint=DalsaTeensy.CONTROL_IN_ENDPOINT,
      length=size,
    )

  def _bulk_in(self, size):
    return self._handle.bulkRead(
      endpoint=DalsaTeensy.BULK_IN_ENDPOINT,
      length=size,
    )

  def _command(self, cmd, data):
    self._control_out(struct.pack("<BI", cmd, len(data)) + data)
    resp = self._control_in(512)
    if len(resp) < 4:
      raise Exception("Invalid response length", len(resp))
    resp_len = struct.unpack("<I", resp[:4])[0]
    resp = resp[4:]
    while len(resp) < resp_len:
      resp += self._control_in(resp_len - len(resp))
    return resp

  def ping(self):
    dat = self._command(0x00, b"")
    assert len(dat) == 1, "Response does not match expected size"
    assert dat[0] == 0xA5, "Invalid ping response"

  def get_state(self):
    dat = self._command(0x01, b"")
    assert len(dat) == self.STRUCT_STATE.size, f"Response does not match expected struct size: {len(dat)} != {self.STRUCT_STATE.size}"
    dat_unpacked = self.STRUCT_STATE.unpack(dat)
    return {
      'row': dat_unpacked[0],
      'col': dat_unpacked[1],
      'ph_v_counter': dat_unpacked[2],
      'readout_pin': dat_unpacked[3],
      'busy': dat_unpacked[4],
      'done': dat_unpacked[5],
    }

  def get_frame(self):
    dat = self._command(0x02, b"")
    assert len(dat) == 4, "Response does not match expected size"
    frame_len = struct.unpack("<I", dat[:4])[0]
    return self._bulk_in(frame_len)

  def start_readout(self, high_gain=False):
    dat = self._command(0x03, b"\x01" if high_gain else b"\x00")
    assert len(dat) == 1, "Response does not match expected size"
    if dat[0] != 0:
      raise Exception("Failed to start readout, is another readout in progress?")

  def get_faxitron_state(self):
    dat = self._command(0x10, b"?S").decode()
    assert len(dat) == 3, "Response does not match expected size"
    if dat == "?SR":
      return self.FAXITRON_STATE_READY
    elif dat == "?SD":
      return self.FAXITRON_STATE_DOOR_OPEN
    elif dat == "?SW":
      return self.FAXITRON_STATE_WARMING_UP
    print(dat)
    return None

  def get_faxitron_exposure_time(self):
    dat = self._command(0x10, b"?T").decode()
    assert "?T" in dat, "Response does not match expected format"
    return int(dat[2:])/10

  def set_faxitron_exposure_time(self, exposure_time):
    assert 0 < exposure_time <= 99.9, "Exposure time must be between 0 and 99.9s"
    self._command(0x10, b"!T" + str(int(exposure_time * 10)).rjust(4, "0")[:4].encode())

  def get_faxitron_voltage(self):
    dat = self._command(0x10, b"?V").decode()
    assert "?V" in dat, "Response does not match expected format"
    return int(dat[2:])

  def set_faxitron_voltage(self, voltage):
    assert 0 < voltage <= 35, "Voltage must be between 0 and 35"
    self._command(0x10, b"!V" + str(voltage).rjust(2, "0")[:2].encode())

  def get_faxitron_mode(self):
    dat = self._command(0x10, b"?M").decode()
    assert len(dat) == 3, "Response does not match expected size"
    if dat == "?MF":
      return self.FAXITRON_MODE_FRONT_PANEL
    elif dat == "?MR":
      return self.FAXITRON_MODE_REMOTE
    return None

  def set_faxitron_mode(self, mode):
    assert mode in [self.FAXITRON_MODE_FRONT_PANEL, self.FAXITRON_MODE_REMOTE], "Invalid mode"
    modes = {
      self.FAXITRON_MODE_FRONT_PANEL: b"!MF",
      self.FAXITRON_MODE_REMOTE: b"!MR",
    }
    self._command(0x10, modes[mode])

  def perform_faxitron_exposure(self):
    dat = self._command(0x10, b"!B")
    assert dat == b"X", "Response does not match expected format"
    dat = self._command(0x10, b"C")
    assert dat == b"P", "Response does not match expected format"
    while len(dat := self._command(0x10, b"")) == 0:
      pass
    assert dat == b"S", "Response does not match expected format"

if __name__ == "__main__":
  dalsa_teensy = DalsaTeensy()
  dalsa_teensy.ping()

  print("Faxitron state:", dalsa_teensy.get_faxitron_state())
  dalsa_teensy.set_faxitron_exposure_time(30)
  print("Faxitron exposure time:", dalsa_teensy.get_faxitron_exposure_time())
  dalsa_teensy.set_faxitron_voltage(20)
  print("Faxitron voltage:", dalsa_teensy.get_faxitron_voltage())
  print("Faxitron mode:", dalsa_teensy.get_faxitron_mode())
  dalsa_teensy.set_faxitron_mode(DalsaTeensy.FAXITRON_MODE_REMOTE)
  print("Faxitron mode:", dalsa_teensy.get_faxitron_mode())

  # dalsa_teensy.perform_faxitron_exposure()
