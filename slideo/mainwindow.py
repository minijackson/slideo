#!/usr/bin/python3
# -*- coding: utf-8 -*-

from slideo.videoplayermanager import VideoPlayerManager
from slideo.projectmanager import ProjectManager
from slideo.doubleclickablelabel import DoubleClickableLabel
from slideo.timeselectdialog import JumpToTimeDialog, AddBreakPointDialog
from slideo.utils import parseTime

from PyQt5.QtWidgets import QWidget, QMainWindow, QAction, QDockWidget, QListView, QHBoxLayout, QVBoxLayout, QPushButton, QSlider, QFileDialog, QLabel, QMenu
from PyQt5.QtGui import QIcon, QKeySequence
from PyQt5.QtCore import pyqtSignal, QCoreApplication, Qt, QStringListModel, QUrl, QFileInfo, QTime
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

        # =============
        # == Toolbar ==
        # =============

        startSlideshowAction = QAction(QIcon.fromTheme("media-playback-start"), "&Start slideshow", self)
        startSlideshowAction.setShortcut(QKeySequence("F5"))
        startSlideshowAction.setDisabled(True)

        toolbar = self.addToolBar("Main toolbar")
        toolbar.addAction(newPresentationAction)
        toolbar.addAction(openPresentationAction)
        toolbar.addSeparator()
        toolbar.addAction(startSlideshowAction)

        # ==========
        # == Dock ==
        # ==========

        self.breakpointListView = QListView()
        self.breakpointListView.setEnabled(False)

        self.breakpointListModel = QStringListModel()
        self.breakpointListModel.dataChanged.connect(self.updateProjectBreakpoint)

        self.breakpointListView.setModel(self.breakpointListModel)

        breakpointsDock = QDockWidget("Breakpoints", self)
        breakpointsDock.setWidget(self.breakpointListView)
        breakpointsDock.setContextMenuPolicy(Qt.CustomContextMenu)
        breakpointsDock.customContextMenuRequested.connect(self.showDockContextMenu)
        self.addDockWidget(Qt.LeftDockWidgetArea, breakpointsDock)

        # ===============
        # == Edit Menu ==
        # ===============

        editMenu = self.menuBar().addMenu("&Edit")

        self.addBreakpointAction = QAction(QIcon.fromTheme("list-add"), "&Add breakpoint", self)
        self.addBreakpointAction.setShortcut(QKeySequence("Ctrl+B"))
        self.addBreakpointAction.setEnabled(False)
        editMenu.addAction(self.addBreakpointAction)

        self.addBreakpointHereAction = QAction("Add breakpoint &here", self)
        self.addBreakpointHereAction.setShortcut(QKeySequence("Ctrl+Shift+B"))
        self.addBreakpointHereAction.setEnabled(False)
        editMenu.addAction(self.addBreakpointHereAction)

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

        viewMenu.addAction(startSlideshowAction)

        startFromHereAction = QAction("&Start slideshow from here", self)
        startFromHereAction.setShortcut(QKeySequence("Shift+F5"))
        startFromHereAction.setDisabled(True)
        viewMenu.addAction(startFromHereAction)

        viewMenu.addSeparator()

        jumpToTimeAction = QAction(QIcon.fromTheme("go-jump"), "&Jump to specific time", self)
        jumpToTimeAction.setShortcut(QKeySequence("Ctrl+T"))
        jumpToTimeAction.setDisabled(True)
        viewMenu.addAction(jumpToTimeAction)

        viewMenu.addSeparator()

        viewMenu.addAction(toolbar.toggleViewAction())
        viewMenu.addAction(breakpointsDock.toggleViewAction())

        # =================
        # == Connections ==
        # =================

        openPresentationAction.triggered.connect(self.openProject)

        self.addBreakpointAction.triggered.connect(self.showAddBreakpointDialog)

        jumpToTimeAction.triggered.connect(self.showTimeSelectDialog)

        self.projectActivated.connect(self.addBreakpointAction.setEnabled)
        self.projectActivated.connect(self.addBreakpointHereAction.setEnabled)
        self.projectActivated.connect(startSlideshowAction.setEnabled)
        self.projectActivated.connect(startFromHereAction.setEnabled)
        self.projectActivated.connect(jumpToTimeAction.setEnabled)
        self.projectActivated.connect(self.connectBreakpointsUpdate)
        self.projectActivated.connect(self.breakpointListView.setEnabled)

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

        playerTimeViewerWidget = QWidget()
        playerTimeViewerLayout = QHBoxLayout()
        playerTimeViewerLayout.setSpacing(0)
        playerTimeViewerLayout.setContentsMargins(0, 0, 0, 0)

        self.playerPositionViewer = DoubleClickableLabel("00:00:00.000")
        playerTimeSeparator       = DoubleClickableLabel(" / ")
        self.playerDurationViewer = DoubleClickableLabel("00:00:00.000")

        self.playerPositionViewer.setEnabled(False)
        playerTimeSeparator.setEnabled(False)
        self.playerDurationViewer.setEnabled(False)

        playerTimeViewerLayout.addWidget(self.playerPositionViewer)
        playerTimeViewerLayout.addWidget(playerTimeSeparator)
        playerTimeViewerLayout.addWidget(self.playerDurationViewer)

        playerTimeViewerWidget.setLayout(playerTimeViewerLayout)
        playerToolbarLayout.addWidget(playerTimeViewerWidget)
        playerToolbarLayout.setContentsMargins(0, 0, 0, 0)

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
        self.projectActivated.connect(self.playerStartButton.setFocus)
        self.projectActivated.connect(playerSeekForwardButton.setEnabled)
        self.projectActivated.connect(self.slider.setEnabled)
        self.projectActivated.connect(self.playerPositionViewer.setEnabled)
        self.projectActivated.connect(playerTimeSeparator.setEnabled)
        self.projectActivated.connect(self.playerDurationViewer.setEnabled)
        self.projectActivated.connect(self.videoPlayer.activateVideo)

        self.videoPlayer.player.stateChanged.connect(self.updatePlayButtonIcon)
        self.videoPlayer.player.durationChanged.connect(self.updateSliderRange)
        self.videoPlayer.player.durationChanged.connect(self.updateDurationViewer)
        self.videoPlayer.player.positionChanged.connect(self.updateSliderPosition)
        self.videoPlayer.player.positionChanged.connect(self.updatePositionViewer)

        self.playerPositionViewer.mouseDoubleClicked.connect(self.showTimeSelectDialog)
        playerTimeSeparator.mouseDoubleClicked.connect(self.showTimeSelectDialog)
        self.playerDurationViewer.mouseDoubleClicked.connect(self.showTimeSelectDialog)

    def showTimeSelectDialog(self):
        dialog = JumpToTimeDialog(self, self.videoPlayer)
        dialog.exec()

    def showFullscreen(self, value):
        if value:
            self.showFullScreen()
        else:
            self.showNormal()

    def openProject(self):
        filename, _ = QFileDialog.getOpenFileName(self, "Open Project", "", "Slideo project file (*.eo)")
        if filename != '':
            self.project = ProjectManager(filename)
            self.projectActivated.emit(True)
            self.project.breakpointsChanged.emit()

    def updatePlayButtonIcon(self, state):
        if state == QMediaPlayer.PlayingState:
            self.playerStartButton.setIcon(QIcon.fromTheme("media-playback-pause"))
        else:
            self.playerStartButton.setIcon(QIcon.fromTheme("media-playback-start"))

    def updateSliderRange(self, duration):
        self.slider.setRange(0, duration)

    def updateSliderPosition(self, position):
        self.slider.setValue(position)

    def updateDurationViewer(self, duration):
        qDuration = QTime(0, 0, 0)
        qDuration = qDuration.addMSecs(duration)
        self.playerDurationViewer.setText(qDuration.toString("HH:mm:ss.zzz"))

    def updatePositionViewer(self, position):
        qPosition = QTime(0, 0, 0)
        qPosition = qPosition.addMSecs(position)
        self.playerPositionViewer.setText(qPosition.toString("HH:mm:ss.zzz"))

    def showAddBreakpointDialog(self):
        dialog = AddBreakPointDialog(self, self.videoPlayer, self.project)
        dialog.exec()

    def connectBreakpointsUpdate(self):
        self.project.breakpointsChanged.connect(self.updateDockBreakpoints)

    def showDockContextMenu(self, contextMenuRelativePosition):
        contextMenuGlobalPosition = self.breakpointListView \
                                        .mapToGlobal(contextMenuRelativePosition)
        contextMenu = QMenu()
        contextMenu.addAction(self.addBreakpointAction)
        contextMenu.addAction(self.addBreakpointHereAction)
        contextMenu.exec(contextMenuGlobalPosition)

    def updateDockBreakpoints(self):
        breakpointList = []
        for breakpoint in sorted(self.project.getBreakpoints()):
            breakpointList.append(QTime(0, 0, 0).addMSecs(breakpoint).toString("HH:mm:ss.zzz"))
        self.breakpointListModel.setStringList(breakpointList)

    def updateProjectBreakpoint(self, index):
        oldPosition = self.project.getSortedBreakpoints()[index.row()]
        newPosition = parseTime(self.breakpointListModel.data(index, Qt.DisplayRole))
        self.project.replaceBreakpoint(oldPosition, newPosition)
