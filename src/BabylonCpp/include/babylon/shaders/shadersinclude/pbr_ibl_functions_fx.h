#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_PBR_IBL_FUNCTIONS_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_PBR_IBL_FUNCTIONS_FX_H

namespace BABYLON {

extern const char* pbrIBLFunctions;

const char* pbrIBLFunctions
  = "#if defined(REFLECTION) || defined(SS_REFRACTION)\n"
    "  float getLodFromAlphaG(float cubeMapDimensionPixels, float microsurfaceAverageSlope) {\n"
    "  float microsurfaceAverageSlopeTexels = microsurfaceAverageSlope * cubeMapDimensionPixels;\n"
    "  float lod = log2(microsurfaceAverageSlopeTexels);\n"
    "  return lod;\n"
    "  }\n"
    "#endif\n"
    "\n"
    "#if defined(ENVIRONMENTBRDF) && defined(RADIANCEOCCLUSION)\n"
    "  float environmentRadianceOcclusion(float ambientOcclusion, float NdotVUnclamped) {\n"
    "  // Best balanced (implementation time vs result vs perf) analytical environment specular occlusion found.\n"
    "  // http://research.tri-ace.com/Data/cedec2011_RealtimePBR_Implementation_e.pptx\n"
    "  float temp = NdotVUnclamped + ambientOcclusion;\n"
    "  return saturate(square(temp) - 1.0 + ambientOcclusion);\n"
    "  }\n"
    "#endif\n"
    "\n"
    "#if defined(ENVIRONMENTBRDF) && defined(HORIZONOCCLUSION)\n"
    "  float environmentHorizonOcclusion(vec3 view, vec3 normal) {\n"
    "  // http://marmosetco.tumblr.com/post/81245981087\n"
    "  vec3 reflection = reflect(view, normal);\n"
    "  float temp = saturate(1.0 + 1.1 * dot(reflection, normal));\n"
    "  return square(temp);\n"
    "  }\n"
    "#endif\n"
    "\n"
    "// ___________________________________________________________________________________\n"
    "//\n"
    "// LEGACY\n"
    "// ___________________________________________________________________________________\n"
    "\n"
    "#if defined(LODINREFLECTIONALPHA) || defined(SS_LODINREFRACTIONALPHA)\n"
    "  // To enable 8 bit textures to be used we need to pack and unpack the LOD\n"
    "  //inverse alpha is used to work around low-alpha bugs in Edge and Firefox\n"
    "  #define UNPACK_LOD(x) (1.0 - x) * 255.0\n"
    "\n"
    "  float getLodFromAlphaG(float cubeMapDimensionPixels, float alphaG, float NdotV) {\n"
    "  float microsurfaceAverageSlope = alphaG;\n"
    "\n"
    "  // Compensate for solid angle change between half-vector measure (Blinn-Phong) and reflected-vector measure (Phong):\n"
    "  //  dWr = 4*cos(theta)*dWh,\n"
    "  // where dWr = solid angle (delta omega) in environment incident radiance (reflection-vector) measure;\n"
    "  // where dWh = solid angle (delta omega) in microfacet normal (half-vector) measure;\n"
    "  // so the relationship is proportional to cosine theta = NdotV.\n"
    "  // The constant factor of four is handled elsewhere as part of the scale/offset filter parameters.\n"
    "  microsurfaceAverageSlope *= sqrt(abs(NdotV));\n"
    "\n"
    "  return getLodFromAlphaG(cubeMapDimensionPixels, microsurfaceAverageSlope);\n"
    "  }\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_PBR_IBL_FUNCTIONS_FX_H
