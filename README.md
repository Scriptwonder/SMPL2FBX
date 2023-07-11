# SMPL2FBX
A C++ Solution that converts an SMPL data to FBX animation file  
Inspired by [SMPL-to-FBX](https://github.com/softcat477/SMPL-to-FBX) but by using C++ library it solved gimbal lock problem in long complex animations.

# Prerequisite
Download FBX SDK from [Autodesk](https://www.autodesk.com/developer-network/platform-technologies/fbx-sdk-2020-3-4), then link the include and lib in your VS2022 Project

# Instruction
This C++ Solution takes a .fbx model and a .csv file as inputs and output a new .fbx model. Remember to change the csvFilePath, fbxFilePath, and fbxFilePath1(outputPath) before using.  

Also for other .fbx models that does not have the same joint names as this project, remember to edit the joints name as well.
(PS: For the special needs of my project, the rotation changes in the Pelvis's X and Z coordinates are locked. You can unlock it anytime in AddAnimation())

## From .Npy file to .CSV file
The .NPY file you get from SMPL model normally contains the root translation and motion data, and using the test.py we could extract these two data and formed them into a .csv file that the C++ solution reads and analyzes.  
  
The only changes that need to be done on the test.py will be the file names of .Npy and .CSV.


