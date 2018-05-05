#ifndef BABYLON_LIGHTS_LIGHT_H
#define BABYLON_LIGHTS_LIGHT_H

#include <babylon/babylon_global.h>
#include <babylon/engine/node.h>
#include <babylon/math/color3.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

/**
 * @brief Base class of all the lights in Babylon. It groups all the generic
 * information about lights. Lights are used, as you would expect, to affect how
 * meshes are seen, in terms of both illumination and colour. All meshes allow
 * light to pass through them unless shadow generation is activated. The default
 * number of lights allowed is four but this can be increased.
 */
class BABYLON_SHARED_EXPORT Light : public Node {

private:
  // lightmapMode Consts
  static constexpr unsigned int _LIGHTMAP_DEFAULT     = 0;
  static constexpr unsigned int _LIGHTMAP_SPECULAR    = 1;
  static constexpr unsigned int _LIGHTMAP_SHADOWSONLY = 2;

  // Intensity Mode Consts
  static constexpr unsigned int _INTENSITYMODE_AUTOMATIC         = 0;
  static constexpr unsigned int _INTENSITYMODE_LUMINOUSPOWER     = 1;
  static constexpr unsigned int _INTENSITYMODE_LUMINOUSINTENSITY = 2;
  static constexpr unsigned int _INTENSITYMODE_ILLUMINANCE       = 3;
  static constexpr unsigned int _INTENSITYMODE_LUMINANCE         = 4;

  // Light types ids const.
  static constexpr unsigned int _LIGHTTYPEID_POINTLIGHT       = 0;
  static constexpr unsigned int _LIGHTTYPEID_DIRECTIONALLIGHT = 1;
  static constexpr unsigned int _LIGHTTYPEID_SPOTLIGHT        = 2;
  static constexpr unsigned int _LIGHTTYPEID_HEMISPHERICLIGHT = 3;

public:
  /**
   * @brief If every light affecting the material is in this lightmapMode,
   * material.lightmapTexture adds or multiplies
   * (depends on material.useLightmapAsShadowmap)
   * after every other light calculations.
   */
  static constexpr unsigned int LIGHTMAP_DEFAULT()
  {
    return Light::_LIGHTMAP_DEFAULT;
  }

  /**
   * @brief material.lightmapTexture as only diffuse lighting from this light
   * adds only specular lighting from this light
   * adds dynamic shadows
   */
  static constexpr unsigned int LIGHTMAP_SPECULAR()
  {
    return Light::_LIGHTMAP_SPECULAR;
  }

  /**
   * @brief material.lightmapTexture as only lighting
   * no light calculation from this light
   * only adds dynamic shadows from this light
   */
  static constexpr unsigned int LIGHTMAP_SHADOWSONLY()
  {
    return Light::_LIGHTMAP_SHADOWSONLY;
  }

  /**
   * @brief Each light type uses the default quantity according to its type:
   *      point/spot lights use luminous intensity
   *      directional lights use illuminance
   */
  static constexpr unsigned int INTENSITYMODE_AUTOMATIC()
  {
    return Light::_INTENSITYMODE_AUTOMATIC;
  }

  /**
   * @brief lumen (lm)
   */
  static constexpr unsigned int INTENSITYMODE_LUMINOUSPOWER()
  {
    return Light::_INTENSITYMODE_LUMINOUSPOWER;
  }

  /**
   * @brief candela (lm/sr)
   */
  static constexpr unsigned int INTENSITYMODE_LUMINOUSINTENSITY()
  {
    return Light::_INTENSITYMODE_LUMINOUSINTENSITY;
  }

  /**
   * @brief lux (lm/m^2)
   */
  static constexpr unsigned int INTENSITYMODE_ILLUMINANCE()
  {
    return Light::_INTENSITYMODE_ILLUMINANCE;
  }

  /**
   * @brief nit (cd/m^2)
   */
  static constexpr unsigned int INTENSITYMODE_LUMINANCE()
  {
    return Light::_INTENSITYMODE_LUMINANCE;
  }

  /**
   * @brief Light type const id of the point light.
   */
  static constexpr unsigned int LIGHTTYPEID_POINTLIGHT()
  {
    return Light::_LIGHTTYPEID_POINTLIGHT;
  }

  /**
   * @brief Light type const id of the directional light.
   */
  static constexpr unsigned int LIGHTTYPEID_DIRECTIONALLIGHT()
  {
    return Light::_LIGHTTYPEID_DIRECTIONALLIGHT;
  }

  /**
   * @brief Light type const id of the spot light.
   */
  static constexpr unsigned int LIGHTTYPEID_SPOTLIGHT()
  {
    return Light::_LIGHTTYPEID_SPOTLIGHT;
  }

  /**
   * @brief Light type const id of the hemispheric light.
   */
  static constexpr unsigned int LIGHTTYPEID_HEMISPHERICLIGHT()
  {
    return Light::_LIGHTTYPEID_HEMISPHERICLIGHT;
  }

public:
  ~Light() override;

  virtual IReflect::Type type() const override;
  void addToScene(unique_ptr_t<Light>&& newLight);

  /**
   * @brief Returns the string "Light".
   * @returns the class name
   */
  const string_t getClassName() const override;

