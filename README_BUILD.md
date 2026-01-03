# How to build with CMake & CodeCreator on Windows
==================================================

## QT
* Install QT (QT 6.5.3 compiles with CodeQuery)

## Vulkan SDK
* Install Vulkan SDK

## Sqlite
=========
* Use Sqlite amalgamation, build and make static library libsqlite3.a
  use sqlite/build.sh, link.sh

## Cmake
========
* Install cmake
* Add cmake/bin to path
* Configure CMakeList.txt Project by open with cmake-gui
* Copy cq Edit doc/generate_build.sh as you like

### Start cmake-gui
=================== 
* Select source and build directories
* Press Configure, solve possibly errors, like missing
* Press Generate
* Fill-in missing paths, etc. (For sqlite library, select libsqlite3.a)
* eg. Vulkan SDK
  VulkanSDK/<version>/Bin/glslangValidator.exe
  VulkanSDK/<version>/Bin/glslc.exe
  VulkanSDK/<version>/include
  VulkanSDK/<version>/Lib/volk.lib

## Start QtCreator
==================
* Check Projects MinGW, etc. Adapt possibly missing/wrong paths/variables
* (Re-)Compile projects 
* Check generated *.exe exist

