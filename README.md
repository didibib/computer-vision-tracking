# Voxel Reconstruction
Voxel Reconstruction based on multiple camera angles. Developed for the course 'Computer Vision (INFOMCV)'. Written in C++.

Implements a simple voxel reconstruction algorithm based on multiple camera angles as described in the course. Uses simple image pre-processing to acquire workable input images. 

Extended to include subject tracking using color models. Simple color models are created using histogram color models and k-means to find dominant colors. Voxels are then clustered (also using k-means) and tracked over multiple frames. 

## Videos
- [Voxel reconstruction demo](https://youtu.be/9j9XlNlU7Zw)
- [Subject tracking demo](https://youtu.be/Ep7bMrkyu48)


# Template info
## Prerequisites
OpenCV needs to be installed on your system and have the build and bin folder added to your system path: <br>
`[YOUR LOCATION]\opencv\build` - Needed for the cmake command `find_package()` to work. <br>
`[YOUR LOCATION]\opencv\build\x64\vc15\bin` - So that there is access to OpenCV executables. <br>

## Build
You can click `build.bat` to build the project for VS17 2022 or edit the bat file to build for other IDE.
