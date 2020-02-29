#ifndef BABYLON_LIGHTS_SHADOWS_CASCADED_SHADOW_GENERATOR_H
#define BABYLON_LIGHTS_SHADOWS_CASCADED_SHADOW_GENERATOR_H

#include <babylon/babylon_api.h>
#include <babylon/culling/bounding_info.h>
#include <babylon/lights/shadows/shadow_generator.h>

namespace BABYLON {

struct ICascade {
  float prevBreakDistance = 0.f;
  float breakDistance     = 0.f;
}; // end of sruct ICascade

class CascadedShadowGenerator;
class DepthReducer;
class DepthRenderer;
class DirectionalLight;
using CascadedShadowGeneratorPtr = std::shared_ptr<CascadedShadowGenerator>;
using DepthReducerPtr            = std::shared_ptr<DepthReducer>;
using DepthRendererPtr           = std::shared_ptr<DepthRenderer>;
using DirectionalLightPtr        = std::shared_ptr<DirectionalLight>;

/**
 * @brief A CSM implementation allowing casting shadows on large scenes.
 * Documentation : https://doc.babylonjs.com/babylon101/cascadedShadows
 * Based on: https://github.com/TheRealMJP/Shadows and
 * https://johanmedestrom.wordpress.com/2016/03/18/opengl-cascaded-shadow-maps/
 */
class BABYLON_SHARED_EXPORT CascadedShadowGenerator : public ShadowGenerator {

public:
  /**
   * Name of the CSM class
   */
  static constexpr const char* CLASSNAME = "CascadedShadowGenerator";

  /**
   * Defines the default number of cascades used by the CSM.
   */
  static constexpr unsigned int DEFAULT_CASCADES_COUNT = 4;
  /**
   * Defines the minimum number of cascades used by the CSM.
   */
  static constexpr unsigned int MIN_CASCADES_COUNT = 2;
  /**
   * Defines the maximum number of cascades used by the CSM.
   */
  static constexpr unsigned int MAX_CASCADES_COUNT = 4;

  template <typename... Ts>
  static CascadedShadowGeneratorPtr New(Ts&&... args)
  {
    return std::shared_ptr<CascadedShadowGenerator>(
      new CascadedShadowGenerator(std::forward<Ts>(args)...));
  }
  ~CascadedShadowGenerator() override; // = default

  /**
   * @brief Sets the minimal and maximal distances to use when computing the cascade breaks.
   *
   * The values of min / max are typically the depth zmin and zmax values of your scene, for a given
   * frame. If you don't know these values, simply leave them to their defaults and don't call this
   * function.
   * @param min minimal distance for the breaks (default to 0.)
   * @param max maximal distance for the breaks (default to 1.)
   */
  void setMinMaxDistance(float min, float max);

  /**
   * @brief Gets the class name of that object.
   * @returns "CascadedShadowGenerator"
   */
  std::string getClassName() const override;

  /**
   * @brief Gets a cascade minimum extents.
   * @param cascadeIndex index of the cascade
   * @returns the minimum cascade extents
   */
  std::optional<Vector3> getCascadeMinExtents(unsigned int cascadeIndex) const;

  /**
   * @brief Gets a cascade maximum extents.
   * @param cascadeIndex index of the cascade
   * @returns the maximum cascade extents
   */
  std::optional<Vector3> getCascadeMaxExtents(unsigned int cascadeIndex) const;

  /**
   * @brief Gets the view matrix corresponding to a given cascade
   * @param cascadeNum cascade to retrieve the view matrix from
   * @returns the cascade view matrix
   */
  std::optional<Matrix> getCascadeViewMatrix(unsigned int cascadeNum) const;

  /**
   * @brief Gets the projection matrix corresponding to a given cascade
   * @param cascadeNum cascade to retrieve the projection matrix from
   * @returns the cascade projection matrix
   */
  std::optional<Matrix> getCascadeProjectionMatrix(unsigned int cascadeNum) const;

