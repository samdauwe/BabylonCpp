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

class ShaderMaterialOperaIslandScene : public IRenderableScene {

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
  // Opera Island ( https://www.shadertoy.com/view/MlSBWG )
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
uniform vec2 iMouse;
uniform vec2 iResolution;

// 'Opera Island' by dr2 - 2018
// License: Creative Commons Attribution-NonCommercial-ShareAlike 3.0
// Unported License

float PrRoundBox2Df(vec2 p, vec2 b, float r);
float PrSphDf(vec3 p, float r);
float PrSphAnDf(vec3 p, float r, float w);
float PrCylDf(vec3 p, float r, float h);
float PrTorusDf(vec3 p, float ri, float rc);
float SmoothBump(float lo, float hi, float w, float x);
vec2 Rot2D(vec2 q, float a);
float Noisefv2(vec2 p);
float Fbm1(float p);
float Fbm2(vec2 p);
vec3 VaryNf(vec3 p, vec3 n, float f);

vec3 sunDir = vec3(0., 0., 0.);
float tCur = 0., dstFar = 0.;
int idObj;
const float pi = 3.14159;

#define DMIN(id) if (d < dMin) { dMin = d;  idObj = id; }

float ObjDf(vec3 p) {
  vec3 q, qq;
  float dMin, d;
  dMin = dstFar;
  q = p;
  q.x = 3.6 - abs(q.x);
  q.z = 3.6 + q.z;
  d = max(max(PrSphAnDf(vec3(abs(q.x) + 2.2, q.y - 0.6, q.z),
                             5., 0.03),
              10. - length(vec2(q.y - 0.6, q.z + 9.))),
          1. - q.y);
  DMIN(1);
  d = max(max(length(vec2(q.x, q.z - 6.1)) - 5.5, 0.1 - q.y),
          PrSphDf(vec3(abs(q.x) + 2.2, q.y - 0.6, q.z), 5.));
  DMIN(2);
  q = p;
  q.xz = 3.6 - abs(q.xz);
  d = max(
      min(max(PrSphAnDf(vec3(abs(q.x) + 1.8, q.y - 0.2, q.z + 1.),
                             4.7, 0.03),
              8.8 - length(vec2(q.y - 0.6, q.z + 9.))),
          max(PrSphAnDf(vec3(abs(q.x) + 1.4, q.y - 0.1, q.z + 1.6),
                        4.1, 0.03),
              8. - length(vec2(q.y - 0.6, q.z + 9.3)))),
      1. - q.y);
  DMIN(1);
  d = min(max(max(length(vec2(q.x, q.z - 4.5)) - 5.3, 0.1 - q.y),
              PrSphDf(vec3(abs(q.x) + 1.8, q.y - 0.2, q.z + 1.),
                           4.7)),
          max(max(length(vec2(q.x, q.z - 3.3)) - 5., 0.1 - q.y),
              PrSphDf(vec3(abs(q.x) + 1.4, q.y - 0.1, q.z + 1.6),
                      4.1)));
  DMIN(2);
  d = min(min(max(PrCylDf(vec3(abs(q.x) + 2.2, q.y - 0.6, q.z).xzy,
                               5.05, 0.5),
                  -q.z - 1.4),
              max(PrCylDf(vec3(q.x, q.y - 0.85, q.z + 0.7).xzy, 2.5,
                               0.25),
                  q.z + 1.4)),
          PrCylDf(vec3(q.x, q.y - 0.35, q.z + 0.7).xzy, 2.9, 0.25));
  DMIN(3);
  d = max(
      PrTorusDf(vec3(q.x, abs(q.y - 1.27) - 0.07, q.z + 0.7).xzy,
                     0.015, 2.45),
      q.z + 1.1);
  qq = q;
  qq.z += 0.7;
  qq.xz =
      Rot2D(qq.xz,
            2. * pi * (floor(64. * atan(qq.z, -qq.x) / (2. * pi) +
            0.5) / 64.));
  d = min(d, max(PrCylDf(vec3(qq.x + 2.45, qq.y - 1.2, qq.z).xzy,
                              0.015, 0.14),
                 q.z + 1.1));
  DMIN(4);
  q = p;
  d = max(PrRoundBox2Df(vec2(q.xz), vec2(3.9, 4.7), 3.2),
                        abs(q.y) - 0.1);
  DMIN(5);
  return dMin;
}

float ObjRay(vec3 ro, vec3 rd) {
  float dHit, d;
  dHit = 0.;
  for (int j = 0; j < 120; j++) {
    d = ObjDf(ro + rd * dHit);
    if (d < 0.0005 || dHit > dstFar)
      break;
    dHit += d;
  }
  return dHit;
}

vec3 ObjNf(vec3 p) {
  vec4 v;
  vec2 e = vec2(0.0005, -0.0005);
  v = vec4(ObjDf(p + e.xxx), ObjDf(p + e.xyy), ObjDf(p + e.yxy),
           ObjDf(p + e.yyx));
  return normalize(vec3(v.x - v.y - v.z - v.w) + 2. * v.yzw);
}

float WaveHt(vec2 p) {
  mat2 qRot = mat2(0.8, -0.6, 0.6, 0.8);
  vec4 t4, v4;
  vec2 q, t, tw;
  float wFreq, wAmp, h;
  q = 0.5 * p + vec2(0., tCur);
  h = 0.6 * sin(dot(q, vec2(-0.05, 1.))) +
      0.45 * sin(dot(q, vec2(0.1, 1.2))) +
      0.3 * sin(dot(q, vec2(-0.2, 1.4)));
  q = 2. * p;
  wFreq = 1.;
  wAmp = 1.;
  tw = 0.1 * tCur * vec2(1., -1.);
  for (int j = 0; j < 3; j++) {
    q *= qRot;
    t4 = q.xyxy * wFreq + tw.xxyy;
    t = vec2(Noisefv2(t4.xy), Noisefv2(t4.zw));
    t4 += 2. * t.xxyy - 1.;
    v4 = (1. - abs(sin(t4))) * (abs(sin(t4)) + abs(cos(t4)));
    t = 1. - sqrt(v4.xz * v4.yw);
    t *= t;
    t *= t;
    h += wAmp * dot(t, t);
    wFreq *= 2.;
    wAmp *= 0.5;
  }
  return h;
}

vec3 WaveNf(vec3 p, float d) {
  vec3 vn;
  vec2 e;
  e = vec2(max(0.01, 0.005 * d * d), 0.);
  p *= 0.5;
  vn.xz = 0.2 * (WaveHt(p.xz) - vec2(WaveHt(p.xz + e.xy),
                                     WaveHt(p.xz + e.yx)));
  vn.y = e.x;
  return normalize(vn);
}

vec3 SkyHrzCol(vec3 ro, vec3 rd) {
  vec3 col;
  float sd, a, f;
  a = atan(rd.z, rd.x) + 0.001 * tCur;
  if (rd.y < 0.01 * Fbm1(32. * a) + 0.005)
    col = mix(vec3(0.25, 0.3, 0.25), vec3(0.3, 0.35, 0.33),
              0.5 + 0.5 * dot(normalize(rd.xz),
              -normalize(sunDir.xz))) *
          (1. - 0.3 * Fbm2(128. * vec2(a, rd.y)));
  else {
    ro.x += 0.5 * tCur;
    f = Fbm2(0.05 * (rd.xz * (100. - ro.y) / rd.y + ro.xz));
    sd = pow(max(dot(rd, sunDir), 0.), 64.);
    col = vec3(0.2, 0.3, 0.5) + 0.1 * pow(1. - max(rd.y, 0.), 4.) +
          (0.35 * sd + 0.65 * min(pow(sd, 4.), 0.3)) *
          vec3(1., 1., 0.3);
    col = mix(col, vec3(0.85), clamp(f * rd.y + 0.1, 0., 1.));
  }
  return col;
}

float ObjSShadow(vec3 ro, vec3 rd) {
  float sh, d, h;
  sh = 1.;
  d = 0.1;
  for (int j = 0; j < 30; j++) {
    h = ObjDf(ro + rd * d);
    sh = min(sh, smoothstep(0., 0.05 * d, h));
    d += min(0.07, 3. * h);
    if (sh < 0.001)
      break;
  }
  return 0.7 + 0.3 * sh;
}

vec3 ShowScene(vec3 ro, vec3 rd) {
  vec4 col4;
  vec3 col, vn;
  vec2 sf;
  float dstObj, dstWat, sh, s;
  bool watRefl, winfRefl;
  watRefl = false;
  winfRefl = false;
  dstObj = ObjRay(ro, rd);
  dstWat = (rd.y < 0.) ? -ro.y / rd.y : dstFar;
  if (dstWat < min(dstObj, dstFar)) {
    watRefl = true;
    ro += dstWat * rd;
    rd = reflect(rd, WaveNf(ro, dstWat));
    ro += 0.01 * rd;
    dstObj = ObjRay(ro, rd);
  }
  if (dstObj < dstFar) {
    sf = vec2(0.);
    ro += dstObj * rd;
    vn = ObjNf(ro);
    if (idObj == 1) {
      col4 = vec4(0.8, 0.8, 0.85, 0.05) *
             (1. - 0.1 * SmoothBump(0.45, 0.55, 0.02,
                         mod(8. * ro.z, 1.)));
      sf = vec2(32., 0.2);
    } else if (idObj == 2) {
      if (abs(mod(4. * (ro.x - 3.6 * sign(ro.x)), 1.) - 0.5) < 0.1 ||
          ro.y < 1.2) {
        if (ro.y < 1.2)
          col4 = 1.2 * vec4(0.6, 0.5, 0.4, 0.1);
        else
          col4 = vec4(0.5, 0.5, 0.55, 0.2);
      } else
        winfRefl = true;
    } else if (idObj == 3) {
      col4 = vec4(0.6, 0.5, 0.4, 0.1);
      if (abs(ro.y - 0.86) < 0.15 && abs(ro.z) > 5.35 ||
          abs(ro.y - 0.36) < 0.15 && abs(ro.z) > 3.9)
        winfRefl = (abs(mod(32. *
                                atan(ro.z - 4.3 * sign(ro.z),
                                     ro.x - 3.6 * sign(ro.x)) /
                                pi,
                            1.) -
                        0.5) > 0.15);
      else if (abs(abs(ro.y - 0.61) - 0.25) < 0.15 &&
               abs(ro.z) > 0.4 &&
               abs(ro.z) < 3.4)
        winfRefl = (abs(mod(48. *
                                atan(ro.z - 3.6 * sign(ro.z),
                                     ro.x - 1.4 * sign(ro.x)) /
                                pi,
                            1.) -
                        0.5) > 0.15);
      else {
        s = length(vec2(ro.z - 4.3 * sign(ro.z),
                        ro.x - 3.6 * sign(ro.x)));
        if (s < 2.4)
          col4 *= 1. - 0.1 * SmoothBump(0.45, 0.55, 0.02,
                                        mod(4. * s, 1.));
        sf = vec2(32., 0.2);
      }
    } else if (idObj == 4) {
      col4 = vec4(0.5, 0.5, 0.6, 0.2);
    } else if (idObj == 5) {
      col4 = vec4(0.3, 0.5, 0.3, 0.05) *
             (1. - 0.2 * Fbm2(16. * ro.xz));
      sf = vec2(16., 1.);
    }
    if (sf.x > 0.)
      vn = VaryNf(sf.x * ro, vn, sf.y);
    if (winfRefl) {
      rd = reflect(rd, vn);
      if (rd.y > 0.)
        col = SkyHrzCol(ro, rd);
      else {
        watRefl = true;
        dstWat = -ro.y / rd.y;
        ro += dstWat * rd;
        col = SkyHrzCol(ro, reflect(rd, WaveNf(ro, dstWat)));
      }
      col = mix(vec3(0.3, 0.3, 0.2), col, 0.3);
    } else {
      sh = ObjSShadow(ro, sunDir);
      col = col4.rgb * (0.2 + 0.1 * max(vn.y, 0.) +
                        0.8 * sh * max(dot(vn, sunDir), 0.)) +
            col4.a * sh *
            pow(max(dot(normalize(sunDir - rd), vn), 0.), 128.);
    }
  } else if (rd.y < 0.) {
    watRefl = true;
    dstWat = -ro.y / rd.y;
    ro += dstWat * rd;
    col = SkyHrzCol(ro, reflect(rd, WaveNf(ro, dstWat)));
  } else {
    col = SkyHrzCol(ro, rd);
  }
  if (watRefl) {
    col = mix(mix(vec3(0.01, 0.01, 0.), vec3(0., 0.05, 0.05),
                  smoothstep(0.4, 0.6, Fbm2(0.5 * ro.xz))),
              0.8 * col, 1. - pow(abs(rd.y), 4.));
  }
  return clamp(col, 0., 1.);
}

void main(void) {
  mat3 vuMat;
  vec4 mPtr;
  vec3 ro, rd;
  vec2 canvas, uv, ori, ca, sa;
  float el, az;
  canvas = iResolution.xy;
  uv = 2. * (-1.0 + 2.0 * vUV.xy);
  uv.x *= canvas.x / canvas.y;
  tCur = iTime;
  mPtr = vec4(iMouse, 0., 0.);
  mPtr.xy = mPtr.xy / canvas - 0.5;
  az = 0.25 * pi;
  el = -0.05 * pi;
  if (mPtr.z > 0.) {
    az += 2. * pi * mPtr.x;
    el += 0.2 * pi * mPtr.y;
  } else {
    az -= 0.02 * pi * tCur;
    el += 0.04 * pi * cos(0.02 * pi * tCur);
  }
  el = clamp(el, -0.1 * pi, -0.01 * pi);
  ori = vec2(el, az);
  ca = cos(ori);
  sa = sin(ori);
  vuMat = mat3(ca.y, 0., -sa.y, 0., 1., 0., sa.y, 0., ca.y) *
          mat3(1., 0., 0., 0., ca.x, -sa.x, 0., sa.x, ca.x);
  ro = vuMat * vec3(0., 1., -40.);
  rd = vuMat * normalize(vec3(uv, 6.));
  dstFar = 100.;
  sunDir = normalize(vec3(1., 1., -1.));
  gl_FragColor = vec4(ShowScene(ro, rd), 1.);
}

float PrRoundBox2Df(vec2 p, vec2 b, float r) {
  return length(max(abs(p) - b, 0.)) - r;
}

float PrSphDf(vec3 p, float r) { return length(p) - r; }

float PrSphAnDf(vec3 p, float r, float w) {
  return abs(length(p) - r) - w;
}

float PrCylDf(vec3 p, float r, float h) {
  return max(length(p.xy) - r, abs(p.z) - h);
}

float PrTorusDf(vec3 p, float ri, float rc) {
  return length(vec2(length(p.xy) - rc, p.z)) - ri;
}

float SmoothBump(float lo, float hi, float w, float x) {
  return (1. - smoothstep(hi - w, hi + w, x)) *
               smoothstep(lo - w, lo + w, x);
}

vec2 Rot2D(vec2 q, float a) {
  return q * cos(a) * vec2(1., 1.) + q.yx * sin(a) * vec2(-1., 1.);
}

const float cHashM = 43758.54;

vec2 Hashv2f(float p) {
  return fract(sin(p + vec2(0., 1.)) * cHashM);
}

vec2 Hashv2v2(vec2 p) {
  vec2 cHashVA2 = vec2(37., 39.);
  return fract(sin(vec2(dot(p, cHashVA2), dot(p + vec2(1., 0.),
               cHashVA2))) * cHashM);
}

float Noiseff(float p) {
  vec2 t;
  float ip, fp;
  ip = floor(p);
  fp = fract(p);
  fp = fp * fp * (3. - 2. * fp);
  t = Hashv2f(ip);
  return mix(t.x, t.y, fp);
}

float Noisefv2(vec2 p) {
  vec2 t, ip, fp;
  ip = floor(p);
  fp = fract(p);
  fp = fp * fp * (3. - 2. * fp);
  t = mix(Hashv2v2(ip), Hashv2v2(ip + vec2(0., 1.)), fp.y);
  return mix(t.x, t.y, fp.x);
}

float Fbm1(float p) {
  float f, a;
  f = 0.;
  a = 1.;
  for (int j = 0; j < 5; j++) {
    f += a * Noiseff(p);
    a *= 0.5;
)ShaderCode"
// Shader code string too long for msvc, it is splitted / joined here
R"ShaderCode(

    p *= 2.;
  }
  return f * (1. / 1.9375);
}

