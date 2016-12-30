#ifndef BABYLON_SHADERS_PARTICLES_FRAGMENT_FX_H
#define BABYLON_SHADERS_PARTICLES_FRAGMENT_FX_H

namespace BABYLON {

extern const char* particlesPixelShader;

const char* particlesPixelShader
  = "// Samplers\n"
    "varying vec2 vUV;\n"
    "varying vec4 vColor;\n"
    "uniform vec4 textureMask;\n"
    "uniform sampler2D diffuseSampler;\n"
    "\n"
    "#ifdef CLIPPLANE\n"
    "varying float fClipDistance;\n"
    "#endif\n"
    "\n"
    "void main(void) {\n"
    "#ifdef CLIPPLANE\n"
    "  if (fClipDistance > 0.0)\n"
    "  discard;\n"
    "#endif\n"
    "  vec4 baseColor = texture2D(diffuseSampler, vUV);\n"
    "\n"
    "  gl_FragColor = (baseColor * textureMask + (vec4(1., 1., 1., 1.) - textureMask)) * vColor;\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_PARTICLES_FRAGMENT_FX_H
