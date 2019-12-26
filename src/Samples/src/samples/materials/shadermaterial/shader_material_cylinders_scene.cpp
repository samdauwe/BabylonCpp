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

class ShaderMaterialCylindersScene : public IRenderableScene {

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

  // Mandelbrot - distance ( https://www.shadertoy.com/view/lsX3W4 )
  static constexpr const char* custom1FragmentShader
    = R"ShaderCode(
// Created by inigo quilez - iq/2013
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0
// Unported License.
//
// This shader computes the distance to the Mandelbrot Set for every
// pixel, and colorizes it accoringly.
//
// Z -> Z²+c, Z0 = 0.
// therefore Z' -> 2·Z·Z' + 1
//
// The Hubbard-Douady potential G(c) is G(c) = log Z/2^n
// G'(c) = Z'/Z/2^n
//
// So the distance is |G(c)|/|G'(c)| = |Z|·log|Z|/|Z'|
//
// More info here:
// http://www.iquilezles.org/www/articles/distancefractals/
// distancefractals.htm

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

void main(void) {
  vec2 p = -1.0 + 2.0 * vUV.xy;

  // animation
  float tz = 0.5 - 0.5 * cos(0.225 * time);
  float zoo = pow(0.5, 13.0 * tz);
  vec2 c = vec2(-0.05, .6805) + p * zoo;

  // iterate
  float di = 1.0;
  vec2 z = vec2(0.0);
  float m2 = 0.0;
  vec2 dz = vec2(0.0);
  for (int i = 0; i < 300; i++) {
    if (m2 > 1024.0) {
      di = 0.0;
      break;
    }

    // Z' -> 2·Z·Z' + 1
    dz = 2.0 * vec2(z.x * dz.x - z.y * dz.y,
                    z.x * dz.y + z.y * dz.x)
         + vec2(1.0, 0.0);

    // Z -> Z² + c
    z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + c;

    m2 = dot(z, z);
  }

  // distance
  // d(c) = |Z|·log|Z|/|Z'|
  float d = 0.5 * sqrt(dot(z, z) / dot(dz, dz)) * log(dot(z, z));
  if (di > 0.5)
    d = 0.0;

  // do some soft coloring based on distance
  d = clamp(pow(4.0 * d / zoo, 0.2), 0.0, 1.0);

  vec3 col = vec3(d);

  gl_FragColor = vec4(col, 1.0);
}
)ShaderCode";

  // Heart - 2D ( https://www.shadertoy.com/view/XsfGRn )
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

void main(void) {
  vec2 p = -1.0 + 2.0 * vUV.xy;

  // background color
  vec3 bcol = vec3(1.0, 0.8, 0.7 - 0.07 * p.y)
                * (1.0 - 0.25 * length(p));

  // animate
  float tt = mod(time, 1.5) / 1.5;
  float ss = pow(tt, .2) * 0.5 + 0.5;
  ss = 1.0 + ss * 0.5 * sin(tt * 6.2831 * 3.0 + p.y * 0.5)
           * exp(-tt * 4.0);
  p *= vec2(0.5, 1.5) + ss * vec2(0.5, -0.5);

  // shape
  p *= 0.8;
  p.y = -0.1 - p.y * 1.2 + abs(p.x) * (1.0 - abs(p.x));
  float r = length(p);
  float d = 0.5;

  // color
  float s = 0.75 + 0.75 * p.x;
  s *= 1.0 - 0.4 * r;
  s = 0.3 + 0.7 * s;
  s *= 0.5 + 0.5 * pow(1.0 - clamp(r / d, 0.0, 1.0), 0.1);
  vec3 hcol = vec3(1.0, 0.5 * r, 0.3) * s;

  vec3 col = mix(bcol, hcol, smoothstep(-0.01, 0.01, d - r));

  gl_FragColor = vec4(col, 1.0);
}
)ShaderCode";

  // [SIG2014] - Total Noob ( https://www.shadertoy.com/view/XdlSDs )
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

