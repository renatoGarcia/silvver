#!/usr/bin/python2

from silvver import *

camera = AbstractCamera_Pose((1,1))

for i in range(10):
    reading = camera.getUnseen();

    print('Timestamp: %d' % reading.timestamp)
    for pose in reading.localizations:
        print(pose)
    print
