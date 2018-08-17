#ifndef BABYLON_SAMPLES_MATERIALS_SHADER_MATERIAL_CLOUDS_SCENE_H
#define BABYLON_SAMPLES_MATERIALS_SHADER_MATERIAL_CLOUDS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Shader Material Clouds Scene. Example demonstrating how to use
 * ShaderMaterial to create advanced effects.
 * @see https://www.babylonjs-playground.com/#ATDL99#0
 */
class ShaderMaterialCloudsScene : public IRenderableScene {

public:
  /** Vertex Shader **/
  static constexpr const char* cloudVertexShader
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
  static constexpr const char* cloudFragmentShader
    = "#ifdef GL_ES\n"
      "precision highp float;\n"
      "#endif\n"
      "\n"
      "// Varying\n"
      "varying vec2 vUV;\n"
      "\n"
      "// Uniforms\n"
      "uniform vec3 fogColor;\n"
      "uniform float fogNear;\n"
      "uniform float fogFar;\n"
      "\n"
      "// Samplers\n"
      "uniform sampler2D textureSampler;\n"
      "\n"
      "void main(void) {\n"
      "  float depth = gl_FragCoord.z / gl_FragCoord.w;\n"
      "  float fogFactor = smoothstep(fogNear, fogFar, depth);\n"
      "\n"
      "  gl_FragColor = texture2D(textureSampler, vUV);\n"
      "  gl_FragColor.w *= pow(abs(gl_FragCoord.z), 20.0);\n"
      "  gl_FragColor = mix(gl_FragColor,\n"
      "                     vec4(fogColor, gl_FragColor.w),\n"
      "                     fogFactor);\n"
      "}\n";

public:
  ShaderMaterialCloudsScene(ICanvas* iCanvas);
  ~ShaderMaterialCloudsScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  FreeCamera* _camera;
  ShaderMaterial* _cloudMaterial;
  long _startTime;

}; // end of class ShaderMaterialCloudsScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_MATERIALS_SHADER_MATERIAL_CLOUDS_SCENE_H