float Fbm2(vec2 p) {
  float f, a;
  f = 0.;
  a = 1.;
  for (int j = 0; j < 5; j++) {
    f += a * Noisefv2(p);
    a *= 0.5;
    p *= 2.;
  }
  return f * (1. / 1.9375);
}

float Fbmn(vec3 p, vec3 n) {
  vec3 s;
  float a;
  s = vec3(0.);
  a = 1.;
  for (int j = 0; j < 5; j++) {
    s += a * vec3(Noisefv2(p.yz), Noisefv2(p.zx), Noisefv2(p.xy));
    a *= 0.5;
    p *= 2.;
  }
  return dot(s, abs(n));
}

vec3 VaryNf(vec3 p, vec3 n, float f) {
  vec3 g;
  vec2 e = vec2(0.1, 0.);
  g = vec3(Fbmn(p + e.xyy, n), Fbmn(p + e.yxy, n),
           Fbmn(p + e.yyx, n)) - Fbmn(p, n);
  return normalize(n + f * (g - n * dot(n, g)));
}
)ShaderCode";

public:
  ShaderMaterialOperaIslandScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas), _time{0.f}, _shaderMaterial{nullptr}
  {
    // Vertex shader
    Effect::ShadersStore()["customVertexShader"] = customVertexShader;

    // Fragment shader
    Effect::ShadersStore()["customFragmentShader"] = customFragmentShader;
  }

  ~ShaderMaterialOperaIslandScene() override = default;

  const char* getName() override
  {
    return "Shader Material Opera Island Scene";
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

}; // end of class ShaderMaterialOperaIslandScene

BABYLON_REGISTER_SAMPLE("Shader Material", ShaderMaterialOperaIslandScene)

} // end of namespace Samples
} // end of namespace BABYLON