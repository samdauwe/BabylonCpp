#ifndef BABYLON_MATERIALS_LIBRARY_SKY_SKY_MATERIAL_H
#define BABYLON_MATERIALS_LIBRARY_SKY_SKY_MATERIAL_H

#include <babylon/babylon_api.h>
#include <babylon/materials/push_material.h>
#include <babylon/materialslibrary/sky/sky_material_defines.h>
#include <babylon/maths/vector3.h>

namespace BABYLON {

class IAnimatable;
using IAnimatablePtr = std::shared_ptr<IAnimatable>;

namespace MaterialsLibrary {

class SkyMaterial;
using SkyMaterialPtr = std::shared_ptr<SkyMaterial>;

/**
 * @brief This is the sky material which allows to create dynamic and texture free effects for
 * skyboxes.
 * @see https://doc.babylonjs.com/extensions/sky
 */
class BABYLON_SHARED_EXPORT SkyMaterial : public PushMaterial {

public:
  template <typename... Ts>
  static SkyMaterialPtr New(Ts&&... args)
  {
    auto material = std::shared_ptr<SkyMaterial>(new SkyMaterial(std::forward<Ts>(args)...));
    material->addMaterialToScene(material);

    return material;
  }
  ~SkyMaterial() override; // = default

  /**
   * @brief Specifies if the material will require alpha blending.
   * @returns a boolean specifying if alpha blending is needed
   */
  [[nodiscard]] bool needAlphaBlending() const override;

  /**
   * @brief Specifies if this material should be rendered in alpha test mode.
   * @returns false as the sky material doesn't need alpha testing.
   */
  [[nodiscard]] bool needAlphaTesting() const override;

  /**
   * @brief Get the texture used for alpha test purpose.
   * @returns null as the sky material has no texture.
   */
  BaseTexturePtr getAlphaTestTexture() override;

  /**
   * @brief Get if the submesh is ready to be used and all its information available.
   * Child classes can use it to update shaders
   * @param mesh defines the mesh to check
   * @param subMesh defines which submesh to check
   * @param useInstances specifies that instances should be used
   * @returns a boolean indicating that the submesh is ready or not
   */
  bool isReadyForSubMesh(AbstractMesh* mesh, BaseSubMesh* subMesh,
                         bool useInstances = false) override;

  /**
   * @brief Binds the submesh to this material by preparing the effect and shader to draw.
   * @param world defines the world transformation matrix
   * @param mesh defines the mesh containing the submesh
   * @param subMesh defines the submesh to bind the material to
   */
  void bindForSubMesh(Matrix& world, Mesh* mesh, SubMesh* subMesh) override;

  /**
   * @brief Get the list of animatables in the material.
   * @returns the list of animatables object used in the material
   */
  std::vector<IAnimatablePtr> getAnimatables() override;

  /**
   * @brief Gets the current class name of the material e.g. "SkyMaterial".
   * Mainly use in serialization.
   * @returns the class name
   */
  [[nodiscard]] std::string getClassName() const override;

  /**
   * @brief Disposes the material.
   * @param forceDisposeEffect specifies if effects should be forcefully disposed
   */
  void dispose(bool forceDisposeEffect = false, bool forceDisposeTextures = false,
               bool notBoundToMesh = false) override;

  /**
   * @brief Makes a duplicate of the material, and gives it a new name.
   * @param name defines the new name for the duplicated material
   * @returns the cloned material
   */
  [[nodiscard]] MaterialPtr clone(const std::string& name,
                                  bool cloneChildren = false) const override;

  /**
   * @brief Serializes this material in a JSON representation.
   * @returns the serialized material object
   */
  [[nodiscard]] json serialize() const;

  /**
   * @brief Creates a sky material from parsed material data.
   * @param source defines the JSON representation of the material
   * @param scene defines the hosting scene
   * @param rootUrl defines the root URL to use to load textures and relative dependencies
   * @returns a new sky material
   */
  static SkyMaterial* Parse(const json& source, Scene* scene, const std::string& rootUrl);

protected:
  /**
   * @brief Instantiates a new sky material.
   * This material allows to create dynamic and texture free
   * effects for skyboxes by taking care of the atmosphere state.
   * @see https://doc.babylonjs.com/extensions/sky
   * @param name Define the name of the material in the scene
   * @param scene Define the scene the material belong to
   */
  SkyMaterial(const std::string& name, Scene* scene);

public:
  /**
   * Defines the overall luminance of sky in interval ]0, 1[.
   */
  float luminance;

  /**
   * Defines the amount (scattering) of haze as opposed to molecules in atmosphere.
   */
  float turbidity;

  /**
   * Defines the sky appearance (light intensity).
   */
  float rayleigh;

  /**
   * Defines the mieCoefficient in interval [0, 0.1] which affects the property .mieDirectionalG.
   */
  float mieCoefficient;

  /**
   * Defines the amount of haze particles following the Mie scattering theory.
   */
  float mieDirectionalG;

  /**
   * Defines the distance of the sun according to the active scene camera.
   */
  float distance;

  /**
   * Defines the sun inclination, in interval [-0.5, 0.5]. When the inclination is not 0, the sun is
   * said "inclined".
   */
  float inclination;

  /**
   * Defines the solar azimuth in interval [0, 1]. The azimuth is the angle in the horizontal plan
   * between an object direction and a reference direction.
   */
  float azimuth;

  /**
   * Defines the sun position in the sky on (x,y,z). If the property .useSunPosition is set to
   * false, then the property is overriden by the inclination and the azimuth and can be read at any
   * moment.
   */
  Vector3 sunPosition;

  /**
   * Defines if the sun position should be computed (inclination and azimuth) according to the given
   * .sunPosition property.
   */
  bool useSunPosition;

  /**
   * Defines an offset vector used to get a horizon offset.
   * @example skyMaterial.cameraOffset.y = camera.globalPosition.y // Set horizon relative to 0 on
   * the Y axis
   */
  Vector3 cameraOffset;

private:
  Vector3 _cameraPosition;

}; // end of class SkyMaterial

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_SKY_SKY_MATERIAL_H
