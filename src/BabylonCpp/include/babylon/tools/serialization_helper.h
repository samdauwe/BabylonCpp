#ifndef BABYLON_TOOLS_SERIALIZATION_HELPER_H
#define BABYLON_TOOLS_SERIALIZATION_HELPER_H

#include <map>
#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <vector>

#include <babylon/babylon_api.h>

using json = nlohmann::json;

namespace BABYLON {

class BaseTexture;
class Camera;
class Light;
class Scene;
class StandardMaterial;
using CameraPtr           = std::shared_ptr<Camera>;
using LightPtr            = std::shared_ptr<Light>;
using StandardMaterialPtr = std::shared_ptr<StandardMaterial>;

struct BABYLON_SHARED_EXPORT SerializationHelper {

  static BaseTexture* Parse(BaseTexture* baseTexture,
                            const json& parsedBaseTexture, Scene* scene);
  static CameraPtr Parse(const CameraPtr& camera, const json& parsedCamera,
                         Scene* scene);
  static LightPtr Parse(const LightPtr& light, const json& parsedLight,
                        Scene* scene);
  static StandardMaterialPtr Parse(const StandardMaterialPtr& standardMaterial,
                                   const json& parsedMaterial, Scene* scene,
                                   const std::string& rootUrl);

}; // end of struct SerializationHelper

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_SERIALIZATION_HELPER_H
