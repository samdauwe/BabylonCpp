#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/helpers/photo_dome.h>
#include <babylon/helpers/texture_dome_options.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Photo Dome Scene. Example demonstrating how to use easily display and control 360 photos.
 * @see https://www.babylonjs-playground.com/#14KRGG#3
 */
class PhotoDomeScene : public IRenderableScene {

public:
  PhotoDomeScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~PhotoDomeScene() override = default;

  const char* getName() override
  {
    return "Photo Dome Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera
      = ArcRotateCamera::New("Camera", -Math::PI_2, Math::PI_2, 5.f, Vector3::Zero(), scene);
    camera->attachControl(canvas, true);
    camera->inputs->attached["mousewheel"]->detachControl(canvas);

    TextureDomeOptions options;
    options.resolution       = 32u;
    options.size             = 1000.f;
    options.useDirectMapping = std::nullopt;

    PhotoDome::New("testdome", "./textures/360photo.jpg", options, scene);
  }

}; // end of class PhotoDomeScene

BABYLON_REGISTER_SAMPLE("Textures", PhotoDomeScene)

} // end of namespace Samples
} // end of namespace BABYLON
