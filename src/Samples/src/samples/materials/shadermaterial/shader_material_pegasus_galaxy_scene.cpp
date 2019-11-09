#include <babylon/samples/samples_index.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_shaders_store.h>
#include <babylon/materials/shader_material.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class ShaderMaterial;
using ShaderMaterialPtr = std::shared_ptr<ShaderMaterial>;

namespace Samples {

class ShaderMaterialPegasusGalaxyScene : public IRenderableScene {

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
  // Pegasus Galaxy ( https://www.shadertoy.com/view/lty3Rt )
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

// Galaxy shader
//
// Created by Frank Hugenroth  /frankenburgh/   07/2015
// Released at nordlicht/bremen 2015

// random/hash function
float hash(float n) { return fract(cos(n) * 41415.92653); }

// 2d noise function
float noise(in vec2 x) {
  vec2 p = floor(x);
  vec2 f = smoothstep(0.0, 1.0, fract(x));
  float n = p.x + p.y * 57.0;

  return mix(mix(hash(n + 0.0), hash(n + 1.0), f.x),
             mix(hash(n + 57.0), hash(n + 58.0), f.x), f.y);
}

float noise(in vec3 x) {
  vec3 p = floor(x);
  vec3 f = smoothstep(0.0, 1.0, fract(x));
  float n = p.x + p.y * 57.0 + 113.0 * p.z;

  return mix(mix(mix(hash(n + 0.0), hash(n + 1.0), f.x),
                 mix(hash(n + 57.0), hash(n + 58.0), f.x), f.y),
             mix(mix(hash(n + 113.0), hash(n + 114.0), f.x),
                 mix(hash(n + 170.0), hash(n + 171.0), f.x), f.y),
             f.z);
}

mat3 m = mat3(0.00, 1.60, 1.20,
             -1.60, 0.72, -0.96,
             -1.20, -0.96, 1.28);

// Fractional Brownian motion
float fbmslow(vec3 p) {
  float f = 0.5000 * noise(p);
  p = m * p * 1.2;
  f += 0.2500 * noise(p);
  p = m * p * 1.3;
  f += 0.1666 * noise(p);
  p = m * p * 1.4;
  f += 0.0834 * noise(p);
  p = m * p * 1.84;
  return f;
}

float fbm(vec3 p) {
  float f = 0., a = 1., s = 0.;
  f += a * noise(p);
  p = m * p * 1.149;
  s += a;
  a *= .75;
  f += a * noise(p);
  p = m * p * 1.41;
  s += a;
  a *= .75;
  f += a * noise(p);
  p = m * p * 1.51;
  s += a;
  a *= .65;
  f += a * noise(p);
  p = m * p * 1.21;
  s += a;
  a *= .35;
  f += a * noise(p);
  p = m * p * 1.41;
  s += a;
  a *= .75;
  f += a * noise(p);
  return f / s;
}

void main(void) {
  float time = iTime * 0.1;

  vec2 xy = -1.0 + 2.0 * vUV.xy;

  // fade in (1=10sec), out after 8=80sec;
  float fade = min(1., time * 1.) * min(1., max(0., 15. - time));
  // start glow after 5=50sec
  float fade2 = max(0., time - 10.) * 0.37;
  float glow = max(-.25,
                   1. + pow(fade2, 10.) - 0.001 * pow(fade2, 25.));

  // get camera position and view direction
  vec3 campos =
      vec3(500.0, 850.,
           -.0 - cos((time - 1.4) / 2.) * 2000.); // moving
  vec3 camtar = vec3(0., 0., 0.);

  float roll = 0.34;
  vec3 cw = normalize(camtar - campos);
  vec3 cp = vec3(sin(roll), cos(roll), 0.0);
  vec3 cu = normalize(cross(cw, cp));
  vec3 cv = normalize(cross(cu, cw));
  vec3 rd = normalize(xy.x * cu + xy.y * cv + 1.6 * cw);

  vec3 light = normalize(vec3(0., 0., 0.) - campos);
  float sundot = clamp(dot(light, rd), 0.0, 1.0);

  // render sky

  // galaxy center glow
  vec3 col = glow * 1.2 *
             min(vec3(1.0, 1.0, 1.0),
                 vec3(2.0, 1.0, 0.5) * pow(sundot, 100.0));
  // moon haze
  col += 0.3 * vec3(0.8, 0.9, 1.2) * pow(sundot, 8.0);

  // stars
  vec3 stars = 85.5 * vec3(pow(fbmslow(rd.xyz * 312.0), 7.0)) *
               vec3(pow(fbmslow(rd.zxy * 440.3), 8.0));

  // moving background fog
  vec3 cpos = 1500. * rd + vec3(831.0 - time * 30., 321.0, 1000.0);
  col += vec3(0.4, 0.5, 1.0) * ((fbmslow(cpos * 0.0035) - .5));

  cpos += vec3(831.0 - time * 33., 321.0, 999.);
  col += vec3(0.6, 0.3, 0.6) * 10.0 *
         pow((fbmslow(cpos * 0.0045)), 10.0);

  cpos += vec3(3831.0 - time * 39., 221.0, 999.0);
  col += 0.03 * vec3(0.6, 0.0, 0.0) * 10.0 *
         pow((fbmslow(cpos * 0.0145)), 2.0);

  // stars
  cpos = 1500. * rd + vec3(831.0, 321.0, 999.);
  col += stars * fbm(cpos * 0.0021);

  // Clouds
  vec2 shift = vec2(time * 100.0, time * 180.0);
  vec4 sum = vec4(0, 0, 0, 0);
  float c = campos.y / rd.y; // cloud height
  vec3 cpos2 = campos - c * rd;
  float radius = length(cpos2.xz) / 1000.0;

  if (radius < 1.8) {
    for (int q = 10; q > -10; q--) // layers
    {
      if (sum.w > 0.999)
        continue;
      float c = (float(q) * 8. - campos.y) / rd.y; // cloud height
      vec3 cpos = campos + c * rd;

      float see = dot(normalize(cpos), normalize(campos));
      vec3 lightUnvis = vec3(.0, .0, .0);
      vec3 lightVis = vec3(1.3, 1.2, 1.2);
      vec3 shine = mix(lightVis, lightUnvis,
                       smoothstep(0.0, 1.0, see));

      // border
      float radius = length(cpos.xz) / 999.;
      if (radius > 1.0)
        continue;

      float rot = 3.00 * (radius)-time;
      cpos.xz = cpos.xz * mat2(cos(rot), -sin(rot), sin(rot), 
cos(rot));

      cpos += vec3(831.0 + shift.x,
                   321.0 + float(q) * mix(250.0, 50.0, radius) -
                   shift.x * 0.2,
                   1330.0 + shift.y);                // cloud position
      cpos *= mix(0.0025, 0.0028, radius);           // zoom
      float alpha = smoothstep(0.50, 1.0,
                               fbm(cpos)); // fractal cloud density
      alpha *= 1.3 *
               pow(smoothstep(1.0, 0.0, radius),
                   0.3); // fade out disc at edges
      vec3 dustcolor =
          mix(vec3(2.0, 1.3, 1.0), vec3(0.1, 0.2, 0.3),
              pow(radius, .5));
      vec3 localcolor =
          mix(dustcolor, shine, alpha); // density color white->gray

      float gstar = 2. * pow(noise(cpos * 21.40), 22.0);
      float gstar2 = 3. * pow(noise(cpos * 26.55), 34.0);
      float gholes = 1. * pow(noise(cpos * 11.55), 14.0);
      localcolor += vec3(1.0, 0.6, 0.3) * gstar;
      localcolor += vec3(1.0, 1.0, 0.7) * gstar2;
      localcolor -= gholes;

      alpha = (1.0 - sum.w) *
                     alpha; // alpha/density saturation (the more a
                            // cloud layer\\'s density, the more the
                            // higher layers will be hidden)
      sum += vec4(localcolor * alpha,
                  alpha); // sum up weightened color
    }

    for (int q = 0; q < 20; q++) // 120 layers
    {
      if (sum.w > 0.999)
        continue;
      float c = (float(q) * 4. - campos.y) / rd.y; // cloud height
      vec3 cpos = campos + c * rd;

      float see = dot(normalize(cpos), normalize(campos));
      vec3 lightUnvis = vec3(.0, .0, .0);
      vec3 lightVis = vec3(1.3, 1.2, 1.2);
      vec3 shine = mix(lightVis, lightUnvis,
                       smoothstep(0.0, 1.0, see));

      // border
      float radius = length(cpos.xz) / 200.0;
      if (radius > 1.0)
        continue;

      float rot = 3.2 * (radius)-time * 1.1;
      cpos.xz = cpos.xz * mat2(cos(rot), -sin(rot),
                               sin(rot), cos(rot));

      cpos += vec3(831.0 + shift.x,
                   321.0 + float(q) *
                   mix(250.0, 50.0, radius) - shift.x * 0.2,
                   1330.0 + shift.y); // cloud position
      float alpha = 0.1 + 
          smoothstep(0.6, 1.0, fbm(cpos)); // fractal cloud density
      alpha *= 1.2 * (pow(smoothstep(1.0, 0.0, radius), 0.72) -
                      pow(smoothstep(1.0, 0.0, radius * 1.875),
                          0.2));             // fade out disc at edges
      vec3 localcolor =
               vec3(0.0, 0.0, 0.0); // density color white->gray

      alpha = (1.0 - sum.w) *
                     alpha; // alpha/density saturation (the more a
                            // cloud layer\\'s density, the more the
                            // higher layers will be hidden)
      sum += vec4(localcolor * alpha,
                               alpha); // sum up weightened color
    }
  }
  float alpha = smoothstep(1. - radius * .5, 1.0, sum.w);
  sum.rgb /= sum.w + 0.0001;
  sum.rgb -= 0.2 * vec3(0.8, 0.75, 0.7) * pow(sundot, 10.0) * alpha;
  sum.rgb += min(glow, 10.0) * 0.2 * vec3(1.2, 1.2, 1.2) *
             pow(sundot, 5.0) *
             (1.0 - alpha);

  col = mix(col, sum.rgb, sum.w); //*pow(sundot,10.0) );

  // haze
  col = fade * mix(col, vec3(0.3, 0.5, .9),
                   29.0 * (pow(sundot, 50.0) - pow(sundot, 60.0)) /
                       (2. + 9. * abs(rd.y)));

  // Vignetting
  vec2 xy2 = vUV.xy;
  col *= vec3(.5, .5, .5) +
         0.25 * pow(100.0 * xy2.x * xy2.y *
                   (1.0 - xy2.x) * (1.0 - xy2.y), .5);

  gl_FragColor = vec4(col, 1.0);
}
)ShaderCode";

public:
  ShaderMaterialPegasusGalaxyScene(
    ICanvas* iCanvas)
      : IRenderableScene(iCanvas), _time{0.f}, _shaderMaterial{nullptr}
  {
    // Vertex shader
    Effect::ShadersStore()["customVertexShader"] = customVertexShader;
  
    // Fragment shader
    Effect::ShadersStore()["customFragmentShader"] = customFragmentShader;
  }
  ~ShaderMaterialPegasusGalaxyScene()
  {
  }

  const char* getName() override
  {
    return "Shader Material Pegasus Galaxy Scene";
  }
  void initializeScene(ICanvas* canvas,
                                                         Scene* scene) override
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
    _shaderMaterial
      = ShaderMaterial::New("boxShader", scene, "custom", shaderMaterialOptions);
  
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

BABYLON_REGISTER_SAMPLE("Shader Material", ShaderMaterialPegasusGalaxyScene)
} // end of namespace Samples
} // end of namespace BABYLON