  /**
   * @brief Converts the light information to a readable string for debug
   * purpose.
   * @param fullDetails Supports for multiple levels of logging within scene
   * loading
   * @returns the human readable light info
   */
  string_t toString(bool fullDetails = false) const;

  /**
   * @brief Set the enabled state of this node.
   * @param value - the new enabled state
   */
  void setEnabled(bool value);

  /**
   * @brief Returns the Light associated shadow generator if any.
   * @return the associated shadow generator.
   */
  virtual ShadowGenerator* getShadowGenerator();

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
  virtual void transferToEffect(Effect* effect, const string_t& lightIndex) = 0;

  virtual void transferToEffect(Effect* effect, const string_t& uniformName0,
                                const string_t& uniformName1);

  /**
   * @brief Internal use only.
   */
  virtual Matrix* _getWorldMatrix() = 0;

  /**
   * @brief Specifies if the light will affect the passed mesh.
   * @param mesh The mesh to test against the light
   * @return true the mesh is affected otherwise, false.
   */
  bool canAffectMesh(AbstractMesh* mesh);

  /**
   * @brief Computes and Returns the light World matrix.
   * @returns the world matrix
   */
  Matrix* getWorldMatrix() override;

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
  unique_ptr_t<Light> clone(const string_t& name);

  /**
   * @brief Serializes the current light into a Serialization object.
   * @returns the serialized object.
   */
  Json::object serialize() const;

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
  static Light* GetConstructorFromName(unsigned int type, const string_t& name,
                                       Scene* scene);

  /**
   * @brief Parses the passed "parsedLight" and returns a new instanced Light
   * from this parsing.
   * @param parsedLight The JSON representation of the light
   * @param scene The scene to create the parsed light in
   * @returns the created light after parsing
   */
  static Light* Parse(const Json::value& parsedLight, Scene* scene);

protected:
  /**
   * @brief Creates a Light object in the scene.
   * Documentation : http://doc.babylonjs.com/tutorials/lights
   * @param name The firendly name of the light
   * @param scene The scene the light belongs too
   */
  Light(const string_t& name, Scene* scene);

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
  vector_t<AbstractMesh*>& get_includedOnlyMeshes();

  /**
   * @brief Sets the only meshes impacted by this light.
   */
  void set_includedOnlyMeshes(const vector_t<AbstractMesh*>& value);

  /**
   * @brief Gets the meshes not impacted by this light.
   */
  vector_t<AbstractMesh*>& get_excludedMeshes();

  /**
   * @brief Sets the meshes not impacted by this light.
   */
  void set_excludedMeshes(const vector_t<AbstractMesh*>& value);

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

  virtual void _buildUniformLayout();
  void _resyncMeshes();

private:
  void _hookArrayForExcluded(const vector_t<AbstractMesh*>& array);
  void _hookArrayForIncludedOnly(const vector_t<AbstractMesh*>& array);

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
   * Strength of the light.
   * Note: By default it is define in the framework own unit.
   * Note: In PBR materials the intensityMode can be use to chose what unit the
   * intensity is defined in.
   */
  float intensity;

  /**
   * Defines how far from the source the light is impacting in scene units.
   * Note: Unused in PBR material as the distance light falloff is defined
   * following the inverse squared falloff.
   */
  float range;

  /**
   * Defines wether or not the shadows are enabled for this light. This can help
   * turning off/on shadow without detaching the current shadow generator.
   */
  bool shadowEnabled;

  /**
   * Shadow generator associted to the light.
   * Internal use only.
   */
  ShadowGenerator* _shadowGenerator;

  /**
   * Internal use only.
   */
  vector_t<string_t> _excludedMeshesIds;

  /**
   * Internal use only.
   */
  vector_t<string_t> _includedOnlyMeshesIds;

  /**
   * The current light unifom buffer.
   * Internal use only.
   */
  unique_ptr_t<UniformBuffer> _uniformBuffer;

  /**
   * Photometric scale used to interpret the intensity
   */
  Property<Light, unsigned int> intensityMode;

  /**
   * Light radius used by PBR Materials to simulate soft area lights
   */
  Property<Light, float> radius;

  /**
   * Rendering priority of the lights. It can help in case of fallback or number
   * of lights exceeding the number allowed of the materials
   */
  Property<Light, int> renderPriority;

  /**
   * Meshes impacted by this light
   */
  Property<Light, vector_t<AbstractMesh*>> includedOnlyMeshes;

  /**
   * Meshes not impacted by this light
   */
  Property<Light, vector_t<AbstractMesh*>> excludedMeshes;

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

private:
  /**
   * Cached photometric scale default to 1.0 as the automatic intensity mode
   * defaults to 1.0 for every type
   * of light.
   */
  float _photometricScale;
  unsigned int _intensityMode;
  float _radius;
  int _renderPriority;
  vector_t<AbstractMesh*> _includedOnlyMeshes;
  vector_t<AbstractMesh*> _excludedMeshes;
  unsigned int _includeOnlyWithLayerMask;
  unsigned int _excludeWithLayerMask;
  unsigned int _lightmapMode;
  unique_ptr_t<Matrix> _parentedWorldMatrix;
  unique_ptr_t<Matrix> _worldMatrix;

}; // end of class Light

} // end of namespace BABYLON

#endif // end of BABYLON_LIGHTS_LIGHT_H
