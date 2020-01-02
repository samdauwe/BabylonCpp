#ifndef BABYLON_ENGINES_EXTENSIONS_CUBE_TEXTURE_EXTENSION_H
#define BABYLON_ENGINES_EXTENSIONS_CUBE_TEXTURE_EXTENSION_H

#include <functional>
#include <memory>
#include <optional>
#include <vector>

#include <babylon/babylon_api.h>

namespace BABYLON {

struct CubeTextureData;
struct DepthTextureCreationOptions;
struct IInternalTextureLoader;
struct Image;
class InternalTexture;
class Scene;
class ThinEngine;
using InternalTexturePtr        = std::shared_ptr<InternalTexture>;
using IInternalTextureLoaderPtr = std::shared_ptr<IInternalTextureLoader>;

/**
 * @brief Hidden
 */
class BABYLON_SHARED_EXPORT CubeTextureExtension {

public:
  CubeTextureExtension(ThinEngine* engine);
  ~CubeTextureExtension();

  /**
   * @brief Creates a depth stencil cube texture.
   * This is only available in WebGL 2.
   * @param size The size of face edge in the cube texture.
   * @param options The options defining the cube texture.
   * @returns The cube texture
   */
  InternalTexturePtr _createDepthStencilCubeTexture(int size,
                                                    const DepthTextureCreationOptions& options);

  /**
   * @brief Creates a cube texture.
   * @param rootUrl defines the url where the files to load is located
   * @param scene defines the current scene
   * @param files defines the list of files to load (1 per face)
   * @param noMipmap defines a boolean indicating that no mipmaps shall be generated (false by
   * default)
   * @param onLoad defines an optional callback raised when the texture is loaded
   * @param onError defines an optional callback raised if there is an issue to load the texture
   * @param format defines the format of the data
   * @param forcedExtension defines the extension to use to pick the right loader
   * @param createPolynomials if a polynomial sphere should be created for the cube texture
   * @param lodScale defines the scale applied to environment texture. This manages the range of LOD
   * level used for IBL according to the roughness
   * @param lodOffset defines the offset applied to environment texture. This manages first LOD
   * level used for IBL according to the roughness
   * @param fallback defines texture to use while falling back when (compressed) texture file not
   * found.
   * @param excludeLoaders array of texture loaders that should be excluded when picking a loader
   * for the texture (defualt: empty array)
   * @returns the cube texture as an InternalTexture
   */
  InternalTexturePtr createCubeTexture(
    std::string rootUrl, Scene* scene, const std::vector<std::string>& files, bool noMipmap,
    const std::function<void(const std::optional<CubeTextureData>& data)>& onLoad = nullptr,
    const std::function<void(const std::string& message, const std::string& exception)>& onError
    = nullptr,
    unsigned int format = 0, const std::string& forcedExtension = "",
    bool createPolynomials = false, float lodScale = 0.f, float lodOffset = 0.f,
    const InternalTexturePtr& fallback                           = nullptr,
    const std::vector<IInternalTextureLoaderPtr>& excludeLoaders = {});

  /**
   * @brief Hidden
   */
  void _cascadeLoadImgs(
    const std::string& rootUrl, Scene* scene,
    const std::function<void(const std::vector<Image>& images)>& onfinish,
    const std::vector<std::string>& files,
    const std::function<void(const std::string& message, const std::string& exception)>& onError
    = nullptr);

  /**
   * @brief Hidden
   */
  void _setCubeMapTextureParams(bool loadMipmap);

private:
  ThinEngine* _this;

}; // end of class CubeTextureExtension

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_EXTENSIONS_CUBE_TEXTURE_EXTENSION_H
