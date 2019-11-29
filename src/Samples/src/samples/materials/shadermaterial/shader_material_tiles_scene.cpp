#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_shaders_store.h>
#include <babylon/materials/shader_material.h>
#include <babylon/maths/vector2.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

class ShaderMaterialTilesScene : public IRenderableScene {

public:
  static constexpr const char* customVertexShader
    = R"ShaderCode(
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

  // Procedural Checkerboard ( https://www.shadertoy.com/view/Xss3Dr )
  static constexpr const char* custom1FragmentShader
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
uniform float time;
uniform float aspectRatio;
uniform vec2 resolution;

// by Nikos Papadopoulos, 4rknova / 2013
// WTFPL

#define S 5. // Scale

void main(void) {
  vec2 p = -1.0 + 2.0 * vUV;
  vec2 uv = floor(S * p.xy * vec2(resolution.x / resolution.y, 1) /
                  resolution.xy);
  gl_FragColor = vec4(vec3(mod(uv.x + uv.y, 2.)), 1);
}
)ShaderCode";

  // Simple procedural brick ( https://www.shadertoy.com/view/lltGDM )
  static constexpr const char* custom2FragmentShader
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
uniform float time;
uniform vec2 resolution;

float brickH = .08;
float brickW = .2;
float mortarR = 0.006;
float mortarW = 0.005;
vec3 brickC = pow(vec3(218, 114, 61) / 255., vec3(2.2));
vec3 mortarC = pow(vec3(227, 210, 170) / 255., vec3(2.2));

vec3 permute(vec3 x) { return mod(((x * 34.0) + 1.0) * x, 289.0); }

float snoise(vec2 v) {
  const vec4 C = vec4(0.211324865405187, 0.366025403784439,
                      -0.577350269189626, 0.024390243902439);
  vec2 i = floor(v + dot(v, C.yy));
  vec2 x0 = v - i + dot(i, C.xx);
  vec2 i1;
  i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  vec4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;
  i = mod(i, 289.0);
  vec3 p =
      permute(permute(i.y + vec3(0.0, i1.y, 1.0)) + i.x +
                      vec3(0.0, i1.x, 1.0));
  vec3 m = max(
      0.5 - vec3(dot(x0, x0), dot(x12.xy, x12.xy),
                 dot(x12.zw, x12.zw)), 0.0);
  m = m * m;
  m = m * m;
  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  vec3 ox = floor(x + 0.5);
  vec3 a0 = x - ox;
  m *= 1.79284291400159 - 0.85373472095314 * (a0 * a0 + h * h);
  vec3 g;
  g.x = a0.x * x0.x + h.x * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return pow((130.0 * dot(m, g) + 1.0) / 2.0, 1.0);
}

float derez(float a, float b) { return ceil(a / b) * b; }

float capSlope(float a, float r, float w) {
  float ir = 1.0 - r;
  return (max(a + w, ir) - ir) / r;
}

float peak(float a, float r, float w) {

  return pow(max(capSlope(1.0 - a, r, w) + capSlope(a, r, w),
                 0.0), 2.0);
}

float mortar(vec2 uv) {
  float isOffset = mod(uv.y, brickH * 2.) < brickH ? 0. : 1.;
  return (
             // vertical mortar lines
             peak(mod(uv.x + isOffset * brickW * .5, brickW) / brickW,
                  mortarR / brickW, mortarW / brickW) +
             // horizontal mortar lines
             peak(mod(uv.y, brickH) / brickH, mortarR / brickH,
                  mortarW / brickH)) > 1. ? 1. : 0.;
}

vec3 lightAjust(vec3 color, float amount) {
  return 1.0 - pow(1.0 - color, vec3(amount));
}

vec3 toneMap(vec3 color) {
  float maxColor = max(color.r, max(color.g, color.b));
  vec3 foo = lightAjust(color / maxColor, maxColor);

  return min(foo, color);
}

