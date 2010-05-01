#!/usr/bin/python

from silvver import *

target = TargetPose((1,1))
target.connect()

for i in range(10):
    pose = target.getNew()
    print(pose)
    print

target.disconnect()
