#ifndef BABYLON_TEST_UTILS_H
#define BABYLON_TEST_UTILS_H

#include <babylon/engines/null_engine.h>

namespace BABYLON {

/**
 * @brief Create a new engine subject before each test.
 */
inline std::unique_ptr<BABYLON::Engine> createSubject()
{
  using namespace BABYLON;
  NullEngineOptions options;
  options.renderHeight          = 256;
  options.renderWidth           = 256;
  options.textureSize           = 256;
  options.deterministicLockstep = false;
  options.lockstepMaxSteps      = 1;
  return NullEngine::New(options);
}

} // end of namespace BABYLON

#endif // end of BABYLON_TEST_UTILS_H