void main(void) {
  vec2 uv = vUV;
  float pi = 3.14152654;

  float brickHNoise = snoise(vec2(1.0, uv.y * 2.0));

  brickH += brickHNoise * 0.002;

  float brickWNoise = snoise(vec2(uv.x, derez(uv.y, brickH)));

  brickW += brickWNoise * .01;

  float isOffset = mod(uv.y, brickH * 2.) < brickH ? 0. : 1.;

  float brickLNoise =
      snoise(vec2(derez((uv.x + isOffset * brickW * .5) * 4.0,
                         brickW * 4.0),
                  derez(uv.y * 4.0, brickH * 4.0)));
  float brickCNoise =
      snoise(vec2(derez(((uv.x) + isOffset * brickW * .5) * 5.0,
                          brickW * 5.0),
                  derez((uv.y) * 5.0, brickH * 5.0)));

  brickC *= brickLNoise * .6 + .4;

  brickC *= vec3((1. - brickCNoise) * 0.2 + 0.8,
                 brickCNoise * 0.2 + 0.8, 1);

  vec3 color = mix(brickC, mortarC, mortar(uv));

  float highlight = clamp(mortar(uv + vec2(-.005, .005)) - mortar(uv),
                          0., 1.);
  float shadow = clamp(mortar(uv + vec2(.005, -.005)) - mortar(uv),
                       0., 1.);
  shadow +=
      clamp((1. - mortar(uv + vec2(-.005, .005))) - (1. - mortar(uv)),
                         0., 1.);

  shadow = clamp(shadow, 0., 1.);

  float light = mix(highlight, 1.0 - shadow, 0.5);

  float textureHighlight =
      clamp(snoise((uv / .01) + vec2(-.1, .1)) - snoise(uv / .01),
                    0., 1.);
  float textureShadow =
      clamp(snoise((uv / .01) + vec2(.1, -.1)) - snoise(uv / .01),
                    0., 1.);
  float texture = mix(textureHighlight, 1.0 - textureShadow, 0.5);

  light = mix(light, texture, 0.5);

  vec3 lightColor = mix(vec3(0, 0, 1), vec3(1, 1, 0), light);

  gl_FragColor =
      pow(vec4(lightAjust(color * lightColor, 2.5), 1),
               vec4(1. / 2.2));
}
)ShaderCode";

  // Utah Sandstone ( https://www.shadertoy.com/view/MdsSDB )
  static constexpr const char* custom3FragmentShader
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
uniform float time;
uniform float aspectRatio;
uniform vec2 resolution;

// perlin noise function by https://github.com/ashima/webgl-noise

vec3 mod289(vec3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }

vec2 mod289(vec2 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }

vec3 permute(vec3 x) { return mod289(((x * 34.0) + 1.0) * x); }

float snoise(vec2 v) {
  const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
                      0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
                      -0.577350269189626, // -1.0 + 2.0 * C.x
                      0.024390243902439); // 1.0 / 41.0
                                          // First corner
  vec2 i = floor(v + dot(v, C.yy));
  vec2 x0 = v - i + dot(i, C.xx);

  // Other corners
  vec2 i1;
  // i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
  // i1.y = 1.0 - i1.x;
  i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  // x0 = x0 - 0.0 + 0.0 * C.xx ;
  // x1 = x0 - i1 + 1.0 * C.xx ;
  // x2 = x0 - 1.0 + 2.0 * C.xx ;
  vec4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;

  // Permutations
  i = mod289(i); // Avoid truncation effects in permutation
  vec3 p =
      permute(permute(i.y + vec3(0.0, i1.y, 1.0)) + i.x +
                      vec3(0.0, i1.x, 1.0));

  vec3 m = max(
      0.5 - vec3(dot(x0, x0), dot(x12.xy, x12.xy),
                 dot(x12.zw, x12.zw)), 0.0);
  m = m * m;
  m = m * m;

  // Gradients: 41 points uniformly over a line,
  //            mapped onto a diamond.
  // The ring size 17*17 = 289 is close to a multiple of
  // 41 (41*7 = 287)

  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  vec3 ox = floor(x + 0.5);
  vec3 a0 = x - ox;

  // Normalise gradients implicitly by scaling m
  // Approximation of: m *= inversesqrt( a0*a0 + h*h );
  m *= 1.79284291400159 - 0.85373472095314 * (a0 * a0 + h * h);

  // Compute final noise value at P
  vec3 g;
  g.x = a0.x * x0.x + h.x * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
}

