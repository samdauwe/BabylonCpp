#ifndef BABYLON_PROCEDURAL_TEXTURES_LIBRARY_WOOD_WOOD_PROCEDURAL_TEXTURE_FRAGMENT_FX_H
#define BABYLON_PROCEDURAL_TEXTURES_LIBRARY_WOOD_WOOD_PROCEDURAL_TEXTURE_FRAGMENT_FX_H

namespace BABYLON {

extern const char* woodProceduralTexturePixelShader;

const char* woodProceduralTexturePixelShader
  = "#ifdef GL_ES\n"
    "precision highp float;\n"
    "#endif\n"
    "\n"
    "varying vec2 vPosition;\n"
    "varying vec2 vUV;\n"
    "\n"
    "uniform float ampScale;\n"
    "uniform vec3 woodColor;\n"
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
    "void main(void) {\n"
    "  float ratioy = mod(vUV.x * ampScale, 2.0 + fbm(vUV * 0.8));\n"
    "  vec3 wood = woodColor * ratioy;\n"
    "  gl_FragColor = vec4(wood, 1.0);\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_PROCEDURAL_TEXTURES_LIBRARY_WOOD_WOOD_PROCEDURAL_TEXTURE_FRAGMENT_FX_H
