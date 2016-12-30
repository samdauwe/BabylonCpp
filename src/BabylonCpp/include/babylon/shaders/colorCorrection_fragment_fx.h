#ifndef BABYLON_SHADERS_COLORCORRECTION_FRAGMENT_FX_H
#define BABYLON_SHADERS_COLORCORRECTION_FRAGMENT_FX_H

namespace BABYLON {

extern const char* colorCorrectionPixelShader;

const char* colorCorrectionPixelShader
  = "// samplers\n"
    "uniform sampler2D textureSampler;  // screen render\n"
    "uniform sampler2D colorTable;    // color table with modified colors\n"
    "\n"
    "// varyings\n"
    "varying vec2 vUV;\n"
    "\n"
    "// constants\n"
    "const float SLICE_COUNT = 16.0;    // how many slices in the color cube; 1 slice = 1 pixel\n"
    "// it means the image is 256x16 pixels\n"
    "\n"
    "vec4 sampleAs3DTexture(sampler2D texture, vec3 uv, float width) {\n"
    "  float sliceSize = 1.0 / width;              // space of 1 slice\n"
    "  float slicePixelSize = sliceSize / width;           // space of 1 pixel\n"
    "  float sliceInnerSize = slicePixelSize * (width - 1.0);  // space of width pixels\n"
    "  float zSlice0 = min(floor(uv.z * width), width - 1.0);\n"
    "  float zSlice1 = min(zSlice0 + 1.0, width - 1.0);\n"
    "  float xOffset = slicePixelSize * 0.5 + uv.x * sliceInnerSize;\n"
    "  float s0 = xOffset + (zSlice0 * sliceSize);\n"
    "  float s1 = xOffset + (zSlice1 * sliceSize);\n"
    "  vec4 slice0Color = texture2D(texture, vec2(s0, uv.y));\n"
    "  vec4 slice1Color = texture2D(texture, vec2(s1, uv.y));\n"
    "  float zOffset = mod(uv.z * width, 1.0);\n"
    "  vec4 result = mix(slice0Color, slice1Color, zOffset);\n"
    "  return result;\n"
    "}\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "  vec4 screen_color = texture2D(textureSampler, vUV);\n"
    "  gl_FragColor = sampleAs3DTexture(colorTable, screen_color.rgb, SLICE_COUNT);\n"
    "\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_COLORCORRECTION_FRAGMENT_FX_H
