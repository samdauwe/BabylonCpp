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

class ShaderMaterialPbrTestScene : public IRenderableScene {

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
  // PBR_Test ( https://www.shadertoy.com/view/MlB3DV )
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
      "/*\n"
      "* References :\n"
      "*\n"
      "* "
      "http://renderwonk.com/publications/s2010-shading-course/hoffman/"
      "s2010_physically_based_shading_hoffman_b_notes.pdf\n"
      "*\n"
      "* "
      "http://www.frostbite.com/wp-content/uploads/2014/11/"
      "course_notes_moving_frostbite_to_pbr.pdf\n"
      "*\n"
      "* http://graphicrants.blogspot.fr/2013/08/specular-brdf-reference.html\n"
      "*\n"
      "* "
      "http://www.filmicworlds.com/2014/04/21/"
      "optimizing-ggx-shaders-with-dotlh/\n"
      "*\n"
      "* "
      "http://blog.selfshadow.com/publications/s2013-shading-course/"
      "#course_content\n"
      "*\n"
      "* Ray marching code from iq\n"
      "*/\n"
      "\n"
      "#define NB_LIGHTS 3\n"
      "\n"
      "// Metals values in linear space\n"
      "#define GOLD vec3(1.0, 0.71, 0.29)\n"
      "#define COPPER vec3(0.95, 0.64, 0.54)\n"
      "#define IRON vec3(0.56, 0.57, 0.58)\n"
      "#define ALUMINIUM vec3(0.91, 0.92, 0.92)\n"
      "#define SILVER vec3(0.95, 0.93, 0.88)\n"
      "\n"
      "float fPlane(vec3 p, vec4 n) {\n"
      "  // n must be normalized\n"
      "  return dot(p, n.xyz) + n.w;\n"
      "}\n"
      "\n"
      "float fSphere(vec3 p, float s) { return length(p) - s; }\n"
      "float opS(float d1, float d2) { return max(-d2, d1); }\n"
      "\n"
      "vec2 opU(vec2 d1, vec2 d2) { return (d1.x < d2.x) ? d1 : d2; }\n"
      "\n"
      "vec3 opRep(vec3 p, vec3 c) { return mod(p, c) - 0.5 * c; }\n"
      "// ---- Scene definition\n"
      "vec2 fScene(vec3 p) {\n"
      "\n"
      "  vec3 pSphere = p /*opRep(p, vec3( 2.0, 0.0, 2.0))*/;\n"
      "\n"
      "  vec2 sphere0 = vec2(fSphere(p, 1.0), 0.5);\n"
      "  vec2 sphere1 = vec2(fSphere(p + vec3(2.1, 0.0, 2.0), 1.0), 2.5);\n"
      "  vec2 sphere2 = vec2(fSphere(p + vec3(-2.1, 0.0, 2.0), 1.0), 3.5);\n"
      "  vec2 sphere3 = vec2(fSphere(p + vec3(2.1, 0.0, -2.0), 1.0), 4.5);\n"
      "  vec2 sphere4 = vec2(fSphere(p + vec3(-2.1, 0.0, -2.0), 1.0), 5.5);\n"
      "\n"
      "  vec2 plane = vec2(fPlane(p, vec4(0, 1, 0, 1.0)), 1.5);\n"
      "\n"
      "  return opU(opU(opU(opU(opU(plane, sphere0), sphere1), sphere2),\n"
      "             sphere3), sphere4);\n"
      "}\n"
      "// -----\n"
      "\n"
      "vec2 castRay(in vec3 ro, in vec3 rd) {\n"
      "  float tmin = 1.0;\n"
      "  float tmax = 100.0;\n"
      "\n"
      "  float precis = 0.00001;\n"
      "  float t = tmin;\n"
      "  float m = -1.0;\n"
      "  for (int i = 0; i < 50; i++) {\n"
      "    vec2 res = fScene(ro + rd * t);\n"
      "    if (res.x < precis || t > tmax)\n"
      "      break;\n"
      "    t += res.x;\n"
      "    m = res.y;\n"
      "  }\n"
      "\n"
      "  if (t > tmax)\n"
      "    m = -1.0;\n"
      "  return vec2(t, m);\n"
      "}\n"
      "\n"
      "float softshadow(in vec3 ro, in vec3 rd,\n"
      "                 in float mint, in float tmax) {\n"
      "  float res = 1.0;\n"
      "  float t = mint;\n"
      "  for (int i = 0; i < 16; i++) {\n"
      "    float h = fScene(ro + rd * t).x;\n"
      "    res = min(res, 8.0 * h / t);\n"
      "    t += clamp(h, 0.02, 0.10);\n"
      "    if (h < 0.001 || t > tmax)\n"
      "      break;\n"
      "  }\n"
      "  return clamp(res, 0.0, 1.0);\n"
      "}\n"
      "\n"
      "vec3 calcNormal(in vec3 pos) {\n"
      "  vec3 eps = vec3(0.001, 0.0, 0.0);\n"
      "  vec3 nor = vec3(fScene(pos + eps.xyy).x - fScene(pos - eps.xyy).x,\n"
      "                  fScene(pos + eps.yxy).x - fScene(pos - eps.yxy).x,\n"
      "                  fScene(pos + eps.yyx).x - fScene(pos - eps.yyx).x);\n"
      "  return normalize(nor);\n"
      "}\n"
      "\n"
      "struct Light {\n"
      "  vec3 pos;\n"
      "  vec3 color;\n"
      "};\n"
      "\n"
      "Light lights[NB_LIGHTS];\n"
      "\n"
      "float G1V(float dotNV, float k) {\n"
      "     return 1.0 / (dotNV * (1.0 - k) + k);\n"
      "}\n"
      "\n"
      "vec3 computePBRLighting(in Light light, in vec3 position,\n"
      "                        in vec3 N, in vec3 V,\n"
      "                        in vec3 albedo, in float roughness,\n"
      "                        in vec3 F0) {\n"
      "\n"
      "  float alpha = roughness * roughness;\n"
      "  vec3 L = normalize(light.pos.xyz - position);\n"
      "  vec3 H = normalize(V + L);\n"
      "\n"
      "  float dotNL = clamp(dot(N, L), 0.0, 1.0);\n"
      "  float dotNV = clamp(dot(N, V), 0.0, 1.0);\n"
      "  float dotNH = clamp(dot(N, H), 0.0, 1.0);\n"
      "  float dotLH = clamp(dot(L, H), 0.0, 1.0);\n"
      "\n"
      "  float D, vis;\n"
      "  vec3 F;\n"
      "\n"
      "  // NDF : GGX\n"
      "  float alphaSqr = alpha * alpha;\n"
      "  float pi = 3.1415926535;\n"
      "  float denom = dotNH * dotNH * (alphaSqr - 1.0) + 1.0;\n"
      "  D = alphaSqr / (pi * denom * denom);\n"
      "\n"
      "  // Fresnel (Schlick)\n"
      "  float dotLH5 = pow(1.0 - dotLH, 5.0);\n"
      "  F = F0 + (1.0 - F0) * (dotLH5);\n"
      "\n"
      "  // Visibility term (G) : Smith with Schlick's approximation\n"
      "  float k = alpha / 2.0;\n"
      "  vis = G1V(dotNL, k) * G1V(dotNV, k);\n"
      "\n"
      "  vec3 specular = /*dotNL **/ D * F * vis;\n"
      "\n"
      "  vec3 ambient = vec3(.01);\n"
      "\n"
      "  float invPi = 0.31830988618;\n"
      "  vec3 diffuse = (albedo * invPi);\n"
      "\n"
      "  return ambient + (diffuse + specular) * light.color.xyz * dotNL;\n"
      "}\n"
      "\n"
      "vec3 addPBR(in vec3 position, in vec3 N, in vec3 V, in vec3 baseColor,\n"
      "            in float metalMask, in float smoothness,\n"
      "            in float reflectance) {\n"
      "  vec3 color = vec3(0.0);\n"
      "\n"
      "  float roughness = 1.0 - smoothness * smoothness;\n"
      "  vec3 F0 = 0.16 * reflectance * reflectance * (1.0 - metalMask) +\n"
      "            baseColor * metalMask;\n"
      "  vec3 albedo = baseColor;\n"
      "\n"
      "  float s = 0.0;\n"
      "\n"
      "  for (int i = 0; i < NB_LIGHTS; ++i) {\n"
      "    vec3 col =\n"
      "        computePBRLighting(lights[i], position, N, V,\n"
      "                           albedo, roughness, F0);\n"
      "    color += col;\n"
      "    s += softshadow(position, normalize(lights[i].pos.xyz - position),\n"
      "                    0.02, 2.5);\n"
      "  }\n"
      "\n"
      "  return color * s;\n"
      "}\n"
      "\n"
      "vec3 render(in vec3 ro, in vec3 rd) {\n"
      "  vec3 col = vec3(0.8, 0.9, 1.0) * 8.0; // Sky color\n"
      "\n"
      "  vec2 res = castRay(ro, rd);\n"
      "  float t = res.x;\n"
      "  float m = res.y;\n"
      "  vec3 p = ro + t * rd;\n"
      "\n"
      "  if (m > -0.5) { // Intersection found\n"
      "    if (m < 1.0) {\n"
      "      // float f = mod( floor( 5.0*p.z ) + floor( 5.0*p.x ), 2.0 );\n"
      "      vec3 sur = vec3(1.0, 1.0, 1.0) *\n"
      "                 smoothstep(-1.0, -0.6, sin(16.0 * p.x));\n"
      "      col = addPBR(p, calcNormal(p), -rd, GOLD * sur, sur.x,\n"
      "                   0.3 + 0.6 * sur.x, 0.5);\n"
      "    } else if (m < 2.0) {\n"
      "      float f = mod(floor(5.0 * p.z) + floor(5.0 * p.x), 2.0);\n"
      "      col = addPBR(p, calcNormal(p), -rd, vec3(0.5),\n"
      "                   0.0, 0.3 + 0.6 * f, 0.5);\n"
      "    } else if (m < 3.0) {\n"
      "      vec3 sur = vec3(1.0, 1.0, 1.0) * smoothstep(-1.0, -0.4,\n"
      "                                                  sin(18.0 * p.x));\n"
      "      col = addPBR(p, calcNormal(p), -rd, COPPER * sur, sur.x,\n"
      "                   0.3 + 0.6 * sur.x, 0.5);\n"
      "    } else if (m < 4.0) {\n"
      "      vec3 sur = vec3(1.0, 1.0, 1.0) *\n"
      "                 smoothstep(-1.0, -0.0995, sin(106.0 * p.x)) *\n"
      "                 smoothstep(-1.0, -0.9, sin(47.0 * p.z));\n"
      "      col = addPBR(p, calcNormal(p), -rd, vec3(0.2), 1.0 - sur.x,\n"
      "                   0.9 * sur.x, 0.5);\n"
      "    } else if (m < 5.0) {\n"
      "      vec3 sur = vec3(1.0) * smoothstep(-1.0, -0.765,\n"
      "                                        sin(24.0 * p.x)) *\n"
      "                 smoothstep(-1.0, -0.4, sin(70.9 * p.z));\n"
      "      col = addPBR(p, calcNormal(p), -rd, GOLD * (1.0 - sur), sur.x,\n"
      "                   0.3 + 0.6 * sur.x, 0.5);\n"
      "    } else if (m < 6.0) {\n"
      "      vec3 sur = vec3(1.0, 1.0, 1.0) * smoothstep(-1.0, -0.4,\n"
      "                                                  sin(18.0 * p.x));\n"
      "      col = addPBR(p, calcNormal(p), -rd, ALUMINIUM * sur, sur.x,\n"
      "                   0.3 + 0.6 * sur.x, 0.5);\n"
      "    }\n"
      "  }\n"
      "\n"
      "  return col;\n"
      "}\n"
      "\n"
      "mat3 setCamera(in vec3 ro, in vec3 ta, float cr) {\n"
      "  vec3 cw = normalize(ta - ro);\n"
      "  vec3 cp = vec3(sin(cr), cos(cr), 0.0);\n"
      "  vec3 cu = normalize(cross(cw, cp));\n"
      "  vec3 cv = normalize(cross(cu, cw));\n"
      "  return mat3(cu, cv, cw);\n"
      "}\n"
      "\n"
      "vec4 hejlToneMapping(in vec4 color) {\n"
      "  vec4 x = max(vec4(0.0), color - vec4(0.004));\n"
      "  return (x * ((6.2 * x) + vec4(0.5))) /\n"
      "         max(x * ((6.2 * x) + vec4(1.7)) + vec4(0.06), vec4(1e-8));\n"
      "}\n"
      "\n"
      "void main(void) {\n"
      "\n"
      "  float time = 0.25 * iTime;\n"
      "\n"
      "  lights[0] = Light(vec3(0.0, 5.0, .0), vec3(1.0));\n"
      "  lights[1] = Light(vec3(12.0 * sin(iTime), 8.0, 12.0 * cos(iTime)),\n"
      "                    vec3(1.0));\n"
      "  lights[2] =\n"
      "      Light(vec3(-12.0 * cos(-iTime), 8.0, 12.0 * sin(-iTime)),\n"
      "            vec3(.05));\n"
      "\n"
      "  vec2 q = vUV.xy;\n"
      "  vec2 p = -1.0 + 2.0 * q;\n"
      "  p.x *= iResolution.x / iResolution.y;\n"
      "\n"
      "  // camera\n"
      "  vec3 ro = vec3(7.0 * sin(time), 7.0, -7.0 * cos(time));\n"
      "  vec3 ta = vec3(0.0);\n"
      "\n"
      "  // camera-to-world transformation\n"
      "  mat3 ca = setCamera(ro, ta, 0.0);\n"
      "\n"
      "  // ray direction\n"
      "  vec3 rd = ca * normalize(vec3(p.xy, 2.5));\n"
      "\n"
      "  // render\n"
      "  vec3 col = render(ro, rd);\n"
      "\n"
      "#if 0\n"
      "        col = pow( col, vec3(0.4545) );\n"
      "        fragColor=vec4( col, 1.0 );\n"
      "#else\n"
      "  float exposure = 0.032 + 0.023 * sin(time - 3.14);\n"
      "  gl_FragColor = hejlToneMapping(vec4(col, 1.0) * exposure);\n"
      "#endif\n"
      "}\n";

public:
  ShaderMaterialPbrTestScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas), _time{0.f}, _shaderMaterial{nullptr}
  {
    // Vertex shader
    Effect::ShadersStore()["customVertexShader"] = customVertexShader;

    // Fragment shader
    Effect::ShadersStore()["customFragmentShader"] = customFragmentShader;
  }

  ~ShaderMaterialPbrTestScene() override = default;

  const char* getName() override
  {
    return "Shader Material PBR Test Scene";
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

}; // end of class ShaderMaterialPBRTestScene

BABYLON_REGISTER_SAMPLE("Shader Materials", ShaderMaterialPbrTestScene)

} // end of namespace Samples
} // end of namespace BABYLON
