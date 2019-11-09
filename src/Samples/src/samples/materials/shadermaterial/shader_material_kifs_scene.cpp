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

class ShaderMaterialKifsScene : public IRenderableScene {

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
  // KIFS ( https://www.shadertoy.com/view/MdlSRM )
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
      "uniform vec2 iMouse;\n"
      "uniform vec2 iResolution;\n"
      "\n"
      "\n"
      "// Try KIFS...\n"
      "\n"
      "float stime, ctime;\n"
      "void ry(inout vec3 p, float a) {\n"
      "  float c, s;\n"
      "  vec3 q = p;\n"
      "  c = cos(a);\n"
      "  s = sin(a);\n"
      "  p.x = c * q.x + s * q.z;\n"
      "  p.z = -s * q.x + c * q.z;\n"
      "}\n"
      "void rx(inout vec3 p, float a) {\n"
      "  float c, s;\n"
      "  vec3 q = p;\n"
      "  c = cos(a);\n"
      "  s = sin(a);\n"
      "  p.y = c * q.y - s * q.z;\n"
      "  p.z = s * q.y + c * q.z;\n"
      "}\n"
      "\n"
      "void rz(inout vec3 p, float a) {\n"
      "  float c, s;\n"
      "  vec3 q = p;\n"
      "  c = cos(a);\n"
      "  s = sin(a);\n"
      "  p.x = c * q.x - s * q.y;\n"
      "  p.y = s * q.x + c * q.y;\n"
      "}\n"
      "float plane(vec3 p, float y) { return length(vec3(p.x, y, p.z) - p); }\n"
      "\n"
      "vec3 mat = vec3(0.0);\n"
      "bool bcolor = false;\n"
      "\n"
      "float menger_spone(in vec3 z0) {\n"
      "  vec4 z = vec4(z0, 1.0);\n"
      "  vec3 offset = vec3(0.785, 1.1, 0.46);\n"
      "  // vec3 offset=vec3(1.0);\n"
      "  float scale = 2.48;\n"
      "  for (int n = 0; n < 4; n++) {\n"
      "    z = abs(z);\n"
      "    if (z.x < z.y)\n"
      "      z.xy = z.yx;\n"
      "    if (z.x < z.z)\n"
      "      z.xz = z.zx;\n"
      "    if (z.y < z.z)\n"
      "      z.yz = z.zy;\n"
      "    z = z * scale;\n"
      "    z.xyz -= offset * (scale - 1.0);\n"
      "    if (bcolor && n == 2)\n"
      "      mat += vec3(0.5) + sin(z.xyz) * vec3(1.0, 0.24, 0.245);\n"
      "    // translate the middle cube to top right corner\n"
      "    if (z.z < -0.5 * offset.z * (scale - 1.0))\n"
      "      z.z += offset.z * (scale - 1.0);\n"
      "  }\n"
      "  return (length(max(abs(z.xyz) - vec3(1.0), 0.0)) - 0.05) / z.w;\n"
      "}\n"
      "\n"
      "vec3 f(vec3 p) {\n"
      "  ry(p, stime);\n"
      "  float d1 = plane(p, -1.0);\n"
      "  float d2 = menger_spone(p);\n"
      "  if (d1 < d2) {\n"
      "    return vec3(d1, 0.0, 0.0);\n"
      "  } else {\n"
      "    return vec3(d2, 1.0, 0.0);\n"
      "  }\n"
      "}\n"
      "\n"
      "float ao(vec3 p, vec3 n) {\n"
      "  float ao = 0.0, sca = 1.0;\n"
      "  for (float i = 0.0; i < 20.0; ++i) {\n"
      "    float hr = 0.05 + 0.015 * i * i;\n"
      "    ao += (hr - f(n * hr + p).x) * sca;\n"
      "    sca *= 0.75;\n"
      "  }\n"
      "  return 1.0 - clamp(ao, 0.0, 1.0);\n"
      "}\n"
      "\n"
      "float rand(vec2 t) {\n"
      "  return fract(sin(dot(t * 0.123, vec2(12.9898, 78.233))) *\n"
      "               43758.5453);\n"
      "}\n"
      "float softshadow(vec3 ro, vec3 rd, float k) {\n"
      "  float akuma = 1.0, h = 0.0;\n"
      "  float t = 0.01;\n"
      "  for (int i = 0; i < 50; ++i) {\n"
      "    h = f(ro + rd * t).x;\n"
      "    if (h < 0.001) {\n"
      "      akuma = 0.0;\n"
      "      break;\n"
      "    }\n"
      "    akuma = min(akuma, k * h / t);\n"
      "    t += abs(h);\n"
      "  }\n"
      "  return clamp(akuma * 0.8 + 0.2, 0.0, 1.0);\n"
      "}\n"
      "vec3 nor(vec3 p) {\n"
      "  vec3 e = vec3(0.0001, 0.0, 0.0);\n"
      "  return normalize(vec3(f(p + e.xyy).x - f(p - e.xyy).x,\n"
      "                        f(p + e.yxy).x - f(p - e.yxy).x,\n"
      "                        f(p + e.yyx).x - f(p - e.yyx).x));\n"
      "}\n"
      "\n"
      "vec3 intersect(in vec3 ro, in vec3 rd) {\n"
      "  float t = 0.0;\n"
      "  vec3 res = vec3(-1.0);\n"
      "  vec3 h = vec3(1.0);\n"
      "  for (int i = 0; i < 64; i++) {\n"
      "    if (h.x < 0.003 || t > 30.0) {\n"
      "    } else {\n"
      "      h = f(ro + rd * t);\n"
      "      res = vec3(t, h.yz);\n"
      "      t += abs(h.x);\n"
      "    }\n"
      "  }\n"
      "  if (t > 30.0)\n"
      "    res = vec3(-1.0);\n"
      "  return res;\n"
      "}\n"
      "\n"
      "void main(void) {\n"
      "  vec2 q = vUV.xy;\n"
      "  vec2 uv = -1.0 + 2.0 * q;\n"
      "  uv.x *= iResolution.x / iResolution.y;\n"
      "  // camera\n"
      "  stime = sin(iTime * 0.2);\n"
      "  ctime = cos(iTime * 0.2);\n"
      "\n"
      "  vec3 ta = vec3(.0, 0.0, 0.0);\n"
      "  // vec3 ro=vec3(3.0*stime,stime*2.0+1.0, 3.0*ctime);\n"
      "  vec3 ro = vec3(3.5 * ctime, 2.2 + 0.5 * ctime, 5.8 + 1.0 * stime) *\n"
      "                 0.5;\n"
      "\n"
      "  vec3 cf = normalize(ta - ro);\n"
      "  vec3 cs = normalize(cross(cf, vec3(0.0, 1.0, 0.0)));\n"
      "  vec3 cu = normalize(cross(cs, cf));\n"
      "  vec3 rd = normalize(uv.x * cs + uv.y * cu +\n"
      "                      2.8 * cf); // transform from view to world\n"
      "\n"
      "  vec3 sundir = normalize(vec3(-3.5, 7.0, 2.8));\n"
      "  vec3 sun = vec3(1.64, 1.27, 0.99);\n"
      "  vec3 skycolor = vec3(0.6, 1.5, 1.0);\n"
      "\n"
      "  vec3 bg = exp(uv.y - 1.0) * vec3(0.6, 0.9, 1.4) * 1.3;\n"
      "\n"
      "  float sc = clamp(dot(sundir, rd), 0.0, 1.0);\n"
      "  vec3 col = bg + vec3(1.0, 0.8, 0.4) * pow(sc, 50.0);\n"
      "\n"
      "  float t = 0.0;\n"
      "  vec3 p = ro;\n"
      "\n"
      "  vec3 res = intersect(ro, rd);\n"
      "  if (res.x > 0.0) {\n"
      "    p = ro + res.x * rd;\n"
      "    bcolor = true;\n"
      "    mat = vec3(0.0);\n"
      "    vec3 n = nor(p);\n"
      "    mat /= 6.0;\n"
      "    bcolor = false;\n"
      "    float occ = ao(p, n);\n"
      "    float shadow = softshadow(p, sundir, 10.0);\n"
      "\n"
      "    float dif = max(0.0, dot(n, sundir));\n"
      "    float sky = 0.6 + 0.4 * max(0.0, dot(n, vec3(0.0, 1.0, 0.0)));\n"
      "    float bac = max(0.3 + 0.7 *\n"
      "                    dot(vec3(-sundir.x, -1.0, -sundir.z), n), 0.0);\n"
      "    float spe =\n"
      "        max(0.0,\n"
      "            pow(clamp(dot(sundir, reflect(rd, n)), 0.0, 1.0), 10.0));\n"
      "\n"
      "    vec3 lin = 5.5 * sun * dif * shadow;\n"
      "    lin += 0.8 * bac * sun * occ;\n"
      "    lin += 0.6 * sky * skycolor * occ;\n"
      "    lin += 3.0 * spe * occ;\n"
      "\n"
      "    col = lin * (vec3(0.9, 0.8, 0.6) * (1.0 - res.y) + mat * res.y) *\n"
      "                 0.2;\n"
      "    col = mix(col, bg, 1.0 - exp(-0.02 * res.x * res.x));\n"
      "  }\n"
      "\n"
      "  // post\n"
      "  col = pow(clamp(col, 0.0, 1.0), vec3(0.45));\n"
      "  col = col * 0.6 + 0.4 * col * col * (3.0 - 2.0 * col); // contrast\n"
      "  col = mix(col, vec3(dot(col, vec3(0.33))), -0.5);      // satuation\n"
      "  col *=\n"
      "      0.5 +\n"
      "      // vigneting\n"
      "      0.5 * pow(16.0 * q.x * q.y * (1.0 - q.x) * (1.0 - q.y), 0.7);\n"
      "  gl_FragColor = vec4(col, 1.0);\n"
      "}\n";

public:
  ShaderMaterialKifsScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas), _time{0.f}, _shaderMaterial{nullptr}
  {
    // Vertex shader
    Effect::ShadersStore()["customVertexShader"] = customVertexShader;

    // Fragment shader
    Effect::ShadersStore()["customFragmentShader"] = customFragmentShader;
  }

  ~ShaderMaterialKifsScene() override = default;

  const char* getName() override
  {
    return "Shader Material KIFS Scene";
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
      = {"iTime", "worldViewProjection", "iAspectRatio", "iResolution", "iMouse"};
    _shaderMaterial = ShaderMaterial::New("boxShader", scene, "custom", shaderMaterialOptions);

    // box + sky = skybox !
    skybox->material = _shaderMaterial;

    // Animation
    scene->onAfterCameraRenderObservable.add([this](Camera*, EventState&) {
      const Vector2 resolution{static_cast<float>(_engine->getRenderWidth()),
                               static_cast<float>(_engine->getRenderHeight())};
      const Vector2 mouse{0.f, 0.f};
      const float aspectRatio = resolution.x / resolution.y;
      _shaderMaterial->setFloat("iTime", _time);
      _shaderMaterial->setFloat("iAspectRatio", aspectRatio);
      _shaderMaterial->setVector2("iResolution", resolution);
      _shaderMaterial->setVector2("iMouse", mouse);
      _time += 0.01f * getScene()->getAnimationRatio();
    });
  }

private:
  float _time;
  ShaderMaterialPtr _shaderMaterial;

}; // end of class ShaderMaterialKIFSScene

BABYLON_REGISTER_SAMPLE("Shader Materials", ShaderMaterialKifsScene)

} // end of namespace Samples
} // end of namespace BABYLON
