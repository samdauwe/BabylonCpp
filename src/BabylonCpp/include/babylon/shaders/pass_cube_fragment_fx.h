#ifndef BABYLON_SHADERS_PASS_CUBE_FRAGMENT_FX_H
#define BABYLON_SHADERS_PASS_CUBE_FRAGMENT_FX_H

namespace BABYLON {

extern const char* passCubePixelShader;

const char* passCubePixelShader
  = "// Samplers\n"
    "varying vec2 vUV;\n"
    "uniform samplerCube textureSampler;\n"
    "\n"
    "void main(void) \n"
    "{\n"
    "  vec2 uv = vUV * 2.0 - 1.0;\n"
    "\n"
    "#ifdef POSITIVEX\n"
    "  gl_FragColor = textureCube(textureSampler, vec3(1.001, uv.y, uv.x));\n"
    "#endif\n"
    "#ifdef NEGATIVEX\n"
    "  gl_FragColor = textureCube(textureSampler, vec3(-1.001, uv.y, uv.x));\n"
    "#endif\n"
    "#ifdef POSITIVEY\n"
    "  gl_FragColor = textureCube(textureSampler, vec3(uv.y, 1.001, uv.x));\n"
    "#endif\n"
    "#ifdef NEGATIVEY\n"
    "  gl_FragColor = textureCube(textureSampler, vec3(uv.y, -1.001, uv.x));\n"
    "#endif\n"
    "#ifdef POSITIVEZ\n"
    "  gl_FragColor = textureCube(textureSampler, vec3(uv, 1.001));\n"
    "#endif\n"
    "#ifdef NEGATIVEZ\n"
    "  gl_FragColor = textureCube(textureSampler, vec3(uv, -1.001));\n"
    "#endif\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_PASS_CUBE_FRAGMENT_FX_H