void main(void) {
  vec2 p = -1.0 + 2.0 * vUV.xy;
  float tau = 3.1415926535 * 2.0;
  float a = atan(p.x, p.y);
  float r = length(p) * 0.75;
  vec2 uv = vec2(a / tau, r);

  // get the color
  float xCol = (uv.x - (time / 3.0)) * 3.0;
  xCol = mod(xCol, 3.0);
  vec3 horColour = vec3(0.25, 0.25, 0.25);

  if (xCol < 1.0) {
    horColour.r += 1.0 - xCol;
    horColour.g += xCol;
  } else if (xCol < 2.0) {
    xCol -= 1.0;
    horColour.g += 1.0 - xCol;
    horColour.b += xCol;
  } else {
    xCol -= 2.0;
    horColour.b += 1.0 - xCol;
    horColour.r += xCol;
  }

  // draw color beam
  uv = (2.0 * uv) - 1.0;
  float beamWidth =
      (0.7 +
       0.5 * cos(uv.x * 10.0 * tau * 0.15 *
                 clamp(floor(5.0 + 10.0 * cos(time)), 0.0, 10.0))) *
      abs(1.0 / (30.0 * uv.y));
  vec3 horBeam = vec3(beamWidth);
  gl_FragColor = vec4(((horBeam)*horColour), 1.0);
}
)ShaderCode";

  // Bubbles ( https://www.shadertoy.com/view/4dl3zn )
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

void main(void) {
  vec2 uv = vUV.xy;

  // background
  vec3 color = vec3(0.8 + 0.2 * uv.y);

  // bubbles
  for (int i = 0; i < 40; i++) {
    // bubble seeds
    float pha = sin(float(i) * 546.13 + 1.0) * 0.5 + 0.5;
    float siz = pow(sin(float(i) * 651.74 + 5.0) * 0.5 + 0.5, 4.0);
    float pox = sin(float(i) * 321.55 + 4.1);

    // buble size, position and color
    float rad = 0.1 + 0.5 * siz;
    vec2 pos = vec2(pox,
                    -1.0 - rad +
                        (2.0 + 2.0 * rad) *
                            mod(pha + 0.1 * time *
                                (0.2 + 0.8 * siz), 1.0));
    float dis = length(uv - pos);
    vec3 col = mix(vec3(0.94, 0.3, 0.0), vec3(0.1, 0.4, 0.8),
                   0.5 + 0.5 * sin(float(i) * 1.2 + 1.9));

    // render
    float f = length(uv - pos) / rad;
    f = sqrt(clamp(1.0 - f * f, 0.0, 1.0));
    color -= col.zyx * (1.0 - smoothstep(rad * 0.95, rad, dis)) * f;
  }

  // vigneting
  color *= sqrt(1.5 - 0.5 * length(uv));

  gl_FragColor = vec4(color, 1.0);
}
)ShaderCode";

  // Basic Fractal ( https://www.shadertoy.com/view/Mss3Wf )
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

const int maxIterations = 8;

float circleSize = 1.0 / (3.0 * pow(2.0, float(maxIterations)));

// generic rotation formula
vec2 rot(vec2 uv, float a) {
  return vec2(uv.x * cos(a) - uv.y * sin(a),
              uv.y * cos(a) + uv.x * sin(a));
}

void main(void) {
  // normalize stuff
  vec2 uv = -1.0 + 2.0 * vUV.xy;

  // global rotation and zoom
  uv = rot(uv, time);
  uv *= sin(time) * 0.5 + 1.5;

  // mirror, rotate and scale 6 times...
  float s = 0.3;
  for (int i = 0; i < maxIterations; i++) {
    uv = abs(uv) - s;
    uv = rot(uv, time);
    s = s / 2.1;
  }

  // draw a circle
  float c = length(uv) > circleSize ? 0.0 : 1.0;

  gl_FragColor = vec4(c, c, c, 1.0);
}
)ShaderCode";

  // Iterations - inversion ( https://www.shadertoy.com/view/XdXGDS )
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

void main(void) {
  vec2 uv = vUV.xy;

  // shape (16 points)
  float iTime = time + 47.0;
  vec2 z = -1.0 + 2.0 * uv;
  vec3 col = vec3(1.0);
  for (int j = 0; j < 16; j++) {
    // deform
    float s = float(j) / 16.0;
    float f = 0.2 * (0.5 + 1.0 * fract(sin(s * 113.1)
                     * 43758.5453123));
    vec2 c = 0.5 * vec2(cos(f * iTime + 17.0 * s),
                        sin(f * iTime + 19.0 * s));
    z -= c;
    float zr = length(z);
    float ar = atan(z.y, z.x) + zr * 0.6;
    z = vec2(cos(ar), sin(ar)) / zr;
    z += c;
    z += 0.05 * sin(2.0 * z.x);

    // color
    col -= 0.7 * exp(-8.0 * dot(z, z)) *
           (0.5 + 0.5 * sin(4.2 * s + vec3(1.6, 0.9, 0.3)));
  }
  col *= 0.75 + 0.25 * clamp(length(z - uv) * 0.6, 0.0, 1.0);

  // 3d effect
  float h = dot(col, vec3(0.333));
  vec3 nor = normalize(vec3(dFdx(h), dFdy(h), 1.0 / 1000.0));
  col -= 0.05 * vec3(1.0, 0.9, 0.5) * dot(nor, vec3(0.8, 0.4, 0.2));
  col += 0.25 * (1.0 - 0.8 * col) * nor.z * nor.z;

  // 2d postpro
  col *= 1.12;
  col = pow(clamp(col, 0.0, 1.0), vec3(0.8));
  col *= 0.8 + 0.2 * pow(16.0 * uv.x * uv.y
                         * (1.0 - uv.x) * (1.0 - uv.y), 0.1);
  gl_FragColor = vec4(col, 1.0);
}
)ShaderCode";

  // Iterations - trigonometric ( https://www.shadertoy.com/view/Mdl3RH )
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

