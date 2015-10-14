#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from PyQt5.QtWidgets import QWidget, QDialog, QTimeEdit, QPushButton, QFormLayout, QVBoxLayout, QHBoxLayout
from PyQt5.QtCore import QTime

class TimeSelectDialog(QDialog):
    def __init__(self, parent, videoManager, windowTitle, timeSelectorLabel):
        super().__init__(parent)

        self.videoManager = videoManager

        position = videoManager.player.position()
        qPosition = QTime(0, 0, 0).addMSecs(position)

        mainLayout = QVBoxLayout()

        formLayout = QFormLayout()
        self.timeEditor = QTimeEdit(qPosition)
        self.timeEditor.setDisplayFormat("HH:mm:ss.zzz")
        formLayout.addRow(timeSelectorLabel, self.timeEditor)

        formWidget = QWidget()
        formWidget.setLayout(formLayout)

        buttonsLayout = QHBoxLayout()

        buttonsLayout.addStretch()

        self.cancelButton = QPushButton("Cancel")
        buttonsLayout.addWidget(self.cancelButton)

        self.okButton = QPushButton("OK")
        self.okButton.setDefault(True)
        buttonsLayout.addWidget(self.okButton)

        buttonsWidget = QWidget()
        buttonsWidget.setLayout(buttonsLayout)

        mainLayout.addWidget(formWidget)
        mainLayout.addWidget(buttonsWidget)

        self.setLayout(mainLayout)
        self.setWindowTitle(windowTitle)

class JumpToTimeDialog(TimeSelectDialog):
    def __init__(self, parent, videoManager):
        super().__init__(parent, videoManager, "Jump to time", "Time to jump to:")

        self.cancelButton.clicked.connect(self.cancel)
        self.okButton.clicked.connect(self.validate)
        self.timeEditor.editingFinished.connect(self.validate)

    def cancel(self):
        self.done(1)

    def validate(self):
        self.videoManager.player.setPosition(-self.timeEditor.dateTime().time().msecsTo(QTime(0, 0, 0)))
        self.done(0)

class AddBreakPointDialog(TimeSelectDialog):
    def __init__(self, parent, videoManager, projectManager):
        super().__init__(parent, videoManager, "Add breakpoint", "Breakpoint position:")

        self.projectManager = projectManager

        self.cancelButton.clicked.connect(self.cancel)
        self.okButton.clicked.connect(self.validate)
        self.timeEditor.editingFinished.connect(self.validate)

    def cancel(self):
        self.done(1)

    def validate(self):
        self.projectManager.addBreakpoint(-self.timeEditor.dateTime().time().msecsTo(QTime(0, 0, 0)))
        self.done(0)
