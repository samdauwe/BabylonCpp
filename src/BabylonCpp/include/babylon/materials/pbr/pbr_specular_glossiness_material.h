#ifndef BABYLON_MATERIALS_PBR_PBR_SPECULAR_GLOSSINESS_MATERIAL_H
#define BABYLON_MATERIALS_PBR_PBR_SPECULAR_GLOSSINESS_MATERIAL_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * The PBR material of BJS following the metal roughness convention.
 *
 * This fits to the PBR convention in the GLTF definition:
 * https://github.com/KhronosGroup/glTF/tree/2.0/specification/2.0
 */
class BABYLON_SHARED_EXPORT PBRSpecularGlossinessMaterial
  : public Internals::PBRBaseSimpleMaterial {

public:
  PBRSpecularGlossinessMaterial();
  ~PBRSpecularGlossinessMaterial();

}; // end of class PBRSpecularGlossinessMaterial

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_PBR_PBR_SPECULAR_GLOSSINESS_MATERIAL_H