// fbm function by
// https://code.google.com/p/fractalterraingeneration/wiki/
// Fractional_Brownian_Motion
float fbm(in vec2 uv) {
  float gain = 0.65;
  float lacunarity = 2.1042;

  float total = 0.0;
  float frequency = 0.5;
  float amplitude = gain;

  uv = uv * 5.0;

  total = snoise(uv);

  for (int i = 0; i < 10; i++) {
    total += snoise(uv * frequency) * amplitude;
    frequency *= lacunarity;
    amplitude *= gain;
  }

  total = (total + 2.0) / 4.0;

  return total;
}

float pattern(in vec2 p) { return fbm(p + fbm(p + fbm(p))); }

void main(void) {
  vec2 uv = -1.0 + 2.0 * vUV;
  vec2 ar = vec2(resolution.x / resolution.y, 1.0);

#ifdef ANIMATE
  float iTime = time / 20.0;
#else
  float iTime = 5.0;
#endif

  vec3 col = vec3(0.0);
  vec2 p = vec2((uv * ar) / 900.0);

  float f = pattern(vec2(p.x + 0.005 * iTime, p.y));

  col = vec3(f * 1.9, f * 1.0, f * 0.45);
  col = sqrt(col) - 0.2;

  gl_FragColor = vec4(col, 1.0);
}
)ShaderCode";

  // Warping - procedural 3 ( https://www.shadertoy.com/view/XsfSD4 )
  static constexpr const char* custom4FragmentShader
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
uniform float time;
uniform float aspectRatio;
uniform vec2 resolution;

// Created by inigo quilez - iq/2014
// License Creative Commons Attribution-NonCommercial-ShareAlike
// 3.0 Unported License.

vec2 hash(vec2 p) {
  p = vec2(dot(p, vec2(2127.1, 81.17)), dot(p, vec2(1269.5, 283.37)));
  return fract(sin(p) * 43758.5453);
}

float noise(in vec2 p) {
  vec2 i = floor(p);
  vec2 f = fract(p);

  vec2 u = f * f * (3.0 - 2.0 * f);

  float n = mix(
      mix(dot(-1.0 + 2.0 * hash(i + vec2(0.0, 0.0)),
              f - vec2(0.0, 0.0)),
          dot(-1.0 + 2.0 * hash(i + vec2(1.0, 0.0)),
              f - vec2(1.0, 0.0)), u.x),
      mix(dot(-1.0 + 2.0 * hash(i + vec2(0.0, 1.0)),
              f - vec2(0.0, 1.0)),
          dot(-1.0 + 2.0 * hash(i + vec2(1.0, 1.0)),
              f - vec2(1.0, 1.0)), u.x),
      u.y);
  return 0.5 + 0.5 * n;
}

float fbm(in vec2 x) {
#ifdef ANIMATE
  float iTime = time;
#else
  float iTime = 0.0;
#endif
  float m = 0.0;
  vec2 p = 6.0 * x;

  float w = 0.5;
  float s = 1.0;
  for (int i = 0; i < 8; i++) {
    float n = noise(p * s);
    m += w * n;

    p.y -= 1.25 * (-1.0 + 2.0 * n) - 0.1 * iTime * w; // warp

    w *= 0.5;
    s *= 2.0;
  }

  return m;
}

