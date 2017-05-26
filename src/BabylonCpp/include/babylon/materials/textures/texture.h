#ifndef BABYLON_MATERIALS_TEXTURES_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_TEXTURE_H

#include <babylon/babylon_global.h>
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
    auto texture = new Texture(std::forward<Ts>(args)...);
    texture->addToScene(static_cast<std::unique_ptr<BaseTexture>>(texture));

    return texture;
  }
  virtual ~Texture();

  virtual IReflect::Type type() const override;

  bool noMipmap() const;
  void delayLoad() override;
  void updateSamplingMode(unsigned int samplingMode);
  Matrix* getTextureMatrix() override;
  Matrix* getReflectionTextureMatrix() override;
  Texture* clone() const;

  /** Statics **/
  static Texture* CreateFromBase64String(
    const std::string& data, const std::string& name, Scene* scene,
    bool noMipmap = false, bool invertY = false,
    unsigned int samplingMode = TextureConstants::TRILINEAR_SAMPLINGMODE,
    const std::function<void()>& onLoad  = nullptr,
    const std::function<void()>& onError = nullptr,
    unsigned int format                  = EngineConstants::TEXTUREFORMAT_RGBA);
  static std::unique_ptr<BaseTexture> Parse(const Json::value& parsedTexture,
                                            Scene* scene,
                                            const std::string& rootUrl);
  static Texture* LoadFromDataString(
    const std::string& name, Buffer* buffer, Scene* scene,
    bool deleteBuffer = false, bool noMipmap = false, bool invertY = true,
    unsigned int samplingMode = TextureConstants::TRILINEAR_SAMPLINGMODE,
    const std::function<void()>& onLoad  = nullptr,
    const std::function<void()>& onError = nullptr,
    unsigned int format                  = EngineConstants::TEXTUREFORMAT_RGBA);

protected:
  Texture(const std::string& url, Scene* scene, bool noMipmap = false,
          bool invertY              = true,
          unsigned int samplingMode = TextureConstants::TRILINEAR_SAMPLINGMODE,
          const std::function<void()>& onLoad  = nullptr,
          const std::function<void()>& onError = nullptr,
          Buffer* buffer = nullptr, bool deleteBuffer = false,
          unsigned int format = EngineConstants::TEXTUREFORMAT_RGBA);

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
  bool _invertY;
  unsigned int _samplingMode;

protected:
  unsigned int _format;

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
  unsigned int _cachedCoordinatesMode;
  Buffer* _buffer;
  bool _deleteBuffer;
  std::function<void()> _delayedOnLoad;
  std::function<void()> _delayedOnError;
  Observable<bool> _onLoadObservable;

  std::function<void()> _onLoad;
  std::function<void()> _load;

}; // end of class Texture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_TEXTURE_H
