#include <babylon/cameras/free_camera.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/directional_light.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_shaders_store.h>
#include <babylon/materials/shader_material.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Fireworks with Shader Code scene.
 * @see https://doc.babylonjs.com/samples/writing2
 */
class FireworksWithShaderCodeScene : public IRenderableScene {

public:
  /** Vertex Shader **/
  static constexpr const char* customVertexShader
    = "#ifdef GL_ES\n"
      "precision highp float;\n"
      "#endif\n"
      "\n"
      "// Attributes\n"
      "attribute vec3 position;\n"
      "attribute vec3 normal;\n"
      "\n"
      "// Uniforms\n"
      "uniform mat4 worldViewProjection;\n"
      "uniform float time;\n"
      "\n"
      "void main(void) {\n"
      "  vec3 p = position;\n"
      "  vec3 j = vec3(0., -1.0, 0.);\n"
      "  p = p + normal * log2(1. + time) * 25.0;\n"
      "  gl_Position = worldViewProjection * vec4(p, 1.0);\n"
      "}\n";

  /** Pixel (Fragment) Shader **/
  static constexpr const char* customFragmentShader
    = "#ifdef GL_ES\n"
      "precision highp float;\n"
      "#endif\n"
      "\n"
      "uniform float time;\n"
      "\n"
      "void main(void) {\n"
      "  gl_FragColor = vec4(1. -log2(1. + time) / 100.,\n"
      "                      1. * log2(1. + time), 0.,\n"
      "                      1. - log2(1. + time / 2.) / log2(1. + 3.95));\n"
      "}\n";

public:
  FireworksWithShaderCodeScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas), _time{0.f}, _shaderMaterial{nullptr}
  {
    // Vertex shader
    Effect::ShadersStore()["customVertexShader"] = customVertexShader;

    // Fragment shader
    Effect::ShadersStore()["customFragmentShader"] = customFragmentShader;
  }

  ~FireworksWithShaderCodeScene() override = default;

  const char* getName() override
  {
    return "Fireworks with Shader Code Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Create a FreeCamera
    auto camera = FreeCamera::New("camera1", Vector3(0, 100, -200), scene);

    // Target the camera to scene origin
    camera->setTarget(Vector3::Zero());

    // Attach the camera to the canvas
    camera->attachControl(canvas, true);

    // Create 2 lights
    DirectionalLight::New("DirectionalLight", Vector3(0, -1, 1), scene);
    auto light2       = HemisphericLight::New("HemiLight", Vector3(0, 1, 0), scene);
    light2->intensity = 0.5f;

    // Create shader material
    IShaderMaterialOptions shaderMaterialOptions;
    shaderMaterialOptions.attributes = {"position", "normal", "uv"};
    shaderMaterialOptions.uniforms
      = {"world", "worldView", "worldViewProjection", "view", "projection"};
    shaderMaterialOptions.needAlphaBlending = true;
    _shaderMaterial = ShaderMaterial::New("shader", scene, "custom", shaderMaterialOptions);
    _shaderMaterial->backFaceCulling = false;

    // Create a built-in "sphere" shape
    SphereOptions sphereOptions;
    sphereOptions.diameter = 10.f;
    auto sphere            = MeshBuilder::CreateSphere("sphere", sphereOptions, scene);
    sphere->convertToFlatShadedMesh();
    sphere->material = _shaderMaterial;

    // Animation
    scene->registerBeforeRender([this](Scene*, EventState&) {
      if (_time < 8.f) {
        _shaderMaterial->setFloat("time", _time);
        _time += 0.05f * getScene()->getAnimationRatio();
      }
      else {
        _time = 0.f;
      }
    });
  }

private:
  float _time;
  ShaderMaterialPtr _shaderMaterial;

}; // end of class FireworksWithShaderCodeScene

BABYLON_REGISTER_SAMPLE("Special FX", FireworksWithShaderCodeScene)

} // end of namespace Samples
} // end of namespace BABYLON
