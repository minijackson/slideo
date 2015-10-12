#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from PyQt5.QtWidgets import QWidget, QDialog, QTimeEdit, QPushButton, QFormLayout, QVBoxLayout, QHBoxLayout
from PyQt5.QtCore import QTime

class TimeSelectDialog(QDialog):
    def __init__(self, parent, videoManager):
        super().__init__(parent)

        self.videoManager = videoManager

        position = videoManager.player.position()
        qPosition = QTime(0, 0, 0).addMSecs(position)

        mainLayout = QVBoxLayout()

        formLayout = QFormLayout()
        self.timeEditor = QTimeEdit(qPosition)
        self.timeEditor.setDisplayFormat("HH:mm:ss.zzz")
        formLayout.addRow("Time to jump to:", self.timeEditor)

        formWidget = QWidget()
        formWidget.setLayout(formLayout)

        buttonsLayout = QHBoxLayout()

        buttonsLayout.addStretch()
        cancelButton = QPushButton("Cancel")
        buttonsLayout.addWidget(cancelButton)
        okButton = QPushButton("OK")
        okButton.setDefault(True)
        buttonsLayout.addWidget(okButton)

        buttonsWidget = QWidget()
        buttonsWidget.setLayout(buttonsLayout)

        mainLayout.addWidget(formWidget)
        mainLayout.addWidget(buttonsWidget)

        self.setLayout(mainLayout)

        cancelButton.clicked.connect(self.cancel)
        okButton.clicked.connect(self.validate)
        self.timeEditor.editingFinished.connect(self.validate)

    def cancel(self):
        self.done(1)

    def validate(self):
        self.videoManager.player.setPosition(-self.timeEditor.dateTime().time().msecsTo(QTime(0, 0, 0)))
        self.done(0)