void main(void) {
  vec2 p = -1.0 + 2.0 * vUV;

  // pattern
  float n = fbm(p);

  // normal
  vec2 e = vec2(0.03, 0.0);
  vec3 nor = normalize(vec3(fbm(p - e.xy) - fbm(p + e.xy), 2.0 * e.x,
                            fbm(p - e.yx) - fbm(p + e.yx)));

  // color
  float c = pow(n, 4.0) * 6.0;
  vec3 col = 0.5 + 0.5 * cos(5.0 + n * 12.0 + vec3(0.8, 0.4, 0.0));
  col *= 0.4 + 0.6 * c;

  // light
  vec3 lig = normalize(vec3(1.0, 0.2, 1.0));
  col *= vec3(0.6, 0.7, 0.8) +
         1.0 * vec3(1.0, 0.7, 0.5) * clamp(0.3 + 0.7 * dot(nor, lig),
                    0.0, 1.0);
  col += 1.5 * vec3(1.0, 0.5, 0.0) * pow(1.0 - nor.y, 2.0) * c;

  // postprocessing
  col = sqrt(col) - 0.15;
  col *= sqrt(1.0 - 0.5 * abs(p.x - 0.5));

  gl_FragColor = vec4(col, 1.0);
}
)ShaderCode";

  // Warping - procedural 4 ( https://www.shadertoy.com/view/MdSXzz )
  static constexpr const char* custom5FragmentShader
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
uniform float time;
uniform float aspectRatio;
uniform vec2 resolution;

// Created by inigo quilez - iq/2014
// License Creative Commons
// Attribution-NonCommercial-ShareAlike 3.0 Unported
// License.

const mat2 m = mat2(0.80, 0.60, -0.60, 0.80);

float hash(vec2 p) {
  float h = dot(p, vec2(127.1, 311.7));
  return -1.0 + 2.0 * fract(sin(h) * 43758.5453123);
}

float noise(in vec2 p) {
  vec2 i = floor(p);
  vec2 f = fract(p);

  vec2 u = f * f * (3.0 - 2.0 * f);

  return mix(mix(hash(i + vec2(0.0, 0.0)),
                 hash(i + vec2(1.0, 0.0)), u.x),
             mix(hash(i + vec2(0.0, 1.0)),
                 hash(i + vec2(1.0, 1.0)), u.x), u.y);
}

float fbm(vec2 p) {
  float f = 0.0;
  f += 0.5000 * noise(p);
  p = m * p * 2.02;
  f += 0.2500 * noise(p);
  p = m * p * 2.03;
  f += 0.1250 * noise(p);
  p = m * p * 2.01;
  f += 0.0625 * noise(p);
  return f / 0.9375;
}

vec2 fbm2(in vec2 p) { return vec2(fbm(p.xy), fbm(p.yx)); }

vec3 map(vec2 p) {
  p *= 0.7;

#ifdef ANIMATE
  float iTime = time;
#else
  float iTime = 7.0;
#endif

  float f = dot(fbm2(1.0 * (0.05 * iTime + p +
                            fbm2(-0.05 * iTime + 2.0 *
                                 (p + fbm2(4.0 * p))))),
                vec2(1.0, -1.0));

  float bl = smoothstep(-0.8, 0.8, f);

  float ti = smoothstep(-1.0, 1.0, fbm(p));

  return mix(mix(vec3(0.50, 0.00, 0.00), vec3(1.00, 0.75, 0.35), ti),
             vec3(0.00, 0.00, 0.02), bl);
}

