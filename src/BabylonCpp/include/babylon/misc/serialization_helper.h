#ifndef BABYLON_MISC_SERIALIZATION_HELPER_H
#define BABYLON_MISC_SERIALIZATION_HELPER_H

#include <functional>
#include <map>
#include <memory>
#include <vector>

#include <nlohmann/json_fwd.hpp>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>

using json = nlohmann::json;

namespace BABYLON {

class Scene;
FWD_CLASS_SPTR(Camera)
FWD_CLASS_SPTR(CubeTexture)
FWD_CLASS_SPTR(Light)
FWD_CLASS_SPTR(StandardMaterial)
FWD_CLASS_SPTR(Texture)

struct BABYLON_SHARED_EXPORT SerializationHelper {

  static CubeTexturePtr Parse(const std::function<CubeTexturePtr()>& creationFunction,
                              const json& source, Scene* scene, const std::string& rootUrl = "");
  static TexturePtr Parse(const std::function<TexturePtr()>& creationFunction, const json& source,
                          Scene* scene, const std::string& rootUrl = "");
  static CameraPtr Parse(const std::function<CameraPtr()>& creationFunction, const json& source,
                         Scene* scene, const std::string& rootUrl = "");
  static LightPtr Parse(const std::function<LightPtr()>& creationFunction, const json& source,
                        Scene* scene, const std::string& rootUrl = "");
  static StandardMaterialPtr Parse(const std::function<StandardMaterialPtr()>& creationFunction,
                                   const json& source, Scene* scene,
                                   const std::string& rootUrl = "");

}; // end of struct SerializationHelper

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_SERIALIZATION_HELPER_H
