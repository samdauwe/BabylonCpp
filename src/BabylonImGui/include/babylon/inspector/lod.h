#ifndef BABYLON_INSPECTOR_LOD_H
#define BABYLON_INSPECTOR_LOD_H

namespace BABYLON {

extern const char* lodPixelShaderName;
extern const char* lodPixelShader;

const char* lodPixelShaderName = "lodPixelShader";
const char* lodPixelShader     = R"ShaderCode(

#extension GL_EXT_shader_texture_lod : enable

precision highp float;

varying vec2 vUV;
uniform sampler2D textureSampler;
uniform float lod;
uniform vec2 texSize;
void main(void)
{
   gl_FragColor = textureLod(textureSampler,vUV,lod);
}

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_INSPECTOR_LOD_H
