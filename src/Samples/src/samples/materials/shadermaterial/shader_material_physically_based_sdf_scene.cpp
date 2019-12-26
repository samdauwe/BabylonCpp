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
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

class ShaderMaterialPhysicallyBasedSdfScene : public IRenderableScene {

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
  // Physically-based SDF ( https://www.shadertoy.com/view/XlKSDR )
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
#define saturate(x) clamp(x, 0.0, 1.0)
#define PI 3.14159265359

//--------------------------------------------------------------------
// Distance field functions
//--------------------------------------------------------------------

float sdPlane(in vec3 p) { return p.y; }

float sdSphere(in vec3 p, float s) { return length(p) - s; }

float sdTorus(in vec3 p, in vec2 t) {
  return length(vec2(length(p.xz) - t.x, p.y)) - t.y;
}

vec2 opUnion(vec2 d1, vec2 d2) { return d1.x < d2.x ? d1 : d2; }

vec2 scene(in vec3 position) {
  vec2 scene =
      opUnion(vec2(sdPlane(position), 1.0),
              vec2(sdSphere(position - vec3(0.0, 0.4, 0.0), 0.4),
                   12.0));
  return scene;
}

//--------------------------------------------------------------------
// Ray casting
//--------------------------------------------------------------------

float shadow(in vec3 origin, in vec3 direction) {
  float hit = 1.0;
  float t = 0.02;

  for (int i = 0; i < 1000; i++) {
    float h = scene(origin + direction * t).x;
    if (h < 0.001)
      return 0.0;
    t += h;
    hit = min(hit, 10.0 * h / t);
    if (t >= 2.5)
      break;
  }

  return clamp(hit, 0.0, 1.0);
}

vec2 traceRay(in vec3 origin, in vec3 direction) {
  float material = -1.0;

  float t = 0.02;

  for (int i = 0; i < 1000; i++) {
    vec2 hit = scene(origin + direction * t);
    if (hit.x < 0.002 || t > 20.0)
      break;
    t += hit.x;
    material = hit.y;
  }

  if (t > 20.0) {
    material = -1.0;
  }

  return vec2(t, material);
}

vec3 normal(in vec3 position) {
  vec3 epsilon = vec3(0.001, 0.0, 0.0);
  vec3 n =
      vec3(scene(position + epsilon.xyy).x -
           scene(position - epsilon.xyy).x,
           scene(position + epsilon.yxy).x -
           scene(position - epsilon.yxy).x,
           scene(position + epsilon.yyx).x -
           scene(position - epsilon.yyx).x);
  return normalize(n);
}

//--------------------------------------------------------------------
// BRDF
//--------------------------------------------------------------------

float pow5(float x) {
  float x2 = x * x;
  return x2 * x2 * x;
}

float D_GGX(float linearRoughness, float NoH, const vec3 h) {
  // Walter et al. 2007,
  // 'Microfacet Models for Refraction through Rough Surfaces'
  float oneMinusNoHSquared = 1.0 - NoH * NoH;
  float a = NoH * linearRoughness;
  float k = linearRoughness / (oneMinusNoHSquared + a * a);
  float d = k * k * (1.0 / PI);
  return d;
}

float V_SmithGGXCorrelated(float linearRoughness,
                           float NoV, float NoL) {
  // Heitz 2014, 'Understanding the Masking-Shadowing Function in
  // Microfacet-Based BRDFs'
  float a2 = linearRoughness * linearRoughness;
  float GGXV = NoL * sqrt((NoV - a2 * NoV) * NoV + a2);
  float GGXL = NoV * sqrt((NoL - a2 * NoL) * NoL + a2);
  return 0.5 / (GGXV + GGXL);
}

vec3 F_Schlick(const vec3 f0, float VoH) {
  // Schlick 1994,
  // 'An Inexpensive BRDF Model for Physically-Based Rendering'
  return f0 + (vec3(1.0) - f0) * pow5(1.0 - VoH);
}

float F_Schlick(float f0, float f90, float VoH) {
  return f0 + (f90 - f0) * pow5(1.0 - VoH);
}

float Fd_Burley(float linearRoughness, float NoV,
                float NoL, float LoH) {
  // Burley 2012, 'Physically-Based Shading at Disney'
  float f90 = 0.5 + 2.0 * linearRoughness * LoH * LoH;
  float lightScatter = F_Schlick(1.0, f90, NoL);
  float viewScatter = F_Schlick(1.0, f90, NoV);
  return lightScatter * viewScatter * (1.0 / PI);
}

float Fd_Lambert() { return 1.0 / PI; }

//--------------------------------------------------------------------
// Indirect lighting
//--------------------------------------------------------------------

vec3 Irradiance_SphericalHarmonics(const vec3 n) {
  // Irradiance from 'Ditch River' IBL
  // (http://www.hdrlabs.com/sibl/archive.html)
  return max(
      vec3(0.754554516862612, 0.748542953903366, 0.790921515418539) +
          vec3(-0.083856548007422,
               0.092533500963210,
               0.322764661032516) *
              (n.y) +
          vec3(0.308152705331738,
               0.366796330467391,
               0.466698181299906) *
              (n.z) +
          vec3(-0.188884931542396,
               -0.277402551592231,
               -0.377844212327557) *
              (n.x),
      0.0);
}

vec2 PrefilteredDFG_Karis(float roughness, float NoV) {
  // Karis 2014, 'Physically Based Material on Mobile'
  const vec4 c0 = vec4(-1.0, -0.0275, -0.572, 0.022);
  const vec4 c1 = vec4(1.0, 0.0425, 1.040, -0.040);

  vec4 r = roughness * c0 + c1;
  float a004 = min(r.x * r.x, exp2(-9.28 * NoV)) * r.x + r.y;

  return vec2(-1.04, 1.04) * a004 + r.zw;
}

//--------------------------------------------------------------------
// Tone mapping and transfer functions
//--------------------------------------------------------------------

vec3 Tonemap_ACES(const vec3 x) {
  // Narkowicz 2015, 'ACES Filmic Tone Mapping Curve'
  const float a = 2.51;
  const float b = 0.03;
  const float c = 2.43;
  const float d = 0.59;
  const float e = 0.14;
  return (x * (a * x + b)) / (x * (c * x + d) + e);
}

vec3 OECF_sRGBFast(const vec3 linear) {
  return pow(linear, vec3(1.0 / 2.2));
}

//--------------------------------------------------------------------
// Rendering
//--------------------------------------------------------------------

vec3 render(in vec3 origin, in vec3 direction, out float distance) {
  // Sky gradient
  vec3 color = vec3(0.65, 0.85, 1.0) + direction.y * 0.72;

  // (distance, material)
  vec2 hit = traceRay(origin, direction);
  distance = hit.x;
  float material = hit.y;

  // We've hit something in the scene
  if (material > 0.0) {
    vec3 position = origin + distance * direction;

    vec3 v = normalize(-direction);
    vec3 n = normal(position);
    vec3 l = normalize(vec3(0.6, 0.7, -0.7));
    vec3 h = normalize(v + l);
    vec3 r = normalize(reflect(direction, n));

    float NoV = abs(dot(n, v)) + 1e-5;
    float NoL = saturate(dot(n, l));
    float NoH = saturate(dot(n, h));
    float LoH = saturate(dot(l, h));

    vec3 baseColor = vec3(0.0);
    float roughness = 0.0;
    float metallic = 0.0;

    float intensity = 2.0;
    float indirectIntensity = 0.64;

    if (material < 4.0) {
      // Checkerboard floor
      float f = mod(floor(6.0 * position.z) + floor(6.0 * position.x),
                    2.0);
      baseColor = 0.4 + f * vec3(0.6);
      roughness = 0.1;
    } else if (material < 16.0) {
      // Metallic objects
      baseColor = vec3(0.3, 0.0, 0.0);
      roughness = 0.2;
    }

    float linearRoughness = roughness * roughness;
    vec3 diffuseColor = (1.0 - metallic) * baseColor.rgb;
    vec3 f0 = 0.04 * (1.0 - metallic) + baseColor.rgb * metallic;

    float attenuation = shadow(position, l);

    // specular BRDF
    float D = D_GGX(linearRoughness, NoH, h);
    float V = V_SmithGGXCorrelated(linearRoughness, NoV, NoL);
    vec3 F = F_Schlick(f0, LoH);
    vec3 Fr = (D * V) * F;

    // diffuse BRDF
    vec3 Fd = diffuseColor *
                 Fd_Burley(linearRoughness, NoV, NoL, LoH);

    color = Fd + Fr;
    color *= (intensity * attenuation * NoL) * vec3(0.98, 0.92, 0.89);

    // diffuse indirect
    vec3 indirectDiffuse =
            Irradiance_SphericalHarmonics(n) * Fd_Lambert();

    vec2 indirectHit = traceRay(position, r);
    vec3 indirectSpecular = vec3(0.65, 0.85, 1.0) + r.y * 0.72;
    if (indirectHit.y > 0.0) {
      if (indirectHit.y < 4.0) {
        vec3 indirectPosition = position + indirectHit.x * r;
        // Checkerboard floor
        float f = mod(floor(6.0 * indirectPosition.z) +
                          floor(6.0 * indirectPosition.x),
                      2.0);
        indirectSpecular = 0.4 + f * vec3(0.6);
      } else if (indirectHit.y < 16.0) {
        // Metallic objects
        indirectSpecular = vec3(0.3, 0.0, 0.0);
      }
    }

    // indirect contribution
    vec2 dfg = PrefilteredDFG_Karis(roughness, NoV);
    vec3 specularColor = f0 * dfg.x + dfg.y;
    vec3 ibl = diffuseColor *
        indirectDiffuse + indirectSpecular * specularColor;

    color += ibl * indirectIntensity;
  }

  return color;
}

//--------------------------------------------------------------------
// Setup and execution
//--------------------------------------------------------------------

mat3 setCamera(in vec3 origin, in vec3 target, float rotation) {
  vec3 forward = normalize(target - origin);
  vec3 orientation = vec3(sin(rotation), cos(rotation), 0.0);
  vec3 left = normalize(cross(forward, orientation));
  vec3 up = normalize(cross(left, forward));
  return mat3(left, up, forward);
}

void main(void) {
  // Normalized coordinates
  vec2 p = -1.0 + 2.1 * vUV.xy;
  // Aspect ratio
  p.x *= iResolution.x / iResolution.y;

  // Camera position and 'look at'
  vec3 origin = vec3(0.0, 0.8, 0.0);
  vec3 target = vec3(0.0);

  origin.x += 1.7 * cos(iTime * 0.2);
  origin.z += 1.7 * sin(iTime * 0.2);

  mat3 toWorld = setCamera(origin, target, 0.0);
  vec3 direction = toWorld * normalize(vec3(p.xy, 2.0));

  // Render scene
  float distance;
  vec3 color = render(origin, direction, distance);

  // Tone mapping
  color = Tonemap_ACES(color);

  // Exponential distance fog
  color = mix(color, 0.8 * vec3(0.7, 0.8, 1.0),
              1.0 - exp2(-0.011 * distance * distance));

  // Gamma compression
  color = OECF_sRGBFast(color);

  gl_FragColor = vec4(color, 1.0);
}
)ShaderCode";

public:
  ShaderMaterialPhysicallyBasedSdfScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas), _time{0.f}, _shaderMaterial{nullptr}
  {
    // Vertex shader
    Effect::ShadersStore()["customVertexShader"] = customVertexShader;

    // Fragment shader
    Effect::ShadersStore()["customFragmentShader"] = customFragmentShader;
  }

  ~ShaderMaterialPhysicallyBasedSdfScene() override = default;

  const char* getName() override
  {
    return "Shader Material Physically-based SDF Scene";
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

}; // end of class ShaderMaterialPhysicallyBasedSDFScene

BABYLON_REGISTER_SAMPLE("Shader Material", ShaderMaterialPhysicallyBasedSdfScene)

} // end of namespace Samples
} // end of namespace BABYLON