#ifndef BABYLON_PROCEDURAL_TEXTURES_LIBRARY_FIRE_FIRE_PROCEDURAL_TEXTURE_FRAGMENT_FX_H
#define BABYLON_PROCEDURAL_TEXTURES_LIBRARY_FIRE_FIRE_PROCEDURAL_TEXTURE_FRAGMENT_FX_H

namespace BABYLON {

extern const char* fireProceduralTexturePixelShader;

const char* fireProceduralTexturePixelShader
  = "#ifdef GL_ES\n"
    "precision highp float;\n"
    "#endif\n"
    "\n"
    "uniform float time;\n"
    "uniform vec3 c1;\n"
    "uniform vec3 c2;\n"
    "uniform vec3 c3;\n"
    "uniform vec3 c4;\n"
    "uniform vec3 c5;\n"
    "uniform vec3 c6;\n"
    "uniform vec2 speed;\n"
    "uniform float shift;\n"
    "uniform float alphaThreshold;\n"
    "\n"
    "varying vec2 vUV;\n"
    "\n"
    "float rand(vec2 n) {\n"
    "  return fract(cos(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);\n"
    "}\n"
    "\n"
    "float noise(vec2 n) {\n"
    "  const vec2 d = vec2(0.0, 1.0);\n"
    "  vec2 b = floor(n), f = smoothstep(vec2(0.0), vec2(1.0), fract(n));\n"
    "  return mix(mix(rand(b), rand(b + d.yx), f.x), mix(rand(b + d.xy), rand(b + d.yy), f.x), f.y);\n"
    "}\n"
    "\n"
    "float fbm(vec2 n) {\n"
    "  float total = 0.0, amplitude = 1.0;\n"
    "  for (int i = 0; i < 4; i++) {\n"
    "  total += noise(n) * amplitude;\n"
    "  n += n;\n"
    "  amplitude *= 0.5;\n"
    "  }\n"
    "  return total;\n"
    "}\n"
    "\n"
    "void main() {\n"
    "  vec2 p = vUV * 8.0;\n"
    "  float q = fbm(p - time * 0.1);\n"
    "  vec2 r = vec2(fbm(p + q + time * speed.x - p.x - p.y), fbm(p + q - time * speed.y));\n"
    "  vec3 c = mix(c1, c2, fbm(p + r)) + mix(c3, c4, r.x) - mix(c5, c6, r.y);\n"
    "  vec3 color = c * cos(shift * vUV.y);\n"
    "  float luminance = dot(color.rgb, vec3(0.3, 0.59, 0.11));\n"
    "\n"
    "  gl_FragColor = vec4(color, luminance * alphaThreshold + (1.0 - alphaThreshold));\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_PROCEDURAL_TEXTURES_LIBRARY_FIRE_FIRE_PROCEDURAL_TEXTURE_FRAGMENT_FX_H
