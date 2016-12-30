#ifndef BABYLON_SHADERS_FILTER_FRAGMENT_FX_H
#define BABYLON_SHADERS_FILTER_FRAGMENT_FX_H

namespace BABYLON {

extern const char* filterPixelShader;

const char* filterPixelShader
  = "// Samplers\n"
    "varying vec2 vUV;\n"
    "uniform sampler2D textureSampler;\n"
    "\n"
    "uniform mat4 kernelMatrix;\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "  vec3 baseColor = texture2D(textureSampler, vUV).rgb;\n"
    "  vec3 updatedColor = (kernelMatrix * vec4(baseColor, 1.0)).rgb;\n"
    "\n"
    "  gl_FragColor = vec4(updatedColor, 1.0);\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_FILTER_FRAGMENT_FX_H
