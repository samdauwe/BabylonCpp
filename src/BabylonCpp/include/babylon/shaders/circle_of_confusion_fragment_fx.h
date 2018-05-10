#ifndef BABYLON_SHADERS_CIRCLE_OF_CONFUSION_FRAGMENT_FX_H
#define BABYLON_SHADERS_CIRCLE_OF_CONFUSION_FRAGMENT_FX_H

namespace BABYLON {

extern const char* circleOfConfusionPixelShader;

const char* circleOfConfusionPixelShader
  = "// samplers\n"
    "uniform sampler2D depthSampler;\n"
    "\n"
    "// varyings\n"
    "varying vec2 vUV;\n"
    "\n"
    "// preconputed uniforms (not effect parameters)\n"
    "uniform vec2 cameraMinMaxZ;\n"
    "\n"
    "// uniforms\n"
    "uniform float focusDistance;\n"
    "uniform float cocPrecalculation;\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "  float depth = texture2D(depthSampler, vUV).r;\n"
    "  float pixelDistance = (cameraMinMaxZ.x + (cameraMinMaxZ.y - cameraMinMaxZ.x)*depth)*1000.0;  // actual distance from the lens in scene units/1000 (eg. millimeter)\n"
    "  float coc = abs(cocPrecalculation* ((focusDistance - pixelDistance)/pixelDistance));\n"
    "  coc = clamp(coc, 0.0, 1.0);\n"
    "  gl_FragColor = vec4(coc, depth, coc, 1.0);\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_CIRCLE_OF_CONFUSION_FRAGMENT_FX_H
