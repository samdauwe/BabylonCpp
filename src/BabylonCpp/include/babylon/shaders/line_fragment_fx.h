#ifndef BABYLON_SHADERS_LINE_FRAGMENT_FX_H
#define BABYLON_SHADERS_LINE_FRAGMENT_FX_H

namespace BABYLON {

extern const char* linePixelShader;

const char* linePixelShader
  = R"ShaderCode(

uniform vec4 color;

void main(void) {
    gl_FragColor = color;
}

)ShaderCode";
} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_LINE_FRAGMENT_FX_H
