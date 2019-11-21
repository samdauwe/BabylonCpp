#ifndef BABYLON_SHADERS_GLOW_MAP_MERGE_FRAGMENT_FX_H
#define BABYLON_SHADERS_GLOW_MAP_MERGE_FRAGMENT_FX_H

namespace BABYLON {

extern const char* glowMapMergePixelShader;

const char* glowMapMergePixelShader
  = R"ShaderCode(

// Samplers
varying vec2 vUV;
uniform sampler2D textureSampler;
#ifdef EMISSIVE
    uniform sampler2D textureSampler2;
#endif

// Offset
uniform float offset;

void main(void) {
    vec4 baseColor = texture2D(textureSampler, vUV);

    #ifdef EMISSIVE
        baseColor += texture2D(textureSampler2, vUV);
        baseColor *= offset;
    #else
        baseColor.a = abs(offset - baseColor.a);

        #ifdef STROKE
            float alpha = smoothstep(.0, .1, baseColor.a);
            baseColor.a = alpha;
            baseColor.rgb = baseColor.rgb * alpha;
        #endif
    #endif

    gl_FragColor = baseColor;
}

)ShaderCode";
} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_GLOW_MAP_MERGE_FRAGMENT_FX_H
