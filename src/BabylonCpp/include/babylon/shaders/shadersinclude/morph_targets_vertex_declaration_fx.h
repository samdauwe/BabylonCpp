#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_MORPH_TARGETS_VERTEX_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_MORPH_TARGETS_VERTEX_DECLARATION_FX_H

namespace BABYLON {

extern const char* morphTargetsVertexDeclaration;

const char* morphTargetsVertexDeclaration
  = R"ShaderCode(

#ifdef MORPHTARGETS
    attribute vec3 position{X};

    #ifdef MORPHTARGETS_NORMAL
    attribute vec3 normal{X};
    #endif

    #ifdef MORPHTARGETS_TANGENT
    attribute vec3 tangent{X};
    #endif

    #ifdef MORPHTARGETS_UV
    attribute vec2 uv_{X};
    #endif
#endif

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_MORPH_TARGETS_VERTEX_DECLARATION_FX_H
