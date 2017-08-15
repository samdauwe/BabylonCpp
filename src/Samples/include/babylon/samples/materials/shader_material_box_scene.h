#ifndef BABYLON_SAMPLES_MATERIALS_SHADER_MATERIAL_BOX_SCENE_H
#define BABYLON_SAMPLES_MATERIALS_SHADER_MATERIAL_BOX_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

class ShaderMaterialBoxScene : public IRenderableScene {

public:
  /** Vertex Shader **/
  // source: https://www.shadertoy.com/new
  static constexpr const char* customVertexShader
    = "#ifdef GL_ES\n"
      "precision highp float;\n"
      "#endif\n"
      "\n"
      "// Attributes\n"
      "attribute vec3 position;\n"
      "attribute vec2 uv;\n"
      "\n"
      "// Uniforms\n"
      "uniform mat4 worldViewProjection;\n"
      "\n"
      "// Varying\n"
      "varying vec2 vUV;\n"
      "\n"
      "void main(void) {\n"
      "    gl_Position = worldViewProjection * vec4(position, 1.0);\n"
      "    vUV = uv;\n"
      "}\n";

  /** Pixel (Fragment) Shader **/
  static constexpr const char* customFragmentShader
    = "#ifdef GL_ES\n"
      "precision highp float;\n"
      "#endif\n"
      "\n"
      "varying vec2 vUV;\n"
      "\n"
      "uniform float time;\n"
      "\n"
      "void main(void)\n"
      "{\n"
      "  vec2 uv = vUV.xy;\n"
      "  gl_FragColor  = vec4(uv,0.5+0.5*sin(time),1.0);\n"
      "}\n";

public:
  ShaderMaterialBoxScene(ICanvas* iCanvas);
  ~ShaderMaterialBoxScene();

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  float _time;
  ShaderMaterial* _shaderMaterial;

}; // end of class ShaderMaterialBoxScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MATERIALS_SHADER_MATERIAL_BOX_SCENE_H
