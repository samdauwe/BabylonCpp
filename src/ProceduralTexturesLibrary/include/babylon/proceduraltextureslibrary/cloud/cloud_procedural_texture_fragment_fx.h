#ifndef BABYLON_PROCEDURAL_TEXTURES_LIBRARY_CLOUD_CLOUD_PROCEDURAL_TEXTURE_FRAGMENT_FX_H
#define BABYLON_PROCEDURAL_TEXTURES_LIBRARY_CLOUD_CLOUD_PROCEDURAL_TEXTURE_FRAGMENT_FX_H

namespace BABYLON {

extern const char* cloudProceduralTexturePixelShader;

const char* cloudProceduralTexturePixelShader
  = "#ifdef GL_ES\n"
    "precision highp float;\n"
    "#endif\n"
    "\n"
    "varying vec2 vUV;\n"
    "\n"
    "uniform vec4 skyColor;\n"
    "uniform vec4 cloudColor;\n"
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
    "\n"
    "  vec2 p = vUV * 12.0;\n"
    "  vec4 c = mix(skyColor, cloudColor, fbm(p));\n"
    "  gl_FragColor = c;\n"
    "\n"
    "}\n"
    "\n";

} // end of namespace BABYLON

#endif // end of BABYLON_PROCEDURAL_TEXTURES_LIBRARY_CLOUD_CLOUD_PROCEDURAL_TEXTURE_FRAGMENT_FX_H
