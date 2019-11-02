#ifndef BABYLON_SAMPLES_PARTICLES_PARTICLES_SAMPLES_INDEX_H
#define BABYLON_SAMPLES_PARTICLES_PARTICLES_SAMPLES_INDEX_H

#include <babylon/samples/samples_common.h>

namespace BABYLON {
namespace Samples {

class _ParticlesSamplesIndex : public _ISamplesIndex {

public:
  /**
   * @brief Returns the category name.
   * @return the category name
   */
  static const std::string CategoryName()
  {
    return "Particles";
  }

public:
  _ParticlesSamplesIndex();
  virtual ~_ParticlesSamplesIndex() = default;

}; // end of class _ParticlesSamplesIndex

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_PARTICLES_PARTICLES_SAMPLES_INDEX_H
