#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_FRAGMENT_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_FRAGMENT_FX_H

namespace BABYLON {

extern const char* clipPlaneFragment;

const char* clipPlaneFragment
  = "#ifdef CLIPPLANE\n"
    "  if (fClipDistance > 0.0)\n"
    "  {\n"
    "  discard;\n"
    "  }\n"
    "#endif\n"
    "\n"
    "#ifdef CLIPPLANE2\n"
    "  if (fClipDistance2 > 0.0)\n"
    "  {\n"
    "  discard;\n"
    "  }\n"
    "#endif\n"
    "\n"
    "#ifdef CLIPPLANE3\n"
    "  if (fClipDistance3 > 0.0)\n"
    "  {\n"
    "  discard;\n"
    "  }\n"
    "#endif\n"
    "\n"
    "#ifdef CLIPPLANE4\n"
    "  if (fClipDistance4 > 0.0)\n"
    "  {\n"
    "  discard;\n"
    "  }\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_FRAGMENT_FX_H
