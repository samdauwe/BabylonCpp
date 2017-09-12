#ifndef BABYLON_LIGHTS_SHADOWS_SHADOW_GENERATOR_H
#define BABYLON_LIGHTS_SHADOWS_SHADOW_GENERATOR_H

#include <babylon/babylon_global.h>
#include <babylon/core/nullable.h>
#include <babylon/lights/shadows/ishadow_generator.h>
#include <babylon/math/isize.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT ShadowGenerator : public IShadowGenerator {

public:
  /** Statics **/
  static constexpr unsigned int FILTER_NONE                          = 0;
  static constexpr unsigned int FILTER_EXPONENTIALSHADOWMAP          = 1;
  static constexpr unsigned int FILTER_POISSONSAMPLING               = 2;
  static constexpr unsigned int FILTER_BLUREXPONENTIALSHADOWMAP      = 3;
  static constexpr unsigned int FILTER_CLOSEEXPONENTIALSHADOWMAP     = 4;
  static constexpr unsigned int FILTER_BLURCLOSEEXPONENTIALSHADOWMAP = 5;

  /**
   * @brief Creates a ShadowGenerator object.
   * A ShadowGenerator is the required tool to use the shadows.
   * Each light casting shadows needs to use its own ShadowGenerator.
   * Required parameters :
   * - `mapSize` (integer): the size of the texture what stores the shadows.
   * Example : 1024.
   * - `light`: the light object generating the shadows.
   * - `useFullFloatFirst`: by default the generator will try to use half float
   * textures but if you need precision (for self shadowing for instance), you
   * can use this option to enforce full float texture.
   * Documentation : http://doc.babylonjs.com/tutorials/shadows
   */
  ShadowGenerator(int mapSize, IShadowLight* light,
                  bool useFullFloatFirst = false);
  ShadowGenerator(const ISize& mapSize, IShadowLight* light,
                  bool useFullFloatFirst = false);
  virtual ~ShadowGenerator();

  /** Members **/
  float bias() const;
  void setBias(float bias);
  int blurBoxOffset() const;
  void setBlurBoxOffset(int value);
  float blurScale() const;
  void setBlurScale(float value);
  float blurKernel() const;
  void setBlurKernel(float value);
  bool useKernelBlur() const;
  void setUseKernelBlur(bool value);
  float depthScale() const;
  void setDepthScale(float value);
  unsigned int filter() const;
  void setFilter(unsigned int value);
  bool usePoissonSampling() const;
  void setUsePoissonSampling(bool value);
  bool useVarianceShadowMap() const;
  void setUseVarianceShadowMap(bool value);
  bool useBlurVarianceShadowMap() const;
  void setUseBlurVarianceShadowMap(bool value);
  bool useExponentialShadowMap() const;
  void setUseExponentialShadowMap(bool value);
  bool useBlurExponentialShadowMap() const;
  void setUseBlurExponentialShadowMap(bool value);
  bool useCloseExponentialShadowMap() const;
  void setUseCloseExponentialShadowMap(bool value);
  bool useBlurCloseExponentialShadowMap() const;
  void setUseBlurCloseExponentialShadowMap(bool value);

  /**
   * @brief Returns the darkness value (float).
   */
  float getDarkness() const;

  /**
   * @brief Sets the ShadowGenerator darkness value (float <= 1.0).
   * @returns The ShadowGenerator.
   */
  ShadowGenerator& setDarkness(float darkness);

  /**
   * @brief Sets the ability to have transparent shadow (boolean).
   * @returns The ShadowGenerator.
   */
  ShadowGenerator& setTransparencyShadow(bool hasShadow);

  /**
   * @brief Returns a RenderTargetTexture object : the shadow map texture.
   */
  RenderTargetTexture* getShadowMap() override;

  /**
   * @brief Returns the most ready computed shadow map as a RenderTargetTexture
   * object.
   */
  RenderTargetTexture* getShadowMapForRendering() override;

  /**
   * @brief Returns the associated light object.
   */
  IShadowLight* getLight();

  /** Methods **/

  /**
   * @brief Force shader compilation including textures ready check
   */
  void forceCompilation(
    const ::std::function<void(ShadowGenerator* generator)>& onCompiled,
    const ShadowGeneratorCompileOptions& options) override;

  /**
   * @brief Returns true when the ShadowGenerator is finally computed.
   */
  bool isReady(SubMesh* subMesh, bool useInstances) override;

  void recreateShadowMap() override;

  /**
   * @brief Returns a Matrix object : the updated transformation matrix.
   */
  Matrix getTransformMatrix() override;

  /**
   * @brief Disposes the ShadowGenerator.
   * @returns Nothing.
   */
  void dispose() override;

  /**
   * @brief Serializes the ShadowGenerator and returns a serializationObject.
   */
  Json::object serialize() const override;

  /**
   * @brief This creates the defines related to the standard BJS materials.
   */
  void prepareDefines(MaterialDefines& defines,
                      unsigned int lightIndex) override;

  /**
   * @brief This binds shadow lights related to the standard BJS materials.
   * It implies the unifroms available on the materials are the standard BJS
   * ones.
   */
  void bindShadowLight(const std::string& lightIndex, Effect* effect) override;

  // Statics

  /**
   * @brief Parses a serialized ShadowGenerator and returns a new
   * ShadowGenerator.
   */
  static ShadowGenerator* Parse(const Json::value& parsedShadowGenerator,
                                Scene* scene);

