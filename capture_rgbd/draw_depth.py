# -*-coding: utf-8 -*-

import numpy as np
import cv2

array = np.loadtxt("depth_test.csv", delimiter=",") 
cv2.imshow("test",array)
while cv2.waitKey(33) != 27:
  pass
