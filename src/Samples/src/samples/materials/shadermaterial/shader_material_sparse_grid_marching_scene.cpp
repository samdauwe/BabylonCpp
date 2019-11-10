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

class ShaderMaterialSparseGridMarchingScene : public IRenderableScene {

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
  // Sparse grid marching ( https://www.shadertoy.com/view/XlfGDs )
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

// Sparse grid marching by nimitz (@stormoid)

/*
  Somewhat efficient way of marching through
  a sparse repeated 3d grid (works in any dimension).

  Getting aligned ray-box intersection to get cell exit
  distance when a cell is empty.  I think this is similar
  to the way iq has done voxel raymarching (haven't studied
  his voxelizer code)
*/

#define ITR 100
#define FAR 100.
#define time iTime

//#define SHOW_CELLS

const float c = 2.6;    // Repetition factor
const float sz = .65;   // Object size (no more than 1/3 the
                        // rep factor since objects are rotating)
const float scr = 0.94; // Scarcity

vec3 rd = vec3(0);
float matid = 0.;

// Helper consts
const float ch = c * 0.5;
const float ch2 = ch + .01;
const float scr2 = scr + (1. - scr) * 1. / 3.;
const float scr3 = scr + (1. - scr) * 2. / 3.;

mat2 mm2(in float a) {
  float c = cos(a), s = sin(a);
  return mat2(c, s, -s, c);
}
float hash(vec2 x) {
  return fract(cos(dot(x.xy, vec2(2.31, 53.21)) * 124.123) * 412.0);
}

// From Dave (https://www.shadertoy.com/view/XlfGWN)
float hash13(vec3 p) {
  p = fract(p * vec3(.16532, .17369, .15787));
  p += dot(p.xyz, p.yzx + 19.19);
  return fract(p.x * p.y * p.z);
}

float dBox(in vec3 ro) {
  vec3 m = 1.2 / rd;
  vec3 t = -m * ro + abs(m) * ch2;
  return min(min(t.x, t.y), t.z);
}

float hash(in float n) { return fract(sin(n) * 43758.5453); }

// knighty's cut&fold technique
float tetra(in vec3 p, in float sz) {
  vec3 q = vec3(0.816497, 0., 0.57735) * sz;
  const vec3 nc = vec3(-0.5, -0.5, 0.7071);
  p.xy = abs(p.xy);
  float t = 2. * dot(p, nc);
  p -= t * nc;
  p.xy = abs(p.xy);
  t = 2. * min(0., dot(p, nc));
  p = (p - t * nc) - q;
  return length(p.yz) - 0.05;
}

vec3 maptex(vec3 p) {
  p.z -= hash(floor(p.x / c + 1.)) * (time * 12. + 92.);
  p.y -= hash(floor(p.z / c + 1.)) * (time * 3. + 89.);
  vec3 iq = floor(p);
  p = fract(p) - 0.5;
  p.xz *= mm2(time * 2. + iq.x);
  p.xy *= mm2(time * 0.6 + iq.y);
  return p;
}

vec3 maptex2(vec3 p) {
  vec3 g = p;
  vec3 gid = floor(p / 20.);
  g.xy *= mm2(-gid.z * .4);
  g.xz = mod(g.xz, 20.) - 10.;
  return g;
}

float slength(in vec2 p) { return max(abs(p.x), abs(p.y)); }
float map(vec3 p) {
  vec3 g = p;
  vec3 gid = floor(p / 20.);
  // movement
  p.z -= hash(floor(p.x / c + 1.)) * (time * 12. + 92.);
  p.y -= hash(floor(p.z / c + 1.)) * (time * 3. + 89.);

  vec3 iq = floor(p / c);
  vec3 q = mod(p, c) - ch;

  matid = dot(iq, vec3(1, 11, 101));

  float rn = hash13(iq);
  float d = dBox(q); // Base distance is cell exit distance

  q.xz *= mm2(time * 2. + iq.x);
  q.xy *= mm2(time * 0.6 + iq.y);

  if (rn >= scr3)
    d = min(d, length(q) - sz);
  else if (rn >= scr2)
    d = min(d, tetra(q, sz));
  else if (rn >= scr)
    d = min(d, dot(abs(q), vec3(0.57735)) - sz);

  // columns
  g.xy *= mm2(-gid.z * .4);
  g.xz = mod(g.xz, 20.) - 10.;
  float clm = slength(g.zx) - 2.;
  if (clm < d)
    matid = 1.;
  d = min(d, clm);

  return d;
}

float march(in vec3 ro, in vec3 rd) {
  float precis = 0.005;
  float h = precis * 2.0;
  float d = 0.;
  for (int i = 0; i < ITR; i++) {
    if (abs(h) < precis || d > FAR)
      break;
    d += h;
    float res = map(ro + rd * d);
    h = res;
#ifdef SHOW_CELLS
    rd.xy *= d * 0.00001 + .996;
#endif
  }
  return d;
}

vec3 rotx(vec3 p, float a) {
  float s = sin(a), c = cos(a);
  return vec3(p.x, c * p.y - s * p.z, s * p.y + c * p.z);
}

vec3 roty(vec3 p, float a) {
  float s = sin(a), c = cos(a);
  return vec3(c * p.x + s * p.z, p.y, -s * p.x + c * p.z);
}

// From TekF (https://www.shadertoy.com/view/ltXGWS)
float cells(in vec3 p) {
  p = fract(p / 2.0) * 2.0;
  p = min(p, 2.0 - p);
  return min(length(p), length(p - 1.0));
}

vec3 bg(in vec3 d) {
  return abs(sin(vec3(1., 2., 2.5) + sin(time * 0.05))) * 0.4 +
         .35 * (cells(d * .5) * 0.4 + 0.6);
}

float bnoise(in vec3 p) {
  float n = cells(p * 15.);
  n = max(n, cells(p * 12.));
  n = (n + exp(n * 3. - 4.)) * .002;
  return n;
}

vec3 bump(in vec3 p, in vec3 n, in float ds) {
  vec2 e = vec2(.01, 0);
  float n0 = bnoise(p);
  vec3 d = vec3(bnoise(p + e.xyy) - n0, bnoise(p + e.yxy) - n0,
                bnoise(p + e.yyx) - n0) /
           e.x;
  n = normalize(n + d * 5. / clamp(sqrt(ds), 1., 5.));
  return n;
}

vec3 normal(in vec3 p) {
  vec2 e = vec2(-1., 1.) * 0.005;
  return normalize(e.yxx * map(p + e.yxx) + e.xxy * map(p + e.xxy) +
                   e.xyx * map(p + e.xyx) + e.yyy * map(p + e.yyy));
}

vec3 shade(in vec3 p, in vec3 rd, in vec3 lpos, in float d) {
  vec3 n = normal(p);
  vec3 col = vec3(1);
  if (matid < 0.) {
    col = sin(vec3(1, 2, 3.) + matid * .002) * 0.3 + 0.4;
    n = bump(maptex(p * 0.5), n, d);
  } else {
    n = bump(maptex2(p * 0.25), n, d);
  }

  vec3 r = reflect(rd, n);
  vec3 ligt = normalize(lpos - p);
  float atn = distance(lpos, p);
  float refl = pow(dot(rd, r) * .75 + 0.75, 2.);
  float dif = clamp(dot(n, ligt), 0., 1.);
  float bac = clamp(dot(n, vec3(-ligt)), 0., 1.);
  col = col * bac * 0.2 + col * dif * .3 + bg(r) * dif * refl * 0.2;
  col *= clamp((1. - exp(atn * .15 - 5.)), 0., 1.);

  return col;
}

// From mu6k
vec3 cc(vec3 col, float f1, float f2) {
  float sm = dot(col, vec3(1));
  return mix(col, vec3(sm) * f1, sm * f2);
}

// from p_malin
vec3 flare(in vec3 ro, in vec3 rd, in float t,
           in vec3 lpos, in float spread) {
  float dotl = dot(lpos - ro, rd);
  dotl = clamp(dotl, 0.0, t);

  vec3 near = ro + rd * dotl;
  float ds = dot(near - lpos, near - lpos);
  return (vec3(1., 0.7, 0.3) * .01 / (ds));
}

void main(void) {
  vec2 p = -1.0 + 2.0 * vUV.xy;
  p.x *= iResolution.x / iResolution.y;
  vec3 iMouse = vec3(0.0, 0.0, 0.0);
  vec2 mo = iMouse.xy / iResolution.xy - .5;
  mo = (mo == vec2(-.5)) ? mo = vec2(0.) : mo;
  mo *= 0.5;
  mo += vec2(0.4, -0.4);
  mo.x *= iResolution.x / iResolution.y;

  vec3 ro = vec3(0, 0, -time * 20.);
  rd = normalize(vec3(p, -1.));
  // rd.z += length(rd)*0.5;
  rd = rotx(rd, mo.y + sin(time * 0.4) * 0.5);
  rd = roty(rd, -mo.x + cos(time) * 0.1);

  float rz = march(ro, rd);

  vec3 col = vec3(0.05);
  vec3 bgc = bg(rd * 5.) * 0.5;
  vec3 pos = ro + rd * FAR;
  vec3 lpos = ro + vec3(0, sin(time * 1.) * 2.,
                        -15. + sin(time * 0.5) * 10.);

  if (rz < FAR) {
    pos = ro + rz * rd;
    float d = distance(ro, pos);
    col = shade(pos, rd, lpos, d);
  }

  col = mix(col, bgc, smoothstep(FAR - 40., FAR, rz));
  col += flare(ro, rd, rz, lpos, 1.);

// Post
#if 1
  col = clamp(col, 0., 1.) * 1.3;
  col -= hash(col.xy + p.xy) * .017;                // Noise
  col -= smoothstep(0.15, 1.9,
                    length(p * vec2(1, 1.5))) * .6; // Vign
  col = pow(clamp(col, 0., 1.), vec3(0.8));
  col = cc(col, .6, .3); // Color modifier
#endif

  gl_FragColor = vec4(col, 1.0);
}
)ShaderCode";

public:
  ShaderMaterialSparseGridMarchingScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas), _time{0.f}, _shaderMaterial{nullptr}
  {
    // Vertex shader
    Effect::ShadersStore()["customVertexShader"] = customVertexShader;

    // Fragment shader
    Effect::ShadersStore()["customFragmentShader"] = customFragmentShader;
  }

  ~ShaderMaterialSparseGridMarchingScene() override = default;

  const char* getName() override
  {
    return "Shader Material Sparse Grid Marching Scene";
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

}; // end of class ShaderMaterialSparseGridMarchingScene

BABYLON_REGISTER_SAMPLE("Shader Material", ShaderMaterialSparseGridMarchingScene)

} // end of namespace Samples
} // end of namespace BABYLON