#ifndef BABYLON_MATERIALS_TEXTURES_BASE_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_BASE_TEXTURE_H

#include <babylon/animations/animation.h>
#include <babylon/animations/ianimatable.h>
#include <babylon/babylon_global.h>
#include <babylon/core/structs.h>
#include <babylon/math/isize.h>
#include <babylon/tools/observable.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT BaseTexture : public IAnimatable {
public:
  static constexpr unsigned int DEFAULT_ANISOTROPIC_FILTERING_LEVEL = 4;

public:
  virtual ~BaseTexture() override;

  virtual IReflect::Type type() const override;
  void addToScene(unique_ptr_t<BaseTexture>&& newTexture);

  void setHasAlpha(bool value);
  bool hasAlpha() const;
  void setCoordinatesMode(unsigned int value);
  unsigned int coordinatesMode() const;

  string_t uid();
  virtual string_t toString() const;

  /**
   * @brief Returns the string "BaseTexture".
   */
  const char* getClassName() const;

  void setOnDispose(
    const ::std::function<void(BaseTexture*, EventState&)>& callback);
  virtual bool isBlocking() const;
  Scene* getScene() const;
  virtual Matrix* getTextureMatrix();
  virtual Matrix* getReflectionTextureMatrix();
  InternalTexture* getInternalTexture();
  virtual bool isReadyOrNotBlocking();
  virtual bool isReady();
  ISize getSize() const;
  ISize getBaseSize();
  virtual void scale(float ratio);
  bool canRescale();
  InternalTexture* _getFromCache(const string_t& url, bool noMipmap,
                                 unsigned int sampling = 0);
  virtual void _rebuild();
  virtual void delayLoad();
  virtual Vector3* boundingBoxSize() const;
  vector_t<Animation*> getAnimations() override;
  unique_ptr_t<BaseTexture> clone() const;
  unsigned int textureType() const;
  unsigned int textureFormat() const;
  ArrayBufferView readPixels(unsigned int faceIndex = 0);
  void releaseInternalTexture();
  SphericalPolynomial* sphericalPolynomial();
  void setSphericalPolynomial(const SphericalPolynomial& value);
  BaseTexture* _lodTextureHigh() const;
  BaseTexture* _lodTextureMid() const;
  BaseTexture* _lodTextureLow() const;
  virtual void dispose();
  Json::object serialize() const;

  static void WhenAllReady(const vector_t<BaseTexture*>& textures,
                           const ::std::function<void()>& callback);

protected:
  BaseTexture(Scene* scene);

public:
  string_t name;
  bool getAlphaFromRGB;
  float level;

  /**
   * How a texture is mapped.
   *
   * | Value | Type                                | Description |
   * | ----- | ----------------------------------- | ----------- |
   * | 0     | EXPLICIT_MODE                       |             |
   * | 1     | SPHERICAL_MODE                      |             |
   * | 2     | PLANAR_MODE                         |             |
   * | 3     | CUBIC_MODE                          |             |
   * | 4     | PROJECTION_MODE                     |             |
   * | 5     | SKYBOX_MODE                         |             |
   * | 6     | INVCUBIC_MODE                       |             |
   * | 7     | EQUIRECTANGULAR_MODE                |             |
   * | 8     | FIXED_EQUIRECTANGULAR_MODE          |             |
   * | 9     | FIXED_EQUIRECTANGULAR_MIRRORED_MODE |             |
   */
  unsigned int coordinatesIndex;

  /**
   * | Value | Type               | Description |
   * | ----- | ------------------ | ----------- |
   * | 0     | CLAMP_ADDRESSMODE  |             |
   * | 1     | WRAP_ADDRESSMODE   |             |
   * | 2     | MIRROR_ADDRESSMODE |             |
   */
  unsigned int wrapU;

  /**
   * | Value | Type               | Description |
   * | ----- | ------------------ | ----------- |
   * | 0     | CLAMP_ADDRESSMODE  |             |
   * | 1     | WRAP_ADDRESSMODE   |             |
   * | 2     | MIRROR_ADDRESSMODE |             |
   */
  unsigned int wrapV;

  /**
   * | Value | Type               | Description |
   * | ----- | ------------------ | ----------- |
   * | 0     | CLAMP_ADDRESSMODE  |             |
   * | 1     | WRAP_ADDRESSMODE   |             |
   * | 2     | MIRROR_ADDRESSMODE |             |
   */
  unsigned int wrapR;

  unsigned int anisotropicFilteringLevel;
  bool isCube;
  bool is3D;
  bool gammaSpace;
  bool invertZ;
  bool lodLevelInAlpha;
  float lodGenerationOffset;
  float lodGenerationScale;
  bool isRenderTarget;
  vector_t<Animation*> animations;
  /**
   * An event triggered when the texture is disposed.
   */
  Observable<BaseTexture> onDisposeObservable;
  unsigned int delayLoadState;
  InternalTexture* _texture;

private:
  bool _hasAlpha;
  unsigned int _coordinatesMode;
  Scene* _scene;
  string_t _uid;
  Observer<BaseTexture>::Ptr _onDisposeObserver;
  Matrix _textureMatrix;
  Matrix _reflectionTextureMatrix;

}; // end of class BaseTexture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_BASE_TEXTURE_H
