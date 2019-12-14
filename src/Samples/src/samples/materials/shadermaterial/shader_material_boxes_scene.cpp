#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_shaders_store.h>
#include <babylon/materials/shader_material.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

class ShaderMaterialBoxesScene : public IRenderableScene {

public:
  static constexpr const char* customVertexShader = R"ShaderCode(
#ifdef GL_ES
precision highp float;
#endif

// Attributes
attribute vec3 position;
attribute vec3 normal;
attribute vec2 uv;

// Uniforms
uniform mat4 worldViewProjection;
uniform float time;

varying vec3 vPosition;
varying vec3 vNormal;
varying vec2 vUV;

void main(void) {
  vec3 v = position;
  gl_Position = worldViewProjection * vec4(v, 1.0);
  vPosition = position;
  vNormal = normal;
  vUV = uv;
}
)ShaderCode";

  static constexpr const char* custom1FragmentShader = R"ShaderCode(
#ifdef GL_ES
precision highp float;
#endif

varying vec3 vPosition;
varying vec3 vNormal;
varying vec2 vUV;

uniform mat4 worldViewProjection;
uniform float time;

float rand(vec2 n) {
  return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}

float noise(vec2 n) {
  const vec2 d = vec2(0.0, 1.0);
  vec2 b = floor(n), f = smoothstep(vec2(0.0), vec2(1.0), fract(n));
  return mix(mix(rand(b), rand(b + d.yx), f.x),
             mix(rand(b + d.xy), rand(b + d.yy), f.x), f.y);
}

float fbm(vec2 n) {
  float total = 0.0, amplitude = 1.0;
  for (int i = 0; i < 7; i++) {
    total += noise(n) * amplitude;
    n += n;
    amplitude *= 0.5;
  }
  return total;
}

void main(void) {
  vec2 t = vUV * vec2(2.0, 1.0) - time * 3.0;
  vec2 t2 = (vec2(1, -1) + vUV) * vec2(2.0, 1.0) - time * 3.0;
  float ycenter = fbm(t) * 0.5;
  float ycenter2 = fbm(t2) * 0.5;
  float diff = abs(vUV.y - ycenter);
  float c1 = 1.0 - mix(0.0, 1.0, diff * 20.0);
  float diff2 = abs(vUV.y - ycenter2);
  float c2 = 1.0 - mix(0.0, 1.0, diff2 * 20.0);
  float c = max(c1, c2);
  vec3 cout = vec3(c * 0.2, 0.6 * c2, c);
  if (cout.x < .2 && cout.y < .2 && cout.z < .2) {
    if (mod(vUV.x * 50., 2.0) < 1.7 && mod(vUV.y * 50., 2.0) < 1.7) {
      discard;
    } else {
      cout = vec3(0.7, 0.7, 0.7);
    }
  }

  gl_FragColor = vec4(cout, 1.);
}
)ShaderCode";

  static constexpr const char* custom2FragmentShader = R"ShaderCode(
#ifdef GL_ES
precision highp float;
#endif

varying vec3 vPosition;
varying vec3 vNormal;
varying vec2 vUV;

uniform mat4 worldViewProjection;
uniform float time;

void main(void) {
  vec2 mv = vec2(.5, .5);
  float r = .3 + mod(time * .1, .4);
  vec3 color = vec3(smoothstep(r, r + .01, distance(mv, vUV)));
  if (color.x < .1 && color.y < .1)
    discard;
  gl_FragColor = vec4(color, 1.);
}
)ShaderCode";

  static constexpr const char* custom3FragmentShader = R"ShaderCode(
#ifdef GL_ES
precision highp float;
#endif

varying vec3 vPosition;
varying vec3 vNormal;
varying vec2 vUV;

uniform mat4 worldViewProjection;
uniform float time;

float PI = 3.141592;
float STROKE = 0.3;

void main(void) {
  vec2 uv = vUV.xy * 5.;
  float freq1 = 0.5 * sin(time * 4. + uv.x * .125 + uv.y * .2) + 0.5;
  float circle = smoothstep(freq1 - STROKE, freq1,
                            cos(uv.x * 2.0 * PI) * cos(uv.y * 2.0 *
                            PI)) -
                 smoothstep(freq1, freq1 + STROKE,
                            cos(uv.x * 2.0 * PI) * cos(uv.y * 2.0 *
                            PI));
  gl_FragColor = vec4(circle * 0.2, circle * 0.3, circle, 1.);
}
)ShaderCode";

  // Latt1c3 ( https://www.shadertoy.com/view/MlX3RB )
  static constexpr const char* custom4FragmentShader = R"ShaderCode(
