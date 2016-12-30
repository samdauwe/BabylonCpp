#ifndef BABYLON_SHADERS_VRDISTORTIONCORRECTION_FRAGMENT_FX_H
#define BABYLON_SHADERS_VRDISTORTIONCORRECTION_FRAGMENT_FX_H

namespace BABYLON {

extern const char* vrDistortionCorrectionPixelShader;

const char* vrDistortionCorrectionPixelShader
  = "// Samplers\n"
    "varying vec2 vUV;\n"
    "uniform sampler2D textureSampler;\n"
    "uniform vec2 LensCenter;\n"
    "uniform vec2 Scale;\n"
    "uniform vec2 ScaleIn;\n"
    "uniform vec4 HmdWarpParam;\n"
    "\n"
    "vec2 HmdWarp(vec2 in01) {\n"
    "\n"
    "  vec2 theta = (in01 - LensCenter) * ScaleIn; // Scales to [-1, 1]\n"
    "  float rSq = theta.x * theta.x + theta.y * theta.y;\n"
    "  vec2 rvector = theta * (HmdWarpParam.x + HmdWarpParam.y * rSq + HmdWarpParam.z * rSq * rSq + HmdWarpParam.w * rSq * rSq * rSq);\n"
    "  return LensCenter + Scale * rvector;\n"
    "}\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "  vec2 tc = HmdWarp(vUV);\n"
    "  if (tc.x <0.0 || tc.x>1.0 || tc.y<0.0 || tc.y>1.0)\n"
    "  gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);\n"
    "  else{\n"
    "  gl_FragColor = vec4(texture2D(textureSampler, tc).rgb, 1.0);\n"
    "  }\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_VRDISTORTIONCORRECTION_FRAGMENT_FX_H
