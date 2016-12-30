#ifndef BABYLON_SHADERS_DEPTH_VERTEX_FX_H
#define BABYLON_SHADERS_DEPTH_VERTEX_FX_H

namespace BABYLON {

extern const char* depthVertexShader;

const char* depthVertexShader
  = "// Attribute\n"
    "attribute vec3 position;\n"
    "#include<bonesDeclaration>\n"
    "\n"
    "// Uniform\n"
    "#include<instancesDeclaration>\n"
    "\n"
    "uniform mat4 viewProjection;\n"
    "\n"
    "#if defined(ALPHATEST) || defined(NEED_UV)\n"
    "varying vec2 vUV;\n"
    "uniform mat4 diffuseMatrix;\n"
    "#ifdef UV1\n"
    "attribute vec2 uv;\n"
    "#endif\n"
    "#ifdef UV2\n"
    "attribute vec2 uv2;\n"
    "#endif\n"
    "#endif\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "#include<instancesVertex>\n"
    "\n"
    "#include<bonesVertex>\n"
    "\n"
    "  gl_Position = viewProjection * finalWorld * vec4(position, 1.0);\n"
    "\n"
    "#if defined(ALPHATEST) || defined(BASIC_RENDER)\n"
    "#ifdef UV1\n"
    "  vUV = vec2(diffuseMatrix * vec4(uv, 1.0, 0.0));\n"
    "#endif\n"
    "#ifdef UV2\n"
    "  vUV = vec2(diffuseMatrix * vec4(uv2, 1.0, 0.0));\n"
    "#endif\n"
    "#endif\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_DEPTH_VERTEX_FX_H
