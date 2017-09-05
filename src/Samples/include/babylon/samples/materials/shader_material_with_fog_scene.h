#ifndef BABYLON_SAMPLES_MATERIALS_SHADER_MATERIAL_WITH_FOG_SCENE_H
#define BABYLON_SAMPLES_MATERIALS_SHADER_MATERIAL_WITH_FOG_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

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
  ShaderMaterialWithFogScene(ICanvas* iCanvas);
  ~ShaderMaterialWithFogScene();

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  ShaderMaterial* _boxMaterial;

}; // end of class ShaderMaterialWithFogScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MATERIALS_SHADER_MATERIAL_WITH_FOG_SCENE_H
