#ifndef BABYLON_TOOLS_SERIALIZATION_HELPER_H
#define BABYLON_TOOLS_SERIALIZATION_HELPER_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT SerializationHelper {

  static BaseTexture* Parse(BaseTexture* baseTexture,
                            const Json::value& parsedBaseTexture, Scene* scene);
  static CameraPtr Parse(const CameraPtr& camera,
                         const Json::value& parsedCamera, Scene* scene);
  static LightPtr Parse(const LightPtr& light, const Json::value& parsedLight,
                        Scene* scene);
  static StandardMaterialPtr Parse(const StandardMaterialPtr& standardMaterial,
                                   const Json::value& parsedMaterial,
                                   Scene* scene, const string_t& rootUrl);

}; // end of struct SerializationHelper

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_SERIALIZATION_HELPER_H
