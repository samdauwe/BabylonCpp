#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_CLIPPLANEFRAGMENT_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_CLIPPLANEFRAGMENT_FX_H

namespace BABYLON {

extern const char* clipPlaneFragment;

const char* clipPlaneFragment
  = "#ifdef CLIPPLANE\n"
    "  if (fClipDistance > 0.0)\n"
    "  {\n"
    "  discard;\n"
    "  }\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_CLIPPLANEFRAGMENT_FX_H
