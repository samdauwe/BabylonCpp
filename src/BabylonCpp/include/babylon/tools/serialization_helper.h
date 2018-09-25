#ifndef BABYLON_TOOLS_SERIALIZATION_HELPER_H
#define BABYLON_TOOLS_SERIALIZATION_HELPER_H

#include <map>
#include <memory>
#include <vector>

#include <babylon/babylon_api.h>

namespace picojson {
class value;
typedef std::vector<value> array;
typedef std::map<std::string, value> object;
} // end of namespace picojson

namespace BABYLON {

class BaseTexture;
class Camera;
class Light;
class Scene;
class StandardMaterial;
using CameraPtr           = std::shared_ptr<Camera>;
using LightPtr            = std::shared_ptr<Light>;
using StandardMaterialPtr = std::shared_ptr<StandardMaterial>;

namespace Json {
typedef picojson::value value;
typedef picojson::array array;
typedef picojson::object object;
} // namespace Json

struct BABYLON_SHARED_EXPORT SerializationHelper {

  static BaseTexture* Parse(BaseTexture* baseTexture,
                            const Json::value& parsedBaseTexture, Scene* scene);
  static CameraPtr Parse(const CameraPtr& camera,
                         const Json::value& parsedCamera, Scene* scene);
  static LightPtr Parse(const LightPtr& light, const Json::value& parsedLight,
                        Scene* scene);
  static StandardMaterialPtr Parse(const StandardMaterialPtr& standardMaterial,
                                   const Json::value& parsedMaterial,
                                   Scene* scene, const std::string& rootUrl);

}; // end of struct SerializationHelper

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_SERIALIZATION_HELPER_H
