#ifndef BABYLON_MATERIALS_TEXTURES_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_TEXTURE_H

#include <babylon/babylon_api.h>
#include <babylon/core/variant.h>
#include <babylon/engine/engine_constants.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/math/matrix.h>
#include <babylon/tools/observable.h>

namespace BABYLON {

class Texture;
using TexturePtr = std::shared_ptr<Texture>;

class BABYLON_SHARED_EXPORT Texture : public BaseTexture {

  friend Scene;

public:
  /**
   * Gets or sets a boolean which defines if the texture url must be build from
   * the serialized URL instead of just using the name and loading them side by
   * side with the scene file
   */
  static bool UseSerializedUrlIfAny;

public:
  template <typename... Ts>
  static TexturePtr New(Ts&&... args)
  {
    auto texture
      = std::shared_ptr<Texture>(new Texture(std::forward<Ts>(args)...));
    texture->addToScene(texture);

    return texture;
  }
  virtual ~Texture() override;

  virtual IReflect::Type type() const override;

  /**
   * @brief Returns the string "Texture".
   */
  const char* getClassName() const;

  /**
   * @brief Update the url (and optional buffer) of this texture if url was null
   * during construction.
   * @param url the url of the texture
   * @param buffer the buffer of the texture (defaults to null)
   */
  void updateURL(const std::string& iUrl,
                 const std::optional<Variant<ArrayBuffer, Image>>& buffer);

  void delayLoad() override;

  /**
   * @brief Default is Trilinear mode.
   *
   * | Value | Type               | Description |
   * | ----- | ------------------ | ----------- |
   * | 1     | NEAREST_SAMPLINGMODE or NEAREST_NEAREST_MIPLINEAR  | Nearest is:
   * mag = nearest, min = nearest, mip = linear |
   * | 2     | BILINEAR_SAMPLINGMODE or LINEAR_LINEAR_MIPNEAREST | Bilinear is:
   * mag = linear, min = linear, mip = nearest |
   * | 3     | TRILINEAR_SAMPLINGMODE or
   * LINEAR_LINEAR_MIPLINEAR | Trilinear is: mag = linear, min = linear, mip =
   * linear |
   * | 4     | NEAREST_NEAREST_MIPNEAREST |             |
   * | 5     | NEAREST_LINEAR_MIPNEAREST |             |
   * | 6     | NEAREST_LINEAR_MIPLINEAR |             |
   * | 7     | NEAREST_LINEAR |             |
   * | 8     | NEAREST_NEAREST |             |
   * | 9     | LINEAR_NEAREST_MIPNEAREST |             |
   * | 10    | LINEAR_NEAREST_MIPLINEAR |             |
   * | 11    | LINEAR_LINEAR |             |
   * | 12    | LINEAR_NEAREST |             |
   *
   *    > _mag_: magnification filter (close to the viewer)
   *    > _min_: minification filter (far from the viewer)
   *    > _mip_: filter used between mip map levels
   *
   */
  void updateSamplingMode(unsigned int samplingMode);

  Matrix* getTextureMatrix() override;
  Matrix* getReflectionTextureMatrix() override;
  TexturePtr clone() const;
  Json::object serialize() const;
  void dispose() override;

  /** Statics **/
  static TexturePtr CreateFromBase64String(
    const std::string& data, const std::string& name, Scene* scene,
    bool noMipmap = false, bool invertY = false,
    unsigned int samplingMode = TextureConstants::TRILINEAR_SAMPLINGMODE,
    const std::function<void()>& onLoad  = nullptr,
    const std::function<void()>& onError = nullptr,
    unsigned int format = EngineConstants::TEXTUREFORMAT_RGBA);
  static std::unique_ptr<BaseTexture> Parse(const Json::value& parsedTexture,
                                            Scene* scene,
                                            const std::string& rootUrl);
  static TexturePtr LoadFromDataString(
    const std::string& name,
    const std::optional<Variant<ArrayBuffer, Image>>& buffer, Scene* scene,
    bool deleteBuffer = false, bool noMipmap = false, bool invertY = true,
    unsigned int samplingMode = TextureConstants::TRILINEAR_SAMPLINGMODE,
    const std::function<void()>& onLoad  = nullptr,
    const std::function<void()>& onError = nullptr,
    unsigned int format = EngineConstants::TEXTUREFORMAT_RGBA);

protected:
  Texture(const std::string& url, Scene* scene, bool noMipmap = false,
          bool invertY              = true,
          unsigned int samplingMode = TextureConstants::TRILINEAR_SAMPLINGMODE,
          const std::function<void()>& onLoad  = nullptr,
          const std::function<void()>& onError = nullptr,
          const std::optional<Variant<ArrayBuffer, Image>>& buffer
          = std::nullopt,
          bool deleteBuffer                         = false,
          const std::optional<unsigned int>& format = std::nullopt);

  bool get_noMipmap() const;
  void set_isBlocking(bool value) override;
  bool get_isBlocking() const override;
  unsigned int get_samplingMode() const;
  Observable<Texture>& get_onLoadObservable();

private:
  void _prepareRowForTextureGeneration(float x, float y, float z, Vector3& t);

public:
  std::string url;
  float uOffset;
  float vOffset;
  float uScale;
  float vScale;
  float uAng;
  float vAng;
  float wAng;

  /**
   * Defines the center of rotation (U)
   */
  float uRotationCenter;

  /**
   * Defines the center of rotation (V)
   */
  float vRotationCenter;

  /**
   * Defines the center of rotation (W)
   */
  float wRotationCenter;

  ReadOnlyProperty<Texture, bool> noMipmap;

  /** Hidden */
  bool _invertY;
  unsigned int _samplingMode;

  /** Hidden */
  ReadOnlyProperty<Texture, unsigned int> samplingMode;
  ReadOnlyProperty<Texture, Observable<Texture>> onLoadObservable;

protected:
  const std::optional<unsigned int> _format;
  Observable<Texture> _onLoadObservable;
  bool _isBlocking;

private:
  bool _noMipmap;
  std::unique_ptr<Matrix> _rowGenerationMatrix;
  std::unique_ptr<Matrix> _cachedTextureMatrix;
  std::unique_ptr<Matrix> _projectionModeMatrix;
  Vector3 _t0;
  Vector3 _t1;
  Vector3 _t2;

  float _cachedUOffset;
  float _cachedVOffset;
  float _cachedUScale;
  float _cachedVScale;
  float _cachedUAng;
  float _cachedVAng;
  float _cachedWAng;
  int _cachedProjectionMatrixId;
  unsigned int _cachedCoordinatesMode;
  std::optional<Variant<ArrayBuffer, Image>> _buffer;
  bool _deleteBuffer;
  std::function<void(InternalTexture*, EventState&)> _delayedOnLoad;
  std::function<void()> _delayedOnError;

  std::function<void()> _onLoad;
  std::function<void(InternalTexture*, EventState&)> _load;

}; // end of class Texture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_TEXTURE_H
