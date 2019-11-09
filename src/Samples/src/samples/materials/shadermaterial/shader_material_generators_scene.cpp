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

class ShaderMaterialGeneratorsScene : public IRenderableScene {

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
  // Generators ( https://www.shadertoy.com/view/Xtf3Rn )
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
      "\n"
      "// \"GENERATORS REDUX\" by Kali\n"
      "\n"
      "// Reworked by eiffie to run faster and under ANGLE:\n"
      "//\n"
      "// -Made the sphere raytraced\n"
      "// -Soft AO shadows\n"
      "// -Various great optimizations\n"
      "//\n"
      "// Thanks eiffie!!!\n"
      "\n"
      "// Original description:\n"
      "// Same fractal as \"Ancient Temple\" + rotations, improved shading\n"
      "// (better coloring, AO and  shadows), some lighting effects,\n"
      "// and a path for the camera following a liquid metal ball.\n"
      "\n"
      "// turn off to enable faster AO soft shadows\n"
      "#define ENABLE_HARD_SHADOWS\n"
      "//#define ENABLE_VIBRATION\n"
      "// Works better on window view rather than full screen\n"
      "//#define ENABLE_POSTPROCESS\n"
      "\n"
      "#define RAY_STEPS 70\n"
      "#define SHADOW_STEPS 50\n"
      "#define LIGHT_COLOR vec3(.85, .9, 1.)\n"
      "#define AMBIENT_COLOR vec3(.8, .83, 1.)\n"
      "#define FLOOR_COLOR vec3(1., .7, .9)\n"
      "#define ENERGY_COLOR vec3(1., .7, .4)\n"
      "#define BRIGHTNESS .9\n"
      "#define GAMMA 1.3\n"
      "#define SATURATION .85\n"
      "\n"
      "#define detail .00005\n"
      "#define t iTime * .25\n"
      "\n"
      "vec3 lightdir = normalize(vec3(0.5, -0.3, -1.));\n"
      "vec3 ambdir = normalize(vec3(0., 0., 1.));\n"
      "const vec3 origin = vec3(0., 3.11, 0.);\n"
      "vec3 energy = vec3(0.01);\n"
      "#ifdef ENABLE_VIBRATION\n"
      "float vibration = sin(iTime * 60.) * .0013;\n"
      "#else\n"
      "float vibration = 0.;\n"
      "#endif\n"
      "float det = 0.0;\n"
      "vec3 pth1 = vec3(0.0, 0.0, 0.0);\n"
      "\n"
      "mat2 rot(float a) { return mat2(cos(a), sin(a), -sin(a), cos(a)); }\n"
      "\n"
      "vec3 path(float ti) {\n"
      "  return vec3(sin(ti), .3 - sin(ti * .632) * .3, cos(ti * .5)) * .5;\n"
      "}\n"
      "\n"
      "float Sphere(vec3 p, vec3 rd, float r) { // A RAY TRACED SPHERE\n"
      "  float b = dot(-p, rd);\n"
      "  float inner = b * b - dot(p, p) + r * r;\n"
      "  if (inner < 0.0)\n"
      "    return -1.0;\n"
      "  return b - sqrt(inner);\n"
      "}\n"
      "\n"
      "vec2 de(vec3 pos) {\n"
      "  float hid = 0.;\n"
      "  vec3 tpos = pos;\n"
      "  tpos.xz = abs(.5 - mod(tpos.xz, 1.));\n"
      "  vec4 p = vec4(tpos, 1.);\n"
      "  float y = max(0., .35 - abs(pos.y - 3.35)) / .35;\n"
      "  for (int i = 0; i < 7; i++) { // LOWERED THE ITERS\n"
      "    p.xyz = abs(p.xyz) - vec3(-0.02, 1.98, -0.02);\n"
      "    p = p * (2.0 + vibration * y) / clamp(dot(p.xyz, p.xyz), .4, 1.) -\n"
      "        vec4(0.5, 1., 0.4, 0.);\n"
      "    p.xz *= mat2(-0.416, -0.91, 0.91, -0.416);\n"
      "  }\n"
      "  float fl = pos.y - 3.013;\n"
      "  float fr = (length(max(abs(p.xyz) - vec3(0.1, 5.0, 0.1),\n"
      "                         vec3(0.0))) - 0.05) /\n"
      "             p.w; // RETURN A RRECT\n"
      "  // float fr=length(p.xyz)/p.w;\n"
      "  float d = min(fl, fr);\n"
      "  d = min(d, -pos.y + 3.95);\n"
      "  if (abs(d - fl) < .001)\n"
      "    hid = 1.;\n"
      "  return vec2(d, hid);\n"
      "}\n"
      "\n"
      "vec3 normal(vec3 p) {\n"
      "  vec3 e = vec3(0.0, det, 0.0);\n"
      "\n"
      "  return normalize(vec3(de(p + e.yxx).x - de(p - e.yxx).x,\n"
      "                        de(p + e.xyx).x - de(p - e.xyx).x,\n"
      "                        de(p + e.xxy).x - de(p - e.xxy).x));\n"
      "}\n"
      "\n"
      "// THIS ONLY RUNS WHEN WITH HARD SHADOWS\n"
      "float shadow(vec3 pos, vec3 sdir) {\n"
      "  float sh = 1.0;\n"
      "  float totdist = 2.0 * det;\n"
      "  float dist = 10.;\n"
      "  float t1 = Sphere((pos - .005 * sdir) - pth1, -sdir, 0.015);\n"
      "  if (t1 > 0. && t1 < .5) {\n"
      "    vec3 sphglowNorm = normalize(pos - t1 * sdir - pth1);\n"
      "    sh = 1. - pow(max(.0, dot(sphglowNorm, sdir)) * 1.2, 3.);\n"
      "  }\n"
      "  for (int steps = 0; steps < SHADOW_STEPS; steps++) {\n"
      "    if (totdist < .6 && dist > detail) {\n"
      "      vec3 p = pos - totdist * sdir;\n"
      "      dist = de(p).x;\n"
      "      sh = min(sh, max(50. * dist / totdist, 0.0));\n"
      "      totdist += max(.01, dist);\n"
      "    }\n"
      "  }\n"
      "\n"
      "  return clamp(sh, 0.1, 1.0);\n"
      "}\n"
      "\n"
      "float calcAO(const vec3 pos, const vec3 nor) {\n"
      "  float aodet = detail * 40.;\n"
      "  float totao = 0.0;\n"
      "  float sca = 14.0;\n"
      "  for (int aoi = 0; aoi < 5; aoi++) {\n"
      "    float hr = aodet * float(aoi * aoi);\n"
      "    vec3 aopos = nor * hr + pos;\n"
      "    float dd = de(aopos).x;\n"
      "    totao += -(dd - hr) * sca;\n"
      "    sca *= 0.7;\n"
      "  }\n"
      "  return clamp(1.0 - 5.0 * totao, 0., 1.0);\n"
      "}\n"
      "\n"
      "float _texture(vec3 p) {\n"
      "  p = abs(.5 - fract(p * 10.));\n"
      "  vec3 c = vec3(3.);\n"
      "  float es, l = es = 0.;\n"
      "  for (int i = 0; i < 10; i++) {\n"
      "    p = abs(p + c) - abs(p - c) - p;\n"
      "    p /= clamp(dot(p, p), .0, 1.);\n"
      "    p = p * -1.5 + c;\n"
      "    if (mod(float(i), 2.) < 1.) {\n"
      "      float pl = l;\n"
      "      l = length(p);\n"
      "      es += exp(-1. / abs(l - pl));\n"
      "    }\n"
      "  }\n"
      "  return es;\n"
      "}\n"
      "\n"
      "vec3 light(in vec3 p, in vec3 dir, in vec3 n, in float hid) {\n"
      "#ifdef ENABLE_HARD_SHADOWS\n"
      "  float sh = shadow(p, lightdir);\n"
      "#else\n"
      "  // USING AO TO MAKE VERY SOFT SHADOWS\n"
      "  float sh = calcAO(p, -2.5 * lightdir);\n"
      "#endif\n"
      "  float ao = calcAO(p, n);\n"
      "  float diff = max(0., dot(lightdir, -n)) * sh;\n"
      "  float y = 3.35 - p.y;\n"
      "  vec3 amb = max(.5, dot(dir, -n)) * .5 * AMBIENT_COLOR;\n"
      "  if (hid < .5) {\n"
      "    amb += max(0.2, dot(vec3(0., 1., 0.), -n)) * FLOOR_COLOR *\n"
      "           pow(max(0., .2 - abs(3. - p.y)) / .2, 1.5) * 2.;\n"
      "    amb += energy * pow(max(0., .4 - abs(y)) / .4, 2.) *\n"
      "           max(0.2, dot(vec3(0., -sign(y), 0.), -n)) * 2.;\n"
      "  }\n"
      "  vec3 r = reflect(lightdir, n);\n"
      "  float spec = pow(max(0., dot(dir, -r)) * sh, 10.);\n"
      "  vec3 col;\n"
      "  float energysource = pow(max(0., .04 - abs(y)) / .04, 4.) * 2.;\n"
      "  if (hid > 1.5) {\n"
      "    col = vec3(1.);\n"
      "    spec = spec * spec;\n"
      "  } else {\n"
      "    float k = _texture(p) * .23 + .2;\n"
      "    k = min(k, 1.5 - energysource);\n"
      "    col = mix(vec3(k, k * k, k * k * k), vec3(k), .3);\n"
      "    if (abs(hid - 1.) < .001)\n"
      "      col *= FLOOR_COLOR * 1.3;\n"
      "  }\n"
      "  col = col * (amb + diff * LIGHT_COLOR) + spec * LIGHT_COLOR;\n"
      "  if (hid < .5) {\n"
      "    col = max(col, energy * 2. * energysource);\n"
      "  }\n"
      "  col *= min(1.,\n"
      "             ao + length(energy) * .5 * max(0., .1 - abs(y)) / .1);\n"
      "  return col;\n"
      "}\n"
      "\n"
      "vec3 raymarch(in vec3 from, in vec3 dir) {\n"
      "  float ey = mod(t * .5, 1.);\n"
      "  float glow, eglow, ref, sphdist,\n"
      "        totdist = glow = eglow = ref = sphdist = 0.;\n"
      "  vec2 d = vec2(1., 0.);\n"
      "  vec3 p, col = vec3(0.);\n"
      "  vec3 origdir = dir, origfrom = from, sphNorm;\n"
      "\n"
      "  // FAKING THE SQUISHY BALL BY MOVING A RAY TRACED BALL\n"
      "  vec3 wob = cos(dir * 500.0 * length(from - pth1) +\n"
      "                 (from - pth1) * 250. +\n"
      "                 iTime * 10.) *\n"
      "             0.0005;\n"
      "  float t1 = Sphere(from - pth1 + wob, dir, 0.015);\n"
      "  float tg = Sphere(from - pth1 + wob, dir, 0.02);\n"
      "  if (t1 > 0.) {\n"
      "    ref = 1.0;\n"
      "    from += t1 * dir;\n"
      "    sphdist = t1;\n"
      "    sphNorm = normalize(from - pth1 + wob);\n"
      "    dir = reflect(dir, sphNorm);\n"
      "  } else if (tg > 0.) {\n"
      "    vec3 sphglowNorm = normalize(from + tg * dir - pth1 + wob);\n"
      "    glow += pow(max(0., dot(sphglowNorm, -dir)), 5.);\n"
      "  };\n"
      "\n"
      "  for (int i = 0; i < RAY_STEPS; i++) {\n"
      "    if (d.x > det && totdist < 3.0) {\n"
      "      p = from + totdist * dir;\n"
      "      d = de(p);\n"
      "      det = detail * (1. + totdist * 60.) * (1. + ref * 5.);\n"
      "      totdist += d.x;\n"
      "      energy = ENERGY_COLOR * (1.5 + sin(iTime * 20. + p.z * 10.)) *\n"
      "               .25;\n"
      "      if (d.x < 0.015)\n"
      "        glow += max(0., .015 - d.x) * exp(-totdist);\n"
      "      // ONLY DOING THE GLOW WHEN IT IS CLOSE ENOUGH\n"
      "      if (d.y < .5 && d.x < 0.03) {\n"
      "        // 2 glows at once\n"
      "        float glw = min(abs(3.35 - p.y - ey), abs(3.35 - p.y + ey));\n"
      "        eglow += max(0., .03 - d.x) / .03 *\n"
      "                 (pow(max(0., .05 - glw) / .05, 5.) +\n"
      "                  pow(max(0., .15 - abs(3.35 - p.y)) / .15, 8.)) *\n"
      "                 1.5;\n"
      "      }\n"
      "    }\n"
      "  }\n"
      "  float l = pow(max(0., dot(normalize(-dir.xz),\n"
      "                normalize(lightdir.xz))), 2.);\n"
      "  l *= max(0.2, dot(-dir, lightdir));\n"
      "  vec3 backg = .5 * (1.2 - l) + LIGHT_COLOR * l * .7;\n"
      "  backg *= AMBIENT_COLOR;\n"
      "  if (d.x <= det) {\n"
      "    // DO THE NORMAL CALC OUTSIDE OF LIGHTING\n"
      "    // (since we already have the sphere normal)\n"
      "    vec3 norm = normal(p - abs(d.x - det) * dir);\n"
      "    col = light(p - abs(d.x - det) * dir, dir, norm, d.y) *\n"
      "          exp(-.2 * totdist * totdist);\n"
      "    col = mix(col, backg, 1.0 - exp(-1. * pow(totdist, 1.5)));\n"
      "  } else {\n"
      "    col = backg;\n"
      "  }\n"
      "  vec3 lglow = LIGHT_COLOR * pow(l, 30.) * .5;\n"
      "  col += glow * (backg + lglow) * 1.3;\n"
      "  col += pow(eglow, 2.) * energy * .015;\n"
      "  col += lglow * min(1., totdist * totdist * .3);\n"
      "  if (ref > 0.5) {\n"
      "    vec3 sphlight = light(origfrom + sphdist * origdir, origdir,\n"
      "                          sphNorm, 2.);\n"
      "    col = mix(col * .3 + sphlight * .7, backg,\n"
      "              1.0 - exp(-1. * pow(sphdist, 1.5)));\n"
      "  }\n"
      "  return col;\n"
      "}\n"
      "\n"
      "vec3 move(inout mat2 rotview1, inout mat2 rotview2) {\n"
      "  vec3 go = path(t);\n"
      "  vec3 adv = path(t + .7);\n"
      "  vec3 advec = normalize(adv - go);\n"
      "  float an = atan(advec.x, advec.z);\n"
      "  rotview1 = mat2(cos(an), sin(an), -sin(an), cos(an));\n"
      "  an = advec.y * 1.7;\n"
      "  rotview2 = mat2(cos(an), sin(an), -sin(an), cos(an));\n"
      "  return go;\n"
      "}\n"
      "\n"
      "void main(void) {\n"
      "  pth1 = path(t + .3) + origin + vec3(0., .01, 0.);\n"
      "  vec2 uv = -1.0 + 2.0 * vUV.xy;\n"
      "  vec2 uv2 = uv;\n"
      "#ifdef ENABLE_POSTPROCESS\n"
      "  uv *= 1. + pow(length(uv2 * uv2 * uv2 * uv2), 4.) * .07;\n"
      "#endif\n"
      "  uv.y *= iResolution.y / iResolution.x;\n"
      "  vec3 iMouse = vec3(0.0, 0.0, 0.0);\n"
      "  vec2 mouse = (iMouse.xy / iResolution.xy - .5) * 3.;\n"
      "  if (iMouse.z < 1.)\n"
      "    mouse = vec2(0.);\n"
      "  mat2 rotview1, rotview2;\n"
      "  vec3 from = origin + move(rotview1, rotview2);\n"
      "  vec3 dir = normalize(vec3(uv * .8, 1.));\n"
      "  dir.yz *= rot(mouse.y);\n"
      "  dir.xz *= rot(mouse.x);\n"
      "  dir.yz *= rotview2;\n"
      "  dir.xz *= rotview1;\n"
      "  vec3 color = raymarch(from, dir);\n"
      "  color = clamp(color, vec3(.0), vec3(1.));\n"
      "  color = pow(color, vec3(GAMMA)) * BRIGHTNESS;\n"
      "  color = mix(vec3(length(color)), color, SATURATION);\n"
      "#ifdef ENABLE_POSTPROCESS\n"
      "  vec3 rain = pow(texture(iChannel0, uv2 + iTime * 7.25468).rgb,\n"
      "                  vec3(1.5));\n"
      "  color = mix(rain, color, clamp(iTime * .5 - .5, 0., 1.));\n"
      "  color *= 1. - pow(length(uv2 * uv2 * uv2 * uv2) * 1.1, 6.);\n"
      "  uv2.y *= iResolution.y / 360.0;\n"
      "  color.r *= (.5 + abs(.5 - mod(uv2.y, .021) / .021) * .5) * 1.5;\n"
      "  color.g *= (.5 + abs(.5 - mod(uv2.y + .007, .021) / .021) * .5)\n"
      "              * 1.5;\n"
      "  color.b *= (.5 + abs(.5 - mod(uv2.y + .014, .021) / .021) * .5)\n"
      "              * 1.5;\n"
      "  color *= .9 + rain * .35;\n"
      "#endif\n"
      "  gl_FragColor = vec4(color, 1.);\n"
      "}\n";

public:
  ShaderMaterialGeneratorsScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas), _time{0.f}, _shaderMaterial{nullptr}
  {
    // Vertex shader
    Effect::ShadersStore()["customVertexShader"] = customVertexShader;

    // Fragment shader
    Effect::ShadersStore()["customFragmentShader"] = customFragmentShader;
  }

  ~ShaderMaterialGeneratorsScene() override = default;

  const char* getName() override
  {
    return "Shader Material Generators Scene";
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

}; // end of class ShaderMaterialGeneratorsScene

BABYLON_REGISTER_SAMPLE("Shader Materials", ShaderMaterialGeneratorsScene)

} // end of namespace Samples
} // end of namespace BABYLON
