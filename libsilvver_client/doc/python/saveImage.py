#!/usr/bin/python

from silvver import *
import cv

camera = AbstractCamera_Pose((1,1))
reading = camera.getUnseen()
image = reading.image.toIplImage()
cv.SaveImage('image.jpg', image)

raw_input('Press enter to quit \n')
