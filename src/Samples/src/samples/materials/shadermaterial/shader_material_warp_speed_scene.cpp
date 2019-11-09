#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/shader_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Warp speed scene. Example demonstrating on how to fly through stars using a custom shader
 * texture.
 * @see https://www.babylonjs-playground.com/#1WBBW0#1
 */
class ShaderMaterialWarpSpeedScene : public IRenderableScene {

public:
  /** Vertex Shader **/
  static constexpr const char* customVertexShader
    = R"ShaderCode(
#ifdef GL_ES
precision highp float;
#endif

// Attributes
attribute vec3 position;
attribute vec3 normal;
attribute vec2 uv;

// Uniforms
uniform mat4 worldViewProjection;

// Varying
varying vec4 vPosition;
varying vec3 vNormal;

void main() {
  vec4 p = vec4(position, 1.);
  vPosition = p;
  vNormal = normal;
  gl_Position = worldViewProjection * p;
}
)ShaderCode";

  /** Pixel (Fragment) Shader **/
  // Interstellar ( https://www.shadertoy.com/view/Xdl3D2 )
  static constexpr const char* customFragmentShader
    = R"ShaderCode(
#ifdef GL_ES
precision highp float;
#endif

// Varying
varying vec4 vPosition;
varying vec3 vNormal;

// Uniforms
uniform sampler2D textureSampler;
uniform sampler2D refSampler;
uniform vec3 iResolution;
uniform mat4 worldView;

const float tau = 6.28318530717958647692;

// Gamma correction
#define GAMMA (2.2)

vec3 ToLinear(in vec3 col) {
  // Simulate a monitor, converting colour values into light values
  return pow(col, vec3(GAMMA));
}

vec3 ToGamma(in vec3 col) {
  // Convert back into colour values, so the correct light will come 
out of the
  // monitor
  return pow(col, vec3(1.0 / GAMMA));
}

vec4 Noise(in ivec2 x) {
  return texture2D(refSampler, (vec2(x) + 0.5) / 256.0, -100.0);
}

vec4 Rand(in int x) {
  vec2 uv;
  uv.x = (float(x) + 0.5) / 256.0;
  uv.y = (floor(uv.x) + 0.5) / 256.0;
  return texture2D(refSampler, uv, -100.0);
}

uniform float time;

void main(void) {

  vec3 ray;
  ray.xy = .2 * (vPosition.xy - vec2(.5));
  ray.z = 1.;

  float offset = time * .5;
  float speed2 = (cos(offset) + 1.0) * 2.0;
  float speed = speed2 + .1;
  offset += sin(offset) * .96;
  offset *= 2.0;

  vec3 col = vec3(0.);

  vec3 stp = ray / max(abs(ray.x), abs(ray.y));

  vec3 pos = 2.0 * stp + .5;
  for (int i = 0; i < 20; i++) {
    float z = Noise(ivec2(pos.xy)).x;
    z = fract(z - offset);
    float d = 50.0 * z - pos.z;
    float w = pow(max(0.0, 1.0 - 8.0 * length(fract(pos.xy) - .5)), 
                  2.0);
    vec3 c = max(vec3(0),
                 vec3(1.0 - abs(d + speed2 * .5) / speed, 1.0 - abs(d) 
                    / speed,
                      1.0 - abs(d - speed2 * .5) / speed));
    col += 1.5 * (1.0 - z) * c * w;
    pos += stp;
  }

  gl_FragColor = vec4(ToGamma(col), 1.);
}
)ShaderCode";

public:
  ShaderMaterialWarpSpeedScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas), _time{0.f}, _shaderMaterial{nullptr}
  {
    // Vertex shader
    Effect::ShadersStore()["customVertexShader"] = customVertexShader;

    // Fragment shader
    Effect::ShadersStore()["customFragmentShader"] = customFragmentShader;
  }

  ~ShaderMaterialWarpSpeedScene() override = default;

  const char* getName() override
  {
    return "Shader Material Warp Speed Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Create a camera
    auto camera = ArcRotateCamera::New("Camera", 0.f, Math::PI_2, 12.f, Vector3::Zero(), scene);
    camera->attachControl(canvas, false);
    camera->lowerRadiusLimit = 1.f;
    camera->minZ             = 1.f;
    camera->setPosition(Vector3(-10.f, 0.f, -0.5f));

    // Create shader material
    IShaderMaterialOptions shaderMaterialOptions;
    shaderMaterialOptions.attributes = {"position", "normal", "uv"};
    shaderMaterialOptions.uniforms
      = {"world", "worldView", "worldViewProjection", "view", "projection"};
    _shaderMaterial = ShaderMaterial::New("shader", scene, "custom", shaderMaterialOptions);

    // Create plane mesh
    auto mesh       = Mesh::CreatePlane("mesh", 10.f, scene);
    auto rotateAxis = Axis::Y();
    mesh->rotate(rotateAxis, Math::PI * 0.5f);

    // Create texture
    auto refTexture = Texture::New("textures/warpSpeedNoise.png", scene);

    // Configure the shader material
    _shaderMaterial->setTexture("refSampler", refTexture);
    _shaderMaterial->setFloat("time", 0.f);
    _shaderMaterial->setVector3("cameraPosition", Vector3::Zero());
    _shaderMaterial->backFaceCulling = false;

    // Assign material
    mesh->material = _shaderMaterial;

    // Animation
    scene->onAfterCameraRenderObservable.add([this](Camera*, EventState&) {
      _shaderMaterial->setFloat("time", _time);
      _time += 0.02f;
      _shaderMaterial->setVector3("cameraPosition", _scene->activeCamera()->position);
    });
  }

private:
  float _time;
  ShaderMaterialPtr _shaderMaterial;

}; // end of class ShaderMaterialWarpSpeedScene

BABYLON_REGISTER_SAMPLE("Shader Materials", ShaderMaterialWarpSpeedScene)

} // end of namespace Samples
} // end of namespace BABYLON