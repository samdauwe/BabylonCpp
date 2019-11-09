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

class ShaderMaterialPegasusGalaxyScene : public IRenderableScene {

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
  // Pegasus Galaxy ( https://www.shadertoy.com/view/lty3Rt )
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
      "// Galaxy shader\n"
      "//\n"
      "// Created by Frank Hugenroth  /frankenburgh/   07/2015\n"
      "// Released at nordlicht/bremen 2015\n"
      "\n"
      "// random/hash function\n"
      "float hash(float n) { return fract(cos(n) * 41415.92653); }\n"
      "\n"
      "// 2d noise function\n"
      "float noise(in vec2 x) {\n"
      "  vec2 p = floor(x);\n"
      "  vec2 f = smoothstep(0.0, 1.0, fract(x));\n"
      "  float n = p.x + p.y * 57.0;\n"
      "\n"
      "  return mix(mix(hash(n + 0.0), hash(n + 1.0), f.x),\n"
      "             mix(hash(n + 57.0), hash(n + 58.0), f.x), f.y);\n"
      "}\n"
      "\n"
      "float noise(in vec3 x) {\n"
      "  vec3 p = floor(x);\n"
      "  vec3 f = smoothstep(0.0, 1.0, fract(x));\n"
      "  float n = p.x + p.y * 57.0 + 113.0 * p.z;\n"
      "\n"
      "  return mix(mix(mix(hash(n + 0.0), hash(n + 1.0), f.x),\n"
      "                 mix(hash(n + 57.0), hash(n + 58.0), f.x), f.y),\n"
      "             mix(mix(hash(n + 113.0), hash(n + 114.0), f.x),\n"
      "                 mix(hash(n + 170.0), hash(n + 171.0), f.x), f.y),\n"
      "             f.z);\n"
      "}\n"
      "\n"
      "mat3 m = mat3(0.00, 1.60, 1.20,\n"
      "             -1.60, 0.72, -0.96,\n"
      "             -1.20, -0.96, 1.28);\n"
      "\n"
      "// Fractional Brownian motion\n"
      "float fbmslow(vec3 p) {\n"
      "  float f = 0.5000 * noise(p);\n"
      "  p = m * p * 1.2;\n"
      "  f += 0.2500 * noise(p);\n"
      "  p = m * p * 1.3;\n"
      "  f += 0.1666 * noise(p);\n"
      "  p = m * p * 1.4;\n"
      "  f += 0.0834 * noise(p);\n"
      "  p = m * p * 1.84;\n"
      "  return f;\n"
      "}\n"
      "\n"
      "float fbm(vec3 p) {\n"
      "  float f = 0., a = 1., s = 0.;\n"
      "  f += a * noise(p);\n"
      "  p = m * p * 1.149;\n"
      "  s += a;\n"
      "  a *= .75;\n"
      "  f += a * noise(p);\n"
      "  p = m * p * 1.41;\n"
      "  s += a;\n"
      "  a *= .75;\n"
      "  f += a * noise(p);\n"
      "  p = m * p * 1.51;\n"
      "  s += a;\n"
      "  a *= .65;\n"
      "  f += a * noise(p);\n"
      "  p = m * p * 1.21;\n"
      "  s += a;\n"
      "  a *= .35;\n"
      "  f += a * noise(p);\n"
      "  p = m * p * 1.41;\n"
      "  s += a;\n"
      "  a *= .75;\n"
      "  f += a * noise(p);\n"
      "  return f / s;\n"
      "}\n"
      "\n"
      "void main(void) {\n"
      "  float time = iTime * 0.1;\n"
      "\n"
      "  vec2 xy = -1.0 + 2.0 * vUV.xy;\n"
      "\n"
      "  // fade in (1=10sec), out after 8=80sec;\n"
      "  float fade = min(1., time * 1.) * min(1., max(0., 15. - time));\n"
      "  // start glow after 5=50sec\n"
      "  float fade2 = max(0., time - 10.) * 0.37;\n"
      "  float glow = max(-.25,\n"
      "                   1. + pow(fade2, 10.) - 0.001 * pow(fade2, 25.));\n"
      "\n"
      "  // get camera position and view direction\n"
      "  vec3 campos =\n"
      "      vec3(500.0, 850.,\n"
      "           -.0 - cos((time - 1.4) / 2.) * 2000.); // moving\n"
      "  vec3 camtar = vec3(0., 0., 0.);\n"
      "\n"
      "  float roll = 0.34;\n"
      "  vec3 cw = normalize(camtar - campos);\n"
      "  vec3 cp = vec3(sin(roll), cos(roll), 0.0);\n"
      "  vec3 cu = normalize(cross(cw, cp));\n"
      "  vec3 cv = normalize(cross(cu, cw));\n"
      "  vec3 rd = normalize(xy.x * cu + xy.y * cv + 1.6 * cw);\n"
      "\n"
      "  vec3 light = normalize(vec3(0., 0., 0.) - campos);\n"
      "  float sundot = clamp(dot(light, rd), 0.0, 1.0);\n"
      "\n"
      "  // render sky\n"
      "\n"
      "  // galaxy center glow\n"
      "  vec3 col = glow * 1.2 *\n"
      "             min(vec3(1.0, 1.0, 1.0),\n"
      "                 vec3(2.0, 1.0, 0.5) * pow(sundot, 100.0));\n"
      "  // moon haze\n"
      "  col += 0.3 * vec3(0.8, 0.9, 1.2) * pow(sundot, 8.0);\n"
      "\n"
      "  // stars\n"
      "  vec3 stars = 85.5 * vec3(pow(fbmslow(rd.xyz * 312.0), 7.0)) *\n"
      "               vec3(pow(fbmslow(rd.zxy * 440.3), 8.0));\n"
      "\n"
      "  // moving background fog\n"
      "  vec3 cpos = 1500. * rd + vec3(831.0 - time * 30., 321.0, 1000.0);\n"
      "  col += vec3(0.4, 0.5, 1.0) * ((fbmslow(cpos * 0.0035) - .5));\n"
      "\n"
      "  cpos += vec3(831.0 - time * 33., 321.0, 999.);\n"
      "  col += vec3(0.6, 0.3, 0.6) * 10.0 *\n"
      "         pow((fbmslow(cpos * 0.0045)), 10.0);\n"
      "\n"
      "  cpos += vec3(3831.0 - time * 39., 221.0, 999.0);\n"
      "  col += 0.03 * vec3(0.6, 0.0, 0.0) * 10.0 *\n"
      "         pow((fbmslow(cpos * 0.0145)), 2.0);\n"
      "\n"
      "  // stars\n"
      "  cpos = 1500. * rd + vec3(831.0, 321.0, 999.);\n"
      "  col += stars * fbm(cpos * 0.0021);\n"
      "\n"
      "  // Clouds\n"
      "  vec2 shift = vec2(time * 100.0, time * 180.0);\n"
      "  vec4 sum = vec4(0, 0, 0, 0);\n"
      "  float c = campos.y / rd.y; // cloud height\n"
      "  vec3 cpos2 = campos - c * rd;\n"
      "  float radius = length(cpos2.xz) / 1000.0;\n"
      "\n"
      "  if (radius < 1.8) {\n"
      "    for (int q = 10; q > -10; q--) // layers\n"
      "    {\n"
      "      if (sum.w > 0.999)\n"
      "        continue;\n"
      "      float c = (float(q) * 8. - campos.y) / rd.y; // cloud height\n"
      "      vec3 cpos = campos + c * rd;\n"
      "\n"
      "      float see = dot(normalize(cpos), normalize(campos));\n"
      "      vec3 lightUnvis = vec3(.0, .0, .0);\n"
      "      vec3 lightVis = vec3(1.3, 1.2, 1.2);\n"
      "      vec3 shine = mix(lightVis, lightUnvis,\n"
      "                       smoothstep(0.0, 1.0, see));\n"
      "\n"
      "      // border\n"
      "      float radius = length(cpos.xz) / 999.;\n"
      "      if (radius > 1.0)\n"
      "        continue;\n"
      "\n"
      "      float rot = 3.00 * (radius)-time;\n"
      "      cpos.xz = cpos.xz * mat2(cos(rot), -sin(rot), sin(rot), "
      "cos(rot));\n"
      "\n"
      "      cpos += vec3(831.0 + shift.x,\n"
      "                   321.0 + float(q) * mix(250.0, 50.0, radius) -\n"
      "                   shift.x * 0.2,\n"
      "                   1330.0 + shift.y);                // cloud position\n"
      "      cpos *= mix(0.0025, 0.0028, radius);           // zoom\n"
      "      float alpha = smoothstep(0.50, 1.0,\n"
      "                               fbm(cpos)); // fractal cloud density\n"
      "      alpha *= 1.3 *\n"
      "               pow(smoothstep(1.0, 0.0, radius),\n"
      "                   0.3); // fade out disc at edges\n"
      "      vec3 dustcolor =\n"
      "          mix(vec3(2.0, 1.3, 1.0), vec3(0.1, 0.2, 0.3),\n"
      "              pow(radius, .5));\n"
      "      vec3 localcolor =\n"
      "          mix(dustcolor, shine, alpha); // density color white->gray\n"
      "\n"
      "      float gstar = 2. * pow(noise(cpos * 21.40), 22.0);\n"
      "      float gstar2 = 3. * pow(noise(cpos * 26.55), 34.0);\n"
      "      float gholes = 1. * pow(noise(cpos * 11.55), 14.0);\n"
      "      localcolor += vec3(1.0, 0.6, 0.3) * gstar;\n"
      "      localcolor += vec3(1.0, 1.0, 0.7) * gstar2;\n"
      "      localcolor -= gholes;\n"
      "\n"
      "      alpha = (1.0 - sum.w) *\n"
      "                     alpha; // alpha/density saturation (the more a\n"
      "                            // cloud layer\\\'s density, the more the\n"
      "                            // higher layers will be hidden)\n"
      "      sum += vec4(localcolor * alpha,\n"
      "                  alpha); // sum up weightened color\n"
      "    }\n"
      "\n"
      "    for (int q = 0; q < 20; q++) // 120 layers\n"
      "    {\n"
      "      if (sum.w > 0.999)\n"
      "        continue;\n"
      "      float c = (float(q) * 4. - campos.y) / rd.y; // cloud height\n"
      "      vec3 cpos = campos + c * rd;\n"
      "\n"
      "      float see = dot(normalize(cpos), normalize(campos));\n"
      "      vec3 lightUnvis = vec3(.0, .0, .0);\n"
      "      vec3 lightVis = vec3(1.3, 1.2, 1.2);\n"
      "      vec3 shine = mix(lightVis, lightUnvis,\n"
      "                       smoothstep(0.0, 1.0, see));\n"
      "\n"
      "      // border\n"
      "      float radius = length(cpos.xz) / 200.0;\n"
      "      if (radius > 1.0)\n"
      "        continue;\n"
      "\n"
      "      float rot = 3.2 * (radius)-time * 1.1;\n"
      "      cpos.xz = cpos.xz * mat2(cos(rot), -sin(rot),\n"
      "                               sin(rot), cos(rot));\n"
      "\n"
      "      cpos += vec3(831.0 + shift.x,\n"
      "                   321.0 + float(q) *\n"
      "                   mix(250.0, 50.0, radius) - shift.x * 0.2,\n"
      "                   1330.0 + shift.y); // cloud position\n"
      "      float alpha = 0.1 + \n"
      "          smoothstep(0.6, 1.0, fbm(cpos)); // fractal cloud density\n"
      "      alpha *= 1.2 * (pow(smoothstep(1.0, 0.0, radius), 0.72) -\n"
      "                      pow(smoothstep(1.0, 0.0, radius * 1.875),\n"
      "                          0.2));             // fade out disc at edges\n"
      "      vec3 localcolor =\n"
      "               vec3(0.0, 0.0, 0.0); // density color white->gray\n"
      "\n"
      "      alpha = (1.0 - sum.w) *\n"
      "                     alpha; // alpha/density saturation (the more a\n"
      "                            // cloud layer\\\'s density, the more the\n"
      "                            // higher layers will be hidden)\n"
      "      sum += vec4(localcolor * alpha,\n"
      "                               alpha); // sum up weightened color\n"
      "    }\n"
      "  }\n"
      "  float alpha = smoothstep(1. - radius * .5, 1.0, sum.w);\n"
      "  sum.rgb /= sum.w + 0.0001;\n"
      "  sum.rgb -= 0.2 * vec3(0.8, 0.75, 0.7) * pow(sundot, 10.0) * alpha;\n"
      "  sum.rgb += min(glow, 10.0) * 0.2 * vec3(1.2, 1.2, 1.2) *\n"
      "             pow(sundot, 5.0) *\n"
      "             (1.0 - alpha);\n"
      "\n"
      "  col = mix(col, sum.rgb, sum.w); //*pow(sundot,10.0) );\n"
      "\n"
      "  // haze\n"
      "  col = fade * mix(col, vec3(0.3, 0.5, .9),\n"
      "                   29.0 * (pow(sundot, 50.0) - pow(sundot, 60.0)) /\n"
      "                       (2. + 9. * abs(rd.y)));\n"
      "\n"
      "  // Vignetting\n"
      "  vec2 xy2 = vUV.xy;\n"
      "  col *= vec3(.5, .5, .5) +\n"
      "         0.25 * pow(100.0 * xy2.x * xy2.y *\n"
      "                   (1.0 - xy2.x) * (1.0 - xy2.y), .5);\n"
      "\n"
      "  gl_FragColor = vec4(col, 1.0);\n"
      "}\n";

public:
  ShaderMaterialPegasusGalaxyScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas), _time{0.f}, _shaderMaterial{nullptr}
  {
    // Vertex shader
    Effect::ShadersStore()["customVertexShader"] = customVertexShader;

    // Fragment shader
    Effect::ShadersStore()["customFragmentShader"] = customFragmentShader;
  }

  ~ShaderMaterialPegasusGalaxyScene() override = default;

  const char* getName() override
  {
    return "Shader Material Pegasus Galaxy Scene";
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

}; // end of class ShaderMaterialPegasusGalaxyScene

BABYLON_REGISTER_SAMPLE("Shader Materials", ShaderMaterialPegasusGalaxyScene)

} // end of namespace Samples
} // end of namespace BABYLON
