#ifndef BABYLON_SAMPLES_GENERIC_FIREWORKS_WITH_SHADER_CODE_SCENE_H
#define BABYLON_SAMPLES_GENERIC_FIREWORKS_WITH_SHADER_CODE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/materials/effect_shaders_store.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Fireworks with Shader Code scene.
 *
 * Source: https://doc.babylonjs.com/samples/writing2
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
  FireworksWithShaderCodeScene(ICanvas* iCanvas);
  ~FireworksWithShaderCodeScene();

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  EffectShadersStore _effectShadersStore;
  float _time;
  ShaderMaterial* _shaderMaterial;

}; // end of class FireworksWithShaderCodeScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_GENERIC_FIREWORKS_WITH_SHADER_CODE_SCENE_H
