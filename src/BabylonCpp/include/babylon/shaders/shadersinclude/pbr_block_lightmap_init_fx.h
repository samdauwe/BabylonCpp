#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_PBR_BLOCK_LIGHTMAP_INIT_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_PBR_BLOCK_LIGHTMAP_INIT_FX_H

namespace BABYLON {

extern const char* pbrBlockLightmapInit;

const char* pbrBlockLightmapInit
  = R"ShaderCode(

#ifdef LIGHTMAP
    vec4 lightmapColor = texture2D(lightmapSampler, vLightmapUV + uvOffset);

    #ifdef RGBDLIGHTMAP
        lightmapColor.rgb = fromRGBD(lightmapColor);
    #endif

    #ifdef GAMMALIGHTMAP
        lightmapColor.rgb = toLinearSpace(lightmapColor.rgb);
    #endif
    lightmapColor.rgb *= vLightmapInfos.y;
#endif

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_PBR_BLOCK_LIGHTMAP_INIT_FX_H
