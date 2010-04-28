#!/usr/bin/python

from silvver import *
import sys

target = TargetPose(1)
target.connect()

for i in range(10):
    pose = target.getNew()
    print pose
