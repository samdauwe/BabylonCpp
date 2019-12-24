#ifndef BABYLON_SHADERS_PASS_CUBE_FRAGMENT_FX_H
#define BABYLON_SHADERS_PASS_CUBE_FRAGMENT_FX_H

namespace BABYLON {

extern const char* passCubePixelShader;

const char* passCubePixelShader
  = R"ShaderCode(

// Samplers
varying vec2 vUV;
uniform samplerCube textureSampler;

void main(void)
{
    vec2 uv = vUV * 2.0 - 1.0;

#ifdef POSITIVEX
    gl_FragColor = textureCube(textureSampler, vec3(1.001, uv.y, uv.x));
#endif
#ifdef NEGATIVEX
    gl_FragColor = textureCube(textureSampler, vec3(-1.001, uv.y, uv.x));
#endif
#ifdef POSITIVEY
    gl_FragColor = textureCube(textureSampler, vec3(uv.y, 1.001, uv.x));
#endif
#ifdef NEGATIVEY
    gl_FragColor = textureCube(textureSampler, vec3(uv.y, -1.001, uv.x));
#endif
#ifdef POSITIVEZ
    gl_FragColor = textureCube(textureSampler, vec3(uv, 1.001));
#endif
#ifdef NEGATIVEZ
    gl_FragColor = textureCube(textureSampler, vec3(uv, -1.001));
#endif
}

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_PASS_CUBE_FRAGMENT_FX_H