vec2 iterate(in vec2 p, in vec4 t) {
  return p -
         0.05 * cos(t.xz + p.x * p.y +
                    cos(t.yw + 1.5 * 3.1415927 * p.yx) +
                    p.yx * p.yx);
}

void main(void) {
  vec2 q = -1.0 + 2.0 * vUV.xy;
  vec2 p = -1.0 + 2.0 * q;
  p.x *= 1.0;
  p *= 1.5;

  vec4 t = 0.15 * time * vec4(1.0, -1.5, 1.2, -1.6) +
                         vec4(0.0, 2.0, 3.0, 1.0);

  vec2 z = p;
  vec3 s = vec3(0.0);
  for (int i = 0; i < 100; i++) {
    z = iterate(z, t);

    float d = dot(z - p, z - p);
    s.x += 1.0 / (0.1 + d);
    s.y += sin(atan(p.x - z.x, p.y - z.y));
    s.z += exp(-0.2 * d);
  }
  s /= 100.0;

  vec3 col = 0.5 + 0.5 * cos(vec3(0.0, 0.4, 0.8)
                 + 2.5 + s.z * 6.2831);

  col *= 0.5 + 0.5 * s.y;
  col *= s.x;
  col *= 0.94 + 0.06 * sin(10.0 * length(z));

  vec3 nor = normalize(vec3(dFdx(s.x), 0.02, dFdy(s.x)));
  float dif = dot(nor, vec3(0.7, 0.1, 0.7));
  col -= 0.05 * vec3(dif);

  col *= 0.3 + 0.7 * pow(16.0 * q.x * q.y * (1.0 - q.x) * (1.0 - q.y),
                         0.2);

  gl_FragColor = vec4(col, 1.0);
}
)ShaderCode";

  // Voronoi - basic ( https://www.shadertoy.com/view/MslGD8 )
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

vec2 hash(vec2 p) {
  p = vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)));
  return fract(sin(p) * 18.5453);
}

// return distance, and cell id
vec2 voronoi(in vec2 x) {
  vec2 n = floor(x);
  vec2 f = fract(x);

  vec3 m = vec3(8.0);
  for (int j = -1; j <= 1; j++)
    for (int i = -1; i <= 1; i++) {
      vec2 g = vec2(float(i), float(j));
      vec2 o = hash(n + g);
      // vec2  r = g - f + o;
      vec2 r = g - f + (0.5 + 0.5 * sin(time + 6.2831 * o));
      float d = dot(r, r);
      if (d < m.x)
        m = vec3(d, o);
    }

  return vec2(sqrt(m.x), m.y + m.z);
}

void main(void) {
  vec2 p = -1.0 + 2.0 * vUV.xy;

  // computer voronoi patterm
  vec2 c = voronoi((14.0 + 6.0 * sin(0.2 * time)) * p);

  // colorize
  vec3 col = 0.5 + 0.5 * cos(c.y * 6.2831 + vec3(0.0, 1.0, 2.0));
  col *= clamp(1.0 - 0.4 * c.x * c.x, 0.0, 1.0);
  col -= (1.0 - smoothstep(0.08, 0.09, c.x));

  gl_FragColor = vec4(col, 1.0);
}
)ShaderCode";

  // Soap bubbles 2D ( https://www.shadertoy.com/view/llsSDf )
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

vec2 uv;

