#!/usr/bin/python3
# -*- coding: utf-8 -*-

from slideo.videoplayermanager import VideoPlayerManager
from slideo.projectmanager import ProjectManager

from PyQt5.QtWidgets import QWidget, QMainWindow, QAction, QDockWidget, QListView, QHBoxLayout, QVBoxLayout, QPushButton, QSlider, QFileDialog
from PyQt5.QtGui import QIcon, QKeySequence
from PyQt5.QtCore import pyqtSignal, QCoreApplication, Qt, QStringListModel, QUrl, QFileInfo
from PyQt5.QtMultimedia import QMediaPlayer

class MainWindow(QMainWindow):
    projectActivated = pyqtSignal(bool)

    def __init__(self):
        super().__init__()

        self.project = None

        self.initCentralZone()
        self.initActionWidgets()

        self.resize(800, 600)
        self.setWindowTitle("Slideo")

        self.show()

    def initActionWidgets(self):

        # ===============
        # == File Menu ==
        # ===============

        fileMenu = self.menuBar().addMenu("&File")

        newPresentationAction = QAction(QIcon.fromTheme("document-new"), "&New presentation", self)
        newPresentationAction.setShortcut(QKeySequence("Ctrl+N"))
        fileMenu.addAction(newPresentationAction)

        openPresentationAction = QAction(QIcon.fromTheme("document-open"), "&Open presentation", self)
        openPresentationAction.setShortcut(QKeySequence("Ctrl+O"))
        fileMenu.addAction(openPresentationAction)

        fileMenu.addSeparator()

        quitAction = QAction(QIcon.fromTheme("application-exit"), "E&xit", self)
        quitAction.setShortcut(QKeySequence("Ctrl+Q"))
        quitAction.triggered.connect(QCoreApplication.instance().quit)
        fileMenu.addAction(quitAction)

        # ===============
        # == View Menu ==
        # ===============

        viewMenu = self.menuBar().addMenu("&View")

        fullscreenAction = QAction("&Fullscreen", self)
        fullscreenAction.setCheckable(True)
        fullscreenAction.setShortcut(QKeySequence("F11"))
        fullscreenAction.toggled.connect(self.showFullscreen)
        viewMenu.addAction(fullscreenAction)

        viewMenu.addSeparator()

        startSlideshowAction = QAction(QIcon.fromTheme("media-playback-start"), "&Start slideshow", self)
        startSlideshowAction.setShortcut(QKeySequence("F5"))
        startSlideshowAction.setDisabled(True)
        viewMenu.addAction(startSlideshowAction)

        startFromHereAction = QAction("&Start slideshow from here", self)
        startFromHereAction.setShortcut(QKeySequence("Shift+F5"))
        startFromHereAction.setDisabled(True)
        viewMenu.addAction(startFromHereAction)

        # =============
        # == Toolbar ==
        # =============

        toolbar = self.addToolBar("Main toolbar")
        toolbar.addAction(newPresentationAction)
        toolbar.addAction(openPresentationAction)
        toolbar.addSeparator()
        toolbar.addAction(startSlideshowAction)

        # ==========
        # == Dock ==
        # ==========

        self.breakpointListView = QListView()
        self.breakpointListModel = QStringListModel()
        self.breakpointListView.setModel(self.breakpointListModel)

        breakpointsDock = QDockWidget("Breakpoints", self)
        breakpointsDock.setWidget(self.breakpointListView)
        self.addDockWidget(Qt.LeftDockWidgetArea, breakpointsDock)

        # =================
        # == Connections ==
        # =================

        openPresentationAction.triggered.connect(self.openProject)

        self.projectActivated.connect(startSlideshowAction.setEnabled)
        self.projectActivated.connect(startFromHereAction.setEnabled)

    def initCentralZone(self):
        self.centralZone = QWidget()
        centralZoneLayout = QVBoxLayout()

        # ==================
        # == Video Player ==
        # ==================

        self.videoPlayer = VideoPlayerManager(self)
        centralZoneLayout.addWidget(self.videoPlayer)

        # ==========================
        # == Video Player Toolbar ==
        # ==========================

        playerToolbarLayout = QHBoxLayout()

        playerSeekBackwardButton = QPushButton(QIcon.fromTheme("media-seek-backward"), "")
        playerSeekBackwardButton.setEnabled(False)
        playerToolbarLayout.addWidget(playerSeekBackwardButton)

        self.playerStartButton = QPushButton(QIcon.fromTheme("media-playback-start"), "")
        self.playerStartButton.setEnabled(False)
        playerToolbarLayout.addWidget(self.playerStartButton)

        playerSeekForwardButton = QPushButton(QIcon.fromTheme("media-seek-forward"), "")
        playerSeekForwardButton.setEnabled(False)
        playerToolbarLayout.addWidget(playerSeekForwardButton)

        self.slider = QSlider(Qt.Horizontal)
        self.slider.setRange(0, 0)
        self.slider.setEnabled(False)
        playerToolbarLayout.addWidget(self.slider)

        playerToolbarWidget = QWidget()
        playerToolbarWidget.setLayout(playerToolbarLayout)
        centralZoneLayout.addWidget(playerToolbarWidget)

        # ==============
        # == Timeline ==
        # ==============

        self.timeline = QWidget()
        centralZoneLayout.addWidget(self.timeline)

        self.centralZone.setLayout(centralZoneLayout)
        self.setCentralWidget(self.centralZone)

        # =================
        # == Connections ==
        # =================

        playerSeekBackwardButton.clicked.connect(self.videoPlayer.seekBackward)
        self.playerStartButton.clicked.connect(self.videoPlayer.playPause)
        playerSeekForwardButton.clicked.connect(self.videoPlayer.seekForward)
        self.slider.valueChanged.connect(self.videoPlayer.setPosition)

        # Activations
        self.projectActivated.connect(playerSeekBackwardButton.setEnabled)
        self.projectActivated.connect(self.playerStartButton.setEnabled)
        self.projectActivated.connect(playerSeekForwardButton.setEnabled)
        self.projectActivated.connect(self.slider.setEnabled)

        self.projectActivated.connect(self.videoPlayer.activateVideo)

        self.videoPlayer.player.stateChanged.connect(self.updatePlayButtonIcon)
        self.videoPlayer.player.durationChanged.connect(self.updateSliderRange)
        self.videoPlayer.player.positionChanged.connect(self.updateSliderPosition)

    def showFullscreen(self, value):
        if value:
            self.showFullScreen()
        else:
            self.showNormal()

    def openProject(self):
        filename, _ = QFileDialog.getOpenFileName(self, "Open Project")
        if filename != '':
            self.project = ProjectManager(filename)
            self.projectActivated.emit(True)

    def updatePlayButtonIcon(self, state):
        if state == QMediaPlayer.PlayingState:
            self.playerStartButton.setIcon(QIcon.fromTheme("media-playback-pause"))
        else:
            self.playerStartButton.setIcon(QIcon.fromTheme("media-playback-start"))

    def updateSliderRange(self, duration):
        self.slider.setRange(0, duration)

    def updateSliderPosition(self, position):
        self.slider.setValue(position)
