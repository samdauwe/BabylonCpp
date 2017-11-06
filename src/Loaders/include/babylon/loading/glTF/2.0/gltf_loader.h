#ifndef BABYLON_LOADING_GLTF_2_0_GLTF_LOADER_H
#define BABYLON_LOADING_GLTF_2_0_GLTF_LOADER_H

#include <babylon/babylon_global.h>
#include <babylon/core/nullable.h>
#include <babylon/loading/glTF/2.0/gltf_loader_interfaces.h>

namespace BABYLON {
namespace GLTF2 {

/**
 * @brief Babylon.js glTF File Loader. This loader supports only glTF 2.0 and
 * will fail to load glTF 1.0.
 */
class BABYLON_SHARED_EXPORT GLTFLoader {

public:
  GLTFLoader();
  ~GLTFLoader();

private:
  static unsigned int
  _GetTextureWrapMode(const Nullable<ETextureWrapMode>& mode);
  static unsigned int
  _GetTextureSamplingMode(const Nullable<ETextureMagFilter>& magFilter,
                          const Nullable<ETextureMinFilter>& minFilter);
  static unsigned int _GetNumComponents(const string_t& type);

}; // end of class GLTFLoader

} // end of namespace GLTF2
} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_GLTF_2_0_GLTF_LOADER_H
