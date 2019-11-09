#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_shaders_store.h>
#include <babylon/materials/shader_material.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
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
  ShaderMaterialScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas)
      , _j{0}
      , _camera{nullptr}
      , _shaderMaterial{nullptr}
      , _polyhedron{nullptr}
  {
    // Vertex shader
    Effect::ShadersStore()["customVertexShader"] = customVertexShader;

    // Fragment shader
    Effect::ShadersStore()["customFragmentShader"] = customFragmentShader;
  }

  ~ShaderMaterialScene() override = default;

  const char* getName() override
  {
    return "Shader Material Scene";
  }
  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Change scene clear color
    scene->clearColor = Color4(0.f, 0.f, 0.f);

    // Create a Camera
    _camera
      = ArcRotateCamera::New("ArcRotateCamera", 0.f, 1.5f, 150.f, Vector3(0.f, 0.f, 0.f), scene);

    // Target the camera to scene origin
    _camera->setTarget(Vector3::Zero());

    // Attach control
    _camera->attachControl(canvas, true);

    // Create shader material
    IShaderMaterialOptions shaderMaterialOptions;
    shaderMaterialOptions.attributes = {"position", "normal"};
    shaderMaterialOptions.uniforms
      = {"worldViewProjection", "world", "vPositionW", "vNormalW", "time"};
    _shaderMaterial = ShaderMaterial::New("ball_shader", scene, "custom", shaderMaterialOptions);

    // Create polyhedron
    PolyhedronOptions polyhedronOptions;
    polyhedronOptions.size = 25.f;
    polyhedronOptions.type = 0;
    _polyhedron            = Mesh::CreatePolyhedron("CreatePolyhedron", polyhedronOptions, scene);
    _polyhedron->material  = _shaderMaterial;

    // Create box
    auto box = Mesh::CreateBox("Box", 500.f, scene);
    box->flipFaces(true);
    box->material = _shaderMaterial;

    // Animation
    scene->onAfterCameraRenderObservable.add([this](Camera*, EventState&) {
      _shaderMaterial->setFloat("time", 3.f / static_cast<float>(_j));
      if (_j == 2000) {
        _j = 0;
      }
      ++_j;
      _polyhedron->rotation().x += 0.03f;
      _camera->alpha += 0.01f;
      _camera->position().x += 0.01f;
    });
  }

private:
  int _j;
  ArcRotateCameraPtr _camera;
  ShaderMaterialPtr _shaderMaterial;
  MeshPtr _polyhedron;

}; // end of class ShaderMaterialScene

BABYLON_REGISTER_SAMPLE("Shader Materials", ShaderMaterialScene)

} // end of namespace Samples
} // end of namespace BABYLON
