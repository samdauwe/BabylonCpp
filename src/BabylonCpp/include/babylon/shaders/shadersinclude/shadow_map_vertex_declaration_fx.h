#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_SHADOW_MAP_VERTEX_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_SHADOW_MAP_VERTEX_DECLARATION_FX_H

namespace BABYLON {

extern const char* shadowMapVertexDeclaration;

const char* shadowMapVertexDeclaration
  = R"ShaderCode(

#if SM_NORMALBIAS == 1
    uniform vec3 lightDataSM;
#endif

uniform vec3 biasAndScaleSM;
uniform vec2 depthValuesSM;

varying float vDepthMetricSM;

#if SM_USEDISTANCE == 1
    varying vec3 vPositionWSM;
#endif

#if defined(SM_DEPTHCLAMP) &&  SM_DEPTHCLAMP == 1
    varying float zSM;
#endif

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_SHADOW_MAP_VERTEX_DECLARATION_FX_H
