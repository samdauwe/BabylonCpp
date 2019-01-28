#include <babylon/samples/specialfx/rgb_shift_glitch_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engine/scene.h>
#include <babylon/lights/point_light.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/mesh/mesh.h>
#include <babylon/postprocess/post_process.h>

namespace BABYLON {
namespace Samples {

RGBShiftGlitchScene::RGBShiftGlitchScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
    , _light{nullptr}
    , _camera{nullptr}
    , _postEffect{nullptr}
    , _noiseTexture0{nullptr}
    , _noiseTexture1{nullptr}
    , _time{0.f}
    , _rate{0.01f}
{
  Effect::ShadersStore()["rgbGlitchEffectFragmentShader"] = rgbGlitchFX;
}

RGBShiftGlitchScene::~RGBShiftGlitchScene()
{
}

const char* RGBShiftGlitchScene::getName()
{
  return "RGB Shift Glitch Scene";
}

void RGBShiftGlitchScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Adding a light
  _light = PointLight::New("Omni", Vector3(20.f, 20.f, 100.f), scene);

  // Adding an Arc Rotate Camera
  _camera
    = ArcRotateCamera::New("Camera", 0.f, 0.8f, 100.f, Vector3::Zero(), scene);
  _camera->attachControl(canvas, true);

  // The first parameter can be used to specify which mesh to import. Here we
  // import all meshes
  SceneLoader::ImportMesh(
    {}, "scenes/", "skull.babylon", scene,
    [this](const std::vector<AbstractMeshPtr>& newMeshes,
           const std::vector<IParticleSystemPtr>& /*newParticleSystems*/,
           const std::vector<SkeletonPtr>& /*newSkeletons*/,
           const std::vector<AnimationGroupPtr>& /*newAnimationGroups*/) {
      // Set the target of the camera to the first imported mesh
      if (!newMeshes.empty()) {
        _camera->setTarget(newMeshes[0].get());
      }
    });

  // Move the light with the camera
  scene->registerBeforeRender([this](Scene* scene, EventState& /*es*/) {
    _light->position = _camera->position;
    _time += scene->getAnimationRatio() * _rate;
  });

  // Create Dawn Bringer postprocess
  _postEffect = PostProcess::New("rgbGlitchEffect", "rgbGlitchEffect",
                                 {"time", "screenSize"},
                                 {"noiseRef0", "noiseRef1"}, 1.f, _camera);

  _noiseTexture0 = Texture::New("./textures/grass.jpg", scene);
  _noiseTexture1 = Texture::New("./textures/ground.jpg", scene);

  _postEffect->onApply = [this](Effect* effect, EventState& /*es*/) {
    effect->setVector2("screenSize",
                       Vector2(static_cast<float>(_postEffect->width),
                               static_cast<float>(_postEffect->height)));
    effect->setFloat("time", _time);
    effect->setTexture("noiseRef0", _noiseTexture0);
    effect->setTexture("noiseRef1", _noiseTexture1);
  };
}

} // end of namespace Samples
} // end of namespace BABYLON
