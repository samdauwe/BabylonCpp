#ifndef BABYLON_SHADERS_FXAA_FRAGMENT_FX_H
#define BABYLON_SHADERS_FXAA_FRAGMENT_FX_H

namespace BABYLON {

extern const char* fxaaPixelShader;

const char* fxaaPixelShader
  = "#define FXAA_REDUCE_MIN   (1.0/128.0)\n"
    "#define FXAA_REDUCE_MUL   (1.0/8.0)\n"
    "#define FXAA_SPAN_MAX     8.0\n"
    "\n"
    "varying vec2 vUV;\n"
    "uniform sampler2D textureSampler;\n"
    "uniform vec2 texelSize;\n"
    "\n"
    "void main(){\n"
    "  vec2 localTexelSize = texelSize;\n"
    "  vec4 rgbNW = texture2D(textureSampler, (vUV + vec2(-1.0, -1.0) * localTexelSize));\n"
    "  vec4 rgbNE = texture2D(textureSampler, (vUV + vec2(1.0, -1.0) * localTexelSize));\n"
    "  vec4 rgbSW = texture2D(textureSampler, (vUV + vec2(-1.0, 1.0) * localTexelSize));\n"
    "  vec4 rgbSE = texture2D(textureSampler, (vUV + vec2(1.0, 1.0) * localTexelSize));\n"
    "  vec4 rgbM = texture2D(textureSampler, vUV);\n"
    "  vec4 luma = vec4(0.299, 0.587, 0.114, 1.0);\n"
    "  float lumaNW = dot(rgbNW, luma);\n"
    "  float lumaNE = dot(rgbNE, luma);\n"
    "  float lumaSW = dot(rgbSW, luma);\n"
    "  float lumaSE = dot(rgbSE, luma);\n"
    "  float lumaM = dot(rgbM, luma);\n"
    "  float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));\n"
    "  float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));\n"
    "\n"
    "  vec2 dir = vec2(-((lumaNW + lumaNE) - (lumaSW + lumaSE)), ((lumaNW + lumaSW) - (lumaNE + lumaSE)));\n"
    "\n"
    "  float dirReduce = max(\n"
    "  (lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL),\n"
    "  FXAA_REDUCE_MIN);\n"
    "\n"
    "  float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);\n"
    "  dir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX),\n"
    "  max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX),\n"
    "  dir * rcpDirMin)) * localTexelSize;\n"
    "\n"
    "  vec4 rgbA = 0.5 * (\n"
    "  texture2D(textureSampler, vUV + dir * (1.0 / 3.0 - 0.5)) +\n"
    "  texture2D(textureSampler, vUV + dir * (2.0 / 3.0 - 0.5)));\n"
    "\n"
    "  vec4 rgbB = rgbA * 0.5 + 0.25 * (\n"
    "  texture2D(textureSampler, vUV + dir *  -0.5) +\n"
    "  texture2D(textureSampler, vUV + dir * 0.5));\n"
    "  float lumaB = dot(rgbB, luma);\n"
    "  if ((lumaB < lumaMin) || (lumaB > lumaMax)) {\n"
    "  gl_FragColor = rgbA;\n"
    "  }\n"
    "  else {\n"
    "  gl_FragColor = rgbB;\n"
    "  }\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_FXAA_FRAGMENT_FX_H
