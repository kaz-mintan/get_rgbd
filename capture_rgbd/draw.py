# -*-coding: utf-8 -*-
import os
import numpy as np

path = "/home/kazumi/prog/get_rgbd/capture_rgbd/test_2/rgbd/"
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

