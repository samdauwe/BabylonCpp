#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_FOGFRAGMENT_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_FOGFRAGMENT_FX_H

namespace BABYLON {

extern const char* fogFragment;

const char* fogFragment
  = "#ifdef FOG\n"
    "  float fog = CalcFogFactor();\n"
    "  color.rgb = fog * color.rgb + (1.0 - fog) * vFogColor;\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_FOGFRAGMENT_FX_H
