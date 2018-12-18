#ifndef BABYLON_LIGHTS_LIGHT_H
#define BABYLON_LIGHTS_LIGHT_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/engine/node.h>
#include <babylon/math/color3.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class Effect;
struct IShadowGenerator;
class Light;
struct MaterialDefines;
class UniformBuffer;
using EffectPtr           = std::shared_ptr<Effect>;
using IShadowGeneratorPtr = std::shared_ptr<IShadowGenerator>;
using LightPtr            = std::shared_ptr<Light>;

/**
 * @brief Base class of all the lights in Babylon. It groups all the generic
 * information about lights. Lights are used, as you would expect, to affect how
 * meshes are seen, in terms of both illumination and colour. All meshes allow
 * light to pass through them unless shadow generation is activated. The default
 * number of lights allowed is four but this can be increased.
 */
class BABYLON_SHARED_EXPORT Light : public Node {

public:
  /**
   * Falloff Default: light is falling off following the material specification:
   * standard material is using standard falloff whereas pbr material can
   * request special falloff per materials.
   */
  static constexpr unsigned int FALLOFF_DEFAULT = 0;

  /**
   * Falloff Physical: light is falling off following the inverse squared
   * distance law.
   */
  static constexpr unsigned int FALLOFF_PHYSICAL = 1;

  /**
   * Falloff gltf: light is falling off as described in the gltf moving to PBR
   * document to enhance interoperability with other engines.
   */
  static constexpr unsigned int FALLOFF_GLTF = 2;

  /**
   * Falloff Standard: light is falling off like in the standard material
   * to enhance interoperability with other materials.
   */
  static constexpr unsigned int FALLOFF_STANDARD = 3;

  // lightmapMode Consts

  /**
   * If every light affecting the material is in this lightmapMode,
   * material.lightmapTexture adds or multiplies (depends on
   * material.useLightmapAsShadowmap) after every other light calculations.
   */
  static constexpr unsigned int LIGHTMAP_DEFAULT = 0;

  /**
   * material.lightmapTexture as only diffuse lighting from this light adds only
   * specular lighting from this light adds dynamic shadows.
   */
  static constexpr unsigned int LIGHTMAP_SPECULAR = 1;

  /**
   * material.lightmapTexture as only lighting no light calculation from this
   * light only adds dynamic shadows from this light.
   */
  static constexpr unsigned int LIGHTMAP_SHADOWSONLY = 2;

  // Intensity Mode Consts

  /**
   * Each light type uses the default quantity according to its type:
   *      point/spot lights use luminous intensity
   *      directional lights use illuminance
   */
  static constexpr unsigned int INTENSITYMODE_AUTOMATIC = 0;

  /**
   * lumen (lm)
   */
  static constexpr unsigned int INTENSITYMODE_LUMINOUSPOWER = 1;

  /**
   * candela (lm/sr)
   */
  static constexpr unsigned int INTENSITYMODE_LUMINOUSINTENSITY = 2;

  /**
   * lux (lm/m^2)
   */
  static constexpr unsigned int INTENSITYMODE_ILLUMINANCE = 3;

  /**
   * nit (cd/m^2)
   */
  static constexpr unsigned int INTENSITYMODE_LUMINANCE = 4;

  // Light types ids const.

  /**
   * Light type const id of the point light.
   */
  static constexpr unsigned int LIGHTTYPEID_POINTLIGHT = 0;

  /**
   * Light type const id of the directional light.
   */
  static constexpr unsigned int LIGHTTYPEID_DIRECTIONALLIGHT = 1;

  /**
   * Light type const id of the spot light.
   */
  static constexpr unsigned int LIGHTTYPEID_SPOTLIGHT = 2;

  /**
   * Light type const id of the hemispheric light.
   */
  static constexpr unsigned int LIGHTTYPEID_HEMISPHERICLIGHT = 3;

public:
  ~Light() override;

  virtual IReflect::Type type() const override;
  void addToScene(const LightPtr& newLight);

