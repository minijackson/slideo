#!/usr/bin/python3
# -*- coding: utf-8 -*-

import yaml
from os.path import dirname

class ProjectManager():
    def __init__(self, projectFile):

        self.projectFile = projectFile

        f = open(projectFile, "r")
        self.project = yaml.load(f.read())
        f.close()

    def getVideoFiles(self):
        return self.project["video-files"]

    def getProjectFileLocation(self):
        return dirname(self.projectFile)
