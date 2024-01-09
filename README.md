# SimpleLandmassGeneration

A very simple and naive implementation of procedural terrain generation, heavily based on Sebastian League's tutorial.

## Building
First install all dependencies needed to build raylib from source, then fetch code with submodules:
```bash
git clone --recurse-submodules https://github.com/cedmundo/SimpleLandmassGeneration.git
```
Build with CMake:
```bash
mkdir -p cmake-build-debug
cmake -DCMAKE_BUILD_TYPE=debug -B cmake-build-debug
cmake --build cmake-build-debug 
```
Done.

## TODOs

* Level of Detail
* Endless terrain
* Threading?
* LOD Switching
* Seams/Falloff
* Normals
* Collisions
* Data Storage
* Colour/Texture Shader
* Optimization