#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_PBR_BLOCK_REFLECTANCE_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_PBR_BLOCK_REFLECTANCE_FX_H

namespace BABYLON {

extern const char* pbrBlockReflectance;

const char* pbrBlockReflectance
  = R"ShaderCode(

#if defined(ENVIRONMENTBRDF) && !defined(REFLECTIONMAP_SKYBOX)
    vec3 specularEnvironmentReflectance = getReflectanceFromBRDFLookup(clearcoatOut.specularEnvironmentR0, specularEnvironmentR90, environmentBrdf);

    #ifdef RADIANCEOCCLUSION
        specularEnvironmentReflectance *= seo;
    #endif

    #ifdef HORIZONOCCLUSION
        #ifdef BUMP
            #ifdef REFLECTIONMAP_3D
                specularEnvironmentReflectance *= eho;
            #endif
        #endif
    #endif
#else
    // Jones implementation of a well balanced fast analytical solution.
    vec3 specularEnvironmentReflectance = getReflectanceFromAnalyticalBRDFLookup_Jones(NdotV, clearcoatOut.specularEnvironmentR0, specularEnvironmentR90, sqrt(microSurface));
#endif

#ifdef CLEARCOAT
    specularEnvironmentReflectance *= clearcoatOut.conservationFactor;

    #if defined(CLEARCOAT_TINT)
        specularEnvironmentReflectance *= clearcoatOut.absorption;
    #endif
#endif

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_PBR_BLOCK_REFLECTANCE_FX_H
