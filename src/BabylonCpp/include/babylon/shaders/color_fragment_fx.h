#ifndef BABYLON_SHADERS_COLOR_FRAGMENT_FX_H
#define BABYLON_SHADERS_COLOR_FRAGMENT_FX_H

namespace BABYLON {

extern const char* colorPixelShader;

const char* colorPixelShader
  = R"ShaderCode(


#ifdef VERTEXCOLOR
varying vec4 vColor;
#else
uniform vec4 color;
#endif

#include<clipPlaneFragmentDeclaration>

void main(void) {
#include<clipPlaneFragment>

#ifdef VERTEXCOLOR
    gl_FragColor = vColor;
#else
    gl_FragColor = color;
#endif
}

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_COLOR_FRAGMENT_FX_H
