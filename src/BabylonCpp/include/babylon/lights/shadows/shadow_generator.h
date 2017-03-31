#ifndef BABYLON_LIGHTS_SHADOWS_SHADOW_GENERATOR_H
#define BABYLON_LIGHTS_SHADOWS_SHADOW_GENERATOR_H

#include <babylon/babylon_global.h>
#include <babylon/lights/shadows/ishadow_generator.h>
#include <babylon/math/isize.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT ShadowGenerator : public IShadowGenerator {

public:
  /** Statics **/
  static constexpr unsigned int FILTER_NONE                     = 0;
  static constexpr unsigned int FILTER_EXPONENTIALSHADOWMAP     = 1;
  static constexpr unsigned int FILTER_POISSONSAMPLING          = 2;
  static constexpr unsigned int FILTER_BLUREXPONENTIALSHADOWMAP = 3;

  /**
   * @brief Creates a ShadowGenerator object.
   * A ShadowGenerator is the required tool to use the shadows.
   * Each light casting shadows needs to use its own ShadowGenerator.
   * Required parameters :
   * -  `mapSize` (integer), the size of the texture what stores the shadows.
   * Example : 1024.
   * - `light` : the light object generating the shadows.
   * Documentation : http://doc.babylonjs.com/tutorials/shadows
   */
  ShadowGenerator(int mapSize, IShadowLight* light);
  ShadowGenerator(const ISize& mapSize, IShadowLight* light);
  virtual ~ShadowGenerator();

  /** Members **/
  float bias() const;
  void setBias(float bias);
  int blurBoxOffset() const;
  void setBlurBoxOffset(int value);
  float depthScale() const;
  void setDepthScale(float value);
  unsigned int filter() const;
  void setFilter(unsigned int value);
  bool useExponentialShadowMap() const;
  void setUseExponentialShadowMap(bool value);
  bool usePoissonSampling() const;
  void setUsePoissonSampling(bool value);
  bool useBlurExponentialShadowMap() const;
  void setUseBlurExponentialShadowMap(bool value);

  /** Custom render function **/
  void renderSubMesh(SubMesh* subMesh);

  /** Methods **/

  /**
   * @brief Returns true when the ShadowGenerator is finally computed.
   */
  bool isReady(SubMesh* subMesh, bool useInstances);

  /**
   * @brief Returns a RenderTargetTexture object : the shadow map texture.
   */
  RenderTargetTexture* getShadowMap() override;

  /**
   * @brief Returns the most ready computed shadow map as a RenderTargetTexture
   * object.
   */
  RenderTargetTexture* getShadowMapForRendering();

  /**
   * @brief Returns the associated light object.
   */
  IShadowLight* getLight();

  /**
   * @brief Returns a Matrix object : the updated transformation matrix.
   */
  Matrix getTransformMatrix();

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
   * @brief Disposes the ShadowGenerator.
   * @returns Nothing.
   */
  void dispose() override;

  /**
   * @brief Serializes the ShadowGenerator and returns a serializationObject.
   */
  Json::object serialize() const;

  // Statics

  /**
   * @brief Parses a serialized ShadowGenerator and returns a new
   * ShadowGenerator.
   */
  static ShadowGenerator* Parse(const Json::value& parsedShadowGenerator,
                                Scene* scene);

private:
  Vector2 _packHalf(float depth);

public:
  float blurScale;
  bool forceBackFacesOnly;

private:
  unsigned int _filter;
  int _blurBoxOffset;
  float _bias;
  Vector3 _lightDirection;
  float _depthScale;
  IShadowLight* _light;
  Scene* _scene;
  std::unique_ptr<RenderTargetTexture> _shadowMap;
  std::unique_ptr<RenderTargetTexture> _shadowMap2;
  float _darkness;
  bool _transparencyShadow;
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
  ISize _mapSize;
  unsigned int _currentFaceIndex;
  unsigned int _currentFaceIndexCache;
  bool _useFullFloat;
  unsigned int _textureType;

}; // end of class ShadowGenerator

} // end of namespace BABYLON

#endif // end of BABYLON_LIGHTS_SHADOWS_SHADOW_GENERATOR_H