  /**
   * @brief Returns the string "Light".
   * @returns the class name
   */
  const std::string getClassName() const override;

  /**
   * @brief Converts the light information to a readable string for debug
   * purpose.
   * @param fullDetails Supports for multiple levels of logging within scene
   * loading
   * @returns the human readable light info
   */
  std::string toString(bool fullDetails = false) const;

  /**
   * @brief Set the enabled state of this node.
   * @param value - the new enabled state
   */
  void setEnabled(bool value);

  /**
   * @brief Returns the Light associated shadow generator if any.
   * @return the associated shadow generator.
   */
  virtual IShadowGeneratorPtr getShadowGenerator();

  /**
   * @brief Returns a Vector3, the absolute light position in the World.
   * @returns the world space position of the light
   */
  virtual Vector3 getAbsolutePosition();

  /**
   * @brief Sets the passed Effect "effect" with the Light information.
   * @param effect The effect to update
   * @param lightIndex The index of the light in the effect to update
   * @returns The light
   */
  virtual void transferToEffect(const EffectPtr& effect,
                                const std::string& lightIndex)
    = 0;

  virtual void transferToEffect(const EffectPtr& effect,
                                const std::string& uniformName0,
                                const std::string& uniformName1);

  /**
   * @brief Specifies if the light will affect the passed mesh.
   * @param mesh The mesh to test against the light
   * @return true the mesh is affected otherwise, false.
   */
  bool canAffectMesh(AbstractMesh* mesh);

  /**
   * @brief Releases resources associated with this node.
   * @param doNotRecurse Set to true to not recurse into each children (recurse
   * into each children by default)
   * @param disposeMaterialAndTextures Set to true to also dispose referenced
   * materials and textures (false by default)
   */
  virtual void dispose(bool doNotRecurse               = false,
                       bool disposeMaterialAndTextures = false) override;

  /**
   * @brief Returns the light type ID (integer).
   * @returns The light Type id as a constant defines in Light.LIGHTTYPEID_x
   */
  virtual unsigned int getTypeID() const;

  /**
   * @brief Returns the intensity scaled by the Photometric Scale according to
   * the light type and intensity mode.
   * @returns the scaled intensity in intensity mode unit
   */
  float getScaledIntensity() const;

  /**
   * @brief Returns a new Light object, named "name", from the current one.
   * @param name The name of the cloned light
   * @returns the new created light
   */
  std::unique_ptr<Light> clone(const std::string& name);

  /**
   * @brief Serializes the current light into a Serialization object.
   * @returns the serialized object.
   */
  json serialize() const;

  /**
   * @brief Forces the meshes to update their light related information in their
   * rendering used effects
   * Internal Use Only
   */
  void _markMeshesAsLightDirty();

  /**
   * @brief Reorder the light in the scene according to their defined priority.
   * Internal Use Only
   */
  void _reorderLightsInScene();

  /**
   * @brief Prepares the list of defines specific to the light type.
   * @param defines the list of defines
   * @param lightIndex defines the index of the light for the effect
   */
  virtual void prepareLightSpecificDefines(MaterialDefines& defines,
                                           unsigned int lightIndex)
    = 0;

  /**
   * @brief Sort function to order lights for rendering.
   * @param a First Light object to compare to second.
   * @param b Second Light object to compare first.
   * @return -1 to reduce's a's index relative to be, 0 for no change, 1 to
   * increase a's index relative to b.
   */
  static int CompareLightsPriority(Light* a, Light* b);

  /**
   * @brief Creates a new typed light from the passed type (integer) : point
   * light = 0, directional light = 1, spot light = 2, hemispheric light = 3.
   * This new light is named "name" and added to the passed scene.
   * @param type Type according to the types available in Light.LIGHTTYPEID_x
   * @param name The friendly name of the light
   * @param scene The scene the new light will belong to
   * @returns the constructor function
   */
  static std::function<LightPtr()>
  GetConstructorFromName(unsigned int type, const std::string& name,
                         Scene* scene);