#ifdef GL_ES
precision highp float;
#endif

varying vec3 vPosition;
varying vec3 vNormal;
varying vec2 vUV;

uniform mat4 worldViewProjection;
uniform float time;

mat3 xrot(float t) {
  return mat3(1.0, 0.0, 0.0,
              0.0, cos(t), -sin(t),
              0.0, sin(t), cos(t));
}

mat3 yrot(float t) {
  return mat3(cos(t), 0.0, -sin(t),
              0.0, 1.0, 0.0,
              sin(t), 0.0, cos(t));
}

mat3 zrot(float t) {
  return mat3(cos(t), -sin(t), 0.0,
              sin(t), cos(t), 0.0,
              0.0, 0.0,1.0);
}

void main(void) {
  float PI = 3.141592;
  float STROKE = 0.3;
  vec2 uv = vUV.xy * 1.;
  vec3 eye = normalize(vec3(uv, 1.0 - dot(uv, uv) * 0.5));
  float tt = 0.0;
  float d = 0.0;
  vec3 col;
  for (int i = 0; i < 16; ++i) {
    vec3 pos = eye * tt;
    pos = pos * xrot(-PI / 4.0) * yrot(-PI / 4.0);
    float theta = time;
    pos = pos * xrot(theta) * yrot(theta) * zrot(theta);
    pos.z += time;
    pos.y += 0.25 + time;
    pos.x += 0.5 + time;
    vec3 coord = floor(pos);
    pos = (pos - coord) - 0.5;
    d = length(pos) - 0.2;
    float idx = dot(coord, vec3(1.0));
    idx = floor(fract(idx / 3.0) * 3.0);
    if (idx == 0.0) {
      col = vec3(1.0, 0.0, 0.0);
    } else if (idx == 1.0) {
      col = vec3(0.0, 1.0, 0.0);
    } else if (idx == 2.0) {
      col = vec3(0.0, 0.0, 1.0);
    }
    float k;
    k = length(pos.xy) - 0.05;
    if (k < d) {
      d = k;
      col = vec3(1.0, 1.0, 1.0);
    }
    k = length(pos.xz) - 0.05;
    if (k < d) {
      d = k;
      col = vec3(1.0, 1.0, 1.0);
    }
    k = length(pos.yz) - 0.05;
    if (k < d) {
      d = k;
      col = vec3(1.0, 1.0, 1.0);
    }
    tt += d;
  }
  float fog = 1.0 / (1.0 + tt * tt * 0.5 + d * 100.0);
  gl_FragColor = vec4(fog * col, 1.);
}
)ShaderCode";

  static constexpr const char* custom5FragmentShader = R"ShaderCode(
#ifdef GL_ES
precision highp float;
#endif

varying vec3 vPosition;
varying vec3 vNormal;
varying vec2 vUV;

uniform mat4 worldViewProjection;
uniform float time;

float r(float n) { return fract(abs(sin(n * 55.753) * 367.34)); }
float r(vec2 n) { return r(dot(n, vec2(2.46, -1.21))); }

vec3 color(float type) {
  float t = floor(type * 3.0);
  vec3 c1 = vec3(0.738, 0.067, 0.185);
  vec3 c2 = vec3(1.0);
  vec3 c3 = vec3(0.15);
  return mix(c1, mix(c2, c3, t - 1.0), clamp(t, 0.0, 1.0));
}

