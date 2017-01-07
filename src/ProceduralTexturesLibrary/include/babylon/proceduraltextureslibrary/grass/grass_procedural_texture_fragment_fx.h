#ifndef BABYLON_PROCEDURAL_TEXTURES_LIBRARY_GRASS_GRASS_PROCEDURAL_TEXTURE_FRAGMENT_FX_H
#define BABYLON_PROCEDURAL_TEXTURES_LIBRARY_GRASS_GRASS_PROCEDURAL_TEXTURE_FRAGMENT_FX_H

namespace BABYLON {

extern const char* grassProceduralTexturePixelShader;

const char* grassProceduralTexturePixelShader
  = "#ifdef GL_ES\n"
    "precision highp float;\n"
    "#endif\n"
    "\n"
    "varying vec2 vPosition;\n"
    "varying vec2 vUV;\n"
    "\n"
    "uniform vec3 herb1Color;\n"
    "uniform vec3 herb2Color;\n"
    "uniform vec3 herb3Color;\n"
    "uniform vec3 groundColor;\n"
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
    "  vec3 color = mix(groundColor, herb1Color, rand(gl_FragCoord.xy * 4.0));\n"
    "  color = mix(color, herb2Color, rand(gl_FragCoord.xy * 8.0));\n"
    "  color = mix(color, herb3Color, rand(gl_FragCoord.xy));\n"
    "  color = mix(color, herb1Color, fbm(gl_FragCoord.xy * 16.0));\n"
    "  gl_FragColor = vec4(color, 1.0);\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_PROCEDURAL_TEXTURES_LIBRARY_GRASS_GRASS_PROCEDURAL_TEXTURE_FRAGMENT_FX_H
