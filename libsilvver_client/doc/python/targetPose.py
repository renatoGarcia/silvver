#!/usr/bin/python

from silvver import *

target = Target_Pose((1,1))

for i in range(10):
    pose = target.getUnseen()
    print(pose)
    print
