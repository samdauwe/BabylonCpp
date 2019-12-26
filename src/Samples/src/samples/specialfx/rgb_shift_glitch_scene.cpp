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
 * @brief RGB Shift Glitch Scene. Example demonstrating how to use a custom post process effect to
 * simulate RGB Shift glitch.
 * @see https://www.babylonjs-playground.com/#TZJ0HQ#10
 */
class RgbShiftGlitchScene : public IRenderableScene {

public:
  /** Pixel (Fragment) Shader **/
  // RGB Shift Glitch (https://www.shadertoy.com/view/4t23Rc)
  static constexpr const char* rgbGlitchFX
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
      "#define AMPLITUDE 0.05\n"
      "#define SPEED 10.0\n"
      "\n"
      "vec4 rgbShift(in vec2 p , in vec4 shift) {\n"
      "    shift *= 2.0*shift.w - 1.0;\n"
      "    vec2 rs = vec2(shift.x,-shift.y);\n"
      "    vec2 gs = vec2(shift.y,-shift.z);\n"
      "    vec2 bs = vec2(shift.z,-shift.x);\n"
      "    float r = texture2D(textureSampler, p+rs, 0.0).x;\n"
      "    float g = texture2D(textureSampler, p+gs, 0.0).y;\n"
      "    float b = texture2D(textureSampler, p+bs, 0.0).z;\n"
      "    return vec4(r,g,b,1.0);\n"
      "}\n"
      "vec4 noise( in vec2 p ) {\n"
      "    return texture2D(noiseRef0, p, 0.0);\n"
      "}\n"
      "\n"
      "vec4 vec4pow( in vec4 v, in float p ) {\n"
      "    return vec4(pow(v.x,p),pow(v.y,p),pow(v.z,p),v.w);\n"
      "}\n"
      "\n"
      "void main(void){\n"
      "    vec2 p = vUV;\n"
      "    vec4 c = vec4(0.0,0.0,0.0,1.0);\n"
      "    vec4 shift = vec4pow(noise(vec2(SPEED*time,\n"
      "                                    2.0*SPEED*time/25.0 )),8.0)\n"
      "                * vec4(AMPLITUDE,AMPLITUDE,AMPLITUDE,1.0);\n"
      "    c += rgbShift(p, shift);\n"
      "   gl_FragColor = c;\n"
      "}\n";

public:
  RgbShiftGlitchScene(ICanvas* iCanvas)
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

  ~RgbShiftGlitchScene() override = default;

  const char* getName() override
  {
    return "RGB Shift Glitch Scene";
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
                                   const std::vector<AnimationGroupPtr>& /*newAnimationGroups*/) {
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
    _postEffect = PostProcess::New("rgbGlitchEffect", "rgbGlitchEffect", {"time", "screenSize"},
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

}; // end of class RGBShiftGlitchScene

BABYLON_REGISTER_SAMPLE("Special FX", RgbShiftGlitchScene)

} // end of namespace Samples
} // end of namespace BABYLON
