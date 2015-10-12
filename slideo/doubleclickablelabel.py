#!/usr/bin/python3
# -*- coding: utf-8 -*-

from PyQt5.QtCore import pyqtSignal
from PyQt5.QtWidgets import QLabel

class DoubleClickableLabel(QLabel):
    mouseDoubleClicked = pyqtSignal()

    def __init__(self):
        super().__init__()

    def __init__(self, string):
        super().__init__(string)

    def mouseDoubleClickEvent(self, event):
        self.mouseDoubleClicked.emit()