void main(void) {
  float PI = 3.141592;
  float STROKE = 0.3;
  vec2 uv = vUV.xy * 300.;
  float a = (radians(60.0));
  float zoom = 192.0;
  vec2 cc = (uv.xy + vec2(time * zoom, 0.0)) * vec2(sin(a), 1.0);
  cc = ((cc + vec2(cc.y, 0.0) * cos(a)) / zoom) +
       vec2(floor((cc.x - cc.y * cos(a)) / zoom * 4.0) / 4.0, 0.0);
  float type = (r(floor(cc * 4.0)) * 0.2 + r(floor(cc * 2.0)) * 0.3 +
                r(floor(cc)) * 0.5);
  vec3 n = color(type);
  float l = fract((fract(cc.y * 4.0) + fract(cc.x * 4.0) +
                   fract((cc.x - cc.y) * 4.0) * 0.5) /
                  2.5) *
                0.3 +
            0.7;
  gl_FragColor = vec4(n * l, 1.);
}
)ShaderCode";

  static constexpr const char* custom6FragmentShader = R"ShaderCode(
#ifdef GL_ES
precision highp float;
#endif

varying vec3 vPosition;
varying vec3 vNormal;
varying vec2 vUV;

uniform mat4 worldViewProjection;
uniform float time;

void main(void) {
  float PI = 3.141592;
  vec2 uv = vUV.xy * 300.;
  float STROKE = 0.3;
  float mag = 10.;
  float pulse = 0.;
  float gs = 500. / 20.;
  float ar = 1.;
  vec2 s = uv.xy / mag + pulse;
  vec2 p = uv.xy / 500. * s - s / 2.;
  float v = .0 + sin((p.x + .8 * time)) + sin((p.y + .8 * time) / 2.) 
            + sin((p.x + p.y + .9 * time) / 2.);
  p += s / 2. * vec2(sin(time / .9), cos(time / .6));
  v += sin(sqrt(p.x * p.x + p.y * p.y + 1.) + time);
  float R = sin(.2 * PI * v);
  float G = cos(.75 * PI * v);
  float B = sin(.9 * PI * v);
  R = ceil(R * 255. / 8.) * 8. / 256.;
  G = ceil(G * 255. / 16.) * 16. / 256.;
  B = ceil(B * 255. / 8.) * 8. / 256.;
  if (mod(R, 16.) < 1.)
    R = G * .5 + .5;
  vec3 col = vec3(R, G, B);
  col *= 0.4 * 1. / length(sin(1. * .1 * gs * p.x));
  col *= 0.8 * 1. / length(sin(ar * .1 * gs * p.y));
  col *= .33 * length(sin(5. * p.y * gs));
  col = clamp(col, vec3(.0), vec3(1.));
  gl_FragColor = vec4(col, 1.);
}
)ShaderCode";

  static constexpr const char* custom7FragmentShader = R"ShaderCode(
#ifdef GL_ES
precision highp float;
#endif

varying vec3 vPosition;
varying vec3 vNormal;
varying vec2 vUV;

uniform mat4 worldViewProjection;
uniform float time;

float dist(vec2 uv, vec2 p0) {
  vec2 off = abs(uv - p0) * 3.0;
  float p = sin(time) * 2.0 + 4.0;
  float r = pow(pow(off.x, p) + pow(off.y, p), 1.0 / p);
  vec2 ox = uv - p;
  float t = atan(ox.y, ox.x);
  return 1.0 -
         abs(1.0 - pow(mod(r * 8.0 / 
                (3.5 + sin(t * 4.0 + time)), 1.0), 2.0));
}

void main(void) {
  float PI = 3.141592;
  vec2 uv = vUV;
  float d1 = dist(uv, vec2(0.3, 0.3));
  float d2 = dist(uv, vec2(0.7, 0.3));
  float d3 = dist(uv, vec2(0.3, 0.7));
  float d4 = dist(uv, vec2(0.7, 0.7));
  float d = d1 - d2 - d3 + d4;
  gl_FragColor = vec4(d, d, d, 1.);
}
)ShaderCode";

  // Tileable Water Caustic ( https://www.shadertoy.com/view/MdlXz8 )
  static constexpr const char* custom8FragmentShader = R"ShaderCode(
#ifdef GL_ES
precision highp float;
#endif

varying vec3 vPosition;
varying vec3 vNormal;
varying vec2 vUV;

uniform mat4 worldViewProjection;
uniform float time;

float rand(vec2 n) {
  return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}

float noise(vec2 n) {
  const vec2 d = vec2(0.0, 1.0);
  vec2 b = floor(n), f = smoothstep(vec2(0.0), vec2(1.0), fract(n));
  return mix(mix(rand(b), rand(b + d.yx), f.x),
             mix(rand(b + d.xy), rand(b + d.yy), f.x), f.y);
}

