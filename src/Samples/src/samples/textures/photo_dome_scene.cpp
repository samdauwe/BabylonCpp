#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/helpers/photo_dome.h>
#include <babylon/helpers/photo_dome_options.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/samples/samples_index.h>

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

    PhotoDome::New("testdome", "./textures/360photo.jpg",
                   PhotoDomeOptions{
                     32u,         // resolution
                     1000u,       // size
                     std::nullopt // useDirectMapping
                   },
                   scene);
  }

}; // end of class PhotoDomeScene

BABYLON_REGISTER_SAMPLE("Textures", PhotoDomeScene)

} // end of namespace Samples
} // end of namespace BABYLON
