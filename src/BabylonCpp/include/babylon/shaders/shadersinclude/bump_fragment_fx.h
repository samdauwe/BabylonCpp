#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_BUMP_FRAGMENT_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_BUMP_FRAGMENT_FX_H

namespace BABYLON {

extern const char* bumpFragment;

const char* bumpFragment
  = "vec2 uvOffset = vec2(0.0, 0.0);\n"
    "\n"
    "#if defined(BUMP) || defined(PARALLAX)\n"
    "  mat3 TBN = cotangent_frame(normalW * vBumpInfos.y, -viewDirectionW, vBumpUV);\n"
    "#endif\n"
    "\n"
    "#ifdef PARALLAX\n"
    "  mat3 invTBN = transposeMat3(TBN);\n"
    "\n"
    "  #ifdef PARALLAXOCCLUSION\n"
    "  uvOffset = parallaxOcclusion(invTBN * -viewDirectionW, invTBN * normalW, vBumpUV, vBumpInfos.z);\n"
    "  #else\n"
    "  uvOffset = parallaxOffset(invTBN * viewDirectionW, vBumpInfos.z);\n"
    "  #endif\n"
    "#endif\n"
    "\n"
    "#ifdef BUMP\n"
    "  normalW = perturbNormal(viewDirectionW, TBN, vBumpUV + uvOffset);\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_BUMP_FRAGMENT_FX_H
