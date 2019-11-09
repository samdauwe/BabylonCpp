#include <babylon/cameras/free_camera.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_shaders_store.h>
#include <babylon/materials/shader_material.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

class ShaderMaterialDoubleHelixScene : public IRenderableScene {

public:
  /** Vertex Shader **/
  static constexpr const char* customVertexShader
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
  // Double Helix ( https://www.shadertoy.com/view/Md3XRM )
  static constexpr const char* customFragmentShader
    = R"ShaderCode(
#ifdef GL_ES
precision highp float;
#endif

// Varying
varying vec3 vPosition;
varying vec3 vNormal;
varying vec2 vUV;

// Uniforms
uniform mat4 worldViewProjection;
uniform float iTime;
uniform float iAspectRatio;
uniform vec2 iResolution;

vec3 LIGHT = normalize(vec3(-0.3, 0.2, -0.1));

float FULL_SIZE = 2.0;
float EDGE_SIZE = 0.2;
float PAIR_SIZE = 0.2;

vec3 n3(vec3 n) {
  return fract(cos(dot(n, vec3(813, 12, 376))) *
         vec3(901.81, 827.46, 615.79));
}

vec3 model(vec3 p) {
  float A = p.z / 3.0 + iTime * 0.25;
  vec3 R = vec3(cos(A), sin(A), 0);
  vec3 C =
      vec3(mod(p.xy + 8., 16.) - 8. + R.yx * vec2(1, -1),
           fract(p.z) - 0.5);

  float H =
      min(length(C.xy + R.xy * FULL_SIZE),
          length(C.xy - R.xy * FULL_SIZE)) *
          0.5 -
      EDGE_SIZE;
  float P = max(length(vec2(dot(C.xy, R.yx * vec2(1, -1)), C.z)) -
                PAIR_SIZE,
                length(C.xy) - FULL_SIZE);

  float T = FULL_SIZE + 0.01 + 2. * EDGE_SIZE - length(C.xy);
  return vec3(min(H, P), T, P);
}

vec3 normal(vec3 p) {
  vec2 N = vec2(-0.04, 0.04);

  return normalize(model(p + N.xyy).x * N.xyy +
                   model(p + N.yxy).x * N.yxy +
                   model(p + N.yyx).x * N.yyx +
                   model(p + N.xxx).x * N.xxx);
}

vec4 raymarch(vec3 p, vec3 d) {
  vec4 M = vec4(p + d * 2.0, 0);
  for (int i = 0; i < 100; i++) {
    float S = model(M.xyz).x;
    M += vec4(d, 1) * S;
    if (S < 0.01 || M.w > 50.0)
      break;
  }
  return M;
}

vec3 sky(vec3 d) {
  float L = dot(d, LIGHT);
  return vec3(0.3, 0.5, 0.6) - 0.3 * (-L * 0.5 + 0.5) +
         exp2(32.0 * (L - 1.0));
}

vec3 color(vec3 p, vec3 d) {
  vec2 M = model(p).yz;
  float A = atan(mod(p.y + 8., 16.) - 8., 8. - mod(p.x + 8., 16.));
  float T1 = ceil(fract(cos(floor(p.z) * 274.63)) - 0.5);
  float T2 = sign(fract(cos(floor(p.z - 80.0) * 982.51)) - 0.5);
  float T3 = T2 * sign(cos(p.z / 3.0 + iTime * 0.25 + A));

  float L = dot(normal(p), LIGHT) * 0.5 + 0.5;
  float R = max(dot(reflect(d, normal(p)), LIGHT), 0.0);
  vec3 C = mix(mix(vec3(0.9 - 0.8 * T3, 0.9 - 0.6 * T3, T3),
                   vec3(1.0 - 0.6 * T3, 0.2 + 0.8 * T3, 0.1 * T3),
                   T1), vec3(0.2), step(0.01, M.y));
  C = mix(C, vec3(0.2, 0.5, 1.0), step(0.01, -M.x));
  return C * L + pow(R, 16.0);
}

void main(void) {
  vec2 iMouse = vec2(0.0, 0.0);
  vec2 A = iMouse.xy / iResolution.xy * vec2(2, 1) * 3.1416;
  vec3 D = vec3(cos(A.x) * sin(A.y), sin(A.x) * sin(A.y), cos(A.y));
  D = mix(vec3(1, 0, 0), D, ceil((A.x + A.y) / 10.0));
  vec3 P = D * 12.0 - vec3(0, 0, iTime * 2.0);

  vec3 X = normalize(-D);
  vec3 Y = normalize(cross(X, vec3(0, 0, 1)));
  vec3 Z = normalize(cross(X, Y));

  vec2 UV = (-1.0 + 2.0 * vUV.xy) -
            (iResolution.xy * 0.05 / iResolution.y);
  vec3 R = normalize(mat3(X, Y, Z) * vec3(1, UV));

  vec4 M = raymarch(P, R);
  vec3 C = mix(color(M.xyz, R), sky(R),
               smoothstep(0.5, 1.0, M.w / 50.0));
  gl_FragColor = vec4(C, 1);
}
)ShaderCode";

