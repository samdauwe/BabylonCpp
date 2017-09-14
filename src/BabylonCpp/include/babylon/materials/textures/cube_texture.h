#ifndef BABYLON_MATERIALS_TEXTURES_CUBE_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_CUBE_TEXTURE_H

#include <babylon/babylon_global.h>
#include <babylon/engine/engine_constants.h>
#include <babylon/materials/textures/base_texture.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT CubeTexture : public BaseTexture {

public:
  CubeTexture(const string_t& rootUrl, Scene* scene,
              const vector_t<string_t>& extensions = {}, bool noMipmap = false,
              const vector_t<string_t>& files        = {},
              const ::std::function<void()>& onLoad  = nullptr,
              const ::std::function<void()>& onError = nullptr,
              unsigned int format = EngineConstants::TEXTUREFORMAT_RGBA,
              bool prefiltered = false, const string_t& forcedExtension = "");
  ~CubeTexture();

  /** Methods **/
  void delayLoad();
  Matrix* getReflectionTextureMatrix();
  void setReflectionTextureMatrix(const Matrix& value);
  std::unique_ptr<CubeTexture> clone() const;

  /** Static methods **/
  static std::unique_ptr<CubeTexture>
  CreateFromImages(const vector_t<string_t>& files, Scene* scene,
                   bool noMipmap = false);
  static std::unique_ptr<CubeTexture>
  CreateFromPrefilteredData(const string_t& url, Scene* scene,
                            const string_t& forcedExtension = "");
  static std::unique_ptr<CubeTexture> Parse(const Json::value& parsedTexture,
                                            Scene* scene,
                                            const string_t& rootUrl);

public:
  string_t url;
  unsigned int coordinatesMode;

private:
  bool _noMipmap;
  vector_t<string_t> _files;
  vector_t<string_t> _extensions;
  std::unique_ptr<Matrix> _textureMatrix;
  unsigned int _format;
  bool _prefiltered;

}; // end of class CubeTexture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_CUBE_TEXTURE_H
