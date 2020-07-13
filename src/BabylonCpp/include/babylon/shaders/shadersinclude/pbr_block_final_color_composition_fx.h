#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_PBR_BLOCK_FINAL_COLOR_COMPOSITION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_PBR_BLOCK_FINAL_COLOR_COMPOSITION_FX_H

namespace BABYLON {

extern const char* pbrBlockFinalColorComposition;

const char* pbrBlockFinalColorComposition
  = R"ShaderCode(

vec4 finalColor = vec4(
        finalAmbient +
        finalDiffuse +
#ifndef UNLIT
    #ifdef REFLECTION
        finalIrradiance +
    #endif
    #ifdef SPECULARTERM
        finalSpecularScaled +
    #endif
    #ifdef SHEEN
        finalSheenScaled +
    #endif
    #ifdef CLEARCOAT
        finalClearCoatScaled +
    #endif
    #ifdef REFLECTION
        finalRadianceScaled +
        #if defined(SHEEN) && defined(ENVIRONMENTBRDF)
            sheenOut.finalSheenRadianceScaled +
        #endif
        #ifdef CLEARCOAT
            clearcoatOut.finalClearCoatRadianceScaled +
        #endif
    #endif
    #ifdef SS_REFRACTION
        subSurfaceOut.finalRefraction +
    #endif
#endif
        finalEmissive,
        alpha);

// _____________________________ LightMappping _____________________________________
#ifdef LIGHTMAP
    #ifndef LIGHTMAPEXCLUDED
        #ifdef USELIGHTMAPASSHADOWMAP
            finalColor.rgb *= lightmapColor.rgb;
        #else
            finalColor.rgb += lightmapColor.rgb;
        #endif
    #endif
#endif

#define CUSTOM_FRAGMENT_BEFORE_FOG

// _____________________________ Finally ___________________________________________
finalColor = max(finalColor, 0.0);

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_PBR_BLOCK_FINAL_COLOR_COMPOSITION_FX_H
