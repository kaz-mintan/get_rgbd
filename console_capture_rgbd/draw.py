# -*-coding: utf-8 -*-
import os
import numpy as np
import cv2

path = "/home/kazumi/prog/get_rgbd/console_capture_rgbd/rgbd/"
directory = os.listdir(path)

for i in directory:
  print(i[2:26])
  rgb_array= np.zeros((480,640,3),dtype=np.uint8)
  data = np.zeros((480,640*3),dtype=np.uint8)
  depth_data = np.zeros((480,640))
  print(i)
  if(i[0]=='0'):
    data = np.loadtxt(path+i, delimiter=",") 
    rgb_array[:,:,2] = data[:,::3]
    rgb_array[:,:,1] = data[:,1::3]
    rgb_array[:,:,0] = data[:,2::3]
    cv2.imshow("color_test",rgb_array)
  if(i[0]=='1'):
    depth_data = np.loadtxt(path+i, delimiter=",") 
    cv2.imshow("test",depth_data)

  while cv2.waitKey(33) != 27:
    pass

