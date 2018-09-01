#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_FRAGMENT_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_FRAGMENT_DECLARATION_FX_H

namespace BABYLON {

extern const char* clipPlaneFragmentDeclaration;

const char* clipPlaneFragmentDeclaration
  = "#ifdef CLIPPLANE\n"
    "  varying float fClipDistance;\n"
    "#endif\n"
    "\n"
    "#ifdef CLIPPLANE2\n"
    "  varying float fClipDistance2;\n"
    "#endif\n"
    "\n"
    "#ifdef CLIPPLANE3\n"
    "  varying float fClipDistance3;\n"
    "#endif\n"
    "\n"
    "#ifdef CLIPPLANE4\n"
    "  varying float fClipDistance4;\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_FRAGMENT_DECLARATION_FX_H
