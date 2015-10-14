#!/usr/bin/python3
# -*- coding: utf-8 -*-

import yaml
from os.path import dirname, basename
from PyQt5.QtCore import pyqtSignal, QObject

class ProjectManager(QObject):
    breakpointsChanged = pyqtSignal()

    def __init__(self, projectFile):
        super().__init__()

        self.projectFile = projectFile
        self.isSaved = True

        f = open(projectFile, "r")
        self.project = yaml.load(f.read())
        f.close()

        self.project["breakpoints"] = set(self.project["breakpoints"])
        self.sortedBreakpoints = sorted(self.project["breakpoints"])

        self.breakpointsChanged.connect(self.updateSortedBreakpoints)
        self.breakpointsChanged.connect(self.changeProjectSavedStatus)

    def saveProject(self):
        tempProject = self.project.copy()
        tempProject["breakpoints"] = list(tempProject["breakpoints"])

        f = open(self.projectFile, "w")
        yaml.dump(tempProject, f)
        f.close()
        self.isSaved = True

    def changeProjectSavedStatus(self):
        self.isSaved = False

    def getVideoFiles(self):
        return self.project["video-files"]

    def getProjectFileLocation(self):
        return dirname(self.projectFile)

    def getProjectFileBaseName(self):
        return basename(self.projectFile)

    def getBreakpoints(self):
        return self.project["breakpoints"]

    def getSortedBreakpoints(self):
        return self.sortedBreakpoints

    def setBreakpoints(self, breakpoints):
        self.project["breakpoints"] = breakpoints

    def addBreakpoint(self, position):
        self.project["breakpoints"].add(position)
        self.breakpointsChanged.emit()

    def removeBreakpoint(self, position):
        self.project["breakpoints"].remove(position)
        self.breakpointsChanged.emit()

    def removeBreakpoints(self, positions):
        for position in positions:
            self.project["breakpoints"].remove(position)
        self.breakpointsChanged.emit()

    def replaceBreakpoint(self, oldPosition, newPosition):
        self.project["breakpoints"].remove(oldPosition)
        self.project["breakpoints"].add(newPosition)
        self.breakpointsChanged.emit()

    def updateSortedBreakpoints(self):
        self.sortedBreakpoints = sorted(self.project["breakpoints"])
