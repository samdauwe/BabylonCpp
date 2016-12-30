#ifndef BABYLON_LIGHTS_SHADOWS_SHADOW_GENERATOR_H
#define BABYLON_LIGHTS_SHADOWS_SHADOW_GENERATOR_H

#include <babylon/babylon_global.h>
#include <babylon/lights/shadows/ishadow_generator.h>
#include <babylon/math/isize.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT ShadowGenerator
  : public std::enable_shared_from_this<ShadowGenerator>,
    public IShadowGenerator {

public:
  /** Statics **/
  static constexpr unsigned int FILTER_NONE                  = 0;
  static constexpr unsigned int FILTER_VARIANCESHADOWMAP     = 1;
  static constexpr unsigned int FILTER_POISSONSAMPLING       = 2;
  static constexpr unsigned int FILTER_BLURVARIANCESHADOWMAP = 3;

  ShadowGenerator(const ISize& mapSize, IShadowLight* light);
  virtual ~ShadowGenerator();

  /** Members **/
  float bias() const;
  void setBias(float bias);
  int blurBoxOffset() const;
  void setBlurBoxOffset(int value);
  unsigned int filter() const;
  void setFilter(unsigned int value);
  bool useVarianceShadowMap() const;
  void setUseVarianceShadowMap(bool value);
  bool usePoissonSampling() const;
  void setUsePoissonSampling(bool value);
  bool useBlurVarianceShadowMap() const;
  void setUseBlurVarianceShadowMap(bool value);

  /** Custom render function **/
  void renderSubMesh(SubMesh* subMesh);

  /** Methods **/
  bool isReady(SubMesh* subMesh, bool useInstances);
  RenderTargetTexture* getShadowMap() override;
  RenderTargetTexture* getShadowMapForRendering();
  IShadowLight* getLight();

  Matrix getTransformMatrix();
  float getDarkness() const;
  void setDarkness(float darkness);
  void setTransparencyShadow(bool hasShadow);
  void dispose() override;
  Json::object serialize() const;

  // Statics
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

}; // end of class ShadowGenerator

} // end of namespace BABYLON

#endif // end of BABYLON_LIGHTS_SHADOWS_SHADOW_GENERATOR_H
