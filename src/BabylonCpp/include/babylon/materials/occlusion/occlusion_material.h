#ifndef BABYLON_MATERIALS_OCCLUSION_OCCLUSION_MATERIAL_H
#define BABYLON_MATERIALS_OCCLUSION_OCCLUSION_MATERIAL_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/materials/shader_material.h>

namespace BABYLON {

FWD_CLASS_SPTR(OcclusionMaterial)

/**
 * @brief A material to use for fast depth-only rendering.
 */
class BABYLON_SHARED_EXPORT OcclusionMaterial : public ShaderMaterial {

public:
  template <typename... Ts>
  static OcclusionMaterialPtr New(Ts&&... args)
  {
    auto material
      = std::shared_ptr<OcclusionMaterial>(new OcclusionMaterial(std::forward<Ts>(args)...));
    material->addMaterialToScene(material);

    return material;
  }
  ~OcclusionMaterial() override; // = default

protected:
  /**
   * @brief Instantiates an Occlusion Material in the given scene
   * @param name The friendly name of the material
   * @param scene The scene to add the material to
   */
  OcclusionMaterial(const std::string& name, Scene* scene);

}; // end of class OcclusionMaterial

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_OCCLUSION_OCCLUSION_MATERIAL_H
