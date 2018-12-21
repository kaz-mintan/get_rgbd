- 2018/12/21, kazumi
-- to design a table of a mole-game experiment.
-- as for the design prj, check the document: https://docs.google.com/document/d/1j8t6NA-VuCN-Exy-PXwb4sIpkI5HYNOl6rnEO9dngIc/edit

#get_rgbd
- includes files to get rgbd-data.

#before usage
- setup PC so that RealSense, Nuitrack and OpenGL could be used.

  - check the wiki page to use realsense: 
  - http://mizuuchi.lab.tuat.ac.jp/pukiwiki/index.php?%E3%83%97%E3%83%AD%E3%82%B0%E3%83%A9%E3%83%9F%E3%83%B3%E3%82%B0%28programming%29%2FRealSense

- as for library path

  - make sure that you need update ld.so.conf

  - add `${LD_LIBRARY_PATH}` in the `/etc/ld.so.conf`. then execute ldconfig

  ` sudo ldconfig -v`


- memo

  - execute the command as shown below;

  `. /etc/profile.d/nuitrack_env.sh`