  /**
   * @brief Parses the passed "parsedLight" and returns a new instanced Light
   * from this parsing.
   * @param parsedLight The JSON representation of the light
   * @param scene The scene to create the parsed light in
   * @returns the created light after parsing
   */
  static LightPtr Parse(const json& parsedLight, Scene* scene);

protected:
  /**
   * @brief Creates a Light object in the scene.
   * Documentation : http://doc.babylonjs.com/tutorials/lights
   * @param name The firendly name of the light
   * @param scene The scene the light belongs too
   */
  Light(const std::string& name, Scene* scene);

  /**
   * @brief Defines how far from the source the light is impacting in scene
   * units. Note: Unused in PBR material as the distance light falloff is
   * defined following the inverse squared falloff.
   */
  float get_range() const;

  /**
   * @brief Defines how far from the source the light is impacting in scene
   * units. Note: Unused in PBR material as the distance light falloff is
   * defined following the inverse squared falloff.
   */
  void set_range(float value);

  /**
   * @brief Gets the photometric scale used to interpret the intensity.
   * This is only relevant with PBR Materials where the light intensity can be
   * defined in a physical way.
   */
  unsigned int get_intensityMode() const;

  /**
   * @brief Sets the photometric scale used to interpret the intensity.
   * This is only relevant with PBR Materials where the light intensity can be
   * defined in a physical way.
   */
  void set_intensityMode(unsigned int value);

  /**
   * @brief Gets the light radius used by PBR Materials to simulate soft area
   * lights.
   */
  float get_radius() const;

  /**
   * @brief Sets the light radius used by PBR Materials to simulate soft area
   * lights.
   */
  void set_radius(float value);

  /**
   * @brief Gets wether or not the shadows are enabled for this light. This can
   * help turning off/on shadow without detaching the current shadow generator.
   */
  bool get_shadowEnabled() const;

  /**
   * @brief Sets wether or not the shadows are enabled for this light. This can
   * help turning off/on shadow without detaching the current shadow generator.
   */
  void set_shadowEnabled(bool value);

  /**
   * @brief Gets the rendering priority of the lights. It can help in case of
   * fallback or number of lights exceeding the number allowed of the materials.
   */
  int get_renderPriority() const;

  /**
   * @brief Sets the rendering priority of the lights. It can help in case of
   * fallback or number of lights exceeding the number allowed of the materials.
   */
  void set_renderPriority(int value);

  /**
   * @brief Gets the only meshes impacted by this light.
   */
  std::vector<AbstractMesh*>& get_includedOnlyMeshes();

  /**
   * @brief Sets the only meshes impacted by this light.
   */
  void set_includedOnlyMeshes(const std::vector<AbstractMesh*>& value);

  /**
   * @brief Gets the meshes not impacted by this light.
   */
  std::vector<AbstractMesh*>& get_excludedMeshes();

  /**
   * @brief Sets the meshes not impacted by this light.
   */
  void set_excludedMeshes(const std::vector<AbstractMesh*>& value);

  /**
   * @brief Gets the layer id use to find what meshes are not impacted by the
   * light. Inactive if 0
   */
  unsigned int get_excludeWithLayerMask() const;

  /**
   * @brief Sets the layer id use to find what meshes are not impacted by the
   * light. Inactive if 0
   */
  void set_excludeWithLayerMask(unsigned int value);

  /**
   * @brief Gets the layer id use to find what meshes are impacted by the light.
   * Inactive if 0
   */
  unsigned int get_includeOnlyWithLayerMask() const;

  /**
   * @brief Sets the layer id use to find what meshes are impacted by the light.
   * Inactive if 0
   */
  void set_includeOnlyWithLayerMask(unsigned int value);

  /**
   * @brief Gets the lightmap mode of this light (should be one of the constants
   * defined by Light.LIGHTMAP_x)
   */
  unsigned int get_lightmapMode() const;

  /**
   * @brief Sets the lightmap mode of this light (should be one of the constants
   * defined by Light.LIGHTMAP_x)
   */
  void set_lightmapMode(unsigned int value);