void main(void) {
  vec2 p = -1.0 + 2.0 * vUV;

  float e = 0.0045;

  vec3 colc = map(p);
  float gc = dot(colc, vec3(0.333));
  vec3 cola = map(p + vec2(e, 0.0));
  float ga = dot(cola, vec3(0.333));
  vec3 colb = map(p + vec2(0.0, e));
  float gb = dot(colb, vec3(0.333));

  vec3 nor = normalize(vec3(ga - gc, e, gb - gc));

  vec3 col = colc;
  col += vec3(1.0, 0.7, 0.6) * 8.0 * abs(2.0 * gc - ga - gb);
  col *= 1.0 + 0.2 * nor.y * nor.y;
  col += 0.05 * nor.y * nor.y * nor.y;

  vec2 q = -1.0 + 2.0 * vUV;
  col *= pow(16.0 * q.x * q.y * (1.0 - q.x) * (1.0 - q.y), 0.1);

  gl_FragColor = vec4(col, 1.0);
}
)ShaderCode";

  // Warping - procedural 2 ( https://www.shadertoy.com/view/lsl3RH )
  static constexpr const char* custom6FragmentShader
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
uniform float time;
uniform float aspectRatio;
uniform vec2 resolution;

// Created by inigo quilez - iq/2013
// License Creative Commons Attribution-NonCommercial-ShareAlike
// 3.0 Unported License.

// See here for a tutorial on how to make this:
// http://www.iquilezles.org/www/articles/warp/warp.htm

const mat2 m = mat2(0.80, 0.60, -0.60, 0.80);

float noise(in vec2 x) { return sin(1.5 * x.x) * sin(1.5 * x.y); }

float fbm4(vec2 p) {
  float f = 0.0;
  f += 0.5000 * noise(p);
  p = m * p * 2.02;
  f += 0.2500 * noise(p);
  p = m * p * 2.03;
  f += 0.1250 * noise(p);
  p = m * p * 2.01;
  f += 0.0625 * noise(p);
  return f / 0.9375;
}

float fbm6(vec2 p) {
  float f = 0.0;
  f += 0.500000 * (0.5 + 0.5 * noise(p));
  p = m * p * 2.02;
  f += 0.250000 * (0.5 + 0.5 * noise(p));
  p = m * p * 2.03;
  f += 0.125000 * (0.5 + 0.5 * noise(p));
  p = m * p * 2.01;
  f += 0.062500 * (0.5 + 0.5 * noise(p));
  p = m * p * 2.04;
  f += 0.031250 * (0.5 + 0.5 * noise(p));
  p = m * p * 2.01;
  f += 0.015625 * (0.5 + 0.5 * noise(p));
  return f / 0.96875;
}

float func(vec2 q, out vec4 ron) {
#ifdef ANIMATE
  float iTime = time;
#else
  float iTime = 0.0;
#endif

  float ql = length(q);
  q.x += 0.05 * sin(0.27 * iTime + ql * 4.1);
  q.y += 0.05 * sin(0.23 * iTime + ql * 4.3);
  q *= 0.5;

  vec2 o = vec2(0.0);
  o.x = 0.5 + 0.5 * fbm4(vec2(2.0 * q));
  o.y = 0.5 + 0.5 * fbm4(vec2(2.0 * q + vec2(5.2)));

  float ol = length(o);
  o.x += 0.02 * sin(0.12 * iTime + ol) / ol;
  o.y += 0.02 * sin(0.14 * iTime + ol) / ol;

  vec2 n;
  n.x = fbm6(vec2(4.0 * o + vec2(9.2)));
  n.y = fbm6(vec2(4.0 * o + vec2(5.7)));

  vec2 p = 4.0 * q + 4.0 * n;

  float f = 0.5 + 0.5 * fbm4(p);

  f = mix(f, f * f * f * 3.5, f * abs(n.x));

  float g = 0.5 + 0.5 * sin(4.0 * p.x) * sin(4.0 * p.y);
  f *= 1.0 - 0.5 * pow(g, 8.0);

  ron = vec4(o, n);

  return f;
}

