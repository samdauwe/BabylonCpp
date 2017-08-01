#ifndef BABYLON_MATERIALS_PBR_PBR_BASE_SIMPLE_MATERIAL_H
#define BABYLON_MATERIALS_PBR_PBR_BASE_SIMPLE_MATERIAL_H

#include <babylon/babylon_global.h>
#include <babylon/materials/pbr/pbr_base_material.h>

namespace BABYLON {
namespace Internals {

/**
 * The Physically based simple base material of BJS.
 *
 * This enables better naming and convention enforcements on top of the
 * pbrMaterial.
 * It is used as the base class for both the specGloss and metalRough
 * conventions.
 */
class BABYLON_SHARED_EXPORT PBRBaseSimpleMaterial : public PBRBaseMaterial {

public:
  PBRBaseSimpleMaterial();
  ~PBRBaseSimpleMaterial();

}; // end of class PBRBaseSimpleMaterial

} // end of namespace Internals
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_PBR_PBR_BASE_SIMPLE_MATERIAL_H
