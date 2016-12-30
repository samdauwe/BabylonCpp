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
  virtual ~BaseTexture();

  void addToScene(std::unique_ptr<BaseTexture>&& newTexture);

  std::string uid();
  virtual std::string toString() const;
  void setOnDispose(const std::function<void()>& callback);
  Scene* getScene() const;
  virtual Matrix* getTextureMatrix();
  virtual Matrix* getReflectionTextureMatrix();
  GL::IGLTexture* getInternalTexture();
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
  void releaseInternalTexture();
  virtual void dispose(bool doNotRecurse = false) override;
  Json::object serialize() const;

protected:
  BaseTexture(Scene* scene);

public:
  std::string name;
  bool hasAlpha;
  bool getAlphaFromRGB;
  float level;
  unsigned int coordinatesIndex;
  unsigned int coordinatesMode;
  unsigned int wrapU;
  unsigned int wrapV;
  unsigned int anisotropicFilteringLevel;
  bool isCube;
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
  Scene* _scene;
  std::string _uid;
  Observer<BaseTexture>::Ptr _onDisposeObserver;

}; // end of class BaseTexture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_BASE_TEXTURE_H