  /**
   * @brief Gets the transformation matrix corresponding to a given cascade
   * @param cascadeNum cascade to retrieve the transformation matrix from
   * @returns the cascade transformation matrix
   */
  std::optional<Matrix> getCascadeTransformMatrix(unsigned int cascadeNum) const;

  /**
   * @brief Create the cascade breaks according to the lambda, shadowMaxZ and min/max distance
   * properties, as well as the camera near and far planes. This function is automatically called
   * when updating lambda, shadowMaxZ and min/max distances, however you should call it yourself if
   * you change the camera near/far planes!
   */
  void splitFrustum();

  /**
   * @brief Prepare all the defines in a material relying on a shadow map at the specified light
   * index.
   * @param defines Defines of the material we want to update
   * @param lightIndex Index of the light in the enabled light list of the material
   */
  void prepareDefines(MaterialDefines& defines, unsigned int lightIndex) override;

  /**
   * @brief Binds the shadow related information inside of an effect (information like near, far,
   * darkness... defined in the generator but impacting the effect).
   * @param lightIndex Index of the light in the enabled light list of the material owning the
   * effect
   * @param effect The effect we are binfing the information for
   */
  void bindShadowLight(const std::string& lightIndex, const EffectPtr& effect) override;

  /**
   * @brief Gets the transformation matrix of the first cascade used to project the meshes into the
   * map from the light point of view. (eq to view projection * shadow projection matrices)
   * @returns The transform matrix used to create the shadow map
   */
  Matrix getTransformMatrix() override;

  /**
   *  @brief Disposes the ShadowGenerator.
   * Returns nothing.
   */
  void dispose() override;

  /**
   * @brief Serializes the shadow generator setup to a json object.
   * @returns The serialized JSON object
   */
  json serialize() const override;

  /**
   * @brief Parses a serialized ShadowGenerator and returns a new ShadowGenerator.
   * @param parsedShadowGenerator The JSON object to parse
   * @param scene The scene to create the shadow map for
   * @returns The parsed shadow generator
   */
  static ShadowGenerator* Parse(const json& parsedShadowGenerator, Scene* scene);

protected:
  /**
   * @brief Creates a Cascaded Shadow Generator object.
   * A ShadowGenerator is the required tool to use the shadows.
   * Each directional light casting shadows needs to use its own ShadowGenerator.
   * Documentation : https://doc.babylonjs.com/babylon101/cascadedShadows
   * @param mapSize The size of the texture what stores the shadows. Example : 1024.
   * @param light The directional light object generating the shadows.
   * @param usefulFloatFirst By default the generator will try to use half float textures but if you
   * need precision (for self shadowing for instance), you can use this option to enforce full float
   * texture.
   */
  CascadedShadowGenerator(int mapSize, const DirectionalLightPtr& light, bool usefulFloatFirst);

  /**
   * @brief Hidden
   */
  unsigned int _validateFilter(unsigned int filter) const override;

  /**
   * @brief Gets or set the number of cascades used by the CSM.
   */
  unsigned int get_numCascades() const;

  /**
   * @brief Gets or set the number of cascades used by the CSM.
   */
  void set_numCascades(unsigned int value);

  /**
   * @brief Enables or disables the shadow casters bounding info computation.
   * If your shadow casters don't move, you can disable this feature.
   * If it is enabled, the bounding box computation is done every frame.
   */
  bool get_freezeShadowCastersBoundingInfo() const;

  /**
   * @brief Enables or disables the shadow casters bounding info computation.
   * If your shadow casters don't move, you can disable this feature.
   * If it is enabled, the bounding box computation is done every frame.
   */
  void set_freezeShadowCastersBoundingInfo(bool freeze);

  /**
   * @brief Hidden
   */
  void _computeShadowCastersBoundingInfo();

