#ifndef BABYLON_SAMPLES_SAMPLES_COMMON_H
#define BABYLON_SAMPLES_SAMPLES_COMMON_H

#include <functional>
#include <memory>

namespace BABYLON {

class ICanvas;
class IRenderableScene;
using IRenderableScenePtr = std::unique_ptr<IRenderableScene>;

namespace Samples {

using FactoryMethod = std::function<IRenderableScenePtr(ICanvas* iCanvas)>;
using Sample        = std::tuple<bool, FactoryMethod>;

class _ISamplesIndex {

public:
  _ISamplesIndex();
  ~_ISamplesIndex();

  /**
   * @brief Returns the list with example.
   * @return the list with example
   */
  const std::unordered_map<std::string, Sample>& samples() const;

protected:
  std::unordered_map<std::string, Sample> _samples;

}; // end of struct _SamplesIndex

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_SAMPLES_COMMON_H