vec2 hash2a(vec2 x, float anim) {
  float r = 523.0 * sin(dot(x, vec2(53.3158, 43.6143)));
  float xa1 = fract(anim);
  float xb1 = anim - xa1;
  anim += 0.5;
  float xa2 = fract(anim);
  float xb2 = anim - xa2;

  vec2 z1 = vec2(fract(15.32354 * (r + xb1)),
                 fract(17.25865 * (r + xb1)));
  r = r + 1.0;
  vec2 z2 = vec2(fract(15.32354 * (r + xb1)),
                 fract(17.25865 * (r + xb1)));
  r = r + 1.0;
  vec2 z3 = vec2(fract(15.32354 * (r + xb2)),
                 fract(17.25865 * (r + xb2)));
  r = r + 1.0;
  vec2 z4 = vec2(fract(15.32354 * (r + xb2)),
                 fract(17.25865 * (r + xb2)));
  return (mix(z1, z2, xa1) + mix(z3, z4, xa2)) * 0.5;
}

float hashNull(vec2 x) {
  float r = fract(523.0 * sin(dot(x, vec2(53.3158, 43.6143))));
  return r;
}

vec4 NC0 = vec4(0.0, 157.0, 113.0, 270.0);
vec4 NC1 = vec4(1.0, 158.0, 114.0, 271.0);

vec4 hash4(vec4 n) { return fract(sin(n) * 753.5453123); }
vec2 hash2(vec2 n) { return fract(sin(n) * 753.5453123); }
float noise2(vec2 x) {
  vec2 p = floor(x);
  vec2 f = fract(x);
  f = f * f * (3.0 - 2.0 * f);

  float n = p.x + p.y * 157.0;
  vec2 s1 = mix(hash2(vec2(n) + NC0.xy), hash2(vec2(n) + NC1.xy),
                vec2(f.x));
  return mix(s1.x, s1.y, f.y);
}

float noise3(vec3 x) {
  vec3 p = floor(x);
  vec3 f = fract(x);
  f = f * f * (3.0 - 2.0 * f);

  float n = p.x + dot(p.yz, vec2(157.0, 113.0));
  vec4 s1 = mix(hash4(vec4(n) + NC0), hash4(vec4(n) + NC1),
                vec4(f.x));
  return mix(mix(s1.x, s1.y, f.y), mix(s1.z, s1.w, f.y), f.z);
}

vec4 booble(vec2 te, vec2 pos, float numCells) {
  float d = dot(te, te);

  vec2 te1 = te + (pos - vec2(0.5, 0.5)) * 0.4 / numCells;
  vec2 te2 = -te1;
  float zb1 = max(pow(noise2(te2 * 1000.11 * d), 10.0), 0.01);
  float zb2 = noise2(te1 * 1000.11 * d);
  float zb3 = noise2(te1 * 200.11 * d);
  float zb4 = noise2(te1 * 200.11 * d + vec2(20.0));

  vec4 colorb = vec4(1.0);
  colorb.xyz = colorb.xyz * (0.7 + noise2(te1 * 1000.11 * d) * 0.3);

  zb2 = max(pow(zb2, 20.1), 0.01);
  colorb.xyz = colorb.xyz * (zb2 * 1.9);

  vec4 color = vec4(noise2(te2 * 10.8),
                    noise2(te2 * 9.5 + vec2(15.0, 15.0)),
                    noise2(te2 * 11.2 + vec2(12.0, 12.0)), 1.0);
  color = mix(color, vec4(1.0),
              noise2(te2 * 20.5 + vec2(200.0, 200.0)));
  color.xyz = color.xyz * (0.7 + noise2(te2 * 1000.11 * d) * 0.3);
  color.xyz = color.xyz * (0.2 + zb1 * 1.9);

  float r1 =
      max(min((0.033 - min(0.04, d)) * 100.0 / sqrt(numCells),
               1.0), -1.6);
  float d2 = (0.06 - min(0.06, d)) * 10.0;
  d = (0.04 - min(0.04, d)) * 10.0;
  color.xyz = color.xyz + colorb.xyz * d * 1.5;

  float f1 = min(d * 10.0, 0.5 - d) * 2.2;
  f1 = pow(f1, 4.0);
  float f2 = min(min(d * 4.1, 0.9 - d) * 2.0 * r1, 1.0);

  float f3 = min(d2 * 2.0, 0.7 - d2) * 2.2;
  f3 = pow(f3, 4.0);

  return vec4(color * max(min(f1 + f2, 1.0), -0.5) + vec4(zb3) * f3 -
              vec4(zb4) * (f2 * 0.5 + f1) * 0.5);
}

