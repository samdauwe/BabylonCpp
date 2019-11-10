#include <babylon/cameras/free_camera.h>
#include <babylon/core/random.h>
#include <babylon/core/time.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_shaders_store.h>
#include <babylon/materials/shader_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/vertex_data.h>
#include <babylon/samples/samples_index.h>

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
    = R"ShaderCode(
#ifdef GL_ES
precision highp float;
#endif

// Attributes
attribute vec3 position;
attribute vec2 uv;

// Uniforms
uniform mat4 worldViewProjection;

// Varying
varying vec2 vUV;

void main(void) {
    gl_Position = worldViewProjection * vec4(position, 1.0);
    vUV = uv;
}
)ShaderCode";

  /** Pixel (Fragment) Shader **/
  static constexpr const char* cloudFragmentShader
    = R"ShaderCode(
#ifdef GL_ES
precision highp float;
#endif

// Varying
varying vec2 vUV;

// Uniforms
uniform vec3 fogColor;
uniform float fogNear;
uniform float fogFar;

// Samplers
uniform sampler2D textureSampler;

void main(void) {
  float depth = gl_FragCoord.z / gl_FragCoord.w;
  float fogFactor = smoothstep(fogNear, fogFar, depth);

  gl_FragColor = texture2D(textureSampler, vUV);
  gl_FragColor.w *= pow(abs(gl_FragCoord.z), 20.0);
  gl_FragColor = mix(gl_FragColor,
                     vec4(fogColor, gl_FragColor.w),
                     fogFactor);
}
)ShaderCode";

public:
  ShaderMaterialCloudsScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas)
      , _camera{nullptr}
      , _cloudMaterial{nullptr}
      , _startTime{Time::unixtimeInMs()}
  {
    // Vertex shader
    Effect::ShadersStore()["cloudVertexShader"] = cloudVertexShader;

    // Fragment shader
    Effect::ShadersStore()["cloudFragmentShader"] = cloudFragmentShader;
  }

  ~ShaderMaterialCloudsScene() override = default;

  const char* getName() override
  {
    return "Shader Material Clouds Scene";
  }

  void initializeScene(ICanvas* /*canvas*/, Scene* scene) override
  {
    // Change scene clear color
    auto clearColor   = Color3::FromHexString("#4584b4");
    scene->clearColor = Color4(clearColor.r, clearColor.g, clearColor.b);

    // Create a camera
    _camera       = FreeCamera::New("camera", Vector3(0.f, -128.f, 0.f), scene);
    _camera->fov  = 30.f;
    _camera->minZ = 1.f;
    _camera->maxZ = 3000.f;

    // Create shader material
    IShaderMaterialOptions shaderMaterialOptions;
    shaderMaterialOptions.needAlphaBlending = true;
    shaderMaterialOptions.attributes        = {"position", "uv"};
    shaderMaterialOptions.uniforms          = {"worldViewProjection"};
    shaderMaterialOptions.samplers          = {"textureSampler"};
    _cloudMaterial = ShaderMaterial::New("cloud", scene, "cloud", shaderMaterialOptions);
    _cloudMaterial->setTexture("textureSampler", Texture::New("/textures/cloud.png", scene));
    _cloudMaterial->setFloat("fogNear", -100);
    _cloudMaterial->setFloat("fogFar", 3000);
    _cloudMaterial->setColor3("fogColor", Color3::FromInts(69, 132, 180));

    // Create merged planes
    auto size                                    = 128.f;
    size_t count                                 = 8000;
    std::unique_ptr<VertexData> globalVertexData = nullptr;

    for (size_t i = 0; i < count; ++i) {
      PlaneOptions planeOptions;
      planeOptions.size    = size;
      auto planeVertexData = VertexData::CreatePlane(planeOptions);

      planeVertexData->normals.clear(); // We do not need normals

      // Transform
      float randomScaling  = Math::random() * Math::random() * 1.5f + 0.5f;
      auto transformMatrix = Matrix::Scaling(randomScaling, randomScaling, 1.f);
      transformMatrix      = transformMatrix.multiply(Matrix::RotationZ(Math::random() * Math::PI));
      transformMatrix      = transformMatrix.multiply(Matrix::Translation(
        Math::random() * 1000.f - 500.f, -Math::random() * Math::random() * 100.f,
        static_cast<float>(count - i)));

      planeVertexData->transform(transformMatrix);

      // Merge
      if (!globalVertexData) {
        globalVertexData = std::move(planeVertexData);
      }
      else {
        globalVertexData->merge(*planeVertexData);
      }
    }

    auto clouds1 = Mesh::New("Clouds1", scene);
    globalVertexData->applyToMesh(*clouds1);
    clouds1->material = _cloudMaterial;

    auto clouds2 = Mesh::New("Clouds2", scene);
    globalVertexData->applyToMesh(*clouds2);
    clouds2->material     = _cloudMaterial;
    clouds2->position().z = -500.f;

    // Animation
    scene->registerBeforeRender([this](Scene* /*scene*/, EventState& /*es*/) {
      float cameraDepth     = std::fmod((Time::unixtimeInMs() - _startTime) * 0.03f, 8000.f);
      _camera->position().z = cameraDepth;
    });
  }

private:
  FreeCameraPtr _camera;
  ShaderMaterialPtr _cloudMaterial;
  long _startTime;

}; // end of class ShaderMaterialCloudsScene

BABYLON_REGISTER_SAMPLE("Shader Material", ShaderMaterialCloudsScene)

} // end of namespace Samples
} // end of namespace BABYLON