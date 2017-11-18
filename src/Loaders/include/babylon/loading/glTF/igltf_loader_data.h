#ifndef BABYLON_LOADING_GLTF_IGLTF_LOADER_DATA_H
#define BABYLON_LOADING_GLTF_IGLTF_LOADER_DATA_H

#include <babylon/babylon_global.h>

namespace BABYLON {
namespace GLTF2 {

struct BABYLON_SHARED_EXPORT IGLTFLoaderData {
  json::object json;
  Nullable<ArrayBufferView> bin;
}; // end of struct IGLTFLoaderData

} // end of namespace GLTF2
} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_GLTF_IGLTF_LOADER_DATA_H