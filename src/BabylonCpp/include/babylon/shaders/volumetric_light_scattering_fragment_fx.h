#ifndef BABYLON_SHADERS_VOLUMETRIC_LIGHT_SCATTERING_FRAGMENT_FX_H
#define BABYLON_SHADERS_VOLUMETRIC_LIGHT_SCATTERING_FRAGMENT_FX_H

namespace BABYLON {

extern const char* volumetricLightScatteringPixelShader;

const char* volumetricLightScatteringPixelShader
  = "uniform sampler2D textureSampler;\n"
    "uniform sampler2D lightScatteringSampler;\n"
    "\n"
    "uniform float decay;\n"
    "uniform float exposure;\n"
    "uniform float weight;\n"
    "uniform float density;\n"
    "uniform vec2 meshPositionOnScreen;\n"
    "\n"
    "varying vec2 vUV;\n"
    "\n"
    "void main(void) {\n"
    "  vec2 tc = vUV;\n"
    "  vec2 deltaTexCoord = (tc - meshPositionOnScreen.xy);\n"
    "  deltaTexCoord *= 1.0 / float(NUM_SAMPLES) * density;\n"
    "\n"
    "  float illuminationDecay = 1.0;\n"
    "\n"
    "  vec4 color = texture2D(lightScatteringSampler, tc) * 0.4;\n"
    "\n"
    "  for(int i=0; i < NUM_SAMPLES; i++) {\n"
    "  tc -= deltaTexCoord;\n"
    "  vec4 sample = texture2D(lightScatteringSampler, tc) * 0.4;\n"
    "  sample *= illuminationDecay * weight;\n"
    "  color += sample;\n"
    "  illuminationDecay *= decay;\n"
    "  }\n"
    "\n"
    "  vec4 realColor = texture2D(textureSampler, vUV);\n"
    "  gl_FragColor = ((vec4((vec3(color.r, color.g, color.b) * exposure), 1)) + (realColor * (1.5 - 0.4)));\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_VOLUMETRIC_LIGHT_SCATTERING_FRAGMENT_FX_H
