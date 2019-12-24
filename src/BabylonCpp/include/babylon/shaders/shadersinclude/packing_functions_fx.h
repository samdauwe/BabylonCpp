#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_PACKING_FUNCTIONS_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_PACKING_FUNCTIONS_FX_H

namespace BABYLON {

extern const char* packingFunctions;

const char* packingFunctions
  = R"ShaderCode(

vec4 pack(float depth)
{
    const vec4 bit_shift = vec4(255.0 * 255.0 * 255.0, 255.0 * 255.0, 255.0, 1.0);
    const vec4 bit_mask = vec4(0.0, 1.0 / 255.0, 1.0 / 255.0, 1.0 / 255.0);

    vec4 res = fract(depth * bit_shift);
    res -= res.xxyz * bit_mask;

    return res;
}

float unpack(vec4 color)
{
    const vec4 bit_shift = vec4(1.0 / (255.0 * 255.0 * 255.0), 1.0 / (255.0 * 255.0), 1.0 / 255.0, 1.0);
    return dot(color, bit_shift);
}

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_PACKING_FUNCTIONS_FX_H
