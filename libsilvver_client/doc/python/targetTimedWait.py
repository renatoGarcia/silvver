#!/usr/bin/python

from silvver import *
from datetime import *

target = Target_Pose((1,1))

try:
    pose = target.getNext(timedelta(seconds=5))
    print(pose)
except TimeExpired:
    print 'Wait time Expired'
