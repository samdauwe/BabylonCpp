#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_shaders_store.h>
#include <babylon/materials/shader_material.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Shader Material The Ring Color Remix Scene. Example demonstrating how to use shadertoy.com
 * shaders with BabylonCpp.
 * @see https://www.babylonjs-playground.com/#EWME8L#9
 */
class ShaderMaterialTheRingColorRemixScene : public IRenderableScene {

public:
  /** Vertex Shader **/
  // source: https://www.shadertoy.com/view/WtG3RD
  static constexpr const char* customVertexShader = R"ShaderCode(
#ifdef GL_ES
precision highp float;
#endif

// Attributes
attribute vec3 position;
attribute vec3 normal;
attribute vec2 uv;

uniform mat4 worldViewProjection;
uniform float time;

varying vec3 vPosition;
varying vec2 vUV;

void main() {
  vec4 p = vec4( position, 1. );
  vPosition = p.xyz;
  vUV = uv;
  gl_Position = worldViewProjection * p;
}
)ShaderCode";

  /** Pixel (Fragment) Shader **/
  static constexpr const char* customFragmentShader = R"ShaderCode(
#ifdef GL_ES
precision highp float;
#endif

uniform float iTime;
uniform vec2 iResolution;

varying vec3 vPosition;
varying vec2 vUV;

// --------- START-SHADER-TOY-CODE-HERE ------------

// Original shader code from https://www.shadertoy.com/view/WtG3RD

#define TAU 6.2831852
#define MOD3 vec3(.1031,.11369,.13787)
#define BLACK_COL vec3(16,21,25)/255.

vec3 hash33(vec3 p3)
{
  p3 = fract(p3 * MOD3);
  p3 += dot(p3, p3.yxz+19.19);
  return -1.0 + 2.0 * fract(vec3((p3.x + p3.y)*p3.z, (p3.x+p3.z)*p3.y, (p3.y+p3.z)*p3.x));
}

float simplex_noise(vec3 p)
{
  const float K1 = 0.333333333;
  const float K2 = 0.166666667;

  vec3 i = floor(p + (p.x + p.y + p.z) * K1);
  vec3 d0 = p - (i - (i.x + i.y + i.z) * K2);

  vec3 e = step(vec3(0.0), d0 - d0.yzx);
  vec3 i1 = e * (1.0 - e.zxy);
  vec3 i2 = 1.0 - e.zxy * (1.0 - e);

  vec3 d1 = d0 - (i1 - 1.0 * K2);
  vec3 d2 = d0 - (i2 - 2.0 * K2);
  vec3 d3 = d0 - (1.0 - 3.0 * K2);

  vec4 h = max(0.6 - vec4(dot(d0, d0), dot(d1, d1), dot(d2, d2), dot(d3, d3)), 0.0);
  vec4 n = h * h * h * h * vec4(dot(d0, hash33(i)), dot(d1, hash33(i + i1)), dot(d2, hash33(i + i2)), dot(d3, hash33(i + 1.0)));

  return dot(vec4(31.316), n);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
  vec2 uv = (fragCoord.xy-iResolution.xy*0.5)/iResolution.y;

  float a = sin(atan(uv.y, uv.x));
  float am = abs(a-.5)/4.;
  float l = length(uv);

  float m1 = clamp(.1/smoothstep(.0, 1.75, l), 0., 1.);
  float m2 = clamp(.1/smoothstep(.42, 0., l), 0., 1.);
  float s1 = (simplex_noise(vec3(uv*2., 1. + iTime*.525))*(max(1.0 - l*1.75, 0.)) + .9);
  float s2 = (simplex_noise(vec3(uv*1., 15. + iTime*.525))*(max(.0 + l*1., .025)) + 1.25);
  float s3 = (simplex_noise(vec3(vec2(am, am*100. + iTime*3.)*.15, 30. + iTime*.525))*(max(.0 + l*1., .25)) + 1.5);
  s3 *= smoothstep(0.0, .3345, l);

  float sh = smoothstep(0.15, .35, l);


  float m = m1*m1*m2 * ((s1*s2*s3) * (1.-l)) * sh;
  //m = clamp(m, 0., 1.);

  vec3 col = mix(BLACK_COL, (0.5 + 0.5*cos(iTime+uv.xyx*3.+vec3(0,2,4))), m);

  fragColor = vec4(col, 1.);
}

// --------- END-SHADER-TOY-CODE-HERE ------------

void main()
{
  mainImage(gl_FragColor, vUV * iResolution.xy);
}
)ShaderCode";

public:
  ShaderMaterialTheRingColorRemixScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas), _time{0.f}, _camera{nullptr}, _shaderMaterial{nullptr}
  {
    // Vertex shader
    Effect::ShadersStore()["customVertexShader"] = customVertexShader;

    // Fragment shader
    Effect::ShadersStore()["customFragmentShader"] = customFragmentShader;
  }

  ~ShaderMaterialTheRingColorRemixScene() override = default;

  const char* getName() override
  {
    return "Shader Material The Ring Color Remix Scene";
  }
  void initializeScene(ICanvas* /*canvas*/, Scene* scene) override
  {
    // Clear color
    scene->clearColor = Color3::Black();

    // Camera
    _camera = ArcRotateCamera::New("camera1", 0.f, 0.f, 10.f, Vector3(0.f, -1.f, 1.f), scene);
    _camera->setTarget(Vector3::Zero());
    // camera.attachControl(canvas, true);
    _camera->mode        = Camera::ORTHOGRAPHIC_CAMERA;
    _camera->orthoTop    = 1.f;
    _camera->orthoBottom = -1.f;
    _camera->orthoLeft   = -1.f;
    _camera->orthoRight  = 1.f;

    // Ligth
    HemisphericLight::New("light1", Vector3(0.f, 1.f, 0.f), scene);

    // Create shader material
    IShaderMaterialOptions shaderMaterialOptions;
    shaderMaterialOptions.attributes = {"position", "normal", "uv"};
    // shaderMaterialOptions.defines    = {"precision highp float"};
    shaderMaterialOptions.uniforms
      = {"world", "worldView", "worldViewProjection", "view", "iTime", "iResolution"};
    _shaderMaterial = ShaderMaterial::New("shader", scene, "custom", shaderMaterialOptions);
    _shaderMaterial->setVector2("iResolution", Vector2(1.f, 1.f));

    // Ground
    auto ground                      = Mesh::CreateGround("ground1", 2, 2, 1, scene);
    ground->material                 = _shaderMaterial;
    _shaderMaterial->backFaceCulling = false;
    ground->rotation().y             = -Math::PI_2;

    // Animation
    scene->registerBeforeRender([this](Scene* scene, EventState& /*es*/) {
      _time += 0.01f * getScene()->getAnimationRatio();
      _shaderMaterial->setFloat("iTime", _time);
      const auto aRatio = scene->getEngine()->getAspectRatio(*_camera);
      _shaderMaterial->setVector2("iResolution", Vector2(aRatio, 1.f));
    });
  }

private:
  float _time;
  ArcRotateCameraPtr _camera;
  ShaderMaterialPtr _shaderMaterial;

}; // end of class ShaderMaterialBoxScene

BABYLON_REGISTER_SAMPLE("Shader Material", ShaderMaterialTheRingColorRemixScene)

} // end of namespace Samples
} // end of namespace BABYLON