// base from https://www.shadertoy.com/view/4djGRh
vec4 Cells(vec2 p, vec2 move, in float numCells,
                              in float count, float blur) {
  vec2 inp = p + move;
  inp *= numCells;
  float d = 1.0;
  vec2 te;
  vec2 pos;
  for (int xo = -1; xo <= 1; xo++) {
    for (int yo = -1; yo <= 1; yo++) {
      vec2 tp = floor(inp) + vec2(xo, yo);
      vec2 rr = mod(tp, numCells);
      tp = tp + (hash2a(rr, time * 0.1) +
                 hash2a(rr, time * 0.1 + 0.25)) * 0.5;
      vec2 l = inp - tp;
      float dr = dot(l, l);
      if (hashNull(rr) > count)
        if (d > dr) {
          d = dr;
          pos = tp;
        }
    }
  }
  if (d >= 0.06)
    return vec4(0.0);
  te = inp - pos;

  if (d < 0.04)
    uv = uv + te * (d)*2.0;
  if (blur > 0.0001) {
    vec4 c = vec4(0.0);
    for (float x = -1.0; x < 1.0; x += 0.5) {
      for (float y = -1.0; y < 1.0; y += 0.5) {
        c += booble(te + vec2(x, y) * blur, p, numCells);
      }
    }
    return c * 0.05;
  }

  return booble(te, p, numCells);
}

void main(void) {
  uv = vUV.xy;

  vec2 l1 = vec2(time * 0.02, time * 0.02);
  vec2 l2 = vec2(-time * 0.01, time * 0.007);
  vec2 l3 = vec2(0.0, time * 0.01);

  vec4 e = vec4(noise3(vec3(uv * 2.0, time * 0.1)),
                noise3(vec3(uv * 2.0 + vec2(200.0), time * 0.1)),
                noise3(vec3(uv * 2.0 + vec2(50.0), time * 0.1)), 0.0);

  vec4 cr1 = Cells(uv, vec2(20.2449, 93.78) + l1, 2.0, 0.5, 0.005);
  vec4 cr2 = Cells(uv, vec2(0.0, 0.0), 3.0, 0.5, 0.003);
  vec4 cr3 = Cells(uv, vec2(230.79, 193.2) + l2, 4.0, 0.5, 0.0);
  vec4 cr4 = Cells(uv, vec2(200.19, 393.2) + l3, 7.0, 0.8, 0.01);
  vec4 cr5 = Cells(uv, vec2(10.3245, 233.645) + l3, 9.2, 0.9, 0.02);
  vec4 cr6 = Cells(uv, vec2(10.3245, 233.645) + l3, 14.2, 0.95, 0.05);

  e = max(e - vec4(dot(cr6, cr6)) * 0.1, 0.0) + cr6 * 1.6;
  e = max(e - vec4(dot(cr5, cr5)) * 0.1, 0.0) + cr5 * 1.6;
  e = max(e - vec4(dot(cr4, cr4)) * 0.1, 0.0) + cr4 * 1.3;
  e = max(e - vec4(dot(cr3, cr3)) * 0.1, 0.0) + cr3 * 1.1;
  e = max(e - vec4(dot(cr2, cr2)) * 0.1, 0.0) + cr2 * 1.4;

  e = max(e - vec4(dot(cr1, cr1)) * 0.1, 0.0) + cr1 * 1.8;

  gl_FragColor = e;
}
)ShaderCode";

public:
  ShaderMaterialCylindersScene(ICanvas* iCanvas) : IRenderableScene(iCanvas), _time{0.f}
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

  ~ShaderMaterialCylindersScene() override = default;

  const char* getName() override
  {
    return "Shader Material Cylinders Scene";
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
      auto id            = std::to_string(i + 1);
      auto cylinder      = Mesh::CreateCylinder("cylinder", 1, 1, 1, 6, 1, scene, false);
      cylinder->position = boxPositions[i];
      // box->enableEdgesRendering(0.99f);
      cylinder->edgesWidth = 2.f;
      // Create shader material
      IShaderMaterialOptions shaderMaterialOptions;
      shaderMaterialOptions.attributes = {"position", "normal", "uv"};
      shaderMaterialOptions.uniforms   = {"time", "worldViewProjection"};
      auto shaderMaterial
        = ShaderMaterial::New("shader" + id, scene, "custom" + id, shaderMaterialOptions);
      if (i == 1) {
        shaderMaterial->backFaceCulling = false;
      }
      cylinder->material = shaderMaterial;
      cylinder->rotation().y += Math::PI;
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

}; // end of class ShaderMaterialCylindersScene

BABYLON_REGISTER_SAMPLE("Shader Material", ShaderMaterialCylindersScene)

} // end of namespace Samples
} // end of namespace BABYLON