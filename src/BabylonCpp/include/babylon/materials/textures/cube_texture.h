#ifndef BABYLON_MATERIALS_TEXTURES_CUBE_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_CUBE_TEXTURE_H

#include <babylon/babylon_global.h>
#include <babylon/materials/textures/base_texture.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT CubeTexture : public BaseTexture {

public:
  CubeTexture(const std::string& rootUrl, Scene* scene,
              const std::vector<std::string>& extensions, bool noMipmap,
              const std::vector<std::string>& files
              = std::vector<std::string>(),
              const std::function<void()>& onLoad  = nullptr,
              const std::function<void()>& onError = nullptr);
  ~CubeTexture();

  /** Methods **/
  void delayLoad();
  Matrix* getReflectionTextureMatrix();
  std::unique_ptr<CubeTexture> clone() const;

  /** Static methods **/
  static CubeTexture* CreateFromImages(const std::vector<std::string>& files,
                                       Scene* scene, bool noMipmap = false);
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

}; // end of class CubeTexture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_CUBE_TEXTURE_H
