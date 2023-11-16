## Building

The following set of tools is required:
- gcc
- vcpkg (replace <i>D:/vcpkg/scripts/buildsystems/vcpkg.cmake</i> with your path in render-demos/engine/CMakeLists.txt)
- cmake

build steps:
- cd render-demos
- mkdir build
- cd build
- cmake ..
- cmake --build .

run: GameApp.exe