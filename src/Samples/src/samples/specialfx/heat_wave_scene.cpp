#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/point_light.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/mesh.h>
#include <babylon/postprocesses/post_process.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Heatwave Scene. Example demonstrating how to use custom post process effect to simulate
 * heat wave effect.
 * @see https://www.babylonjs-playground.com/#TZJ0HQ#11
 */
class HeatWaveScene : public IRenderableScene {

public:
  static constexpr const char* thanosFX
    = "#ifdef GL_ES\n"
      "precision highp float;\n"
      "#endif\n"
      "\n"
      "varying vec2 vUV;\n"
      "uniform sampler2D textureSampler;\n"
      "uniform vec2 screenSize;\n"
      "\n"
      "uniform sampler2D noiseRef0;\n"
      "uniform sampler2D noiseRef1;\n"
      "\n"
      "uniform float time;\n"
      "\n"
      "void main(void){\n"
      "    vec2 unit = vUV/screenSize;\n"
      "    unit*=16.0+(sin(time*0.5)*50.0);\n"
      "    vec2 pos = vUV;\n"
      "    pos.x += sin(time*0.35);\n"
      "    pos.y -= time*0.2;\n"
      "    vec2 r = ((texture2D(noiseRef0, pos).rb)*2.0-1.0)*unit;\n"
      "    vec3 c = texture2D(textureSampler, vUV+r).rgb;\n"
      "   gl_FragColor = vec4(c, 1.0);\n"
      "}\n";

public:
  HeatWaveScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas)
      , _light{nullptr}
      , _camera{nullptr}
      , _postEffect{nullptr}
      , _noiseTexture0{nullptr}
      , _noiseTexture1{nullptr}
      , _time{0.f}
      , _rate{0.01f}
  {
    Effect::ShadersStore()["thanosEffectFragmentShader"] = thanosFX;
  }

  ~HeatWaveScene() override = default;

  const char* getName() override
  {
    return "Heat Wave Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Adding a light
    _light = PointLight::New("Omni", Vector3(20.f, 20.f, 100.f), scene);

    // Adding an Arc Rotate Camera
    _camera = ArcRotateCamera::New("Camera", 0.f, 0.8f, 100.f, Vector3::Zero(), scene);
    _camera->attachControl(canvas, true);

    // The first parameter can be used to specify which mesh to import. Here we
    // import all meshes
    SceneLoader::ImportMesh({}, "scenes/", "skull.babylon", scene,
                            [this](const std::vector<AbstractMeshPtr>& newMeshes,
                                   const std::vector<IParticleSystemPtr>& /*newParticleSystems*/,
                                   const std::vector<SkeletonPtr>& /*newSkeletons*/,
                                   const std::vector<AnimationGroupPtr>& /*newAnimationGroups*/,
                                   const std::vector<TransformNodePtr>& /*transformNodes*/,
                                   const std::vector<GeometryPtr>& /*geometries*/,
                                   const std::vector<LightPtr>& /*lights*/) {
                              // Set the target of the camera to the first imported mesh
                              if (!newMeshes.empty()) {
                                _camera->setTarget(newMeshes[0]);
                              }
                            });

    // Move the light with the camera
    scene->registerBeforeRender([this](Scene* scene, EventState& /*es*/) {
      _light->position = _camera->position;
      _time += scene->getAnimationRatio() * _rate;
    });

    // Create Dawn Bringer postprocess
    _postEffect = PostProcess::New("thanosEffect", "thanosEffect", {"time", "screenSize"},
                                   {"noiseRef0", "noiseRef1"}, 1.f, _camera);

    _noiseTexture0 = Texture::New("./textures/grass.jpg", scene);
    _noiseTexture1 = Texture::New("./textures/ground.jpg", scene);

    _postEffect->onApply = [this](Effect* effect, EventState& /*es*/) {
      effect->setVector2("screenSize", Vector2(static_cast<float>(_postEffect->width),
                                               static_cast<float>(_postEffect->height)));
      effect->setFloat("time", _time);
      effect->setTexture("noiseRef0", _noiseTexture0);
      effect->setTexture("noiseRef1", _noiseTexture1);
    };
  }

private:
  PointLightPtr _light;
  ArcRotateCameraPtr _camera;

  PostProcessPtr _postEffect;
  TexturePtr _noiseTexture0;
  TexturePtr _noiseTexture1;

  float _time;
  float _rate;

}; // end of class HeatWaveScene

BABYLON_REGISTER_SAMPLE("Special FX", HeatWaveScene)

} // end of namespace Samples
} // end of namespace BABYLON