vec3 doMagic(vec2 p) {
  vec2 q = p * 0.6;

  vec4 on = vec4(0.0);
  float f = func(q, on);

  vec3 col = vec3(0.0);
  col = mix(vec3(0.2, 0.1, 0.4), vec3(0.3, 0.05, 0.05), f);
  col = mix(col, vec3(0.9, 0.9, 0.9), dot(on.zw, on.zw));
  col = mix(col, vec3(0.4, 0.3, 0.3), 0.5 * on.y * on.y);
  col = mix(col, vec3(0.0, 0.2, 0.4),
            0.5 * smoothstep(1.2, 1.3, abs(on.z) + abs(on.w)));
  col = clamp(col * f * 2.0, 0.0, 1.0);

  vec3 nor =
      normalize(vec3(dFdx(f) * resolution.x, 6.0,
                     dFdy(f) * resolution.y));

  vec3 lig = normalize(vec3(0.9, -0.2, -0.4));
  float dif = clamp(0.3 + 0.7 * dot(nor, lig), 0.0, 1.0);
  vec3 bdrf;
  bdrf = vec3(0.70, 0.90, 0.95) * (nor.y * 0.5 + 0.5);
  bdrf += vec3(0.15, 0.10, 0.05) * dif;
  col *= 1.2 * bdrf;
  col = 1.0 - col;
  return 1.1 * col * col;
}
void main(void) {
  vec2 q = -1.0 + 2.0 * vUV;
  vec2 p = -1.0 + 2.0 * q;
  p.x *= resolution.x / resolution.y;

  gl_FragColor = vec4(doMagic(p), 1.0);
}
)ShaderCode";

  // Worley-Perlin noise ( https://www.shadertoy.com/view/MdGSzt )
  static constexpr const char* custom7FragmentShader
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
uniform float time;
uniform float aspectRatio;
uniform vec2 resolution;

// copy from https://www.shadertoy.com/view/4l2GzW
float r(float n) { return fract(cos(n * 89.42) * 343.42); }
vec2 r(vec2 n) {
  return vec2(r(n.x * 23.62 - 300.0 + n.y * 34.35),
              r(n.x * 45.13 + 256.0 + n.y * 38.89));
}
float worley(vec2 n, float s) {
  float dis = 2.0;
  for (int x = -1; x <= 1; x++) {
    for (int y = -1; y <= 1; y++) {
      vec2 p = floor(n / s) + vec2(x, y);
      float d = length(r(p) + vec2(x, y) - fract(n / s));
      if (dis > d) {
        dis = d;
      }
    }
  }
  return 1.0 - dis;
}

// copy from https://www.shadertoy.com/view/4sc3z2

#define MOD3 vec3(.1031, .11369, .13787)

vec3 hash33(vec3 p3) {
  p3 = fract(p3 * MOD3);
  p3 += dot(p3, p3.yxz + 19.19);
  return -1.0 +
         2.0 * fract(vec3((p3.x + p3.y) * p3.z, (p3.x + p3.z) * p3.y,
                          (p3.y + p3.z) * p3.x));
}
float perlin_noise(vec3 p) {
  vec3 pi = floor(p);
  vec3 pf = p - pi;

  vec3 w = pf * pf * (3.0 - 2.0 * pf);

  return mix(mix(mix(dot(pf - vec3(0, 0, 0), hash33(pi + vec3(0, 0, 
0))),
                     dot(pf - vec3(1, 0, 0), hash33(pi + vec3(1, 0, 
0))), w.x),
                 mix(dot(pf - vec3(0, 0, 1), hash33(pi + vec3(0, 0, 
1))),
                     dot(pf - vec3(1, 0, 1), hash33(pi + vec3(1, 0, 
1))), w.x),
                 w.z),
             mix(mix(dot(pf - vec3(0, 1, 0), hash33(pi + vec3(0, 1, 
0))),
                     dot(pf - vec3(1, 1, 0), hash33(pi + vec3(1, 1, 
0))), w.x),
                 mix(dot(pf - vec3(0, 1, 1), hash33(pi + vec3(0, 1, 
1))),
                     dot(pf - vec3(1, 1, 1), hash33(pi + vec3(1, 1, 
1))), w.x),
                 w.z),
             w.y);
}

