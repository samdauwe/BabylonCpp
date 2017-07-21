#ifndef BABYLON_MATERIALS_TEXTURES_BASE_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_BASE_TEXTURE_H

#include <babylon/animations/animation.h>
#include <babylon/animations/ianimatable.h>
#include <babylon/babylon_global.h>
#include <babylon/core/structs.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/math/isize.h>
#include <babylon/tools/observable.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT BaseTexture : public IAnimatable,
                                          public IDisposable {
public:
  static unsigned int DEFAULT_ANISOTROPIC_FILTERING_LEVEL;

public:
  virtual ~BaseTexture();

  virtual IReflect::Type type() const override;
  void addToScene(std::unique_ptr<BaseTexture>&& newTexture);

  bool hasAlpha() const;
  void setHasAlpha(bool value);
  unsigned int coordinatesMode() const;
  void setCoordinatesMode(unsigned int value);

  std::string uid();
  virtual std::string toString() const;
  void setOnDispose(const std::function<void()>& callback);
  virtual bool isBlocking() const;
  Scene* getScene() const;
  virtual Matrix* getTextureMatrix();
  virtual Matrix* getReflectionTextureMatrix();
  GL::IGLTexture* getInternalTexture();
  virtual bool isReadyOrNotBlocking();
  virtual bool isReady();
  ISize getSize() const;
  ISize getBaseSize();
  virtual void scale(float ratio);
  bool canRescale();
  void _removeFromCache(const std::string& url, bool noMipmap);
  GL::IGLTexture* _getFromCache(const std::string& url, bool noMipmap,
                                unsigned int sampling = 0);
  virtual void delayLoad();
  std::vector<Animation*> getAnimations() override;
  std::unique_ptr<BaseTexture> clone() const;
  unsigned int textureType() const;
  unsigned int textureFormat() const;
  Uint8Array readPixels(unsigned int faceIndex = 0);
  void releaseInternalTexture();
  SphericalPolynomial* sphericalPolynomial();
  void setSphericalPolynomial(const SphericalPolynomial& value);
  BaseTexture* _lodTextureHigh() const;
  BaseTexture* _lodTextureMid() const;
  BaseTexture* _lodTextureLow() const;
  virtual void dispose(bool doNotRecurse = false) override;
  Json::object serialize() const;

  static void WhenAllReady(const std::vector<BaseTexture*>& textures,
                           const std::function<void()>& onLoad);

protected:
  BaseTexture(Scene* scene);

public:
  std::string name;
  bool getAlphaFromRGB;
  float level;
  unsigned int coordinatesIndex;
  unsigned int wrapU;
  unsigned int wrapV;
  unsigned int anisotropicFilteringLevel;
  bool isCube;
  bool gammaSpace;
  bool invertZ;
  bool lodLevelInAlpha;
  float lodGenerationOffset;
  float lodGenerationScale;
  bool isRenderTarget;
  std::vector<Animation*> animations;
  /**
   * An event triggered when the texture is disposed.
   */
  Observable<BaseTexture> onDisposeObservable;
  unsigned int delayLoadState;
  unsigned int _cachedAnisotropicFilteringLevel;
  GL::IGLTexture* _texture;

private:
  bool _hasAlpha;
  unsigned int _coordinatesMode;
  Scene* _scene;
  std::string _uid;
  Observer<BaseTexture>::Ptr _onDisposeObserver;

}; // end of class BaseTexture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_BASE_TEXTURE_H
