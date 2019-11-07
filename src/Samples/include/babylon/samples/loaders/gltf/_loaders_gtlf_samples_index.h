#ifndef BABYLON_SAMPLES_LOADERS_GLTF_LOADERS_GLTF_SAMPLES_INDEX_H
#define BABYLON_SAMPLES_LOADERS_GLTF_LOADERS_GLTF_SAMPLES_INDEX_H

#include <babylon/samples/samples_common.h>

namespace BABYLON {
namespace Samples {

class _LoadersGLTFSamplesIndex : public _ISamplesIndex {

public:
  /**
   * @brief Returns the category name.
   * @return the category name
   */
  static const std::string CategoryName()
  {
    return "Loaders - glTF format";
  }

  _LoadersGLTFSamplesIndex()          = default;
  virtual ~_LoadersGLTFSamplesIndex() = default;

}; // end of class _LoadersGLTFSamplesIndex

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_LOADERS_GLTF_LOADERS_GLTF_SAMPLES_INDEX_H