void main(void) {

#ifdef ANIMATE
  float iTime = time;
#else
  float iTime = 7.0;
#endif

  vec2 p = -1.0 + 2.0 * vUV;

  float dis =
      (1.0 + perlin_noise(vec3(p / resolution.xy, iTime * 0.05) * 8.0)) 
*
      (1.0 + (worley(p, 32.0) + 0.5 * worley(2.0 * p, 32.0) +
              0.25 * worley(4.0 * p, 32.0)));
  gl_FragColor = vec4(vec3(dis / 4.0), 1.0);
}
)ShaderCode";

  // Worley-Noise ( https://www.shadertoy.com/view/MstGRl )
  static constexpr const char* custom8FragmentShader
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
uniform float time;
uniform float aspectRatio;
uniform vec2 resolution;

// Determines how many cells there are
#define NUM_CELLS 16.0

// Arbitrary random, can be replaced with a function of your choice
float rand(vec2 co) {
  return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

// Returns the point in a given cell
vec2 get_cell_point(ivec2 cell) {
  vec2 cell_base = vec2(cell) / NUM_CELLS;
  float noise_x = rand(vec2(cell));
  float noise_y = rand(vec2(cell.yx));
  return cell_base + (0.5 + 1.5 * vec2(noise_x, noise_y)) / NUM_CELLS;
}

// Performs worley noise by checking all adjacent cells
// and comparing the distance to their points
float worley(vec2 coord) {
  ivec2 cell = ivec2(coord * NUM_CELLS);
  float dist = 1.0;

  // Search in the surrounding 5x5 cell block
  for (int x = 0; x < 5; x++) {
    for (int y = 0; y < 5; y++) {
      vec2 cell_point = get_cell_point(cell + ivec2(x - 2, y - 2));
      dist = min(dist, distance(cell_point, coord));
    }
  }

  dist /= length(vec2(1.0 / NUM_CELLS));
  dist = 1.0 - dist;
  return dist;
}

void main(void) {
  vec2 uv = -1.0 + 2.0 * vUV.xy;
  uv.y *= resolution.y / resolution.x;
  gl_FragColor = vec4(worley(uv));
}
)ShaderCode";

  // Kintsugi ( https://www.shadertoy.com/view/Xt33WX )
  static constexpr const char* custom9FragmentShader
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
uniform float time;
uniform float aspectRatio;
uniform vec2 resolution;

//
// Description : Array and textureless GLSL 2D simplex noise function.
//      Author : Ian McEwan, Ashima Arts.
//  Maintainer : stegu
//     Lastmod : 20110822 (ijm)
//     License : Copyright (C) 2011 Ashima Arts. All rights reserved.
//               Distributed under the MIT License. See LICENSE file.
//               https://github.com/ashima/webgl-noise
//               https://github.com/stegu/webgl-noise
//

vec3 mod289(vec3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }

vec2 mod289(vec2 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }

vec3 permute(vec3 x) { return mod289(((x * 34.0) + 1.0) * x); }

float snoise(vec2 v) {
  const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
                      0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
                      -0.577350269189626, // -1.0 + 2.0 * C.x
                      0.024390243902439); // 1.0 / 41.0
                                          // First corner
  vec2 i = floor(v + dot(v, C.yy));
  vec2 x0 = v - i + dot(i, C.xx);

  // Other corners
  vec2 i1;
  // i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
  // i1.y = 1.0 - i1.x;
  i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  // x0 = x0 - 0.0 + 0.0 * C.xx ;
  // x1 = x0 - i1 + 1.0 * C.xx ;
  // x2 = x0 - 1.0 + 2.0 * C.xx ;
  vec4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;

  // Permutations
  i = mod289(i); // Avoid truncation effects in permutation
  vec3 p =
      permute(permute(i.y + vec3(0.0, i1.y, 1.0)) +
                      i.x + vec3(0.0, i1.x, 1.0));

  vec3 m = max(
      0.5 - vec3(dot(x0, x0), dot(x12.xy, x12.xy),
                 dot(x12.zw, x12.zw)), 0.0);
  m = m * m;
  m = m * m;

  // Gradients: 41 points uniformly over a line,
  //            mapped onto a diamond.
  // The ring size 17*17 = 289 is close to a multiple of
  // 41 (41*7 = 287)

  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  vec3 ox = floor(x + 0.5);
  vec3 a0 = x - ox;

  // Normalise gradients implicitly by scaling m
  // Approximation of: m *= inversesqrt( a0*a0 + h*h );
  m *= 1.79284291400159 - 0.85373472095314 * (a0 * a0 + h * h);

  // Compute final noise value at P
  vec3 g;
  g.x = a0.x * x0.x + h.x * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
}

