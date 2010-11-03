#!/usr/bin/python2

from silvver import *

target = TargetSet_Pose(1)

for i in range(10):
    vec_pose = target.getUnseen()
    for pose in vec_pose:
        print(pose)
    print
