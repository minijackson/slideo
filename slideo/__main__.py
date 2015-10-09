#!/usr/bin/python3
# -*- coding: utf-8 -*-

from slideo.mainwindow import MainWindow

import sys
from PyQt5.QtWidgets import QApplication

if __name__ == "__main__":

    app = QApplication(sys.argv)
    mainWindow = MainWindow()

    sys.exit(app.exec_())
