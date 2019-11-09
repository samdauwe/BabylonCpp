#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_shaders_store.h>
#include <babylon/materials/shader_material.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

class ShaderMaterialWithFogScene : public IRenderableScene {

public:
  /** Vertex Shader **/
  static constexpr const char* customVertexShader
    = "#ifdef GL_ES\n"
      "precision highp float;\n"
      "#endif\n"
      "\n"
      "// Attributes\n"
      "attribute vec3 position;\n"
      "\n"
      "// Uniforms\n"
      "uniform mat4 world;\n"
      "uniform mat4 view;\n"
      "uniform mat4 viewProjection;\n"
      "\n"
      "// Varying\n"
      " varying float fFogDistance;\n"
      "\n"
      "void main(void) {\n"
      "  vec4 worldPosition = world * vec4(position, 1.0);\n"
      "  fFogDistance = (view * worldPosition).z;\n"
      "  gl_Position =  viewProjection * worldPosition;\n"
      "}\n";

  /** Pixel (Fragment) Shader **/
  static constexpr const char* customFragmentShader
    = "#ifdef GL_ES\n"
      "precision highp float;\n"
      "#endif\n"
      "\n"
      "#define FOGMODE_NONE 0.\n"
      "#define FOGMODE_EXP 1.\n"
      "#define FOGMODE_EXP2 2.\n"
      "#define FOGMODE_LINEAR 3.\n"
      "#define E 2.71828\n"
      "\n"
      "// Uniforms\n"
      "uniform vec4 vFogInfos;\n"
      "uniform vec3 vFogColor;\n"
      "uniform sampler2D textureSampler; \n"
      "\n"
      "// Varying\n"
      "varying float fFogDistance;\n"
      "\n"
      "float CalcFogFactor()\n"
      "{\n"
      "  float fogCoeff = 1.0;\n"
      "  float fogStart = vFogInfos.y;\n"
      "  float fogEnd = vFogInfos.z;\n"
      "  float fogDensity = vFogInfos.w;\n"
      "\n"
      "  if (FOGMODE_LINEAR == vFogInfos.x)\n"
      "  {\n"
      "    fogCoeff = (fogEnd - fFogDistance) / (fogEnd - fogStart);\n"
      "  }\n"
      "  else if (FOGMODE_EXP == vFogInfos.x)\n"
      "  {\n"
      "    fogCoeff = 1.0 / pow(E, fFogDistance * fogDensity);\n"
      "  }\n"
      "  else if (FOGMODE_EXP2 == vFogInfos.x)\n"
      "  {\n"
      "    fogCoeff = 1.0 / pow(E, fFogDistance * fFogDistance * fogDensity * "
      "                            fogDensity);\n"
      "  }\n"
      "  return clamp(fogCoeff, 0.0, 1.0);\n"
      "}\n"
      "\n"
      "void main(void) {\n"
      "  float fog = CalcFogFactor();\n"
      "  vec3 color = vec3(1.0, 0., 1.0);\n"
      "  color = fog * color + (1.0 - fog) * vFogColor;\n"
      "  gl_FragColor = vec4(color, 1.);\n"
      "}\n";

public:
  ShaderMaterialWithFogScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
    // Vertex shader
    Effect::ShadersStore()["customVertexShader"] = customVertexShader;

    // Fragment shader
    Effect::ShadersStore()["customFragmentShader"] = customFragmentShader;
  }

  ~ShaderMaterialWithFogScene() override = default;

  const char* getName() override
  {
    return "Shader Material With Fog Scene";
  }
  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Create a camera
    auto camera
      = ArcRotateCamera::New("camera1", Math::PI, Math::PI_2, 5.f, Vector3(0.f, 0.f, 0.f), scene);

    // Target the camera to scene origin
    camera->setTarget(Vector3::Zero());

    // Attach the camera to the canvas
    camera->attachControl(canvas, true);

    // Create a basic light, aiming 0,1,0 - meaning, to the sky
    HemisphericLight::New("light1", Vector3(0.f, 1.f, 0.f), scene);

    // Create a built-in "box" shape
    auto box = Mesh::CreateBox("box1", 1.f, scene);

    // Create shader material
    IShaderMaterialOptions shaderMaterialOptions;
    shaderMaterialOptions.needAlphaBlending = true;
    shaderMaterialOptions.attributes        = {"position"};
    shaderMaterialOptions.uniforms = {"world", "view", "viewProjection", "vFogInfos", "vFogColor"};
    _boxMaterial  = ShaderMaterial::New("boxShader", scene, "custom", shaderMaterialOptions);
    box->material = _boxMaterial;

    _boxMaterial->onBind = [this](AbstractMesh*, EventState&) {
      auto effect = _boxMaterial->getEffect();
      effect->setFloat4("vFogInfos", static_cast<float>(_scene->fogMode()), _scene->fogStart,
                        _scene->fogEnd, _scene->fogDensity);
      effect->setColor3("vFogColor", _scene->fogColor);
    };

    scene->fogMode  = Scene::FOGMODE_LINEAR;
    scene->fogStart = 0.f;
    scene->fogEnd   = 10.f;
    scene->fogColor = Color3(scene->clearColor.r, scene->clearColor.g, scene->clearColor.b);
  }

private:
  ShaderMaterialPtr _boxMaterial;

}; // end of class ShaderMaterialWithFogScene

BABYLON_REGISTER_SAMPLE("Shader Materials", ShaderMaterialWithFogScene)

} // end of namespace Samples
} // end of namespace BABYLON
