#ifndef BABYLON_LIGHTS_LIGHT_H
#define BABYLON_LIGHTS_LIGHT_H

#include <babylon/babylon_global.h>
#include <babylon/engine/node.h>
#include <babylon/math/color3.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT Light : public Node {

public:
  // LightmapMode Consts
  /**
   * If every light affecting the material is in this lightmapMode,
   * material.lightmapTexture adds or multiplies
   * (depends on material.useLightmapAsShadowmap)
   * after every other light calculations.
   */
  static constexpr unsigned int LIGHTMAP_DEFAULT = 0;

  /**
   * If every light affecting the material is in this lightmapMode,
   * material.lightmapTexture adds or multiplies
   * (depends on material.useLightmapAsShadowmap)
   * after every other light calculations.
   */
  static constexpr unsigned int LIGHTMAP_SPECULAR = 1;

  /**
   * material.lightmapTexture as only diffuse lighting from this light
   * adds pnly specular lighting from this light
   * adds dynamic shadows
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
  ~Light();

  virtual IReflect::Type type() const override;
  void addToScene(unique_ptr_t<Light>&& newLight);

  /**
   * @brief Returns the string "Light".
   */
  const char* getClassName() const override;

  /**
   * @param {boolean} fullDetails - support for multiple levels of logging
   * within scene loading
   */
  string_t toString(bool fullDetails = false) const;

  /**
   * @brief Set the enabled state of this node.
   * @param {boolean} value - the new enabled state
   * @see isEnabled
   */
  void setEnabled(bool value);

  //** Getters / Setters **/
  /**
   * @brief Gets the photometric scale used to interpret the intensity.
   * This is only relevant with PBR Materials where the light intensity can be
   * defined in a physical way.
   */
  unsigned int intensityMode() const;

  /**
   * @brief Sets the photometric scale used to interpret the intensity.
   * This is only relevant with PBR Materials where the light intensity can be
   * defined in a physical way.
   */
  void setIntensityMode(unsigned int value);

  /**
   * @brief Gets the light radius used by PBR Materials to simulate soft area
   * lights.
   */
  float radius() const;

  /**
   * @brief Sets the light radius used by PBR Materials to simulate soft area
   * lights.
   */
  void setRadius(float value);

  int renderPriority() const;
  vector_t<AbstractMesh*>& includedOnlyMeshes();
  void setIncludedOnlyMeshes(const vector_t<AbstractMesh*>& value);
  vector_t<AbstractMesh*>& excludedMeshes();
  void setExcludedMeshes(const vector_t<AbstractMesh*>& value);
  unsigned int includeOnlyWithLayerMask() const;
  void setIncludeOnlyWithLayerMask(unsigned int value);
  unsigned int excludeWithLayerMask() const;
  void setExcludeWithLayerMask(unsigned int value);
  unsigned int lightmapMode() const;
  void setLightmapMode(unsigned int value);

  /**
   * @brief Returns the Light associated shadow generator.
   */
  virtual ShadowGenerator* getShadowGenerator();

  /**
   * @brief Returns a Vector3, the absolute light position in the World.
   */
  virtual Vector3 getAbsolutePosition();

  virtual void transferToEffect(Effect* effect, const string_t& lightIndex);
  virtual void transferToEffect(Effect* effect, const string_t& uniformName0,
                                const string_t& uniformName1);
  virtual Matrix* _getWorldMatrix();

  /**
   * @brief Returns if the light will affect the passed mesh.
   */
  bool canAffectMesh(AbstractMesh* mesh);

  /**
   * @brief Returns the light World matrix.
   */
  Matrix* getWorldMatrix() override;

  /**
   * @brief Marks the meshes as dirty.
   */
  void _markMeshesAsLightDirty();

  /**
   * @brief Disposes the light.
   */
  virtual void dispose(bool doNotRecurse = false) override;

  /**
   * @brief Returns the light type ID (integer).
   */
  virtual unsigned int getTypeID() const;

  /**
   * @brief Returns the intensity scaled by the Photometric Scale according to
   * the light type and intensity mode.
   */
  float getScaledIntensity() const;

  /**
   * @brief Returns a new Light object, named "name", from the current one.
   */
  unique_ptr_t<Light> clone(const string_t& name);

  /**
   * @brief Serializes the current light into a Serialization object.
   * @returns The serialized object.
   */
  Json::object serialize() const;

  // Statics

  /**
   * @brief Sort function to order lights for rendering.
   * @param a First Light object to compare to second.
   * @param b Second Light object to compare first.
   * @return -1 to reduce's a's index relative to be, 0 for no change, 1 to
   * increase a's index relative to b.
   */
  static int compareLightsPriority(Light* a, Light* b);

  /**
   * @brief Creates a new typed light from the passed type (integer) : point
   * light = 0, directional light = 1, spot light = 2, hemispheric light = 3.
   * This new light is named "name" and added to the passed scene.
   */
  static Light* GetConstructorFromName(unsigned int type, const string_t& name,
                                       Scene* scene);

  /**
   * @brief Parses the passed "parsedLight" and returns a new instanced Light
   * from this parsing.
   */
  static Light* Parse(const Json::value& parsedLight, Scene* scene);

protected:
  /**
   * @brief Creates a Light object in the scene.
   * Documentation : http://doc.babylonjs.com/tutorials/lights
   */
  Light(const string_t& name, Scene* scene);

  virtual void _buildUniformLayout();

private:
  void _hookArrayForExcluded(const vector_t<AbstractMesh*>& array);
  void _hookArrayForIncludedOnly(const vector_t<AbstractMesh*>& array);
  void _resyncMeshes();

  /**
   * @brief Recomputes the cached photometric scale if needed.
   */
  void _computePhotometricScale();

  /**
   * @brief Returns the Photometric Scale according to the light type and
   * intensity mode.
   */
  float _getPhotometricScale();

  /**
   * @brief Reorders the lights in the scene.
   */
  void _reorderLightsInScene();

public:
  Color3 diffuse;
  Color3 specular;
  float intensity;
  float range;
  /**
   * Defines wether or not the shadows are enabled for this light. This can help
   * turning off/on shadow without detaching the current shadow generator.
   */
  bool shadowEnabled;
  ShadowGenerator* _shadowGenerator;
  vector_t<string_t> _excludedMeshesIds;
  vector_t<string_t> _includedOnlyMeshesIds;
  // Light uniform buffer
  unique_ptr_t<UniformBuffer> _uniformBuffer;

private:
  /**
   * Cached photometric scale default to 1.0 as the automatic intensity mode
   * defaults to 1.0 for every type
   * of light.
   */
  float _photometricScale;
  unsigned int _intensityMode;
  float _radius;
  /**
   * Defines the rendering priority of the lights. It can help in case of
   * fallback or number of lights exceeding the number allowed of the materials.
   */
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
