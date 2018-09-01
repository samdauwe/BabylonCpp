#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_FRAGMENT_DECLARATION2_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_FRAGMENT_DECLARATION2_FX_H

namespace BABYLON {

extern const char* clipPlaneFragmentDeclaration2;

const char* clipPlaneFragmentDeclaration2
  = "#ifdef CLIPPLANE\n"
    "  in float fClipDistance;\n"
    "#endif\n"
    "\n"
    "#ifdef CLIPPLANE2\n"
    "  in float fClipDistance2;\n"
    "#endif\n"
    "\n"
    "#ifdef CLIPPLANE3\n"
    "  in float fClipDistance3;\n"
    "#endif\n"
    "\n"
    "#ifdef CLIPPLANE4\n"
    "  in float fClipDistance4;\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_FRAGMENT_DECLARATION2_FX_H
