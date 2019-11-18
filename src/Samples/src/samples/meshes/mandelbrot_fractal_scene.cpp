#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

class MandelbrotFractalScene : public IRenderableScene {

public:
  MandelbrotFractalScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~MandelbrotFractalScene() override = default;

  const char* getName() override
  {
    return "Mandelbrot Fractal Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Create a camera
    _camera = ArcRotateCamera::New("camera1", Math::PI2, 0.268f, 400.f, Vector3::Zero(), scene);

    // Set camera position
    _camera->setPosition(Vector3(106.f, 386.f, -0.415f));

    // Attach the camera to the canvas
    _camera->attachControl(canvas, true);

    // Create a basic light, aiming 0,1,0 - meaning, to the sky
    auto light = HemisphericLight::New("light1", Vector3(0.f, 1.f, 0.f), scene);

    // Default intensity is 1. Let's dim the light
    light->intensity = 0.3f;

    // Create a point light
    _pl            = PointLight::New("pl", Vector3::Zero(), scene);
    _pl->diffuse   = Color3(1.f, 1.f, 0.f);
    _pl->intensity = 0.5f;

    auto mat          = StandardMaterial::New("mat1", scene);
    mat->diffuseColor = Color3(0.8f, 0.8f, 0.5f);

    // inspired from
    // http://code.activestate.com/recipes/577277-mandelbrot-fractal-using-html5-canvas/
    float xmin = -2.f, xmax = 1.f;
    float ymin = -1.5f, ymax = 1.5f;
    unsigned int xr = 200, yr = 200, maxIt = 256;
    auto xrf     = static_cast<float>(xr);
    auto yrf     = static_cast<float>(yr);
    auto maxItf  = static_cast<float>(maxIt);
    float i      = 0.f;
    float x = 0.f, y = 0.f;
    float zx = 0.f, zx0 = 0.f, zy = 0.f, zx2 = 0.f, zy2 = 0.f;
    std::vector<std::vector<Vector3>> paths;
    paths.reserve(yr);

    for (float ky = 0.f; ky < yrf; ++ky) {
      std::vector<Vector3> path;
      path.reserve(xr);
      y = ymin + (ymax - ymin) * ky / yrf;

      for (float kx = 0.f; kx < xrf; ++kx) {
        x  = xmin + (xmax - xmin) * kx / xrf;
        zx = x;
        zy = y;

        for (i = 0.f; i < maxItf; ++i) {
          zx2 = zx * zx;
          zy2 = zy * zy;
          if (zx2 + zy2 > 4.f) {
            break;
          }
          zx0 = zx2 - zy2 + x;
          zy  = 2.f * zx * zy + y;
          zx  = zx0;
        }

        path.emplace_back(Vector3(ky, -i, kx));
      }

      paths.emplace_back(path);
    }

    auto ribbon      = Mesh::CreateRibbon("mandel", paths, false, false, -1, scene);
    ribbon->material = mat;
    ribbon->position().x -= xrf / 2.f;
    ribbon->position().z -= yrf / 2.f;

    _scene->registerBeforeRender(
      [this](Scene*, EventState&) { _pl->position = _camera->position; });
  }

private:
  ArcRotateCameraPtr _camera;
  PointLightPtr _pl;

}; // end of class MandelbrotFractalScene

BABYLON_REGISTER_SAMPLE("Meshes", MandelbrotFractalScene)

} // end of namespace Samples
} // end of namespace BABYLON
