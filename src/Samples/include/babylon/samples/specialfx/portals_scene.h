#ifndef BABYLON_SAMPLES_SPECIAL_FX_PORTALS_SCENE_H
#define BABYLON_SAMPLES_SPECIAL_FX_PORTALS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class RenderTargetTexture;
class ShaderMaterial;
using RenderTargetTexturePtr = std::shared_ptr<RenderTargetTexture>;
using ShaderMaterialPtr      = std::shared_ptr<ShaderMaterial>;

namespace Samples {

/**
 * @brief Portals scene. Example demonstrating how to create a portal effect
 * using custom shaders.
 * @see https://www.babylonjs-playground.com/#EEOWP#7
 */
class PortalsScene : public IRenderableScene {

public:
  static constexpr const char* depthVertexShader
    = "#ifdef GL_ES\n"
      "precision highp float;\n"
      "#endif\n"
      "attribute vec3 position;\n"
      "uniform mat4 worldViewProjection;\n"
      "void main(void) {\n"
      "gl_Position = worldViewProjection * vec4(position, 1.0);\n"
      "}";

  static constexpr const char* depthPixelShader
    = "#ifdef GL_ES\n"
      "precision highp float;\n"
      "#endif\n"
      "\n"
      "void main(void) {\n"
      "gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);\n"
      "}\n"
      "";

public:
  PortalsScene(ICanvas* iCanvas);
  ~PortalsScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  RenderTargetTexturePtr _renderTarget;
  ShaderMaterialPtr _depthMaterial;

}; // end of class PortalsScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_SPECIAL_FX_PORTALS_SCENE_H