float fbm(vec2 n) {
  float total = 0.0, amplitude = 1.0;
  for (int i = 0; i < 7; i++) {
    total += noise(n) * amplitude;
    n += n;
    amplitude *= 0.5;
  }
  return total;
}

mat2 rotate2d(float angle) {
  return mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
}

float variation(vec2 v1, vec2 v2, float strength, float speed) {
  return sin(dot(normalize(v1), normalize(v2)) * strength + time *
         speed) / 100.0;
}

vec3 paintCircle(vec2 uv, vec2 center, float rad, float width) {

  vec2 diff = center - uv;
  float len = length(diff);

  len += variation(diff, vec2(0.0, 1.0), 5.0, 2.0);
  len -= variation(diff, vec2(1.0, 0.0), 5.0, 2.0);

  float circle =
      smoothstep(rad - width, rad, len) - 
      smoothstep(rad, rad + width, len);
  return vec3(circle);
}
void main(void) {
  float PI = 3.141592;
  vec2 uv = vUV.xy * 1.01;
  
  float TAU = 6.28318530718;
  vec2 p = mod(uv * TAU, TAU) - 250.0;
  vec2 i = vec2(p);
  float c = 1.0;
  float inten = .005;
  for (int n = 0; n < 5; n++) {
    float t = time * (1.0 - (3.5 / float(n + 1)));
    i = p + vec2(cos(t - i.x) + sin(t + i.y), sin(t - i.y)
        + cos(t + i.x));
    c += 1.0 / length(vec2(p.x / (sin(i.x + t) / inten),
                           p.y / (cos(i.y + t) / inten)));
  }
  c /= float(5);
  c = 1.17 - pow(c, 1.4);
  vec3 colour = vec3(pow(abs(c), 8.0));
  colour = clamp(colour + vec3(0.0, 0.35, 0.5), 0.0, 1.0);
  // Flash tile borders...
  vec2 pixel = 2.0 / vec2(50., 50.).xy;
  uv *= 2.0;

  // Flash value.
  float f = 0.; // floor(mod(time*.5, 2.0));
  // Rule out first screen pixels and flash.
  vec2 first = step(pixel, uv) * f;
  // Add one line of pixels per tile.
  uv = step(fract(uv), pixel);
  // Yellow line
  colour = mix(colour, vec3(1.0, 1.0, 0.0),
               (uv.x + uv.y) * first.x * first.y);
  gl_FragColor = vec4(colour, 1.);
}
)ShaderCode";

  // Shiny Circle ( https://www.shadertoy.com/view/ltBXRc )
  static constexpr const char* custom9FragmentShader = R"ShaderCode(
#ifdef GL_ES
precision highp float;
#endif

varying vec3 vPosition;
varying vec3 vNormal;
varying vec2 vUV;

uniform mat4 worldViewProjection;
uniform float time;

mat2 rotate2d(float angle) {
  return mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
}

float variation(vec2 v1, vec2 v2, float strength, float speed) {
  return sin(dot(normalize(v1), normalize(v2))
                 * strength +
             time * speed) /
         100.0;
}

vec3 paintCircle(vec2 uv, vec2 center, float rad, float width) {
  vec2 diff = center - uv;
  float len = length(diff);
  len += variation(diff, vec2(0.0, 1.0), 5.0, 2.0);
  len -= variation(diff, vec2(1.0, 0.0), 5.0, 2.0);
  float circle = smoothstep(rad - width, rad, len)
                 - smoothstep(rad, rad + width, len);
  return vec3(circle);
}

void main(void) {
  float PI = 3.141592;
  vec2 uv = vUV.xy * 1.01;
  vec3 color;
  float radius = 0.35;
  vec2 center = vec2(0.5);
  color = paintCircle(uv, center, radius, 0.1);
  vec2 v = rotate2d(time) * uv;
  color *= vec3(v.x, v.y, 0.7 - v.y * v.x);
  color += paintCircle(uv, center, radius, 0.01);
  gl_FragColor = vec4(color, 1.);
}
)ShaderCode";

