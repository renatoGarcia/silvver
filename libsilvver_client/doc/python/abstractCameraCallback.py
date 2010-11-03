#!/usr/bin/python2

from silvver import *

def print_pose(reading):
    print('Timestamp: %d' % reading.timestamp)
    for pose in reading.localizations:
        print(pose)
    print

cb = CallbackWrapper(AbstractCamera_Pose((1,1)), print_pose)

raw_input("Press [enter] to quit.")
