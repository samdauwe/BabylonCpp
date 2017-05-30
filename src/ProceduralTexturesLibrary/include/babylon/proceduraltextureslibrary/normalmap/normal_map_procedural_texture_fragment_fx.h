#ifndef BABYLON_PROCEDURAL_TEXTURES_LIBRARY_NORMALMAP_NORMAL_MAP_PROCEDURAL_TEXTURE_FRAGMENT_FX_H
#define BABYLON_PROCEDURAL_TEXTURES_LIBRARY_NORMALMAP_NORMAL_MAP_PROCEDURAL_TEXTURE_FRAGMENT_FX_H

namespace BABYLON {

extern const char* normalMapProceduralTexturePixelShader;

const char* normalMapProceduralTexturePixelShader
  = "#ifdef GL_ES\n"
    "precision highp float;\n"
    "#endif\n"
    "\n"
    "// Uniforms\n"
    "uniform sampler2D baseSampler;\n"
    "uniform float size;\n"
    "\n"
    "// Varyings\n"
    "varying vec2 vUV;\n"
    "\n"
    "// Constants\n"
    "const vec3 LUMA_COEFFICIENT = vec3(0.2126, 0.7152, 0.0722);\n"
    "\n"
    "float lumaAtCoord(vec2 coord)\n"
    "{\n"
    "  vec3 pixel = texture2D(baseSampler, coord).rgb;\n"
    "  float luma = dot(pixel, LUMA_COEFFICIENT);\n"
    "  return luma;\n"
    "}\n"
    "\n"
    "void main()\n"
    "{\n"
    "  float lumaU0 = lumaAtCoord(vUV + vec2(-1.0,  0.0) / size);\n"
    "  float lumaU1 = lumaAtCoord(vUV + vec2( 1.0,  0.0) / size);\n"
    "  float lumaV0 = lumaAtCoord(vUV + vec2( 0.0, -1.0) / size);\n"
    "  float lumaV1 = lumaAtCoord(vUV + vec2( 0.0,  1.0) / size);\n"
    "\n"
    "  vec2 slope = (vec2(lumaU0 - lumaU1, lumaV0 - lumaV1) + 1.0) * 0.5;\n"
    "\n"
    "  gl_FragColor = vec4(slope, 1.0, 1.0);\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_PROCEDURAL_TEXTURES_LIBRARY_NORMALMAP_NORMAL_MAP_PROCEDURAL_TEXTURE_FRAGMENT_FX_H
