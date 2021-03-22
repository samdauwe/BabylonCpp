#ifndef BABYLON_MATERIALS_LIBRARY_GRID_GRID_MATERIAL_H
#define BABYLON_MATERIALS_LIBRARY_GRID_GRID_MATERIAL_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/materials/push_material.h>
#include <babylon/materialslibrary/grid/grid_material_defines.h>
#include <babylon/maths/color3.h>
#include <babylon/maths/vector4.h>

namespace BABYLON {
namespace MaterialsLibrary {

FWD_CLASS_SPTR(GridMaterial)

/**
 * @brief The grid materials allows you to wrap any shape with a grid.
 * Colors are customizable.
 */
class BABYLON_SHARED_EXPORT GridMaterial : public PushMaterial {

public:
  template <typename... Ts>
  static GridMaterialPtr New(Ts&&... args)
  {
    auto material = std::shared_ptr<GridMaterial>(new GridMaterial(std::forward<Ts>(args)...));
    material->addMaterialToScene(material);

    return material;
  }
  ~GridMaterial() override; // = default

  /**
   * @brief Returns whether or not the grid requires alpha blending.
   */
  [[nodiscard]] bool needAlphaBlending() const override;
  [[nodiscard]] bool needAlphaBlendingForMesh(const AbstractMesh& mesh) const override;
  bool isReadyForSubMesh(AbstractMesh* mesh, SubMesh* subMesh, bool useInstances = false) override;
  void bindForSubMesh(Matrix& world, Mesh* mesh, SubMesh* subMesh) override;
  [[nodiscard]] std::string getClassName() const override;

  /**
   * @brief Dispose the material and its associated resources.
   * @param forceDisposeEffect will also dispose the used effect when true
   */
  void dispose(bool forceDisposeEffect = false, bool forceDisposeTextures = false,
               bool notBoundToMesh = false) override;

  [[nodiscard]] MaterialPtr clone(const std::string& name,
                                  bool cloneChildren = false) const override;
  [[nodiscard]] json serialize() const;

  /** Statics **/
  static GridMaterial* Parse(const json& source, Scene* scene, const std::string& rootUrl);

protected:
  /**
   * @brief Constructor
   * @param name The name given to the material in order to identify it
   * afterwards.
   * @param scene The scene the material is used in.
   */
  GridMaterial(const std::string& name, Scene* scene);

  BaseTexturePtr& get_opacityTexture();
  void set_opacityTexture(const BaseTexturePtr& value);

public:
  /**
   * Main color of the grid (e.g. between lines)
   */
  Color3 mainColor;

  /**
   * Color of the grid lines.
   */
  Color3 lineColor;

  /**
   * The scale of the grid compared to unit.
   */
  float gridRatio;

  /**
   * Allows setting an offset for the grid lines.
   */
  Vector3 gridOffset;

  /**
   * The frequency of thicker lines.
   */
  float majorUnitFrequency;

  /**
   * The visibility of minor units in the grid.
   */
  float minorUnitVisibility;

  /**
   * The grid opacity outside of the lines.
   */
  float opacity;

  /**
   * Determine RBG output is premultiplied by alpha value.
   */
  bool preMultiplyAlpha;

  /**
   * The opacity texture used by the grid material.
   */
  Property<GridMaterial, BaseTexturePtr> opacityTexture;

private:
  BaseTexturePtr _opacityTexture;
  Vector4 _gridControl;

}; // end of class GridMaterial

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_GRID_GRID_MATERIAL_H