public:
  ShaderMaterialBoxesScene(ICanvas* iCanvas) : IRenderableScene(iCanvas), _time{0.f}
  {
    // Vertex shaders
    Effect::ShadersStore()["custom1VertexShader"] = customVertexShader;
    Effect::ShadersStore()["custom2VertexShader"] = customVertexShader;
    Effect::ShadersStore()["custom3VertexShader"] = customVertexShader;
    Effect::ShadersStore()["custom4VertexShader"] = customVertexShader;
    Effect::ShadersStore()["custom5VertexShader"] = customVertexShader;
    Effect::ShadersStore()["custom6VertexShader"] = customVertexShader;
    Effect::ShadersStore()["custom7VertexShader"] = customVertexShader;
    Effect::ShadersStore()["custom8VertexShader"] = customVertexShader;
    Effect::ShadersStore()["custom9VertexShader"] = customVertexShader;

    // Fragment shaders
    Effect::ShadersStore()["custom1FragmentShader"] = custom1FragmentShader;
    Effect::ShadersStore()["custom2FragmentShader"] = custom2FragmentShader;
    Effect::ShadersStore()["custom3FragmentShader"] = custom3FragmentShader;
    Effect::ShadersStore()["custom4FragmentShader"] = custom4FragmentShader;
    Effect::ShadersStore()["custom5FragmentShader"] = custom5FragmentShader;
    Effect::ShadersStore()["custom6FragmentShader"] = custom6FragmentShader;
    Effect::ShadersStore()["custom7FragmentShader"] = custom7FragmentShader;
    Effect::ShadersStore()["custom8FragmentShader"] = custom8FragmentShader;
    Effect::ShadersStore()["custom9FragmentShader"] = custom9FragmentShader;
  }

  ~ShaderMaterialBoxesScene() override = default;

  const char* getName() override
  {
    return "Shader Material Boxes Scene";
  }
  void initializeScene(ICanvas* canvas, Scene* scene) override
  {

    // Render camera
    auto camera1 = ArcRotateCamera::New("ArcRotateCamera", 1.f, 0.8f, 6.f, Vector3::Zero(), scene);
    camera1->attachControl(canvas, false);
    camera1->lowerRadiusLimit = 1.f;
    scene->activeCamera       = camera1;

    // Create a light
    PointLight::New("Omni", Vector3(-60.f, 60.f, 80.f), scene);

    // Box positions
    const std::array<Vector3, 9> boxPositions{
      {Vector3(-1.5f, 1.f, -1.5f), Vector3(0.f, 1.f, -1.5f), Vector3(1.5f, 1.f, -1.5f),
       Vector3(-1.5f, 1.f, 0.f), Vector3(0.f, 1.f, 0.f), Vector3(1.5, 1.f, 0.f),
       Vector3(-1.5, 1.f, 1.5), Vector3(0.f, 1.f, 1.5f), Vector3(1.5f, 1.f, 1.5f)}};

    // Create boxes
    for (unsigned int i = 0; i < boxPositions.size(); ++i) {
      // Create box
      auto id       = std::to_string(i + 1);
      auto box      = Mesh::CreateBox("box" + id, 1.f, scene);
      box->position = boxPositions[i];
      // box->enableEdgesRendering(0.99f);
      box->edgesWidth = 2.f;
      // Create shader material
      IShaderMaterialOptions shaderMaterialOptions;
      shaderMaterialOptions.attributes = {"position", "normal", "uv"};
      shaderMaterialOptions.uniforms   = {"time", "worldViewProjection"};
      auto shaderMaterial
        = ShaderMaterial::New("shader" + id, scene, "custom" + id, shaderMaterialOptions);
      if (i == 1) {
        shaderMaterial->backFaceCulling = false;
      }
      box->material       = shaderMaterial;
      _shaderMaterials[i] = shaderMaterial;
    }

    // Animation
    scene->onAfterCameraRenderObservable.add([this](Camera*, EventState&) {
      for (auto& shaderMaterial : _shaderMaterials) {
        shaderMaterial->setFloat("time", _time);
      }
      _time += 0.01f * getScene()->getAnimationRatio();
    });
  }

private:
  float _time;
  std::array<ShaderMaterialPtr, 9> _shaderMaterials;

}; // end of class ShaderMaterialBoxesScene

BABYLON_REGISTER_SAMPLE("Shader Material", ShaderMaterialBoxesScene)

} // end of namespace Samples
} // end of namespace BABYLON