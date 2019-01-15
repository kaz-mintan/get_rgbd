# -*-coding: utf-8 -*-
import os
import numpy as np
import cv2

path = "/home/kazumi/prog/get_rgbd/console_capture_rgbd/build/rgbd/"
directory = os.listdir(path)

for i in directory:
  print(i[2:26])
  rgb_data = np.zeros((480,640,3),dtype=np.uint8)
  depth_data = np.zeros((480,640))
  print(i)
  if(i[0]=='r'):
    rgb_data[:,:,2] = np.loadtxt(path+i, delimiter=",") 
  if(i[0]=='g'):
    rgb_data[:,:,1] = np.loadtxt(path+i, delimiter=",") 
  if(i[0]=='b'):
    rgb_data[:,:,2] = np.loadtxt(path+i, delimiter=",") 
  if(i[0]=='d'):
    depth_data = np.loadtxt(path+i, delimiter=",") 
    cv2.imshow("test",depth_data)
  while cv2.waitKey(33) != 27:
    pass

