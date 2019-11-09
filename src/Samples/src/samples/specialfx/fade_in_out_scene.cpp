#include <babylon/babylon_common.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/core/time.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/effect.h>
#include <babylon/meshes/mesh.h>
#include <babylon/postprocesses/post_process.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Environment scene. This example demonstrates how to fade in and out a whole scene.
 * @see http://www.babylonjs-playground.com/#2FGYE8#2
 */
class FadeInOutScene : public IRenderableScene {

public:
  static constexpr const char* fadePixelShader
    = "#ifdef GL_ES\n"
      "precision highp float;\n"
      "#endif\n"
      "\n"
      "varying vec2 vUV;\n"
      "\n"
      "uniform sampler2D textureSampler;\n"
      "uniform float fadeLevel;\n"
      "\n"
      "void main(void) {\n"
      "  vec4 baseColor = texture2D(textureSampler, vUV) * fadeLevel;\n"
      "  baseColor.a = 1.0;\n"
      "  gl_FragColor = baseColor;\n"
      "}\n";

public:
  FadeInOutScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas)
      , _startTime{Time::highresTimepointNow()}
      , _msFade{5000.f}
      , _fadeLevel{1.f}
  {
    // Pixel shader
    Effect::ShadersStore()["fadePixelShader"] = fadePixelShader;
  }

  ~FadeInOutScene() override = default;

  const char* getName() override
  {
    return "Fade In / Out Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Create a FreeCamera, and set its position to (x:0, y:5, z:-10)
    auto camera = FreeCamera::New("camera1", Vector3(0, 5, -10), scene);

    // Target the camera to scene origin
    camera->setTarget(Vector3::Zero());

    // Attach the camera to the canvas
    camera->attachControl(canvas, true);

    // Create a basic light, aiming 0,1,0 - meaning, to the sky
    auto light = HemisphericLight::New("light1", Vector3(0, 1, 0), scene);

    // Default intensity is 1. Let's dim the light a small amount
    light->intensity = 0.7f;

    // Create a built-in "sphere" shape; its constructor takes 5 params: name,
    // subdivs, size, scene
    auto sphere = Mesh::CreateSphere("sphere1", 16, 2.f, scene);

    // Move the sphere upward 1/2 of its height
    sphere->position().y = 1.f;

    // Create a built-in "ground" shape.
    // Params: name, width, depth, subdivs, scene
    Mesh::CreateGround("ground1", 6, 6, 2, scene);

    // Create fade in / out postprocess
    auto postProcess = PostProcess::New("Fade", "fade", {"fadeLevel"}, {}, 1.f, camera);
    postProcess->onApply
      = [this](Effect* effect, EventState& /**/) { effect->setFloat("fadeLevel", _fadeLevel); };

    scene->registerBeforeRender([this](Scene* /*scene*/, EventState& /*es*/) {
      auto elapsed = Time::fpTimeSince<float, std::milli>(_startTime);
      _fadeLevel   = std::abs(std::cos(Math::PI * 0.5f * (elapsed / _msFade)));
    });
  }

private:
  high_res_time_point_t _startTime;
  float _msFade;
  float _fadeLevel;

}; // end of class FadeInOutScene

BABYLON_REGISTER_SAMPLE("Special FX", FadeInOutScene)

} // end of namespace Samples
} // end of namespace BABYLON
