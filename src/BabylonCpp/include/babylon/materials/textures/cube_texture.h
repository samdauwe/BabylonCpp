#ifndef BABYLON_MATERIALS_TEXTURES_CUBE_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_CUBE_TEXTURE_H

#include <babylon/babylon_global.h>
#include <babylon/engine/engine_constants.h>
#include <babylon/materials/textures/base_texture.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT CubeTexture : public BaseTexture {

public:
  CubeTexture(const std::string& rootUrl, Scene* scene,
              const std::vector<std::string>& extensions = {},
              bool noMipmap = false, const std::vector<std::string>& files = {},
              const std::function<void()>& onLoad  = nullptr,
              const std::function<void()>& onError = nullptr,
              unsigned int format = EngineConstants::TEXTUREFORMAT_RGBA,
              bool prefiltered    = false);
  ~CubeTexture();

  /** Methods **/
  void delayLoad();
  Matrix* getReflectionTextureMatrix();
  void setReflectionTextureMatrix(const Matrix& value);
  std::unique_ptr<CubeTexture> clone() const;

  /** Static methods **/
  static std::unique_ptr<CubeTexture>
  CreateFromImages(const std::vector<std::string>& files, Scene* scene,
                   bool noMipmap = false);
  static std::unique_ptr<CubeTexture>
  CreateFromPrefilteredData(const std::string& url, Scene* scene);
  static std::unique_ptr<CubeTexture> Parse(const Json::value& parsedTexture,
                                            Scene* scene,
                                            const std::string& rootUrl);

public:
  std::string url;
  unsigned int coordinatesMode;

private:
  bool _noMipmap;
  std::vector<std::string> _files;
  std::vector<std::string> _extensions;
  std::unique_ptr<Matrix> _textureMatrix;
  unsigned int _format;
  bool _prefiltered;

}; // end of class CubeTexture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_CUBE_TEXTURE_H
