#ifndef BABYLON_SHADERS_GLOWMAPMERGE_VERTEX_FX_H
#define BABYLON_SHADERS_GLOWMAPMERGE_VERTEX_FX_H

namespace BABYLON {

extern const char* glowMapMergeVertexShader;

const char* glowMapMergeVertexShader
  = "// Attributes\n"
    "attribute vec2 position;\n"
    "\n"
    "// Output\n"
    "varying vec2 vUV;\n"
    "\n"
    "const vec2 madd = vec2(0.5, 0.5);\n"
    "\n"
    "void main(void) {\n"
    "  vUV = position * madd + madd;\n"
    "  gl_Position = vec4(position, 0.0, 1.0);\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_GLOWMAPMERGE_VERTEX_FX_H
