#!/usr/bin/env python3

import cv2
import sys
import time
import signal
import numpy as np

from PySide6.QtCore import Qt, Signal, Slot, QTimer, QThread
from PySide6.QtWidgets import QApplication, QFrame, QWidget, QHBoxLayout, QVBoxLayout, QSplitter, QLabel, QLineEdit, QGroupBox, QPushButton
from PySide6.QtGui import QImage, QPixmap

from dalsa_teensy import DalsaTeensy

dalsa_teensy = None
raw_frame = np.zeros((DalsaTeensy.FRAME_WIDTH, DalsaTeensy.FRAME_HEIGHT), dtype=np.uint16)

class Setting(QHBoxLayout):
  value_changed = Signal(str)

  def __init__(self, title, default_value=None, parent=None):
    super().__init__(parent)

    self.label = QLabel(title)
    self.addWidget(self.label)

    self.value = QLineEdit()
    if default_value is not None:
      self.value.setText(default_value)
    self.value.returnPressed.connect(lambda: self.value_changed.emit(self.value.text()))
    self.value.focusOutEvent = lambda _: self.value_changed.emit(self.value.text())
    self.addWidget(self.value)

class FaxitronGroupBox(QGroupBox):
  def exposure_changed(self, value):
    try:
      val = float(value)
      if val != dalsa_teensy.get_faxitron_exposure_time():
        dalsa_teensy.set_faxitron_exposure_time(val)
        print(f"Exposure changed to {value}s")
    except Exception as e:
      print(e)

  def voltage_changed(self, value):
    try:
      val = float(value)
      if val != dalsa_teensy.get_faxitron_voltage():
        dalsa_teensy.set_faxitron_voltage(val)
        print(f"Voltage changed to {value}s")
    except Exception as e:
      print(e)

  def tick(self):
    if self.exposure_start_time is not None:
      self.fire_button.setText(f"Exposing... ({int(time.monotonic() - self.exposure_start_time)}s / {int(self.exposure_time + 2)}s)")
    else:
      self.state_label.setText(f"State: {dalsa_teensy.get_faxitron_state()}")

  class FireThread(QThread):
    done = Signal()

    def __init__(self, parent=None):
      super().__init__(parent)

    def run(self):
      dalsa_teensy.perform_faxitron_exposure()
      self.done.emit()

  def fire(self):
    self.exposure_time = dalsa_teensy.get_faxitron_exposure_time()

    def fire_done():
      self.exposure_start_time = None
      self.fire_button.setEnabled(True)
      self.fire_button.setText("Fire!")

    # start this in a thread
    self.fire_thread = self.FireThread()
    self.fire_thread.done.connect(fire_done)

    self.exposure_start_time = time.monotonic()
    self.fire_button.setEnabled(False)
    self.fire_thread.start()

  def __init__(self, parent=None):
    super().__init__("Faxitron", parent)

    self.exposure_start_time = None
    self.exposure_time = 0

    layout = QVBoxLayout()

    self.state_label = QLabel("State: N/A")
    layout.addWidget(self.state_label)

    exposure_control = Setting("Exposure (s)", str(dalsa_teensy.get_faxitron_exposure_time()))
    exposure_control.value_changed.connect(self.exposure_changed)
    layout.addLayout(exposure_control)

    voltage_control = Setting("Voltage (kV)", str(dalsa_teensy.get_faxitron_voltage()))
    voltage_control.value_changed.connect(self.voltage_changed)
    layout.addLayout(voltage_control)

    self.fire_button = QPushButton("Fire!")
    self.fire_button.clicked.connect(self.fire)
    layout.addWidget(self.fire_button)

    self.setLayout(layout)

    # start periodic update timer
    self.timer = QTimer()
    self.timer.timeout.connect(self.tick)
    self.timer.start(250)


class DalsaGroupBox(QGroupBox):
  new_frame = Signal()

  class ReadoutThread(QThread):
    done = Signal()
    progress = Signal(int)

    def __init__(self, parent=None):
      super().__init__(parent)

    def run(self):
      global raw_frame

      dalsa_teensy.start_readout(True)
      state = dalsa_teensy.get_state()
      while not state['done']:
        state = dalsa_teensy.get_state()
        self.progress.emit(int(state['row'] / 1032 * 100))
        time.sleep(0.1)
      frame = dalsa_teensy.get_frame()
      assert len(frame) == 2150688, "Frame is not of the expected size"
      raw_frame = np.frombuffer(frame, dtype=np.uint16).reshape((DalsaTeensy.FRAME_WIDTH, DalsaTeensy.FRAME_HEIGHT))

      self.done.emit()

  def readout(self):
    def readout_done():
      self.readout_button.setEnabled(True)
      self.readout_button.setText("Read out")
      self.new_frame.emit()

    self.readout_button.setEnabled(False)
    self.readout_thread = self.ReadoutThread()
    self.readout_thread.done.connect(readout_done)
    self.readout_thread.progress.connect(lambda p: self.readout_button.setText(f"Reading out... ({p}%)"))
    self.readout_thread.start()

  def __init__(self, parent=None):
    super().__init__("Dalsa", parent)

    layout = QVBoxLayout()

    self.readout_button = QPushButton("Read out")
    self.readout_button.clicked.connect(self.readout)
    layout.addWidget(self.readout_button)

    self.setLayout(layout)

class App(QWidget):
  def new_frame(self):
    normalized_img = cv2.normalize(-1 * raw_frame, None, 0, 2**16, cv2.NORM_MINMAX, dtype=cv2.CV_16U)
    self.image_label.setPixmap(QPixmap.fromImage(QImage(bytes(normalized_img), DalsaTeensy.FRAME_HEIGHT, DalsaTeensy.FRAME_WIDTH, QImage.Format_Grayscale16)))

  def __init__(self, parent=None):
    super().__init__(parent)

    splitter = QSplitter()
    splitter.setOrientation(Qt.Horizontal)

    left_column = QVBoxLayout()

    # add left column stuff
    self.image_label = QLabel()
    self.new_frame()
    left_column.addWidget(self.image_label)

    left_widget = QWidget()
    left_widget.setLayout(left_column)
    splitter.addWidget(left_widget)

    right_column = QVBoxLayout()

    self.faxitron_group_box = FaxitronGroupBox()
    right_column.addWidget(self.faxitron_group_box)

    self.dalsa_group_box = DalsaGroupBox()
    self.dalsa_group_box.new_frame.connect(self.new_frame)
    right_column.addWidget(self.dalsa_group_box)

    right_column.addStretch()

    # add right column stuff

    right_widget = QWidget()
    right_widget.setLayout(right_column)
    splitter.addWidget(right_widget)

    layout = QHBoxLayout()
    layout.addWidget(splitter)
    parent.setLayout(layout)

if __name__ == "__main__":
  # make ctrl-C work
  signal.signal(signal.SIGINT, signal.SIG_DFL)

  dalsa_teensy = DalsaTeensy()
  dalsa_teensy.ping()
  dalsa_teensy.set_faxitron_mode(DalsaTeensy.FAXITRON_MODE_REMOTE)

  app = QApplication(sys.argv)
  frame = QFrame()
  frame.setWindowTitle("Faxitron Dalsa Teensy")
  frame.resize(1600, 1200)
  sp = App(frame)
  frame.show()
  app.exec()
