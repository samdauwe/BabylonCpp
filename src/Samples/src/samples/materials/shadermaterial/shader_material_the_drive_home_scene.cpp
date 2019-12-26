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

class ShaderMaterialTheDriveHomeScene : public IRenderableScene {

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
  // The Drive Home ( https://www.shadertoy.com/view/MdfBRX )
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

// 'The Drive Home' by Martijn Steinrucken aka BigWings - 2017
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0
// Unported License.
//
// I was looking for something 3d, that can be made just with a 
// point-line distance function. Then I saw the cover graphic of the 
// song I'm using here on soundcloud, which is a bokeh traffic shot 
// which is a perfect for for what I was looking for.
//
// It took me a while to get to a satisfying rain effect. Most other 
// people use a render buffer for this so that is how I started. In the 
// end though, I got a better effect without. Uncomment the DROP_DEBUG 
// define to get a better idea of what is going on.
//
// If you are watching this on a weaker device, you can uncomment the
// HIGH_QUALITY define
//
// Music:
// Mr. Bill - Cheyah (Zefora's digital rain remix)
// https://soundcloud.com/zefora/cheyah
//
// Video can be found here:
// https://www.youtube.com/watch?v=WrxZ4AZPdOQ
//
//

#define S(x, y, z) smoothstep(x, y, z)
#define B(a, b, edge, t) S(a - edge, a + edge, t) * S(b + edge, b - edge, t)
#define sat(x) clamp(x, 0., 1.)

#define streetLightCol vec3(1., .7, .3)
#define headLightCol vec3(.8, .8, 1.)
#define tailLightCol vec3(1., .1, .1)

//#define HIGH_QUALITY
#define CAM_SHAKE 1.
#define LANE_BIAS .5
#define RAIN
//#define DROP_DEBUG

vec3 ro, rd;

float N(float t) { return fract(sin(t * 10234.324) * 123423.23512); }
vec3 N31(float p) {
  // 3 out, 1 in... DAVE HOSKINS
  vec3 p3 = fract(vec3(p) * vec3(.1031, .11369, .13787));
  p3 += dot(p3, p3.yzx + 19.19);
  return fract(
      vec3((p3.x + p3.y) * p3.z, (p3.x + p3.z) * p3.y, (p3.y + p3.z) *
            p3.x));
}
float N2(vec2 p) {
  // Dave Hoskins - https://www.shadertoy.com/view/4djSRW
  vec3 p3 = fract(vec3(p.xyx) * vec3(443.897, 441.423, 437.195));
  p3 += dot(p3, p3.yzx + 19.19);
  return fract((p3.x + p3.y) * p3.z);
}

float DistLine(vec3 ro, vec3 rd, vec3 p) {
  return length(cross(p - ro, rd));
}

vec3 ClosestPoint(vec3 ro, vec3 rd, vec3 p) {
  // returns the closest point on ray r to point p
  return ro + max(0., dot(p - ro, rd)) * rd;
}

float Remap(float a, float b, float c, float d, float t) {
  return ((t - a) / (b - a)) * (d - c) + c;
}

float BokehMask(vec3 ro, vec3 rd, vec3 p, float size, float blur) {
  float d = DistLine(ro, rd, p);
  float m = S(size, size * (1. - blur), d);

#ifdef HIGH_QUALITY
  m *= mix(.7, 1., S(.8 * size, size, d));
#endif

  return m;
}

float SawTooth(float t) {
  return cos(t + cos(t)) + sin(2. * t) * .2 + sin(4. * t) * .02;
}

float DeltaSawTooth(float t) {
  return 0.4 * cos(2. * t) + 0.08 * cos(4. * t) -
         (1. - sin(t)) * sin(t + cos(t));
}

vec2 GetDrops(vec2 uv, float seed, float m) {

  float t = iTime + m * 30.;
  vec2 o = vec2(0.);

#ifndef DROP_DEBUG
  uv.y += t * .05;
#endif

  uv *= vec2(10., 2.5) * 2.;
  vec2 id = floor(uv);
  vec3 n = N31(id.x + (id.y + seed) * 546.3524);
  vec2 bd = fract(uv);

  vec2 uv2 = bd;

  bd -= .5;

  bd.y *= 4.;

  bd.x += (n.x - .5) * .6;

  t += n.z * 6.28;
  float slide = SawTooth(t);

  float ts = 1.5;
  vec2 trailPos = vec2(bd.x * ts,
                      (fract(bd.y * ts * 2. - t * 2.) - .5) * .5);

  bd.y += slide * 2.; // make drops slide down

#ifdef HIGH_QUALITY
  float dropShape = bd.x * bd.x;
  dropShape *= DeltaSawTooth(t);
  bd.y += dropShape; // change shape of drop when it is falling
#endif

  float d = length(bd); // distance to main drop

  // mask out drops that are below the main
  float trailMask = S(-.2, .2, bd.y);
  trailMask *= bd.y;                  // fade dropsize
  // distance to trail drops
  float td = length(trailPos * max(.5, trailMask));

  float mainDrop = S(.2, .1, d);
  float dropTrail = S(.1, .02, td);

  dropTrail *= trailMask;
  // mix main drop and drop trail
  o = mix(bd * mainDrop, trailPos, dropTrail);

#ifdef DROP_DEBUG
  if (uv2.x < .02 || uv2.y < .01)
    o = vec2(1.);
#endif

  return o;
}

void CameraSetup(vec2 uv, vec3 pos, vec3 lookat,
                 float zoom, float m) {
  ro = pos;
  vec3 f = normalize(lookat - ro);
  vec3 r = cross(vec3(0., 1., 0.), f);
  vec3 u = cross(f, r);
  float t = iTime;

  vec2 offs = vec2(0.);
#ifdef RAIN
  vec2 dropUv = uv;

#ifdef HIGH_QUALITY
  float x = (sin(t * .1) * .5 + .5) * .5;
  x = -x * x;
  float s = sin(x);
  float c = cos(x);

  mat2 rot = mat2(c, -s, s, c);

#ifndef DROP_DEBUG
  dropUv = uv * rot;
  dropUv.x += -sin(t * .1) * .5;
#endif
#endif

  offs = GetDrops(dropUv, 1., m);

#ifndef DROP_DEBUG
  offs += GetDrops(dropUv * 1.4, 10., m);
#ifdef HIGH_QUALITY
  offs += GetDrops(dropUv * 2.4, 25., m);
// offs += GetDrops(dropUv*3.4, 11.);
// offs += GetDrops(dropUv*3., 2.);
#endif

  float ripple = sin(t + uv.y * 3.1415 * 30. + uv.x * 124.) * .5 + .5;
  ripple *= .005;
  offs += vec2(ripple * ripple, ripple);
#endif
#endif
  vec3 center = ro + f * zoom;
  vec3 i = center + (uv.x - offs.x) * r + (uv.y - offs.y) * u;

  rd = normalize(i - ro);
}

vec3 HeadLights(float i, float t) {
  float z = fract(-t * 2. + i);
  vec3 p = vec3(-.3, .1, z * 40.);
  float d = length(p - ro);

  float size = mix(.03, .05, S(.02, .07, z)) * d;
  float m = 0.;
  float blur = .1;
  m += BokehMask(ro, rd, p - vec3(.08, 0., 0.), size, blur);
  m += BokehMask(ro, rd, p + vec3(.08, 0., 0.), size, blur);

#ifdef HIGH_QUALITY
  m += BokehMask(ro, rd, p + vec3(.1, 0., 0.), size, blur);
  m += BokehMask(ro, rd, p - vec3(.1, 0., 0.), size, blur);
#endif

  float distFade = max(.01, pow(1. - z, 9.));

  blur = .8;
  size *= 2.5;
  float r = 0.;
  r += BokehMask(ro, rd, p + vec3(-.09, -.2, 0.), size, blur);
  r += BokehMask(ro, rd, p + vec3(.09, -.2, 0.), size, blur);
  r *= distFade * distFade;

  return headLightCol * (m + r) * distFade;
}

vec3 TailLights(float i, float t) {
  t = t * 1.5 + i;

  float id = floor(t) + i;
  vec3 n = N31(id);

  float laneId = S(LANE_BIAS, LANE_BIAS + .01, n.y);

  float ft = fract(t);

  float z = 3. - ft * 3.; // distance ahead

  laneId *= S(.2, 1.5, z); // get out of the way!
  float lane = mix(.6, .3, laneId);
  vec3 p = vec3(lane, .1, z);
  float d = length(p - ro);

  float size = .05 * d;
  float blur = .1;
  float m = BokehMask(ro, rd, p - vec3(.08, 0., 0.), size, blur) +
            BokehMask(ro, rd, p + vec3(.08, 0., 0.), size, blur);

#ifdef HIGH_QUALITY
  float bs = n.z * 3.; // start braking at random distance
  float brake = S(bs, bs + .01, z);
  brake *= S(bs + .01, bs, z - .5 * n.y); // n.y = random brake 
duration

  m += (BokehMask(ro, rd, p + vec3(.1, 0., 0.), size, blur) +
        BokehMask(ro, rd, p - vec3(.1, 0., 0.), size, blur)) *
       brake;
#endif

  float refSize = size * 2.5;
  m += BokehMask(ro, rd, p + vec3(-.09, -.2, 0.), refSize, .8);
  m += BokehMask(ro, rd, p + vec3(.09, -.2, 0.), refSize, .8);
  vec3 col = tailLightCol * m * ft;

  float b = BokehMask(ro, rd, p + vec3(.12, 0., 0.), size, blur);
  b += BokehMask(ro, rd, p + vec3(.12, -.2, 0.), refSize, .8) * .2;

  vec3 blinker = vec3(1., .7, .2);
  blinker *= S(1.5, 1.4, z) * S(.2, .3, z);
  blinker *= sat(sin(t * 200.) * 100.);
  blinker *= laneId;
  col += blinker * b;

  return col;
}

vec3 StreetLights(float i, float t) {
  float side = sign(rd.x);
  float offset = max(side, 0.) * (1. / 16.);
  float z = fract(i - t + offset);
  vec3 p = vec3(2. * side, 2., z * 60.);
  float d = length(p - ro);
  float blur = .1;
  vec3 rp = ClosestPoint(ro, rd, p);
  float distFade = Remap(1., .7, .1, 1.5, 1. - pow(1. - z, 6.));
  distFade *= (1. - z);
  float m = BokehMask(ro, rd, p, .05 * d, blur) * distFade;

  return m * streetLightCol;
}

vec3 EnvironmentLights(float i, float t) {
  float n = N(i + floor(t));

  float side = sign(rd.x);
  float offset = max(side, 0.) * (1. / 16.);
  float z = fract(i - t + offset + fract(n * 234.));
  float n2 = fract(n * 100.);
  vec3 p = vec3((3. + n) * side, n2 * n2 * n2 * 1., z * 60.);
  float d = length(p - ro);
  float blur = .1;
  vec3 rp = ClosestPoint(ro, rd, p);
  float distFade = Remap(1., .7, .1, 1.5, 1. - pow(1. - z, 6.));
  float m = BokehMask(ro, rd, p, .05 * d, blur);
  m *= distFade * distFade * .5;

  m *= 1. - pow(sin(z * 6.28 * 20. * n) * .5 + .5, 20.);
  vec3 randomCol = vec3(fract(n * -34.5), fract(n * 4572.),
                        fract(n * 1264.));
  vec3 col = mix(tailLightCol, streetLightCol, fract(n * -65.42));
  col = mix(col, randomCol, n);
  return m * col * .2;
}

void main(void) {
  float t = iTime;
  vec3 col = vec3(0.);
  vec2 uv = /* -1.0 + 2.0 * */ vUV.xy; // 0 <> 1

  uv -= .5;
  uv.x *= iResolution.x / iResolution.y;

  vec2 iMouse = vec2(0.0, 0.0);
  vec2 mouse = iMouse.xy / iResolution.xy;

  vec3 pos = vec3(.3, .15, 0.);

  float bt = t * 5.;
  float h1 = N(floor(bt));
  float h2 = N(floor(bt + 1.));
  float bumps = mix(h1, h2, fract(bt)) * .1;
  bumps = bumps * bumps * bumps * CAM_SHAKE;

  pos.y += bumps;
  float lookatY = pos.y + bumps;
  vec3 lookat = vec3(0.3, lookatY, 1.);
  vec3 lookat2 = vec3(0., lookatY, .7);
  lookat = mix(lookat, lookat2, sin(t * .1) * .5 + .5);

  uv.y += bumps * 4.;
  CameraSetup(uv, pos, lookat, 2., mouse.x);

  t *= .03;
  t += mouse.x;

// fix for GLES devices by MacroMachines
#ifdef GL_ES
  const float stp = 1. / 8.;
#else
  float stp = 1. / 8.;
#endif

  for (float i = 0.; i < 1.; i += stp) {
    col += StreetLights(i, t);
  }

  for (float i = 0.; i < 1.; i += stp) {
    float n = N(i + floor(t));
    col += HeadLights(i + n * stp * .7, t);
  }

#ifndef GL_ES
#ifdef HIGH_QUALITY
  stp = 1. / 32.;
#else
  stp = 1. / 16.;
#endif
#endif

  for (float i = 0.; i < 1.; i += stp) {
    col += EnvironmentLights(i, t);
  }

  col += TailLights(0., t);
  col += TailLights(.5, t);

  col += sat(rd.y) * vec3(.6, .5, .9);

  gl_FragColor = vec4(col, 0.);
}
)ShaderCode";

public:
  ShaderMaterialTheDriveHomeScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas), _time{0.f}, _shaderMaterial{nullptr}
  {
    // Vertex shader
    Effect::ShadersStore()["customVertexShader"] = customVertexShader;

    // Fragment shader
    Effect::ShadersStore()["customFragmentShader"] = customFragmentShader;
  }

  ~ShaderMaterialTheDriveHomeScene() override = default;

  const char* getName() override
  {
    return "Shader Material The Drive Home Scene";
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

}; // end of class ShaderMaterialTheDriveHomeScene

BABYLON_REGISTER_SAMPLE("Shader Material", ShaderMaterialTheDriveHomeScene)

} // end of namespace Samples
} // end of namespace BABYLON