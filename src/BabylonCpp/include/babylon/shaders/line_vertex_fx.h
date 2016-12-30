#ifndef BABYLON_SHADERS_LINE_VERTEX_FX_H
#define BABYLON_SHADERS_LINE_VERTEX_FX_H

namespace BABYLON {

extern const char* lineVertexShader;

const char* lineVertexShader
  = "// Attributes\n"
    "attribute vec3 position;\n"
    "attribute vec4 normal;\n"
    "\n"
    "// Uniforms\n"
    "uniform mat4 worldViewProjection;\n"
    "\n"
    "uniform float width;\n"
    "uniform float aspectRatio;\n"
    "\n"
    "void main(void) {\n"
    "  vec4 viewPosition = worldViewProjection * vec4(position, 1.0);\n"
    "  vec4 viewPositionNext = worldViewProjection * vec4(normal.xyz, 1.0);\n"
    "\n"
    "  vec2 currentScreen = viewPosition.xy / viewPosition.w;\n"
    "  vec2 nextScreen = viewPositionNext.xy / viewPositionNext.w;\n"
    "\n"
    "  currentScreen.x *= aspectRatio;\n"
    "  nextScreen.x *= aspectRatio;\n"
    "\n"
    "  vec2 dir = normalize(nextScreen - currentScreen);\n"
    "  vec2 normalDir = vec2(-dir.y, dir.x);\n"
    "\n"
    "  normalDir *= width / 2.0;\n"
    "  normalDir.x /= aspectRatio;\n"
    "\n"
    "  vec4 offset = vec4(normalDir * normal.w, 0.0, 0.0);\n"
    "  gl_Position = viewPosition + offset;\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_LINE_VERTEX_FX_H
