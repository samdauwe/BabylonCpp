#ifndef BABYLON_SHADERS_COLOR_VERTEX_FX_H
#define BABYLON_SHADERS_COLOR_VERTEX_FX_H

namespace BABYLON {

extern const char* colorVertexShader;

const char* colorVertexShader
  = "// Attributes\n"
    "attribute vec3 position;\n"
    "\n"
    "#ifdef VERTEXCOLOR\n"
    "attribute vec4 color;\n"
    "#endif\n"
    "\n"
    "#include<bonesDeclaration>\n"
    "\n"
    "#include<clipPlaneVertexDeclaration>\n"
    "\n"
    "// Uniforms\n"
    "\n"
    "#include<instancesDeclaration>\n"
    "uniform mat4 viewProjection;\n"
    "\n"
    "// Output\n"
    "#ifdef VERTEXCOLOR\n"
    "varying vec4 vColor;\n"
    "#endif\n"
    "\n"
    "void main(void) {\n"
    "#include<instancesVertex>\n"
    "#include<bonesVertex>\n"
    "  vec4 worldPos = finalWorld * vec4(position, 1.0);\n"
    "\n"
    "  gl_Position = viewProjection * worldPos;\n"
    "\n"
    "#include<clipPlaneVertex>\n"
    "\n"
    "#ifdef VERTEXCOLOR\n"
    "  // Vertex color\n"
    "  vColor = color;\n"
    "#endif\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_COLOR_VERTEX_FX_H
