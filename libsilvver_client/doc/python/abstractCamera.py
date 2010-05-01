#!/usr/bin/python

from silvver import *

def printCameraReading(reading):
    print('Timestamp: %d' % reading.timestamp)
    for l in reading.localizations:
        print(l)
    print

c = AbstractCameraPose(printCameraReading, (1,1))
c.connect()

raw_input('Press enter to quit \n')

c.disconnect()