  /**
   * @brief Gets the shadow casters bounding info.
   * If you provide your own shadow casters bounding info, first enable
   * freezeShadowCastersBoundingInfo so that the system won't overwrite the bounds you provide
   */
  BoundingInfo& get_shadowCastersBoundingInfo();

  /**
   * @brief Sets the shadow casters bounding info.
   * If you provide your own shadow casters bounding info, first enable
   * freezeShadowCastersBoundingInfo so that the system won't overwrite the bounds you provide
   */
  void set_shadowCastersBoundingInfo(const BoundingInfo& boundingInfo);

  /**
   * @brief Gets the minimal distance used in the cascade break computation.
   */
  float get_minDistance() const;

  /**
   * @brief Gets the maximal distance used in the cascade break computation.
   */
  float get_maxDistance() const;

  /**
   * @brief Gets the shadow max z distance. It's the limit beyond which shadows are not displayed.
   * It defaults to camera.maxZ
   */
  float get_shadowMaxZ() const;

  /**
   * @brief Sets the shadow max z distance.
   */
  void set_shadowMaxZ(float value);

  /**
   * @brief Gets the debug flag.
   * When enabled, the cascades are materialized by different colors on the screen.
   */
  bool get_debug() const;

  /**
   * @brief Sets the debug flag.
   * When enabled, the cascades are materialized by different colors on the screen.
   */
  void set_debug(bool dbg);

  /**
   * @brief Gets the depth clamping value.
   *
   * When enabled, it improves the shadow quality because the near z plane of the light frustum
   * don't need to be adjusted to account for the shadow casters far away.
   *
   * Note that this property is incompatible with PCSS filtering, so it won't be used in that case.
   */
  bool get_depthClamp() const;

  /**
   * @brief Sets the depth clamping value.
   *
   * When enabled, it improves the shadow quality because the near z plane of the light frustum
   * don't need to be adjusted to account for the shadow casters far away.
   *
   * Note that this property is incompatible with PCSS filtering, so it won't be used in that case.
   */
  void set_depthClamp(bool value);

  /**
   * @brief Gets the percentage of blending between two cascades (value between 0. and 1.).
   * It defaults to 0.1 (10% blending).
   */
  float get_cascadeBlendPercentage() const;

  /**
   * @brief Sets the percentage of blending between two cascades (value between 0. and 1.).
   * It defaults to 0.1 (10% blending).
   */
  void set_cascadeBlendPercentage(float value);

  /**
   * @brief Gets the lambda parameter.
   * This parameter is used to split the camera frustum and create the cascades.
   * It's a value between 0. and 1.: If 0, the split is a uniform split of the frustum, if 1 it is a
   * logarithmic split. For all values in-between, it's a linear combination of the uniform and
   * logarithm split algorithm.
   */
  float get_lambda() const;

  /**
   * @brief Sets the lambda parameter.
   * This parameter is used to split the camera frustum and create the cascades.
   * It's a value between 0. and 1.: If 0, the split is a uniform split of the frustum, if 1 it is a
   * logarithmic split. For all values in-between, it's a linear combination of the uniform and
   * logarithm split algorithm.
   */
  void set_lambda(float value);

  /**
   * @brief Gets the autoCalcDepthBounds property.
   *
   * When enabled, a depth rendering pass is first performed (with an internally created depth
   * renderer or with the one you provide by calling setDepthRenderer). Then, a min/max reducing is
   * applied on the depth map to compute the minimal and maximal depth of the map and those values
   * are used as inputs for the setMinMaxDistance() function. It can greatly enhance the shadow
   * quality, at the expense of more GPU works. When using this option, you should increase the
   * value of the lambda parameter, and even set it to 1 for best results.
   */
  bool get_autoCalcDepthBounds() const;