  /**
   * @brief Hidden
   */
  void _syncParentEnabledState() override;

  virtual void _buildUniformLayout();
  void _resyncMeshes();

private:
  void _hookArrayForExcluded(const std::vector<AbstractMesh*>& array);
  void _hookArrayForIncludedOnly(const std::vector<AbstractMesh*>& array);

  /**
   * @brief Recomputes the cached photometric scale if needed.
   */
  void _computePhotometricScale();

  /**
   * @brief Returns the Photometric Scale according to the light type and
   * intensity mode.
   */
  float _getPhotometricScale();

public:
  /**
   * Diffuse gives the basic color to an object.
   */
  Color3 diffuse;

  /**
   * Specular produces a highlight color on an object.
   * Note: This is note affecting PBR materials.
   */
  Color3 specular;

  /**
   * Defines how far from the source the light is impacting in scene units.
   * Note: Unused in PBR material as the distance light falloff is defined
   * following the inverse squared falloff.
   */
  Property<Light, float> range;

  /**
   * Defines the falloff type for this light. This lets overrriding how punctual
   * light are falling off base on range or angle. This can be set to any values
   * in Light.FALLOFF_x.
   *
   * Note: This is only usefull for PBR Materials at the moment. This could be
   * extended if required to other types of materials.
   */
  unsigned int falloffType;

  /**
   * Strength of the light.
   * Note: By default it is define in the framework own unit.
   * Note: In PBR materials the intensityMode can be use to chose what unit the
   * intensity is defined in.
   */
  float intensity;

  /**
   * Shadow generator associted to the light.
   * Hidden Internal use only.
   */
  IShadowGeneratorPtr _shadowGenerator;

  /**
   * Internal use only.
   */
  std::vector<std::string> _excludedMeshesIds;

  /**
   * Internal use only.
   */
  std::vector<std::string> _includedOnlyMeshesIds;

  /**
   * The current light unifom buffer.
   * Internal use only.
   */
  std::unique_ptr<UniformBuffer> _uniformBuffer;

  /**
   * Photometric scale used to interpret the intensity
   */
  Property<Light, unsigned int> intensityMode;

  /**
   * Light radius used by PBR Materials to simulate soft area lights
   */
  Property<Light, float> radius;

  /**
   * Wether or not the shadows are enabled for this light. This can help turning
   * off/on shadow without detaching the current shadow generator.
   */
  Property<Light, bool> shadowEnabled;

  /**
   * Rendering priority of the lights. It can help in case of fallback or number
   * of lights exceeding the number allowed of the materials
   */
  Property<Light, int> renderPriority;

  /**
   * Meshes impacted by this light
   */
  Property<Light, std::vector<AbstractMesh*>> includedOnlyMeshes;

  /**
   * Meshes not impacted by this light
   */
  Property<Light, std::vector<AbstractMesh*>> excludedMeshes;

  /**
   * Layer id use to find what meshes are not impacted by the light
   */
  Property<Light, unsigned int> excludeWithLayerMask;

  /**
   * Layer id use to find what meshes are impacted by the light
   */
  Property<Light, unsigned int> includeOnlyWithLayerMask;

  /**
   * Lightmap mode of this light
   */
  Property<Light, unsigned int> lightmapMode;

protected:
  float _inverseSquaredRange;

private:
  float _range;

  /**
   * Cached photometric scale default to 1.0 as the automatic intensity mode
   * defaults to 1.0 for every type
   * of light.
   */
  float _photometricScale;
  unsigned int _intensityMode;
  float _radius;
  int _renderPriority;
  bool _shadowEnabled;
  std::vector<AbstractMesh*> _includedOnlyMeshes;
  std::vector<AbstractMesh*> _excludedMeshes;
  unsigned int _includeOnlyWithLayerMask;
  unsigned int _excludeWithLayerMask;
  unsigned int _lightmapMode;

}; // end of class Light

} // end of namespace BABYLON

#endif // end of BABYLON_LIGHTS_LIGHT_H
