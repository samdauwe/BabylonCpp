#include <babylon/samples/textures/photo_dome_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/helpers/photo_dome.h>
#include <babylon/helpers/photo_dome_options.h>

namespace BABYLON {
namespace Samples {

PhotoDomeScene::PhotoDomeScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
{
}

PhotoDomeScene::~PhotoDomeScene()
{
}

const char* PhotoDomeScene::getName()
{
  return "Photo Dome Scene";
}

void PhotoDomeScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  auto camera = ArcRotateCamera::New("Camera", -Math::PI_2, Math::PI_2, 5,
                                     Vector3::Zero(), scene);
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

} // end of namespace Samples
} // end of namespace BABYLON
