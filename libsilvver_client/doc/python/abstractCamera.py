#!/usr/bin/python

from silvver import *

def printCameraReading(reading):
    print('Timestamp: %d' % reading.timestamp)
    for l in reading.localizations:
        print l

c = AbstractCameraPose(printCameraReading, "target1cam1")
c.connect()

raw_input('Press enter to quit \n')
