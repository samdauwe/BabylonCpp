#ifndef BABYLON_SAMPLES_MATERIALS_SHADER_MATERIAL_SKYBOX_SCENE_H
#define BABYLON_SAMPLES_MATERIALS_SHADER_MATERIAL_SKYBOX_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

class ShaderMaterialSkyboxScene : public IRenderableScene {

public:
  /** Vertex Shader **/
  static constexpr const char* gradientVertexShader
    = "#ifdef GL_ES\n"
      "precision mediump float;\n"
      "#endif\n"
      "\n"
      "// Attributes\n"
      "attribute vec3 position;\n"
      "attribute vec3 normal;\n"
      "attribute vec2 uv;\n"
      "\n"
      "// Uniforms\n"
      "uniform mat4 worldViewProjection;\n"
      "\n"
      "// Varying\n"
      "varying vec4 vPosition;\n"
      "varying vec3 vNormal;\n"
      "\n"
      "void main(void) {\n"
      "    vec4 p = vec4(position,1.);\n"
      "    vPosition = p;\n"
      "    vNormal = normal;\n"
      "    gl_Position = worldViewProjection * p;\n"
      "}\n";

  /** Pixel (Fragment) Shader **/
  static constexpr const char* gradientPixelShader
    = "#ifdef GL_ES\n"
      "precision mediump float;\n"
      "#endif\n"
      "\n"
      "// Uniforms\n"
      "uniform float offset;\n"
      "uniform mat4 worldView;\n"
      "uniform vec3 topColor;\n"
      "uniform vec3 bottomColor;\n"
      "\n"
      "// Varying\n"
      "varying vec4 vPosition;\n"
      "varying vec3 vNormal;\n"
      "\n"
      "void main(void)\n"
      "{\n"
      "  float h = normalize(vPosition+offset).y;\n"
      "  gl_FragColor = vec4(mix(bottomColor,topColor,\n"
      "                          max(pow(max(h,0.0),0.6),0.0)),1.0);\n"
      "}\n";

public:
  ShaderMaterialSkyboxScene(ICanvas* iCanvas);
  ~ShaderMaterialSkyboxScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  ShaderMaterial* _shaderMaterial;

}; // end of class ShaderMaterialSkyboxScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MATERIALS_SHADER_MATERIAL_SKYBOX_SCENE_H
