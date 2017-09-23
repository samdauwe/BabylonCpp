#ifndef BABYLON_MATERIALS_TEXTURES_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_TEXTURE_H

#include <babylon/babylon_global.h>
#include <babylon/core/nullable.h>
#include <babylon/engine/engine_constants.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/math/matrix.h>
#include <babylon/tools/observable.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT Texture : public BaseTexture {

public:
  template <typename... Ts>
  static Texture* New(Ts&&... args)
  {
    auto texture = new Texture(::std::forward<Ts>(args)...);
    texture->addToScene(static_cast<unique_ptr_t<BaseTexture>>(texture));

    return texture;
  }
  virtual ~Texture();

  virtual IReflect::Type type() const override;

  /**
   * @brief Returns the string "Texture".
   */
  const char* getClassName() const;

  void setIsBlocking(bool value);
  bool isBlocking() const override;
  unsigned int samplingMode() const;

  bool noMipmap() const;
  void updateURL(const string_t& iUrl);
  void delayLoad() override;
  void updateSamplingMode(unsigned int samplingMode);
  Matrix* getTextureMatrix() override;
  Matrix* getReflectionTextureMatrix() override;
  Texture* clone() const;
  Nullable<Observable<Texture>>& onLoadObservable();
  Json::object serialize() const;
  void dispose(bool doNotRecurse = false) override;

  /** Statics **/
  static Texture* CreateFromBase64String(
    const string_t& data, const string_t& name, Scene* scene,
    bool noMipmap = false, bool invertY = false,
    unsigned int samplingMode = TextureConstants::TRILINEAR_SAMPLINGMODE,
    const ::std::function<void()>& onLoad  = nullptr,
    const ::std::function<void()>& onError = nullptr,
    unsigned int format = EngineConstants::TEXTUREFORMAT_RGBA);
  static unique_ptr_t<BaseTexture> Parse(const Json::value& parsedTexture,
                                         Scene* scene, const string_t& rootUrl);
  static Texture* LoadFromDataString(
    const string_t& name, Buffer* buffer, Scene* scene,
    bool deleteBuffer = false, bool noMipmap = false, bool invertY = true,
    unsigned int samplingMode = TextureConstants::TRILINEAR_SAMPLINGMODE,
    const ::std::function<void()>& onLoad  = nullptr,
    const ::std::function<void()>& onError = nullptr,
    unsigned int format = EngineConstants::TEXTUREFORMAT_RGBA);

protected:
  Texture(const string_t& url, Scene* scene, bool noMipmap = false,
          bool invertY              = true,
          unsigned int samplingMode = TextureConstants::TRILINEAR_SAMPLINGMODE,
          const ::std::function<void()>& onLoad  = nullptr,
          const ::std::function<void()>& onError = nullptr,
          Buffer* buffer = nullptr, bool deleteBuffer = false,
          unsigned int format = EngineConstants::TEXTUREFORMAT_RGBA);

private:
  void _prepareRowForTextureGeneration(float x, float y, float z, Vector3& t);

public:
  string_t url;
  float uOffset;
  float vOffset;
  float uScale;
  float vScale;
  float uAng;
  float vAng;
  float wAng;
  bool _invertY;
  unsigned int _samplingMode;

protected:
  unsigned int _format;
  bool _isBlocking;

private:
  bool _noMipmap;
  unique_ptr_t<Matrix> _rowGenerationMatrix;
  unique_ptr_t<Matrix> _cachedTextureMatrix;
  unique_ptr_t<Matrix> _projectionModeMatrix;
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
  Buffer* _buffer;
  bool _deleteBuffer;
  ::std::function<void(InternalTexture*, EventState&)> _delayedOnLoad;
  ::std::function<void()> _delayedOnError;
  Nullable<Observable<Texture>> _onLoadObservable;

  ::std::function<void()> _onLoad;
  ::std::function<void(InternalTexture*, EventState&)> _load;

}; // end of class Texture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_TEXTURE_H
