#ifndef BABYLON_SHADERS_VOLUMETRIC_LIGHT_SCATTERING_PASS_FRAGMENT_FX_H
#define BABYLON_SHADERS_VOLUMETRIC_LIGHT_SCATTERING_PASS_FRAGMENT_FX_H

namespace BABYLON {

extern const char* volumetricLightScatteringPassPixelShader;

const char* volumetricLightScatteringPassPixelShader
  = "#if defined(ALPHATEST) || defined(NEED_UV)\n"
    "varying vec2 vUV;\n"
    "#endif\n"
    "\n"
    "#if defined(ALPHATEST)\n"
    "uniform sampler2D diffuseSampler;\n"
    "#endif\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "#if defined(ALPHATEST)\n"
    "  vec4 diffuseColor = texture2D(diffuseSampler, vUV);\n"
    "\n"
    "  if (diffuseColor.a < 0.4)\n"
    "  discard;\n"
    "#endif\n"
    "\n"
    "  gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);\n"
    "}\n"
    "\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_VOLUMETRIC_LIGHT_SCATTERING_PASS_FRAGMENT_FX_H
