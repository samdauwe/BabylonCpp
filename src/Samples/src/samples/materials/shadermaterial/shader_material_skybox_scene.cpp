#include <babylon/cameras/free_camera.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_shaders_store.h>
#include <babylon/materials/shader_material.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/samples_index.h>

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
  ShaderMaterialSkyboxScene(ICanvas* iCanvas) : IRenderableScene(iCanvas), _shaderMaterial{nullptr}
  {
    // Vertex shader
    Effect::ShadersStore()["gradientVertexShader"] = gradientVertexShader;

    // Pixel (Fragment) Shader
    Effect::ShadersStore()["gradientFragmentShader"] = gradientPixelShader;
  }

  ~ShaderMaterialSkyboxScene() override = default;

  const char* getName() override
  {
    return "Shader Material Skybox Scene";
  }
  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Create a FreeCamera
    auto camera = FreeCamera::New("cam", Vector3(0.f, 0.f, -500.f), scene);

    // Target the camera to scene origin
    camera->setTarget(Vector3::Zero());

    // Attach the camera to the canvas
    camera->attachControl(canvas, true);

    // Hemispheric light to light the scene
    HemisphericLight::New("hemi", Vector3(0.f, 1.f, 0.f), scene);

    // The skybox creation
    auto skybox = Mesh::CreateSphere("skyBox", 10u, 1000.f, scene);

    // Create shader material
    IShaderMaterialOptions shaderMaterialOptions;
    shaderMaterialOptions.attributes = {"position", "normal", "uv"};
    shaderMaterialOptions.uniforms   = {"worldViewProjection"};
    _shaderMaterial = ShaderMaterial::New("gradient", scene, "gradient", shaderMaterialOptions);
    _shaderMaterial->setFloat("offset", 0.f);
    _shaderMaterial->setColor3("topColor", Color3::FromInts(0, 119, 255));
    _shaderMaterial->setColor3("bottomColor", Color3::FromInts(240, 240, 255));
    _shaderMaterial->backFaceCulling = false;

    // box + sky = skybox !
    skybox->material = _shaderMaterial;
  }

private:
  ShaderMaterialPtr _shaderMaterial;

}; // end of class ShaderMaterialSkyboxScene

BABYLON_REGISTER_SAMPLE("Shader Materials", ShaderMaterialSkyboxScene)

} // end of namespace Samples
} // end of namespace BABYLON
