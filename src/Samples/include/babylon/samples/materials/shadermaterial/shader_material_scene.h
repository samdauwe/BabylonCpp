#ifndef BABYLON_SAMPLES_MATERIALS_SHADER_MATERIAL_SCENE_H
#define BABYLON_SAMPLES_MATERIALS_SHADER_MATERIAL_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class ArcRotateCamera;
class Mesh;
class ShaderMaterial;
using ArcRotateCameraPtr = std::shared_ptr<ArcRotateCamera>;
using MeshPtr            = std::shared_ptr<Mesh>;
using ShaderMaterialPtr  = std::shared_ptr<ShaderMaterial>;

namespace Samples {

class ShaderMaterialScene : public IRenderableScene {

public:
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
      "uniform mat4 world;\n"
      "uniform mat4 worldViewProjection;\n"
      "\n"
      "varying vec3 vPositionW;\n"
      "varying vec3 vNormalW;\n"
      "\n"
      "void main(void) {\n"
      "   vec4 outPosition = worldViewProjection * vec4(position, 1.0);\n"
      "   gl_Position = outPosition;\n"
      "\n"
      "   vPositionW = vec3(world * vec4(position, 1.0));\n"
      "   vNormalW = normalize(vec3(world * vec4(normal, 0.0)));\n"
      "}\n";

  static constexpr const char* customFragmentShader
    = "#ifdef GL_ES\n"
      "precision highp float;\n"
      "#endif\n"
      "\n"
      "// Lights\n"
      "varying vec3 vPositionW;\n"
      "varying vec3 vNormalW;\n"
      "\n"
      "// Refs\n"
      "uniform vec3 cameraPosition;\n"
      "uniform float time;\n"
      "\n"
      "void main(void) {\n"
      "\n"
      "   vec3 color = vec3(.0, 1., 1.);\n"
      "   vec3 viewDirectionW = normalize(cameraPosition + vPositionW);\n"
      "\n"
      "   vec3 position = viewDirectionW;\n"
      "\n"
      "   float newPos = sin(time)*30.;\n"
      "   if (position.y > 0.) newPos *= -1.;\n"
      "\n"
      "       float t =(position.x+position.y)+sin(position.x*25.)*1.2;\n"
      "       float b = (position.x+position.y) * sin( .1 /abs(position.y - \n"
      "                 .060*t)*1.5);\n"
      "\n"
      "       for ( float i =0.0; i< 20.0; ++i )\n"
      "       {\n"
      "           position.x += sin (( position.y  + i )* 10.)* 1.6+newPos ;\n"
      "           float  ftmp = abs(1./ position.x / 500.0 );\n"
      "           b += ftmp;\n"
      "       }\n"
      "\n"
      "       gl_FragColor = vec4(color*b, 1.);\n"
      "}\n";

public:
  ShaderMaterialScene(ICanvas* iCanvas);
  ~ShaderMaterialScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  int _j;
  ArcRotateCameraPtr _camera;
  ShaderMaterialPtr _shaderMaterial;
  MeshPtr _polyhedron;

}; // end of class ShaderMaterialScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MATERIALS_SHADER_MATERIAL_SCENE_H
