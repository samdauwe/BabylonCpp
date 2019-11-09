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

class ShaderMaterialXyptonjtrozScene : public IRenderableScene {

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
  // Xyptonjtroz ( https://www.shadertoy.com/view/4ts3z2 )
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
      "// Xyptonjtroz by nimitz (twitter: @stormoid)\n"
      "\n"
      "// Audio by Dave_Hoskins\n"
      "\n"
      "#define ITR 100\n"
      "#define FAR 30.\n"
      "#define time iTime\n"
      "\n"
      "/*\n"
      "  Believable animated volumetric dust storm in 7 samples,\n"
      "  blending each layer in based on geometry distance allows to\n"
      "  render it without visible seams. 3d Triangle noise is\n"
      "  used for the dust volume.\n"
      "\n"
      "  Also included is procedural bump mapping and glow based on\n"
      "  curvature*fresnel. (see: https://www.shadertoy.com/view/Xts3WM)\n"
      "\n"
      "\n"
      "  Further explanation of the dust generation (per Dave's request):\n"
      "\n"
      "  The basic idea is to have layers of gradient shaded volumetric\n"
      "  animated noise. The problem is when geometry is intersected\n"
      "  before the ray reaches the far plane. A way to smoothly blend\n"
      "  the low sampled noise is needed.  So I am blending (smoothstep)\n"
      "  each dust layer based on current ray distance and the solid\n"
      "  interesction distance. I am also scaling the noise taps as a\n"
      "  function of the current distance so that the distant dust doesn't\n"
      "  appear too noisy and as a function of current height to get some\n"
      "  \"ground hugging\" effect.\n"
      "\n"
      "*/\n"
      "\n"
      "mat2 mm2(in float a) {\n"
      "  float c = cos(a), s = sin(a);\n"
      "  return mat2(c, s, -s, c);\n"
      "}\n"
      "\n"
      "float height(in vec2 p) {\n"
      "  p *= 0.2;\n"
      "  return sin(p.y) * 0.4 + sin(p.x) * 0.4;\n"
      "}\n"
      "\n"
      "// smooth min form iq\n"
      "float smin(float a, float b) {\n"
      "  const float k = 0.7;\n"
      "  float h = clamp(0.5 + 0.5 * (b - a) / k, 0.0, 1.0);\n"
      "  return mix(b, a, h) - k * h * (1.0 - h);\n"
      "}\n"
      "\n"
      "// form Dave\n"
      "vec2 hash22(vec2 p) {\n"
      "  p = fract(p * vec2(5.3983, 5.4427));\n"
      "  p += dot(p.yx, p.xy + vec2(21.5351, 14.3137));\n"
      "  return fract(vec2(p.x * p.y * 95.4337, p.x * p.y * 97.597));\n"
      "}\n"
      "\n"
      "float vine(vec3 p, in float c, in float h) {\n"
      "  p.y += sin(p.z * 0.2625) * 2.5;\n"
      "  p.x += cos(p.z * 0.1575) * 3.;\n"
      "  vec2 q = vec2(mod(p.x, c) - c / 2., p.y);\n"
      "  return length(q) - h -\n"
      "         sin(p.z * 2. + sin(p.x * 7.) * 0.5 + time * 0.5) * 0.13;\n"
      "}\n"
      "\n"
      "float map(vec3 p) {\n"
      "  p.y += height(p.zx);\n"
      "\n"
      "  vec3 bp = p;\n"
      "  vec2 hs = hash22(floor(p.zx / 4.));\n"
      "  p.zx = mod(p.zx, 4.) - 2.;\n"
      "\n"
      "  float d = p.y + 0.5;\n"
      "  p.y -= hs.x * 0.4 - 0.15;\n"
      "  p.zx += hs * 1.3;\n"
      "  d = smin(d, length(p) - hs.x * 0.4);\n"
      "\n"
      "  d = smin(d, vine(bp + vec3(1.8, 0., 0), 15., .8));\n"
      "  d = smin(d, vine(bp.zyx + vec3(0., 0, 17.), 20., 0.75));\n"
      "\n"
      "  return d * 1.1;\n"
      "}\n"
      "\n"
      "float march(in vec3 ro, in vec3 rd) {\n"
      "  float precis = 0.002;\n"
      "  float h = precis * 2.0;\n"
      "  float d = 0.;\n"
      "  for (int i = 0; i < ITR; i++) {\n"
      "    if (abs(h) < precis || d > FAR)\n"
      "      break;\n"
      "    d += h;\n"
      "    float res = map(ro + rd * d);\n"
      "    h = res;\n"
      "  }\n"
      "  return d;\n"
      "}\n"
      "\n"
      "float tri(in float x) { return abs(fract(x) - .5); }\n"
      "vec3 tri3(in vec3 p) {\n"
      "  return vec3(tri(p.z + tri(p.y * 1.)), tri(p.z + tri(p.x * 1.)),\n"
      "              tri(p.y + tri(p.x * 1.)));\n"
      "}\n"
      "\n"
      "mat2 m2 = mat2(0.970, 0.242, -0.242, 0.970);\n"
      "\n"
      "float triNoise3d(in vec3 p, in float spd) {\n"
      "  float z = 1.4;\n"
      "  float rz = 0.;\n"
      "  vec3 bp = p;\n"
      "  for (float i = 0.; i <= 3.; i++) {\n"
      "    vec3 dg = tri3(bp * 2.);\n"
      "    p += (dg + time * spd);\n"
      "\n"
      "    bp *= 1.8;\n"
      "    z *= 1.5;\n"
      "    p *= 1.2;\n"
      "    // p.xz*= m2;\n"
      "\n"
      "    rz += (tri(p.z + tri(p.x + tri(p.y)))) / z;\n"
      "    bp += 0.14;\n"
      "  }\n"
      "  return rz;\n"
      "}\n"
      "\n"
      "float fogmap(in vec3 p, in float d) {\n"
      "  p.x += time * 1.5;\n"
      "  p.z += sin(p.x * .5);\n"
      "  return triNoise3d(p * 2.2 / (d + 20.), 0.2) * (1. - smoothstep(0., "
      ".7, "
      "p.y));\n"
      "}\n"
      "\n"
      "vec3 fog(in vec3 col, in vec3 ro, in vec3 rd, in float mt) {\n"
      "  float d = .5;\n"
      "  for (int i = 0; i < 7; i++) {\n"
      "    vec3 pos = ro + rd * d;\n"
      "    float rz = fogmap(pos, d);\n"
      "    float grd =\n"
      "        clamp((rz - fogmap(pos + .8 - float(i) * 0.1, d)) * 3., 0.1, "
      "1.);\n"
      "    vec3 col2 =\n"
      "        (vec3(.1, 0.8, .5) * .5 + .5 * vec3(.5, .8, 1.) * (1.7 - grd)) "
      "* "
      "0.55;\n"
      "    col = mix(col, col2,\n"
      "              clamp(rz * smoothstep(d - 0.4, d + 2. + d * .75, mt), 0., "
      "1.));\n"
      "    d *= 1.5 + 0.3;\n"
      "    if (d > mt)\n"
      "      break;\n"
      "  }\n"
      "  return col;\n"
      "}\n"
      "\n"
      "vec3 normal(in vec3 p) {\n"
      "  vec2 e = vec2(-1., 1.) * 0.005;\n"
      "  return normalize(e.yxx * map(p + e.yxx) + e.xxy * map(p + e.xxy) +\n"
      "                   e.xyx * map(p + e.xyx) + e.yyy * map(p + e.yyy));\n"
      "}\n"
      "\n"
      "float bnoise(in vec3 p) {\n"
      "  float n = sin(triNoise3d(p * .3, 0.0) * 11.) * 0.6 + 0.4;\n"
      "  n += sin(triNoise3d(p * 1., 0.05) * 40.) * 0.1 + 0.9;\n"
      "  return (n * n) * 0.003;\n"
      "}\n"
      "\n"
      "vec3 bump(in vec3 p, in vec3 n, in float ds) {\n"
      "  vec2 e = vec2(.005, 0);\n"
      "  float n0 = bnoise(p);\n"
      "  vec3 d = vec3(bnoise(p + e.xyy) - n0, bnoise(p + e.yxy) - n0,\n"
      "                bnoise(p + e.yyx) - n0) /\n"
      "           e.x;\n"
      "  n = normalize(n - d * 2.5 / sqrt(ds));\n"
      "  return n;\n"
      "}\n"
      "\n"
      "float shadow(in vec3 ro, in vec3 rd, in float mint, in float tmax) {\n"
      "  float res = 1.0;\n"
      "  float t = mint;\n"
      "  for (int i = 0; i < 10; i++) {\n"
      "    float h = map(ro + rd * t);\n"
      "    res = min(res, 4. * h / t);\n"
      "    t += clamp(h, 0.05, .5);\n"
      "    if (h < 0.001 || t > tmax)\n"
      "      break;\n"
      "  }\n"
      "  return clamp(res, 0.0, 1.0);\n"
      "}\n"
      "\n"
      "float curv(in vec3 p, in float w) {\n"
      "  vec2 e = vec2(-1., 1.) * w;\n"
      "\n"
      "  float t1 = map(p + e.yxx), t2 = map(p + e.xxy);\n"
      "  float t3 = map(p + e.xyx), t4 = map(p + e.yyy);\n"
      "\n"
      "  return .125 / (e.x * e.x) * (t1 + t2 + t3 + t4 - 4. * map(p));\n"
      "}\n"
      "\n"
      "void main(void) {\n"
      "  vec2 p = (-1.0 + 2.0 * vUV.xy);\n"
      "  vec2 q = -1.0 + 2.0 * vUV.xy;\n"
      "  p.x *= iResolution.x / iResolution.y;\n"
      "  vec2 mo = iMouse.xy / iResolution.xy - .5;\n"
      "  mo = (mo == vec2(-.5)) ? mo = vec2(-0.1, 0.07) : mo;\n"
      "  mo.x *= iResolution.x / iResolution.y;\n"
      "\n"
      "  vec3 ro = vec3(smoothstep(0., 1., tri(time * .45) * 2.) * 0.1,\n"
      "                 smoothstep(0., 1., tri(time * 0.9) * 2.) * 0.07, -time "
      "* "
      "0.6);\n"
      "  ro.y -= height(ro.zx) + 0.05;\n"
      "  mo.x += smoothstep(0.6, 1., sin(time * .6) * 0.5 + 0.5) - 1.5;\n"
      "  vec3 eyedir = normalize(vec3(\n"
      "      cos(mo.x), mo.y * 2. - 0.2 + sin(time * 0.45 * 1.57) * 0.1, "
      "sin(mo.x)));\n"
      "  vec3 rightdir = normalize(vec3(cos(mo.x + 1.5708), 0., sin(mo.x + "
      "1.5708)));\n"
      "  vec3 updir = normalize(cross(rightdir, eyedir));\n"
      "  vec3 rd = normalize((p.x * rightdir + p.y * updir) * 1. + eyedir);\n"
      "\n"
      "  vec3 ligt = normalize(vec3(.5, .05, -.2));\n"
      "  vec3 ligt2 = normalize(vec3(.5, -.1, -.2));\n"
      "\n"
      "  float rz = march(ro, rd);\n"
      "\n"
      "  vec3 fogb = mix(vec3(.7, .8, .8) * 0.3, vec3(1., 1., .77) * .95,\n"
      "                  pow(dot(rd, ligt2) + 1.2, 2.5) * .25);\n"
      "  fogb *= clamp(rd.y * .5 + .6, 0., 1.);\n"
      "  vec3 col = fogb;\n"
      "\n"
      "  if (rz < FAR) {\n"
      "    vec3 pos = ro + rz * rd;\n"
      "    vec3 nor = normal(pos);\n"
      "    float d = distance(pos, ro);\n"
      "    nor = bump(pos, nor, d);\n"
      "    float crv = clamp(curv(pos, .4), .0, 10.);\n"
      "    float shd = shadow(pos, ligt, 0.1, 3.);\n"
      "    float dif = clamp(dot(nor, ligt), 0.0, 1.0) * shd;\n"
      "    float spe = pow(clamp(dot(reflect(rd, nor), ligt), 0.0, 1.0), 50.) "
      "* "
      "shd;\n"
      "    float fre = pow(clamp(1.0 + dot(nor, rd), 0.0, 1.0), 1.5);\n"
      "    vec3 brdf = vec3(0.10, 0.11, 0.13);\n"
      "    brdf += 1.5 * dif * vec3(1.00, 0.90, 0.7);\n"
      "    col = mix(vec3(0.1, 0.2, 1), vec3(.3, .5, 1), pos.y * .5) * 0.2 + "
      ".1;\n"
      "    col *= (sin(bnoise(pos) * 900.) * 0.2 + 0.8);\n"
      "    col = col * brdf + col * spe * .5 + fre * vec3(.7, 1., 0.2) * .3 * "
      "crv;\n"
      "  }\n"
      "\n"
      "  // ordinary distance fog first\n"
      "  col = mix(col, fogb, smoothstep(FAR - 7., FAR, rz));\n"
      "\n"
      "  // then volumetric fog\n"
      "  col = fog(col, ro, rd, rz);\n"
      "\n"
      "  // post\n"
      "  col = pow(col, vec3(0.8));\n"
      "  col *= 1. - smoothstep(0.1, 2., length(p));\n"
      "\n"
      "  gl_FragColor = vec4(col, 1.0);\n"
      "}\n";

public:
  ShaderMaterialXyptonjtrozScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas), _time{0.f}, _shaderMaterial{nullptr}
  {
    // Vertex shader
    Effect::ShadersStore()["customVertexShader"] = customVertexShader;

    // Fragment shader
    Effect::ShadersStore()["customFragmentShader"] = customFragmentShader;
  }

  ~ShaderMaterialXyptonjtrozScene() override = default;

  const char* getName() override
  {
    return "Shader Material Xyptonjtroz Scene";
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

}; // end of class ShaderMaterialXyptonjtrozScene

BABYLON_REGISTER_SAMPLE("Shader Materials", ShaderMaterialXyptonjtrozScene)

} // end of namespace Samples
} // end of namespace BABYLON
