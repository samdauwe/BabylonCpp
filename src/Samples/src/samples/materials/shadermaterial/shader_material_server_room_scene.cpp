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

class ShaderMaterialServerRoomScene : public IRenderableScene {

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
  // Server Room ( https://www.shadertoy.com/view/MdySzc )
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
      "const float tmax = 20.0;\n"
      "\n"
      "float hash(float n) { return fract(sin(n) * 43758.5453); }\n"
      "\n"
      "vec3 hash(vec3 p) {\n"
      "\n"
      "  float n = sin(dot(p, vec3(7, 157, 113)));\n"
      "  return fract(vec3(2097152, 262144, 32768) * n);\n"
      "}\n"
      "\n"
      "float noise(float g) {\n"
      "  float p = floor(g);\n"
      "  float f = fract(g);\n"
      "\n"
      "  return mix(hash(p), hash(p + 1.0), f);\n"
      "}\n"
      "\n"
      "float voronoi(vec3 x) {\n"
      "  vec3 p = floor(x);\n"
      "  vec3 f = fract(x);\n"
      "\n"
      "  vec2 res = vec2(8.0);\n"
      "\n"
      "  for (int i = -1; i <= 1; i++)\n"
      "    for (int j = -1; j <= 1; j++)\n"
      "      for (int k = -1; k <= 1; k++) {\n"
      "        vec3 g = vec3(float(i), float(j), float(k));\n"
      "        vec3 r = g + hash(p + g) - f;\n"
      "\n"
      "        float d = max(abs(r.x), max(abs(r.y), abs(r.z)));\n"
      "\n"
      "        if (d < res.x) {\n"
      "          res.y = res.x;\n"
      "          res.x = d;\n"
      "        } else if (d < res.y) {\n"
      "          res.y = d;\n"
      "        }\n"
      "      }\n"
      "\n"
      "  return res.y - res.x;\n"
      "}\n"
      "\n"
      "vec2 path(float z) {\n"
      "  return vec2(cos(z / 8.0) * sin(z / 12.0) * 12.0, 0.0);\n"
      "}\n"
      "\n"
      "float map(vec3 p) {\n"
      "  vec4 q = vec4(p, 1.0);\n"
      "  q.x += 1.0;\n"
      "\n"
      "  for (int i = 0; i < 6; i++) {\n"
      "    q.xyz = -1.0 + 2.0 * fract(0.5 + 0.5 * q.xyz);\n"
      "    q = 1.2 * q / max(dot(q.xyz, q.xyz), 0.1);\n"
      "  }\n"
      "\n"
      "  vec2 tun = abs(p.xy - path(p.z)) * vec2(0.6, 0.5);\n"
      "\n"
      "  return min(0.25 * abs(q.y) / q.w, 1.0 - max(tun.x, tun.y));\n"
      "}\n"
      "\n"
      "float march(vec3 ro, vec3 rd, float mx) {\n"
      "  float t = 0.0;\n"
      "\n"
      "  for (int i = 0; i < 200; i++) {\n"
      "    float d = map(ro + rd * t);\n"
      "    if (d < 0.001 || t >= mx)\n"
      "      break;\n"
      "    t += d * 0.75;\n"
      "  }\n"
      "\n"
      "  return t;\n"
      "}\n"
      "\n"
      "vec3 normal(vec3 p) {\n"
      "  vec2 h = vec2(0.001, 0.0);\n"
      "  vec3 n =\n"
      "      vec3(map(p + h.xyy) - map(p - h.xyy),\n"
      "           map(p + h.yxy) - map(p - h.yxy),\n"
      "           map(p + h.yyx) - map(p - h.yyx));\n"
      "  return normalize(n);\n"
      "}\n"
      "\n"
      "float ao(vec3 p, vec3 n) {\n"
      "  float o = 0.0, s = 0.005;\n"
      "  for (int i = 0; i < 15; i++) {\n"
      "    float d = map(p + n * s);\n"
      "    o += (s - d);\n"
      "    s += s / float(i + 1);\n"
      "  }\n"
      "\n"
      "  return 1.0 - clamp(o, 0.0, 1.0);\n"
      "}\n"
      "\n"
      "vec3 material(vec3 p) {\n"
      "  float v = 0.0;\n"
      "  float a = 0.7, f = 1.0;\n"
      "\n"
      "  for (int i = 0; i < 4; i++) {\n"
      "    float v1 = voronoi(p * f + 5.0);\n"
      "    float v2 = 0.0;\n"
      "\n"
      "    if (i > 0) {\n"
      "      v2 = voronoi(p * f * 0.1 + 50.0 + 0.15 * iTime);\n"
      "\n"
      "      float va = 0.0, vb = 0.0;\n"
      "      va = 1.0 - smoothstep(0.0, 0.1, v1);\n"
      "      vb = 1.0 - smoothstep(0.0, 0.08, v2);\n"
      "      v += a * pow(va * (0.5 + vb), 4.0);\n"
      "    }\n"
      "\n"
      "    v1 = 1.0 - smoothstep(0.0, 0.3, v1);\n"
      "    v2 = a * noise(v1 * 5.5 + 0.1);\n"
      "\n"
      "    v += v2;\n"
      "\n"
      "    f *= 3.0;\n"
      "    a *= 0.5;\n"
      "  }\n"
      "\n"
      "  return vec3(pow(v, 6.0), pow(v, 4.0), pow(v, 2.0)) * 2.0;\n"
      "}\n"
      "\n"
      "mat3 camera(vec3 eye, vec3 lat) {\n"
      "  vec3 ww = normalize(lat - eye);\n"
      "  vec3 uu = normalize(cross(vec3(0, 1, 0), ww));\n"
      "  vec3 vv = normalize(cross(ww, uu));\n"
      "\n"
      "  return mat3(uu, vv, ww);\n"
      "}\n"
      "\n"
      "void main(void) {\n"
      "  vec2 uv = (-1.0 + 2.0 * vUV.xy);\n"
      "  uv.x *= iResolution.x / iResolution.y;\n"
      "\n"
      "  vec3 col = vec3(0);\n"
      "\n"
      "  vec3 ro = vec3(0.63 * cos(iTime * 0.1), 0.67, iTime * 0.5);\n"
      "  vec3 la = ro + vec3(0, 0, 0.3);\n"
      "\n"
      "  ro.xy += path(ro.z);\n"
      "  la.xy += path(la.z);\n"
      "  vec3 rd = normalize(camera(ro, la) * vec3(uv, 1.97));\n"
      "\n"
      "  float i = march(ro, rd, tmax);\n"
      "  if (i < tmax) {\n"
      "    vec3 pos = ro + rd * i;\n"
      "    vec3 nor = normal(pos);\n"
      "    vec3 ref = normalize(reflect(rd, nor));\n"
      "\n"
      "    float occ = ao(pos, nor);\n"
      "\n"
      "    col = material(pos) * occ;\n"
      "    col += 2.0 * pow(clamp(dot(-rd, ref), 0.0, 1.0), 16.0) * occ;\n"
      "  }\n"
      "\n"
      "  col = mix(col, vec3(0), 1.0 - exp(-0.5 * i));\n"
      "\n"
      "  col = 1.0 - exp(-0.5 * col);\n"
      "  col = pow(abs(col), vec3(1.0 / 2.2));\n"
      "\n"
      "  gl_FragColor = vec4(col, 1);\n"
      "}\n";

public:
  ShaderMaterialServerRoomScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas), _time{0.f}, _shaderMaterial{nullptr}
  {
    // Vertex shader
    Effect::ShadersStore()["customVertexShader"] = customVertexShader;

    // Fragment shader
    Effect::ShadersStore()["customFragmentShader"] = customFragmentShader;
  }

  ~ShaderMaterialServerRoomScene() override = default;

  const char* getName() override
  {
    return "Shader Material Server Room Scene";
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

}; // end of class ShaderMaterialServerRoomScene

BABYLON_REGISTER_SAMPLE("Shader Materials", ShaderMaterialServerRoomScene)

} // end of namespace Samples
} // end of namespace BABYLON
