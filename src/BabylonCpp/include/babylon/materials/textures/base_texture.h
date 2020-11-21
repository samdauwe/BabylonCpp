#ifndef BABYLON_MATERIALS_TEXTURES_BASE_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_BASE_TEXTURE_H

#include <babylon/animations/animation.h>
#include <babylon/animations/ianimatable.h>
#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/core/array_buffer_view.h>
#include <babylon/core/structs.h>
#include <babylon/materials/textures/thin_texture.h>
#include <babylon/maths/isize.h>
#include <babylon/misc/observable.h>
#include <babylon/misc/observer.h>

namespace BABYLON {

class ThinEngine;
FWD_CLASS_SPTR(BaseTexture)
FWD_CLASS_SPTR(InternalTexture)
FWD_CLASS_SPTR(SphericalPolynomial)

/**
 * @brief Base class of all the textures in babylon.
 * It groups all the common properties the materials, post process, lights... might need
 * in order to make a correct use of the texture.
 */
class BABYLON_SHARED_EXPORT BaseTexture : public std::enable_shared_from_this<BaseTexture>,
                                          public ThinTexture,
                                          public IAnimatable {
public:
  /**
   * Default anisotropic filtering level for the application.
   * It is set to 4 as a good tradeoff between perf and quality.
   */
  static constexpr unsigned int DEFAULT_ANISOTROPIC_FILTERING_LEVEL = 4;

public:
  template <typename... Ts>
  static BaseTexturePtr New(Ts&&... args)
  {
    auto texture = std::shared_ptr<BaseTexture>(new BaseTexture(std::forward<Ts>(args)...));
    texture->addToScene(texture);

    return texture;
  }
  ~BaseTexture() override; // = default

  template <typename Derived>
  std::shared_ptr<Derived> shared_from_base()
  {
    return std::static_pointer_cast<Derived>(shared_from_this());
  }

  Type type() const override;
  void addToScene(const BaseTexturePtr& newTexture);

  /**
   * @brief Return a string representation of the texture.
   * @returns the texture as a string
   */
  virtual std::string toString() const;

  /**
   * @brief Get the class name of the texture.
   * @returns "BaseTexture"
   */
  std::string getClassName() const;

  /**
   * @brief Get the scene the texture belongs to.
   * @returns the scene or null if undefined
   */
  Scene* getScene() const;

  /**
   * @brief Checks if the texture has the same transform matrix than another texture.
   * @param texture texture to check against
   * @returns true if the transforms are the same, else false
   */
  virtual bool checkTransformsAreIdentical(const BaseTexturePtr& texture) const;

  /**
   * @brief Get the texture transform matrix used to offset tile the texture for istance.
   * @returns the transformation matrix
   */
  virtual Matrix* getTextureMatrix(int uBase = 1);

  /**
   * @brief Get the texture reflection matrix used to rotate/transform the reflection.
   * @returns the reflection matrix
   */
  virtual Matrix* getReflectionTextureMatrix();

  /**
   * @brief Get if the texture is ready to be consumed (either it is ready or it is not blocking)
   * @returns true if ready or not blocking
   */
  virtual bool isReadyOrNotBlocking();

  /**
   * @brief Scales the texture if is `canRescale()`
   * @param ratio the resize factor we want to use to rescale
   */
  virtual void scale(float ratio);

  /**
   * @brief Get if the texture can rescale.
   */
  bool canRescale();

  /**
   * @brief Hidden
   */
  InternalTexturePtr _getFromCache(const std::string& url, bool noMipmap, unsigned int sampling = 0,
                                   const std::optional<bool>& invertY = std::nullopt);
  /**
   * @brief Hidden
   */
  virtual void _rebuild(bool forceFullRebuild = false);

  /**
   * @brief Hidden
   */
  std::vector<AnimationPtr> getAnimations() override;

  /**
   * @brief Clones the texture.
   * @returns the cloned texture
   */
  std::unique_ptr<BaseTexture> clone() const;

  /**
   * @brief Reads the pixels stored in the webgl texture and returns them as an ArrayBuffer. This
   * will returns an RGBA array buffer containing either in values (0-255) or float values (0-1)
   * depending of the underlying buffer type.
   * @param faceIndex defines the face of the texture to read (in case of cube texture)
   * @param level defines the LOD level of the texture to read (in case of Mip Maps)
   * @param buffer defines a user defined buffer to fill with data (can be null)
   * @returns The Array buffer containing the pixels data.
   */
  ArrayBufferView readPixels(unsigned int faceIndex = 0, int level = 0,
                             std::optional<ArrayBufferView> buffer = std::nullopt);

  /**
   * @brief Dispose the texture and release its associated resources.
   */
  virtual void dispose() override;

  /**
   * @brief Serialize the texture into a JSON representation that can be parsed later on.
   * @returns the JSON representation of the texture
   */
  json serialize() const;

  /**
   * @brief Helper function to be called back once a list of texture contains only ready textures.
   * @param textures Define the list of textures to wait for
   * @param callback Define the callback triggered once the entire list will be ready
   */
  static void WhenAllReady(const std::vector<BaseTexture*>& textures,
                           const std::function<void()>& callback);

protected:
  /**
   * @brief Instantiates a new BaseTexture.
   * Base class of all the textures in babylon.
   * It groups all the common properties the materials, post process, lights... might need
   * in order to make a correct use of the texture.
   * @param sceneOrEngine Define the scene or engine the texture blongs to
   */
  BaseTexture(const std::optional<std::variant<Scene*, ThinEngine*>>& sceneOrEngine);

  void set_hasAlpha(bool value);
  bool get_hasAlpha() const;
  void set_coordinatesMode(unsigned int value) override;
  unsigned int get_coordinatesMode() const override;
  unsigned int get_wrapU() const override;
  void set_wrapU(unsigned int value) override;
  unsigned int get_wrapV() const override;
  void set_wrapV(unsigned int value) override;
  bool get_isCube() const override;
  void set_isCube(bool value) override;
  bool get_is3D() const override;
  void set_is3D(bool value) override;
  bool get_is2DArray() const override;
  void set_is2DArray(bool value) override;
  bool get_gammaSpace() const;
  void set_gammaSpace(bool gamma);
  virtual bool get_noMipmap() const;

  /**
   * @brief Gets whether or not the texture contains RGBD data.
   */
  bool get_isRGBD() const;

  /**
   * @brief Gets whether or not the texture contains RGBD data.
   */
  void set_isRGBD(bool value);

  virtual void set_boundingBoxSize(const std::optional<Vector3>& value);
  virtual std::optional<Vector3>& get_boundingBoxSize();
  float get_lodGenerationOffset() const;
  void set_lodGenerationOffset(float value);
  float get_lodGenerationScale() const;
  void set_lodGenerationScale(float value);
  bool get_linearSpecularLOD() const;
  void set_linearSpecularLOD(bool value);
  BaseTexturePtr& get_irradianceTexture();
  void set_irradianceTexture(const BaseTexturePtr& value);
  std::string get_uid() const;
  void set_onDispose(const std::function<void(BaseTexture*, EventState&)>& callback);
  virtual bool get_isBlocking() const;
  virtual void set_isBlocking(bool value);
  unsigned int get_textureType() const;
  unsigned int get_textureFormat() const;
  SphericalPolynomialPtr& get_sphericalPolynomial();
  void set_sphericalPolynomial(const SphericalPolynomialPtr& value);
  BaseTexturePtr& get__lodTextureHigh();
  BaseTexturePtr& get__lodTextureMid();
  BaseTexturePtr& get__lodTextureLow();

  /**
   * @brief Hidden
   */
  ThinEngine* _getEngine() const;

  /**
   * @brief Indicates that textures need to be re-calculated for all materials.
   */
  void _markAllSubMeshesAsTexturesDirty();

private:
  static bool _isScene(const std::variant<Scene*, ThinEngine*>& sceneOrEngine);

public:
  /**
   * Gets or sets the unique id of the texture
   */
  size_t uniqueId;

  /**
   * Define the name of the texture.
   */
  std::string name;

  /**
   * Define if the texture is having a usable alpha value (can be use for transparency or glossiness
   * for instance).
   */
  Property<BaseTexture, bool> hasAlpha;

  /**
   * Defines if the alpha value should be determined via the rgb values.
   * If true the luminance of the pixel might be used to find the corresponding
   * alpha value.
   */
  bool getAlphaFromRGB;

  /**
   * Intensity or strength of the texture.
   * It is commonly used by materials to fine tune the intensity of the texture
   */
  float level;

  /**
   * Define the UV chanel to use starting from 0 and defaulting to 0.
   * This is part of the texture as textures usually maps to one uv set.
   */
  unsigned int coordinatesIndex;

  /**
   * With compliant hardware and browser (supporting anisotropic filtering) this defines the level
   * of anisotropic filtering in the texture. The higher the better but the slower. This defaults to
   * 4 as it seems to be the best tradeoff.
   */
  unsigned int anisotropicFilteringLevel;

  /**
   * Define if the texture contains data in gamma space (most of the png/jpg aside bump). HDR
   * texture are usually stored in linear space. This only impacts the PBR and Background materials
   */
  Property<BaseTexture, bool> gammaSpace;

  /**
   * Gets or sets whether or not the texture contains RGBD data.
   */
  Property<BaseTexture, bool> isRGBD;

  /**
   * Is Z inverted in the texture (useful in a cube texture).
   */
  bool invertZ;

  /**
   * Are mip maps generated for this texture or not.
   */
  ReadOnlyProperty<BaseTexture, bool> noMipmap;

  /**
   * Hidden
   */
  bool lodLevelInAlpha;

  /**
   * With prefiltered texture, defined the offset used during the prefiltering steps.
   */
  Property<BaseTexture, float> lodGenerationOffset;

  /**
   * With prefiltered texture, defined the scale used during the prefiltering steps.
   */
  Property<BaseTexture, float> lodGenerationScale;

  /**
   * With prefiltered texture, defined if the specular generation is based on a linear ramp. By
   * default we are using a log2 of the linear roughness helping to keep a better resolution for
   * average roughness values.
   */
  Property<BaseTexture, bool> linearSpecularLOD;

  /**
   * In case a better definition than spherical harmonics is required for the diffuse part of the
   * environment. You can set the irradiance texture to rely on a texture instead of the spherical
   * approach. This texture need to have the same characteristics than its parent (Cube vs 2d,
   * coordinates mode, Gamma/Linear, RGBD).
   */
  Property<BaseTexture, BaseTexturePtr> irradianceTexture;

  /**
   * Define if the texture is a render target.
   */
  bool isRenderTarget;

  /**
   * @hidden
   */
  bool _prefiltered;

  /**
   * Define the unique id of the texture in the scene.
   */
  ReadOnlyProperty<BaseTexture, std::string> uid;

  /**
   * Define the list of animation attached to the texture.
   */
  std::vector<AnimationPtr> animations;
  /**
   * An event triggered when the texture is disposed.
   */
  Observable<BaseTexture> onDisposeObservable;

  /**
   * Callback triggered when the texture has been disposed.
   * Kept for back compatibility, you can use the onDisposeObservable instead.
   */
  WriteOnlyProperty<BaseTexture, std::function<void(BaseTexture*, EventState&)>> onDispose;

  /**
   * Define if the texture is preventinga material to render or not.
   * If not and the texture is not ready, the engine will use a default black
   * texture instead.
   */
  Property<BaseTexture, bool> isBlocking;

  /**
   * Size of the bounding box associated with the texture (when in cube mode)
   */
  Property<BaseTexture, std::optional<Vector3>> boundingBoxSize;

  /**
   *Get the texture underlying type (INT, FLOAT...)
   */
  ReadOnlyProperty<BaseTexture, unsigned int> textureType;

  /**
   * Get the texture underlying format (RGB, RGBA...)
   */
  ReadOnlyProperty<BaseTexture, unsigned int> textureFormat;

  /**
   * Get the polynomial representation of the texture data.
   * This is mainly use as a fast way to recover IBL Diffuse irradiance data.
   * @see https://learnopengl.com/PBR/IBL/Diffuse-irradiance
   */
  Property<BaseTexture, SphericalPolynomialPtr> sphericalPolynomial;

  /** @hidden */
  ReadOnlyProperty<BaseTexture, BaseTexturePtr> _lodTextureHigh;

  /** @hidden */
  ReadOnlyProperty<BaseTexture, BaseTexturePtr> _lodTextureMid;

  /** @hidden */
  ReadOnlyProperty<BaseTexture, BaseTexturePtr> _lodTextureLow;

protected:
  unsigned int _coordinatesMode;
  Scene* _scene;

private:
  bool _hasAlpha;
  bool _isCube;
  bool _gammaSpace;
  std::string _uid;
  Observer<BaseTexture>::Ptr _onDisposeObserver;
  Matrix _textureMatrix;
  Matrix _reflectionTextureMatrix;
  std::optional<Vector3> emptyVector3;
  SphericalPolynomialPtr _nullSphericalPolynomial;
  BaseTexturePtr _nullBaseTexture;

}; // end of class BaseTexture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_BASE_TEXTURE_H
