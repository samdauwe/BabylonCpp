#ifndef BABYLON_MISC_BRDF_TEXTURE_TOOLS_H
#define BABYLON_MISC_BRDF_TEXTURE_TOOLS_H

#include <memory>

#include <babylon/babylon_api.h>

namespace BABYLON {

class BaseTexture;
class InternalTexture;
class Scene;
using BaseTexturePtr     = std::shared_ptr<BaseTexture>;
using InternalTexturePtr = std::shared_ptr<InternalTexture>;

/**
 * @brief Class used to host texture specific utilities.
 */
class BABYLON_SHARED_EXPORT BRDFTextureTools {

public:
  /**
   * @brief Gets a default environment BRDF for MS-BRDF Height Correlated BRDF.
   * @param scene defines the hosting scene
   * @returns the environment BRDF texture
   */
  static BaseTexturePtr GetEnvironmentBRDFTexture(Scene* scene);

private:
  /**
   * @brief Expand the BRDF Texture from RGBD to Half Float if necessary.
   * @param texture the texture to expand.
   */
  static void _ExpandDefaultBRDFTexture(const InternalTexturePtr& texture);

private:
  static std::string _environmentBRDFBase64Texture;

}; // end of class BRDFTextureTools

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_BRDF_TEXTURE_TOOLS_H
