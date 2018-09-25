#ifndef BABYLON_MATERIALS_TEXTURES_BASE_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_BASE_TEXTURE_H

#include <babylon/animations/animation.h>
#include <babylon/animations/ianimatable.h>
#include <babylon/babylon_api.h>
#include <babylon/core/structs.h>
#include <babylon/math/isize.h>
#include <babylon/tools/observable.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

class BaseTexture;
class InternalTexture;
class SphericalPolynomial;
using BaseTexturePtr = std::shared_ptr<BaseTexture>;

class BABYLON_SHARED_EXPORT BaseTexture
    : public std::enable_shared_from_this<BaseTexture>,
      public IAnimatable {
public:
  static constexpr unsigned int DEFAULT_ANISOTROPIC_FILTERING_LEVEL = 4;

public:
  virtual ~BaseTexture() override;

  template <typename Derived>
  std::shared_ptr<Derived> shared_from_base()
  {
    return std::static_pointer_cast<Derived>(shared_from_this());
  }

  virtual IReflect::Type type() const override;
  void addToScene(const BaseTexturePtr& newTexture);

  std::string uid();
  virtual std::string toString() const;

  /**
   * @brief Returns the string "BaseTexture".
   */
  const std::string getClassName() const;

  void setOnDispose(
    const std::function<void(BaseTexture*, EventState&)>& callback);
  Scene* getScene() const;
  virtual Matrix* getTextureMatrix();
  virtual Matrix* getReflectionTextureMatrix();
  InternalTexture* getInternalTexture();
  virtual bool isReadyOrNotBlocking();
  virtual bool isReady();
  ISize getSize();
  ISize getBaseSize();
  virtual void scale(float ratio);
  bool canRescale();
  /** Hidden */
  InternalTexture* _getFromCache(const std::string& url, bool noMipmap,
                                 unsigned int sampling = 0);
  /** Hidden */
  virtual void _rebuild();
  virtual void delayLoad();
  std::vector<AnimationPtr> getAnimations() override;
  std::unique_ptr<BaseTexture> clone() const;
  unsigned int textureType() const;
  unsigned int textureFormat() const;

  /**
   * @brief Reads the pixels stored in the webgl texture and returns them as an
   * ArrayBuffer. This will returns an RGBA array buffer containing either in
   * values (0-255) or float values (0-1) depending of the underlying buffer
   * type.
   * @param faceIndex The face of the texture to read (in case of cube texture)
   * @param level The LOD level of the texture to read (in case of Mip Maps)
   * @returns The Array buffer containing the pixels data.
   */
  ArrayBufferView readPixels(unsigned int faceIndex = 0, int level = 0);

  void releaseInternalTexture();
  SphericalPolynomial* sphericalPolynomial();
  void setSphericalPolynomial(const SphericalPolynomial& value);
  BaseTexturePtr _lodTextureHigh() const;
  BaseTexturePtr _lodTextureMid() const;
  BaseTexturePtr _lodTextureLow() const;
  virtual void dispose();
  Json::object serialize() const;

  static void WhenAllReady(const std::vector<BaseTexture*>& textures,
                           const std::function<void()>& callback);

protected:
  BaseTexture(Scene* scene);

  void set_hasAlpha(bool value);
  bool get_hasAlpha() const;
  void set_coordinatesMode(unsigned int value);
  unsigned int get_coordinatesMode() const;

  /**
   * @brief Gets whether or not the texture contains RGBD data.
   */
  bool get_isRGBD() const;

  virtual bool get_isBlocking() const;
  virtual void set_isBlocking(bool value);
  virtual void set_boundingBoxSize(const std::optional<Vector3>& value);
  virtual std::optional<Vector3>& get_boundingBoxSize();
  float get_lodGenerationOffset() const;
  void set_lodGenerationOffset(float value);
  float get_lodGenerationScale() const;
  void set_lodGenerationScale(float value);

public:
  std::string name;
  Property<BaseTexture, bool> hasAlpha;
  bool getAlphaFromRGB;
  float level;
  unsigned int coordinatesIndex;

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
  Property<BaseTexture, unsigned int> coordinatesMode;

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

  /**
   *Whether or not the texture contains RGBD data.
   */
  ReadOnlyProperty<BaseTexture, bool> isRGBD;

  Property<BaseTexture, bool> isBlocking;

  bool invertZ;
  bool lodLevelInAlpha;

  Property<BaseTexture, float> lodGenerationOffset;

  Property<BaseTexture, float> lodGenerationScale;

  bool isRenderTarget;
  std::vector<AnimationPtr> animations;
  /**
   * An event triggered when the texture is disposed.
   */
  Observable<BaseTexture> onDisposeObservable;
  unsigned int delayLoadState;
  /** Hidden */
  InternalTexture* _texture;

  /**
   * Size of the bounding box associated with the texture (when in cube mode)
   */
  Property<BaseTexture, std::optional<Vector3>> boundingBoxSize;

private:
  bool _hasAlpha;
  unsigned int _coordinatesMode;
  Scene* _scene;
  std::string _uid;
  Observer<BaseTexture>::Ptr _onDisposeObserver;
  Matrix _textureMatrix;
  Matrix _reflectionTextureMatrix;
  std::optional<Vector3> emptyVector3;
  ISize _cachedSize;

}; // end of class BaseTexture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_BASE_TEXTURE_H