  /**
   * @brief Sets the autoCalcDepthBounds property.
   *
   * When enabled, a depth rendering pass is first performed (with an internally created depth
   * renderer or with the one you provide by calling setDepthRenderer). Then, a min/max reducing is
   * applied on the depth map to compute the minimal and maximal depth of the map and those values
   * are used as inputs for the setMinMaxDistance() function. It can greatly enhance the shadow
   * quality, at the expense of more GPU works. When using this option, you should increase the
   * value of the lambda parameter, and even set it to 1 for best results.
   */
  void set_autoCalcDepthBounds(bool value);

  /**
   * @brief Defines the refresh rate of the min/max computation used when autoCalcDepthBounds is set
   * to true Use 0 to compute just once, 1 to compute on every frame, 2 to compute every two frames
   * and so on... Note that if you provided your own depth renderer through a call to
   * setDepthRenderer, you are responsible for setting the refresh rate on the renderer yourself!
   */
  int get_autoCalcDepthBoundsRefreshRate() const;

  /**
   * @brief Defines the refresh rate of the min/max computation used when autoCalcDepthBounds is set
   * to true Use 0 to compute just once, 1 to compute on every frame, 2 to compute every two frames
   * and so on... Note that if you provided your own depth renderer through a call to
   * setDepthRenderer, you are responsible for setting the refresh rate on the renderer yourself!
   */
  void set_autoCalcDepthBoundsRefreshRate(int value);

  /**
   * @brief Hidden
   */
  void _initializeGenerator() override;

  /**
   * @brief Hidden
   */
  void _createTargetRenderTexture() override;

  /**
   * @brief Hidden
   */
  void _bindCustomEffectForRenderSubMeshForShadowMap(SubMesh* subMesh, Effect* effect) override;

  /**
   * @brief Hidden
   */
  void _isReadyCustomDefines(std::vector<std::string>& defines, SubMesh* subMesh,
                             bool useInstances) override;

private:
  void _splitFrustum();
  void _computeMatrices();
  // Get the 8 points of the view frustum in world space
  void _computeFrustumInWorldSpace(unsigned int cascadeIndex);
  void _computeCascadeFrustum(unsigned int cascadeIndex);

public:
  /**
   * Gets or sets the actual darkness of the soft shadows while using PCSS filtering (value between
   * 0. and 1.)
   */
  float penumbraDarkness;

  /**
   * Gets or set the number of cascades used by the CSM
   */
  Property<CascadedShadowGenerator, unsigned int> numCascades;

  /**
   * Sets this to true if you want that the edges of the shadows don't "swimm" / "shimmer" when
   * rotating the camera. The trade off is that you loose some precision in the shadow rendering
   * when enabling this setting.
   */
  bool stabilizeCascades;

  /**
   * Enables or disables the shadow casters bounding info computation.
   * If your shadow casters don't move, you can disable this feature.
   * If it is enabled, the bounding box computation is done every frame.
   */
  Property<CascadedShadowGenerator, bool> freezeShadowCastersBoundingInfo;

  /**
   * Gets or sets the shadow casters bounding info.
   * If you provide your own shadow casters bounding info, first enable
   * freezeShadowCastersBoundingInfo so that the system won't overwrite the bounds you provide
   */
  Property<CascadedShadowGenerator, BoundingInfo> shadowCastersBoundingInfo;

  /**
   * Gets the minimal distance used in the cascade break computation.
   */
  ReadOnlyProperty<CascadedShadowGenerator, float> minDistance;

  /**
   * Gets the maximal distance used in the cascade break computation.
   */
  ReadOnlyProperty<CascadedShadowGenerator, float> maxDistance;

  /**
   * Gets the shadow max z distance. It's the limit beyond which shadows are not displayed.
   * It defaults to camera.maxZ
   */
  Property<CascadedShadowGenerator, float> shadowMaxZ;

  /**
   * Gets or sets the debug flag.
   * When enabled, the cascades are materialized by different colors on the screen.
   */
  Property<CascadedShadowGenerator, bool> debug;

