#ifndef BABYLON_SAMPLES_TEXTURES_CUSTOM_RENDER_TARGETS_SCENE_H
#define BABYLON_SAMPLES_TEXTURES_CUSTOM_RENDER_TARGETS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class ArcRotateCamera;
class RenderTargetTexture;
class ShaderMaterial;
using ArcRotateCameraPtr = std::shared_ptr<ArcRotateCamera>;
using RenderTargetTexturePtr = std::shared_ptr<RenderTargetTexture>;
using ShaderMaterialPtr = std::shared_ptr<ShaderMaterial>;

namespace Samples {

/**
 * @brief Custom render targets scene. Example demonstrating how to use render
 * target textures to generate procedural data.
 * @see https://www.babylonjs-playground.com/#CJWDJR#0
 */
class CustomRenderTargetsScene : public IRenderableScene {

public:
  static constexpr const char* customDepthVertexShader
    = "#ifdef GL_ES\n"
      "precision highp float;\n"
      "#endif\n"
      "attribute vec3 position;\n"
      "uniform mat4 worldViewProjection;\n"
      "void main(void) {\n"
      "gl_Position = worldViewProjection * vec4(position, 1.0);\n"
      "}";

  static constexpr const char* customDepthPixelShader
    = "#ifdef GL_ES\n"
      "precision highp float;\n"
      "#endif\n"
      "\n"
      "void main(void) {\n"
      "float depth =  1.0 - \n"
      "              (2.0 / (100.0 + 1.0 - gl_FragCoord.z * (100.0 - 1.0)));\n"
      "gl_FragColor = vec4(depth, depth, depth, 1.0);\n"
      "}\n"
      "";

public:
  CustomRenderTargetsScene(ICanvas* iCanvas);
  ~CustomRenderTargetsScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  ArcRotateCameraPtr _camera;
  RenderTargetTexturePtr _renderTarget;
  ShaderMaterialPtr _depthMaterial;

}; // end of class CustomRenderTargetsScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_TEXTURES_CUSTOM_RENDER_TARGETS_SCENE_H
