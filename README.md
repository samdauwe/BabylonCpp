BabylonCpp
===========

#### A port of Babylon.js to C++ #####

[Babylon.js](http://www.babylonjs.com) is a complete JavaScript framework for building 3D games with HTML 5 and WebGL.

The goal with BabylonCpp is to fully implement the relevant portions of the excellent Babylon.js 3D engine in C++11, facilitating the creation of 3D games and applications with native performance.

### Get the Sources ###
* `git clone --recursive https://github.com/samdauwe/BabylonCpp.git`
* `cd BabylonCpp`

### Build BabylonCpp from Source ###
* `mkdir build`
* `cd build`
* `cmake ../`
* `make`

### Sample code ###

The following code initializes a basic scene by creating a camera, a light, and two basic meshes (a sphere and a ground plane).

```c++

void initializeScene(ICanvas* canvas, Scene* scene)
{
  // Create a FreeCamera, and set its position to (x:0, y:5, z:-10)
  auto camera = FreeCamera::New("camera1", Vector3(0, 5, -10), scene);

  // Target the camera to the scene origin
  camera->setTarget(Vector3::Zero());

  // Attach the camera to the canvas
  camera->attachControl(canvas, true);

  // Create a basic light, aiming 0,1,0 - meaning, to the sky
  auto light = HemisphericLight::New("light1", Vector3(0, 1, 0), scene);

  // Default intensity is 1. Let's dim the light a small amount
  light->intensity = 0.7f;

  // Create a built-in "sphere" shape; its constructor takes 5 params: name,
  // subdivs, size, scene
  auto sphere = Mesh::CreateSphere("sphere1", 32, 2.f, scene);

  // Move the sphere upward 1/2 of its height
  sphere->position.y = 1.f;

  // Create a built-in "ground" shape.
  // Params: name, width, depth, subdivs, scene
  Mesh::CreateGround("ground1", 6, 6, 2, scene);
}

```

This code result in the following scene:

![Basic scene](assets/screenshots/basic_scene.png?raw=true "Basic scene")

### Dependencies ###
* `CMake`
* `Earcut` polygon triangulation library
* `Google Test` for unit tests (Optional)
* `GLFW`, `GLXW` and `dear imgui` for the examples

### Supported Compilers ###
* GCC >= 4.8
* Clang >= 3.2
* MSVC >= 2015, update 3 (Not tested yet!)

### Supported Operating Systems ###
* Linux
* Windows >= 7 (Not tested yet!)

## License
Open-source under [Apache 2.0 license](http://www.tldrlegal.com/license/apache-license-2.0-%28apache-2.0%29).
