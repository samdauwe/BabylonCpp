#ifndef BABYLON_MATERIALS_PBR_PBR_MATERIAL_H
#define BABYLON_MATERIALS_PBR_PBR_MATERIAL_H

#include <babylon/babylon_global.h>
#include <babylon/materials/push_material.h>

namespace BABYLON {

/**
 * The Physically based material of BJS.
 *
 * This offers the main features of a standard PBR material.
 * For more information, please refer to the documentation :
 * http://doc.babylonjs.com/extensions/Physically_Based_Rendering
 */
class BABYLON_SHARED_EXPORT PBRMaterial : public PBRBaseMaterial {

public:
  PBRMaterial();
  ~PBRMaterial();

}; // end of class PBRMaterial

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_PBR_PBR_MATERIAL_H