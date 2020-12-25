#ifndef BABYLON_MISC_BRDF_TEXTURE_TOOLS_H
#define BABYLON_MISC_BRDF_TEXTURE_TOOLS_H

#include <memory>
#include <string>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>

namespace BABYLON {

class Scene;
FWD_CLASS_SPTR(BaseTexture)

/**
 * @brief Class used to host texture specific utilities.
 */
class BABYLON_SHARED_EXPORT BRDFTextureTools {

private:
  /**
   * Prevents texture cache collision
   */
  static size_t _instanceNumber;

public:
  /**
   * @brief Gets a default environment BRDF for MS-BRDF Height Correlated BRDF
   * @param scene defines the hosting scene
   * @returns the environment BRDF texture
   */
  static BaseTexturePtr GetEnvironmentBRDFTexture(Scene* scene);

private:
  static std::string _environmentBRDFBase64Texture;

}; // end of class BRDFTextureTools

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_BRDF_TEXTURE_TOOLS_H
