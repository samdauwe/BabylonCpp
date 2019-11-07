#ifndef BABYLON_SAMPLES_SAMPLES_COMMON_H
#define BABYLON_SAMPLES_SAMPLES_COMMON_H

#include <string>
#include <functional>
#include <memory>
#include <unordered_map>

namespace BABYLON {

class ICanvas;
class IRenderableScene;
using IRenderableScenePtr = std::shared_ptr<IRenderableScene>;

namespace Samples {

using SampleFactoryFunction = std::function<IRenderableScenePtr(ICanvas * iCanvas)>;

class _ISamplesIndex {

public:
  _ISamplesIndex() = default;
  ~_ISamplesIndex() = default;

  /**
   * @brief Returns the list with example.
   * @return the list with example
   */
  const std::unordered_map<std::string, SampleFactoryFunction>& samples() const;

  void addSample(const std::string& exampleName, SampleFactoryFunction f) {
    _samples[exampleName] = f;
  }

protected:
  std::unordered_map<std::string, SampleFactoryFunction> _samples;

}; // end of struct _SamplesIndex

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_SAMPLES_COMMON_H
