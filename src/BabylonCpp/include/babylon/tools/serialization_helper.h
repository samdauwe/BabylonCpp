#ifndef BABYLON_TOOLS_SERIALIZATION_HELPER_H
#define BABYLON_TOOLS_SERIALIZATION_HELPER_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT SerializationHelper {

  static Light* Parse(Light* light, const Json::value& parsedLight,
                      Scene* scene);

}; // end of struct SerializationHelper

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_SERIALIZATION_HELPER_H
