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

class ShaderMaterialPhysicallyBasedSdfScene : public IRenderableScene {

public:
  /** Vertex Shader **/
  static constexpr const char* customVertexShader
    = "#ifdef GL_ES\n"
      "precision highp float;\n"
      "#endif\n"
      "\n"
      "// Attributes\n"
      "attribute vec3 position;\n"
      "attribute vec2 uv;\n"
      "\n"
      "// Uniforms\n"
      "uniform mat4 worldViewProjection;\n"
      "\n"
      "// Varying\n"
      "varying vec2 vUV;\n"
      "\n"
      "void main(void) {\n"
      "    gl_Position = worldViewProjection * vec4(position, 1.0);\n"
      "    vUV = uv;\n"
      "}\n";

  /** Pixel (Fragment) Shader **/
  // Physically-based SDF ( https://www.shadertoy.com/view/XlKSDR )
  static constexpr const char* customFragmentShader
    = "#ifdef GL_ES\n"
      "precision highp float;\n"
      "#endif\n"
      "\n"
      "// Varying\n"
      "varying vec3 vPosition;\n"
      "varying vec3 vNormal;\n"
      "varying vec2 vUV;\n"
      "\n"
      "// Uniforms\n"
      "uniform mat4 worldViewProjection;\n"
      "uniform float iTime;\n"
      "uniform float iAspectRatio;\n"
      "uniform vec2 iResolution;\n"
      "#define saturate(x) clamp(x, 0.0, 1.0)\n"
      "#define PI 3.14159265359\n"
      "\n"
      "//--------------------------------------------------------------------\n"
      "// Distance field functions\n"
      "//--------------------------------------------------------------------\n"
      "\n"
      "float sdPlane(in vec3 p) { return p.y; }\n"
      "\n"
      "float sdSphere(in vec3 p, float s) { return length(p) - s; }\n"
      "\n"
      "float sdTorus(in vec3 p, in vec2 t) {\n"
      "  return length(vec2(length(p.xz) - t.x, p.y)) - t.y;\n"
      "}\n"
      "\n"
      "vec2 opUnion(vec2 d1, vec2 d2) { return d1.x < d2.x ? d1 : d2; }\n"
      "\n"
      "vec2 scene(in vec3 position) {\n"
      "  vec2 scene =\n"
      "      opUnion(vec2(sdPlane(position), 1.0),\n"
      "              vec2(sdSphere(position - vec3(0.0, 0.4, 0.0), 0.4),\n"
      "                   12.0));\n"
      "  return scene;\n"
      "}\n"
      "\n"
      "//--------------------------------------------------------------------\n"
      "// Ray casting\n"
      "//--------------------------------------------------------------------\n"
      "\n"
      "float shadow(in vec3 origin, in vec3 direction) {\n"
      "  float hit = 1.0;\n"
      "  float t = 0.02;\n"
      "\n"
      "  for (int i = 0; i < 1000; i++) {\n"
      "    float h = scene(origin + direction * t).x;\n"
      "    if (h < 0.001)\n"
      "      return 0.0;\n"
      "    t += h;\n"
      "    hit = min(hit, 10.0 * h / t);\n"
      "    if (t >= 2.5)\n"
      "      break;\n"
      "  }\n"
      "\n"
      "  return clamp(hit, 0.0, 1.0);\n"
      "}\n"
      "\n"
      "vec2 traceRay(in vec3 origin, in vec3 direction) {\n"
      "  float material = -1.0;\n"
      "\n"
      "  float t = 0.02;\n"
      "\n"
      "  for (int i = 0; i < 1000; i++) {\n"
      "    vec2 hit = scene(origin + direction * t);\n"
      "    if (hit.x < 0.002 || t > 20.0)\n"
      "      break;\n"
      "    t += hit.x;\n"
      "    material = hit.y;\n"
      "  }\n"
      "\n"
      "  if (t > 20.0) {\n"
      "    material = -1.0;\n"
      "  }\n"
      "\n"
      "  return vec2(t, material);\n"
      "}\n"
      "\n"
      "vec3 normal(in vec3 position) {\n"
      "  vec3 epsilon = vec3(0.001, 0.0, 0.0);\n"
      "  vec3 n =\n"
      "      vec3(scene(position + epsilon.xyy).x -\n"
      "           scene(position - epsilon.xyy).x,\n"
      "           scene(position + epsilon.yxy).x -\n"
      "           scene(position - epsilon.yxy).x,\n"
      "           scene(position + epsilon.yyx).x -\n"
      "           scene(position - epsilon.yyx).x);\n"
      "  return normalize(n);\n"
      "}\n"
      "\n"
      "//--------------------------------------------------------------------\n"
      "// BRDF\n"
      "//--------------------------------------------------------------------\n"
      "\n"
      "float pow5(float x) {\n"
      "  float x2 = x * x;\n"
      "  return x2 * x2 * x;\n"
      "}\n"
      "\n"
      "float D_GGX(float linearRoughness, float NoH, const vec3 h) {\n"
      "  // Walter et al. 2007,"
      "  // 'Microfacet Models for Refraction through Rough Surfaces'\n"
      "  float oneMinusNoHSquared = 1.0 - NoH * NoH;\n"
      "  float a = NoH * linearRoughness;\n"
      "  float k = linearRoughness / (oneMinusNoHSquared + a * a);\n"
      "  float d = k * k * (1.0 / PI);\n"
      "  return d;\n"
      "}\n"
      "\n"
      "float V_SmithGGXCorrelated(float linearRoughness,\n"
      "                           float NoV, float NoL) {\n"
      "  // Heitz 2014, 'Understanding the Masking-Shadowing Function in\n"
      "  // Microfacet-Based BRDFs'\n"
      "  float a2 = linearRoughness * linearRoughness;\n"
      "  float GGXV = NoL * sqrt((NoV - a2 * NoV) * NoV + a2);\n"
      "  float GGXL = NoV * sqrt((NoL - a2 * NoL) * NoL + a2);\n"
      "  return 0.5 / (GGXV + GGXL);\n"
      "}\n"
      "\n"
      "vec3 F_Schlick(const vec3 f0, float VoH) {\n"
      "  // Schlick 1994,\n"
      "  // 'An Inexpensive BRDF Model for Physically-Based Rendering'\n"
      "  return f0 + (vec3(1.0) - f0) * pow5(1.0 - VoH);\n"
      "}\n"
      "\n"
      "float F_Schlick(float f0, float f90, float VoH) {\n"
      "  return f0 + (f90 - f0) * pow5(1.0 - VoH);\n"
      "}\n"
      "\n"
      "float Fd_Burley(float linearRoughness, float NoV,\n"
      "                float NoL, float LoH) {\n"
      "  // Burley 2012, 'Physically-Based Shading at Disney'\n"
      "  float f90 = 0.5 + 2.0 * linearRoughness * LoH * LoH;\n"
      "  float lightScatter = F_Schlick(1.0, f90, NoL);\n"
      "  float viewScatter = F_Schlick(1.0, f90, NoV);\n"
      "  return lightScatter * viewScatter * (1.0 / PI);\n"
      "}\n"
      "\n"
      "float Fd_Lambert() { return 1.0 / PI; }\n"
      "\n"
      "//--------------------------------------------------------------------\n"
      "// Indirect lighting\n"
      "//--------------------------------------------------------------------\n"
      "\n"
      "vec3 Irradiance_SphericalHarmonics(const vec3 n) {\n"
      "  // Irradiance from 'Ditch River' IBL\n"
      "  // (http://www.hdrlabs.com/sibl/archive.html)\n"
      "  return max(\n"
      "      vec3(0.754554516862612, 0.748542953903366, 0.790921515418539) +\n"
      "          vec3(-0.083856548007422,\n"
      "               0.092533500963210,\n"
      "               0.322764661032516) *\n"
      "              (n.y) +\n"
      "          vec3(0.308152705331738,\n"
      "               0.366796330467391,\n"
      "               0.466698181299906) *\n"
      "              (n.z) +\n"
      "          vec3(-0.188884931542396,\n"
      "               -0.277402551592231,\n"
      "               -0.377844212327557) *\n"
      "              (n.x),\n"
      "      0.0);\n"
      "}\n"
      "\n"
      "vec2 PrefilteredDFG_Karis(float roughness, float NoV) {\n"
      "  // Karis 2014, 'Physically Based Material on Mobile'\n"
      "  const vec4 c0 = vec4(-1.0, -0.0275, -0.572, 0.022);\n"
      "  const vec4 c1 = vec4(1.0, 0.0425, 1.040, -0.040);\n"
      "\n"
      "  vec4 r = roughness * c0 + c1;\n"
      "  float a004 = min(r.x * r.x, exp2(-9.28 * NoV)) * r.x + r.y;\n"
      "\n"
      "  return vec2(-1.04, 1.04) * a004 + r.zw;\n"
      "}\n"
      "\n"
      "//--------------------------------------------------------------------\n"
      "// Tone mapping and transfer functions\n"
      "//--------------------------------------------------------------------\n"
      "\n"
      "vec3 Tonemap_ACES(const vec3 x) {\n"
      "  // Narkowicz 2015, 'ACES Filmic Tone Mapping Curve'\n"
      "  const float a = 2.51;\n"
      "  const float b = 0.03;\n"
      "  const float c = 2.43;\n"
      "  const float d = 0.59;\n"
      "  const float e = 0.14;\n"
      "  return (x * (a * x + b)) / (x * (c * x + d) + e);\n"
      "}\n"
      "\n"
      "vec3 OECF_sRGBFast(const vec3 linear) {\n"
      "  return pow(linear, vec3(1.0 / 2.2));\n"
      "}\n"
      "\n"
      "//--------------------------------------------------------------------\n"
      "// Rendering\n"
      "//--------------------------------------------------------------------\n"
      "\n"
      "vec3 render(in vec3 origin, in vec3 direction, out float distance) {\n"
      "  // Sky gradient\n"
      "  vec3 color = vec3(0.65, 0.85, 1.0) + direction.y * 0.72;\n"
      "\n"
      "  // (distance, material)\n"
      "  vec2 hit = traceRay(origin, direction);\n"
      "  distance = hit.x;\n"
      "  float material = hit.y;\n"
      "\n"
      "  // We've hit something in the scene\n"
      "  if (material > 0.0) {\n"
      "    vec3 position = origin + distance * direction;\n"
      "\n"
      "    vec3 v = normalize(-direction);\n"
      "    vec3 n = normal(position);\n"
      "    vec3 l = normalize(vec3(0.6, 0.7, -0.7));\n"
      "    vec3 h = normalize(v + l);\n"
      "    vec3 r = normalize(reflect(direction, n));\n"
      "\n"
      "    float NoV = abs(dot(n, v)) + 1e-5;\n"
      "    float NoL = saturate(dot(n, l));\n"
      "    float NoH = saturate(dot(n, h));\n"
      "    float LoH = saturate(dot(l, h));\n"
      "\n"
      "    vec3 baseColor = vec3(0.0);\n"
      "    float roughness = 0.0;\n"
      "    float metallic = 0.0;\n"
      "\n"
      "    float intensity = 2.0;\n"
      "    float indirectIntensity = 0.64;\n"
      "\n"
      "    if (material < 4.0) {\n"
      "      // Checkerboard floor\n"
      "      float f = mod(floor(6.0 * position.z) + floor(6.0 * position.x),\n"
      "                    2.0);\n"
      "      baseColor = 0.4 + f * vec3(0.6);\n"
      "      roughness = 0.1;\n"
      "    } else if (material < 16.0) {\n"
      "      // Metallic objects\n"
      "      baseColor = vec3(0.3, 0.0, 0.0);\n"
      "      roughness = 0.2;\n"
      "    }\n"
      "\n"
      "    float linearRoughness = roughness * roughness;\n"
      "    vec3 diffuseColor = (1.0 - metallic) * baseColor.rgb;\n"
      "    vec3 f0 = 0.04 * (1.0 - metallic) + baseColor.rgb * metallic;\n"
      "\n"
      "    float attenuation = shadow(position, l);\n"
      "\n"
      "    // specular BRDF\n"
      "    float D = D_GGX(linearRoughness, NoH, h);\n"
      "    float V = V_SmithGGXCorrelated(linearRoughness, NoV, NoL);\n"
      "    vec3 F = F_Schlick(f0, LoH);\n"
      "    vec3 Fr = (D * V) * F;\n"
      "\n"
      "    // diffuse BRDF\n"
      "    vec3 Fd = diffuseColor *\n"
      "                 Fd_Burley(linearRoughness, NoV, NoL, LoH);\n"
      "\n"
      "    color = Fd + Fr;\n"
      "    color *= (intensity * attenuation * NoL) * vec3(0.98, 0.92, 0.89);\n"
      "\n"
      "    // diffuse indirect\n"
      "    vec3 indirectDiffuse =\n"
      "            Irradiance_SphericalHarmonics(n) * Fd_Lambert();\n"
      "\n"
      "    vec2 indirectHit = traceRay(position, r);\n"
      "    vec3 indirectSpecular = vec3(0.65, 0.85, 1.0) + r.y * 0.72;\n"
      "    if (indirectHit.y > 0.0) {\n"
      "      if (indirectHit.y < 4.0) {\n"
      "        vec3 indirectPosition = position + indirectHit.x * r;\n"
      "        // Checkerboard floor\n"
      "        float f = mod(floor(6.0 * indirectPosition.z) +\n"
      "                          floor(6.0 * indirectPosition.x),\n"
      "                      2.0);\n"
      "        indirectSpecular = 0.4 + f * vec3(0.6);\n"
      "      } else if (indirectHit.y < 16.0) {\n"
      "        // Metallic objects\n"
      "        indirectSpecular = vec3(0.3, 0.0, 0.0);\n"
      "      }\n"
      "    }\n"
      "\n"
      "    // indirect contribution\n"
      "    vec2 dfg = PrefilteredDFG_Karis(roughness, NoV);\n"
      "    vec3 specularColor = f0 * dfg.x + dfg.y;\n"
      "    vec3 ibl = diffuseColor *\n"
      "        indirectDiffuse + indirectSpecular * specularColor;\n"
      "\n"
      "    color += ibl * indirectIntensity;\n"
      "  }\n"
      "\n"
      "  return color;\n"
      "}\n"
      "\n"
      "//--------------------------------------------------------------------\n"
      "// Setup and execution\n"
      "//--------------------------------------------------------------------\n"
      "\n"
      "mat3 setCamera(in vec3 origin, in vec3 target, float rotation) {\n"
      "  vec3 forward = normalize(target - origin);\n"
      "  vec3 orientation = vec3(sin(rotation), cos(rotation), 0.0);\n"
      "  vec3 left = normalize(cross(forward, orientation));\n"
      "  vec3 up = normalize(cross(left, forward));\n"
      "  return mat3(left, up, forward);\n"
      "}\n"
      "\n"
      "void main(void) {\n"
      "  // Normalized coordinates\n"
      "  vec2 p = -1.0 + 2.1 * vUV.xy;\n"
      "  // Aspect ratio\n"
      "  p.x *= iResolution.x / iResolution.y;\n"
      "\n"
      "  // Camera position and 'look at'\n"
      "  vec3 origin = vec3(0.0, 0.8, 0.0);\n"
      "  vec3 target = vec3(0.0);\n"
      "\n"
      "  origin.x += 1.7 * cos(iTime * 0.2);\n"
      "  origin.z += 1.7 * sin(iTime * 0.2);\n"
      "\n"
      "  mat3 toWorld = setCamera(origin, target, 0.0);\n"
      "  vec3 direction = toWorld * normalize(vec3(p.xy, 2.0));\n"
      "\n"
      "  // Render scene\n"
      "  float distance;\n"
      "  vec3 color = render(origin, direction, distance);\n"
      "\n"
      "  // Tone mapping\n"
      "  color = Tonemap_ACES(color);\n"
      "\n"
      "  // Exponential distance fog\n"
      "  color = mix(color, 0.8 * vec3(0.7, 0.8, 1.0),\n"
      "              1.0 - exp2(-0.011 * distance * distance));\n"
      "\n"
      "  // Gamma compression\n"
      "  color = OECF_sRGBFast(color);\n"
      "\n"
      "  gl_FragColor = vec4(color, 1.0);\n"
      "}\n";

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

BABYLON_REGISTER_SAMPLE("Shader Materials", ShaderMaterialPhysicallyBasedSdfScene)

} // end of namespace Samples
} // end of namespace BABYLON
