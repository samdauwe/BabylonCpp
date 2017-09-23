#ifndef BABYLON_SHADERS_OUTLINE_VERTEX_FX_H
#define BABYLON_SHADERS_OUTLINE_VERTEX_FX_H

namespace BABYLON {

extern const char* outlineVertexShader;

const char* outlineVertexShader
  = "// Attribute\n"
    "attribute vec3 position;\n"
    "attribute vec3 normal;\n"
    "\n"
    "#include<bonesDeclaration>\n"
    "\n"
    "// Uniform\n"
    "uniform float offset;\n"
    "\n"
    "#include<instancesDeclaration>\n"
    "\n"
    "uniform mat4 viewProjection;\n"
    "\n"
    "#ifdef ALPHATEST\n"
    "varying vec2 vUV;\n"
    "uniform mat4 diffuseMatrix;\n"
    "#ifdef UV1\n"
    "attribute vec2 uv;\n"
    "#endif\n"
    "#ifdef UV2\n"
    "attribute vec2 uv2;\n"
    "#endif\n"
    "#endif\n"
    "#include<logDepthDeclaration>\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "  vec3 offsetPosition = position + normal * offset;\n"
    "\n"
    "#include<instancesVertex>\n"
    "#include<bonesVertex>\n"
    "\n"
    "  gl_Position = viewProjection * finalWorld * vec4(offsetPosition, 1.0);\n"
    "\n"
    "#ifdef ALPHATEST\n"
    "#ifdef UV1\n"
    "  vUV = vec2(diffuseMatrix * vec4(uv, 1.0, 0.0));\n"
    "#endif\n"
    "#ifdef UV2\n"
    "  vUV = vec2(diffuseMatrix * vec4(uv2, 1.0, 0.0));\n"
    "#endif\n"
    "#endif\n"
    "#include<logDepthVertex>\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_OUTLINE_VERTEX_FX_H
