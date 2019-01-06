#ifndef BABYLON_TOOLS_SERIALIZATION_HELPER_H
#define BABYLON_TOOLS_SERIALIZATION_HELPER_H

#include <functional>
#include <map>
#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <vector>

#include <babylon/babylon_api.h>

using json = nlohmann::json;

namespace BABYLON {

class Camera;
class Light;
class Scene;
class StandardMaterial;
class Texture;
using CameraPtr           = std::shared_ptr<Camera>;
using LightPtr            = std::shared_ptr<Light>;
using StandardMaterialPtr = std::shared_ptr<StandardMaterial>;
using TexturePtr          = std::shared_ptr<Texture>;

struct BABYLON_SHARED_EXPORT SerializationHelper {

  static TexturePtr Parse(const std::function<TexturePtr()>& creationFunction,
                          const json& source, Scene* scene,
                          const std::string& rootUrl = "");
  static CameraPtr Parse(const std::function<CameraPtr()>& creationFunction,
                         const json& source, Scene* scene,
                         const std::string& rootUrl = "");
  static LightPtr Parse(const std::function<LightPtr()>& creationFunction,
                        const json& source, Scene* scene,
                        const std::string& rootUrl = "");
  static StandardMaterialPtr
  Parse(const std::function<StandardMaterialPtr()>& creationFunction,
        const json& source, Scene* scene, const std::string& rootUrl = "");

}; // end of struct SerializationHelper

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_SERIALIZATION_HELPER_H
