#!/usr/bin/env python3

import sys
import signal

from PySide6.QtCore import Qt
from PySide6.QtWidgets import QApplication, QFrame, QWidget, QHBoxLayout, QVBoxLayout, QSplitter

from dalsa_teensy import DalsaTeensy

class App(QWidget):
  def __init__(self, parent=None):
    super().__init__(parent)

    splitter = QSplitter()
    splitter.setOrientation(Qt.Horizontal)

    left_column = QVBoxLayout()

    # add left column stuff

    left_widget = QWidget()
    left_widget.setLayout(left_column)
    splitter.addWidget(left_widget)

    right_column = QVBoxLayout()

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

  app = QApplication(sys.argv)
  frame = QFrame()
  frame.setWindowTitle("Faxitron Dalsa Teensy")
  frame.resize(1600, 1200)
  sp = App(frame)
  frame.show()
  app.exec()
