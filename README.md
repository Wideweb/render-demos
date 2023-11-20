# Demos

Demos can be run on Windows with DirectX 12.

* [Building](#building)
* [Cascade Shadows](#cascade-shadows)
* [Frustum Culling with binary space partitioning](#frustum-culling-with-binary-space-partitioning)
* [Frustum Culling on GPU](#frustum-culling-on-gpu)
* [Geometry Particles](#geometry-particles)
* [Generate Skybox and Fog with Noise](#generate-skybox-and-fog-with-noise)

## Building

The following set of tools is required:
- gcc
- vcpkg (replace *D:/vcpkg/scripts/buildsystems/vcpkg.cmake* with your path in *render-demos/engine/CMakeLists.txt*)
- cmake

build steps:
- cd render-demos
- mkdir build
- cd build
- cmake ..
- cmake --build .

run: GameApp.exe

<br>

## Cascade Shadows
![cascade-demo-1](https://github.com/Wideweb/render-demos/assets/3997537/a95becdf-d8a2-461f-a0ec-2727f464c8c6)

Blending between cascade layers

![cascade-blending](https://github.com/Wideweb/render-demos/assets/3997537/93ca9536-2f95-42f4-b3f1-74a86e3302b9)

<br>

## Frustum Culling with binary space partitioning
There are 5000 objects on the game stage.

https://github.com/Wideweb/render-demos/assets/3997537/f764f849-2a01-4b76-9695-27ad86d61898

<br>

## Frustum Culling on GPU

There are 6400 objects on the game stage.

https://github.com/Wideweb/render-demos/assets/3997537/a1b119b9-1407-4ca8-82f3-a70b5517dc88

<br>

## Geometry Particles

https://github.com/Wideweb/render-demos/assets/3997537/46abb899-8418-4267-9f25-d28001d650e9

Each particle moves along its own plane, which cuts the model.

![geometry-particle-plane](https://github.com/Wideweb/render-demos/assets/3997537/86d27f52-702d-467e-b62d-be66e6fc68db)

<br>

## Generate Skybox and Fog with Noise

### FBM
![sky-fbm](https://github.com/Wideweb/render-demos/assets/3997537/eda231d8-0cf2-4735-847a-06b6f08e3ea8)

### Turbulence
![turbulance](https://github.com/Wideweb/render-demos/assets/3997537/60b1de52-fa2a-435b-983c-3e23a8052ed1)


### Ridget
![ridget-0](https://github.com/Wideweb/render-demos/assets/3997537/da9da515-a3f6-42af-a79f-48ccba3d7f9d)

