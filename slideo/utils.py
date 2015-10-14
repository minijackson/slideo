#!/usr/bin/python3
# -*- coding: utf-8 -*-

def parseTime(time):
    msecs = 0
    splittedTime = time.split(":")
    secAndMsecs = splittedTime[2].split(".")
    msecs += int(splittedTime[0]) * 3600000
    msecs += int(splittedTime[1]) *   60000
    msecs += int(secAndMsecs[0])  *    1000
    msecs += int(secAndMsecs[1])
    return msecs
