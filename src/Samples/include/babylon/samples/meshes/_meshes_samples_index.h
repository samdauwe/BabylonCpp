#ifndef BABYLON_SAMPLES_MESHES_MESHES_SAMPLES_INDEX_H
#define BABYLON_SAMPLES_MESHES_MESHES_SAMPLES_INDEX_H

#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/samples/samples_common.h>

namespace BABYLON {
namespace Samples {

class BABYLON_SHARED_EXPORT _MeshesSamplesIndex : public _ISamplesIndex {

private:
  static constexpr const char* _CategoryName = "Meshes";

public:
  /**
   * @brief Returns the category name.
   * @return the category name
   */
  static const string_t CategoryName()
  {
    return _MeshesSamplesIndex::_CategoryName;
  }

public:
  _MeshesSamplesIndex();
  virtual ~_MeshesSamplesIndex();

}; // end of class _MeshesSamplesIndex

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESHES_MESHES_SAMPLES_INDEX_H