  /**
   * Gets or sets the depth clamping value.
   *
   * When enabled, it improves the shadow quality because the near z plane of the light frustum
   * don't need to be adjusted to account for the shadow casters far away.
   *
   * Note that this property is incompatible with PCSS filtering, so it won't be used in that case.
   */
  Property<CascadedShadowGenerator, bool> depthClamp;

  /**
   * Gets or sets the percentage of blending between two cascades (value between 0. and 1.).
   * It defaults to 0.1 (10% blending).
   */
  Property<CascadedShadowGenerator, float> cascadeBlendPercentage;

  /**
   * Gets or set the lambda parameter.
   * This parameter is used to split the camera frustum and create the cascades.
   * It's a value between 0. and 1.: If 0, the split is a uniform split of the frustum, if 1 it is a
   * logarithmic split. For all values in-between, it's a linear combination of the uniform and
   * logarithm split algorithm.
   */
  Property<CascadedShadowGenerator, float> lambda;

  /**
   * Gets or sets the autoCalcDepthBounds property.
   *
   * When enabled, a depth rendering pass is first performed (with an internally created depth
   * renderer or with the one you provide by calling setDepthRenderer). Then, a min/max reducing is
   * applied on the depth map to compute the minimal and maximal depth of the map and those values
   * are used as inputs for the setMinMaxDistance() function. It can greatly enhance the shadow
   * quality, at the expense of more GPU works. When using this option, you should increase the
   * value of the lambda parameter, and even set it to 1 for best results.
   */
  Property<CascadedShadowGenerator, bool> autoCalcDepthBounds;

  /**
   * Defines the refresh rate of the min/max computation used when autoCalcDepthBounds is set to
   * true Use 0 to compute just once, 1 to compute on every frame, 2 to compute every two frames and
   * so on... Note that if you provided your own depth renderer through a call to setDepthRenderer,
   * you are responsible for setting the refresh rate on the renderer yourself!
   */
  Property<CascadedShadowGenerator, int> autoCalcDepthBoundsRefreshRate;

protected:
  BoundingInfo _shadowCastersBoundingInfo;
  bool _breaksAreDirty;
  float _minDistance;
  float _maxDistance;
  bool _debug;

private:
  const Vector3 UpDir;
  const Vector3 ZeroVec;
  Vector3 tmpv1;
  Vector3 tmpv2;
  Matrix matrix;
  static const std::vector<Vector3> frustumCornersNDCSpace;
  unsigned int _numCascades;
  bool _freezeShadowCastersBoundingInfo;
  Observer<Scene>::Ptr _freezeShadowCastersBoundingInfoObservable;
  Vector3 _scbiMin;
  Vector3 _scbiMax;
  std::vector<Vector3> _cascadeMinExtents;
  std::vector<Vector3> _cascadeMaxExtents;
  std::vector<ICascade> _cascades;
  int _currentLayer;
  Float32Array _viewSpaceFrustumsZ;
  std::vector<Matrix> _viewMatrices;
  std::vector<Matrix> _projectionMatrices;
  std::vector<Matrix> _transformMatrices;
  Float32Array _transformMatricesAsArray;
  Float32Array _frustumLengths;
  Float32Array _lightSizeUVCorrection;
  Float32Array _depthCorrection;
  std::vector<std::vector<Vector3>> _frustumCornersWorldSpace;
  std::vector<Vector3> _frustumCenter;
  std::vector<Vector3> _shadowCameraPos;
  float _shadowMaxZ;
  bool _depthClamp;
  float _cascadeBlendPercentage;
  float _lambda;
  DepthRendererPtr _depthRenderer;
  DepthReducerPtr _depthReducer;
  bool _autoCalcDepthBounds;

}; // end of class ShadowGenerator

} // end of namespace BABYLON

#endif // end of BABYLON_LIGHTS_SHADOWS_SHADOW_GENERATOR_H
