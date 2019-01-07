# -*-coding: utf-8 -*-

import numpy as np
import cv2

data=np.zeros((480,640,3),dtype=np.uint8)

data[:,:,2] = np.loadtxt("r_56.csv", delimiter=",") 
data[:,:,1] = np.loadtxt("g_56.csv", delimiter=",") 
data[:,:,0] = np.loadtxt("b_56.csv", delimiter=",") 

#test=np.ones_like(r)

cv2.imshow("test",data)
while cv2.waitKey(33) != 27:
  pass
