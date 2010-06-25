#!/usr/bin/python

from silvver import *

def print_pose(reading):
    print('Timestamp: %d' % reading.timestamp)
    for l in reading.localizations:
        print(l)
    print

camera = AbstractCamera_Pose((1,1))
camera.setCallback(print_pose)

raw_input('Press enter to quit \n')