public:
  ShaderMaterialDoubleHelixScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas), _time{0.f}, _shaderMaterial{nullptr}
  {
    // Vertex shader
    Effect::ShadersStore()["customVertexShader"] = customVertexShader;

    // Fragment shader
    Effect::ShadersStore()["customFragmentShader"] = customFragmentShader;
  }

  ~ShaderMaterialDoubleHelixScene() override = default;

  const char* getName() override
  {
    return "Shader Material Double Helix Scene";
  }
  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Create a FreeCamera, and set its position to (x:0, y:0, z:-8)
    auto camera = FreeCamera::New("camera1", Vector3(0.f, 0.f, -8.f), scene);

    // Target the camera to scene origin
    camera->setTarget(Vector3::Zero());

    // Attach the camera to the canvas
    camera->attachControl(canvas, true);

    // Create a basic light, aiming 0,1,0 - meaning, to the sky
    HemisphericLight::New("light1", Vector3(0.f, 1.f, 0.f), scene);

    // Create a built-in "box" shape
    const float ratio = static_cast<float>(getEngine()->getRenderWidth())
                        / static_cast<float>(getEngine()->getRenderHeight());
    BoxOptions options;
    options.size            = 5.f;
    options.sideOrientation = Mesh::DEFAULTSIDE;
    options.updatable       = false;
    options.width           = *options.size * ratio;
    auto skybox             = MeshBuilder::CreateBox("skybox", options, scene);

    // Create shader material
    IShaderMaterialOptions shaderMaterialOptions;
    shaderMaterialOptions.attributes = {"position", "uv"};
    shaderMaterialOptions.uniforms
      = {"iTime", "worldViewProjection", "iAspectRatio", "iResolution"};
    _shaderMaterial = ShaderMaterial::New("boxShader", scene, "custom", shaderMaterialOptions);

    // box + sky = skybox !
    skybox->material = _shaderMaterial;

    // Animation
    scene->onAfterCameraRenderObservable.add([this](Camera*, EventState&) {
      const Vector2 resolution{static_cast<float>(_engine->getRenderWidth()),
                               static_cast<float>(_engine->getRenderHeight())};
      const float aspectRatio = resolution.x / resolution.y;
      _shaderMaterial->setFloat("iTime", _time);
      _shaderMaterial->setFloat("iAspectRatio", aspectRatio);
      _shaderMaterial->setVector2("iResolution", resolution);
      _time += 0.01f * getScene()->getAnimationRatio();
    });
  }

private:
  float _time;
  ShaderMaterialPtr _shaderMaterial;

}; // end of class ShaderMaterialDoubleHelixScene

BABYLON_REGISTER_SAMPLE("Shader Materials", ShaderMaterialDoubleHelixScene)

} // end of namespace Samples
} // end of namespace BABYLON