private:
  void _initializeGenerator();
  void _initializeShadowMap();
  void _initializeBlurRTTAndPostProcesses();
  void _renderForShadowMap(const std::vector<SubMesh*>& opaqueSubMeshes,
                           const std::vector<SubMesh*>& alphaTestSubMeshes,
                           const std::vector<SubMesh*>& transparentSubMeshes);
  void _renderSubMeshForShadowMap(SubMesh* subMesh);
  void _applyFilterValues();
  void _disposeBlurPostProcesses();
  void _disposeRTTandPostProcesses();

public:
  /**
   * Controls the extent to which the shadows fade out at the edge of the
   * frustum
   * Used only by directionals and spots
   */
  float frustumEdgeFalloff;
  bool forceBackFacesOnly;

private:
  float _bias;
  int _blurBoxOffset;
  float _blurScale;
  float _blurKernel;
  bool _useKernelBlur;
  Nullable<float> _depthScale;
  unsigned int _filter;
  float _darkness;
  bool _transparencyShadow;
  std::unique_ptr<RenderTargetTexture> _shadowMap;
  std::unique_ptr<RenderTargetTexture> _shadowMap2;
  IShadowLight* _light;
  Scene* _scene;
  Vector3 _lightDirection;
  Effect* _effect;
  Matrix _viewMatrix;
  Matrix _projectionMatrix;
  Matrix _transformMatrix;
  Matrix _worldViewProjection;
  bool _cacheInitialized;
  Vector3 _cachedPosition;
  Vector3 _cachedDirection;
  std::string _cachedDefines;
  int _currentRenderID;
  std::unique_ptr<PassPostProcess> _downSamplePostprocess;
  std::unique_ptr<PostProcess> _boxBlurPostprocess;
  std::unique_ptr<PostProcess> _kernelBlurXPostprocess;
  std::unique_ptr<PostProcess> _kernelBlurYPostprocess;
  std::vector<PostProcess*> _blurPostProcesses;
  ISize _mapSize;
  unsigned int _currentFaceIndex;
  unsigned int _currentFaceIndexCache;
  bool _useFullFloat;
  unsigned int _textureType;
  bool _isCube;
  Matrix _defaultTextureMatrix;

}; // end of class ShadowGenerator

} // end of namespace BABYLON

#endif // end of BABYLON_LIGHTS_SHADOWS_SHADOW_GENERATOR_H
