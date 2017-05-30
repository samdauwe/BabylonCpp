#ifndef BABYLON_SHADERS_SPRITES_VERTEX_FX_H
#define BABYLON_SHADERS_SPRITES_VERTEX_FX_H

namespace BABYLON {

extern const char* spritesVertexShader;

const char* spritesVertexShader
  = "// Attributes\n"
    "attribute vec4 position;\n"
    "attribute vec4 options;\n"
    "attribute vec4 cellInfo;\n"
    "attribute vec4 color;\n"
    "\n"
    "// Uniforms\n"
    "uniform vec2 textureInfos;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "\n"
    "// Output\n"
    "varying vec2 vUV;\n"
    "varying vec4 vColor;\n"
    "\n"
    "#include<fogVertexDeclaration>\n"
    "\n"
    "void main(void) {  \n"
    "  vec3 viewPos = (view * vec4(position.xyz, 1.0)).xyz; \n"
    "  vec2 cornerPos;\n"
    "  \n"
    "  float angle = position.w;\n"
    "  vec2 size = vec2(options.x, options.y);\n"
    "  vec2 offset = options.zw;\n"
    "  vec2 uvScale = textureInfos.xy;\n"
    "\n"
    "  cornerPos = vec2(offset.x - 0.5, offset.y  - 0.5) * size;\n"
    "\n"
    "  // Rotate\n"
    "  vec3 rotatedCorner;\n"
    "  rotatedCorner.x = cornerPos.x * cos(angle) - cornerPos.y * sin(angle);\n"
    "  rotatedCorner.y = cornerPos.x * sin(angle) + cornerPos.y * cos(angle);\n"
    "  rotatedCorner.z = 0.;\n"
    "\n"
    "  // Position\n"
    "  viewPos += rotatedCorner;\n"
    "  gl_Position = projection * vec4(viewPos, 1.0);   \n"
    "\n"
    "  // Color\n"
    "  vColor = color;\n"
    "  \n"
    "  // Texture\n"
    "  vec2 uvOffset = vec2(abs(offset.x - cellInfo.x), 1.0 - abs(offset.y - cellInfo.y));\n"
    "\n"
    "  vUV = (uvOffset + cellInfo.zw) * uvScale;\n"
    "\n"
    "  // Fog\n"
    "#ifdef FOG\n"
    "  vFogDistance = viewPos;\n"
    "#endif\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SPRITES_VERTEX_FX_H
