#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_FRAGMENT_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_FRAGMENT_DECLARATION_FX_H

namespace BABYLON {

extern const char* clipPlaneFragmentDeclaration;

const char* clipPlaneFragmentDeclaration
  = "#ifdef CLIPPLANE\n"
    "  varying float fClipDistance;\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_FRAGMENT_DECLARATION_FX_H