float turbulence(vec2 P) {
  float val = 0.0;
  float freq = 0.7;
  for (int i = 0; i < 5; i++) {
    val += snoise(P * freq);
    // freq *= 2.07;
    freq *= 1.57;
  }
  return val;
}

// Expects -1<x<1
vec4 marble_color(float x) {
  vec4 col;
  x = 0.5 * (x + 1.0);          // transform -1<x<1 to 0<x<1
  for (int i = 0; i < 3; i++) { // make x fall of rapidly...
    x = sqrt(x);
  }

  if (x <= 0.45) {
    col.r = 1.0;
    col.g = 0.84;
    col.b = 0.0;
    col *= vec4(1.95 - x) * 0.55;
    col.a = 1.0;
  } else {
    col = vec4(x);
  }

  col.r = min(col.r, 1.0);
  col.g = min(col.g, 1.0);
  col.b = min(col.b, 1.0);

  return col;
}

void main(void) {
  vec2 uv = -1.0 + 2.0 * vUV;
  float amplitude = 2.0;

#ifdef ANIMATE
  float iTime = time;
#else
  float iTime = 0.0;
#endif

  float t = uv.x * 10.0;
  t += amplitude * turbulence(uv.xy + vec2(iTime / 40.0));
  t = sin(t);
  gl_FragColor = marble_color(t);
}
)ShaderCode";

public:
  ShaderMaterialTilesScene(ICanvas* iCanvas) : IRenderableScene(iCanvas), _time{0.f}
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

  ~ShaderMaterialTilesScene() override = default;

  const char* getName() override
  {
    return "Shader Material Tiles Scene";
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
      auto id = std::to_string(i + 1);
      BoxOptions options;
      options.size            = 1.5f;
      options.sideOrientation = Mesh::DEFAULTSIDE;
      options.updatable       = false;
      options.height          = *options.height * 0.01f;
      auto box                = MeshBuilder::CreateBox("box" + id, options, scene);
      box->position           = boxPositions[i];
      box->edgesWidth         = 2.f;
      _boxTopFaceResolution   = Vector2(*options.width, *options.depth);
      _boxTopFaceAspectRatio  = _boxTopFaceResolution.x / _boxTopFaceResolution.y;
      // Create shader material
      IShaderMaterialOptions shaderMaterialOptions;
      shaderMaterialOptions.attributes = {"position", "normal", "uv"};
      shaderMaterialOptions.uniforms = {"time", "worldViewProjection", "aspectRatio", "resolution"};
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
        shaderMaterial->setFloat("aspectRatio", _boxTopFaceAspectRatio);
        shaderMaterial->setVector2("resolution", _boxTopFaceResolution);
      }
      _time += 0.01f * getScene()->getAnimationRatio();
    });
  }

private:
  float _time;
  std::array<ShaderMaterialPtr, 9> _shaderMaterials;
  Vector2 _boxTopFaceResolution;
  float _boxTopFaceAspectRatio;

}; // end of class ShaderMaterialTilesScene

BABYLON_REGISTER_SAMPLE("Shader Material", ShaderMaterialTilesScene)

} // end of namespace Samples
} // end of namespace BABYLON