#!/usr/bin/python3
# -*- coding: utf-8 -*-

from PyQt5.QtMultimediaWidgets import QVideoWidget
from PyQt5.QtMultimedia import QMediaPlayer, QMediaPlaylist, QMediaContent
from PyQt5.QtWidgets import QMessageBox
from PyQt5.QtCore import QUrl

import time

class VideoPlayerManager(QVideoWidget):
    def __init__(self, parent):
        super().__init__(parent)

        self.parent = parent

        self.player = QMediaPlayer(parent)
        self.playlist = QMediaPlaylist(self)

        self.player.setVideoOutput(self)
        self.player.setPlaylist(self.playlist)

        self.seekDuration = 1

        self.player.error.connect(self.handleError)
        self.player.durationChanged.connect(self.updateSeekDuration)

    def activateVideo(self):
        self.playlist.clear()

        basePath = self.parent.project.getProjectFileLocation()
        for file in self.parent.project.getVideoFiles():
            self.playlist.addMedia(QMediaContent(QUrl.fromLocalFile(basePath + "/" + file)))

        self.playlist.setCurrentIndex(0)

        self.player.play()

    def updateSeekDuration(self, duration):
        self.seekDuration = duration / 10
        if self.seekDuration > 1000:
            self.seekDuration = 1000

    def handleError(self, error):
        errorMessage = "Media Player error: " + self.player.errorString()
        QMessageBox.critical(self.parent, "Error", str(error))

    def play(self, value=True):
        if value:
            self.player.play()
        else:
            self.player.pause()

    def playPause(self):
        if self.player.state() == QMediaPlayer.PlayingState:
            self.player.pause()
        else:
            self.player.play()

    def setPosition(self, position):
        self.player.setPosition(position)

    def seekForward(self):
        self.player.setPosition(self.player.position() + self.seekDuration)

    def seekBackward(self):
        self.player.setPosition(self.player.position() - self.seekDuration)
