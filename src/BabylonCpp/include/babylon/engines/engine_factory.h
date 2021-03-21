#ifndef BABYLON_ENGINES_ENGINE_FACTORY_H
#define BABYLON_ENGINES_ENGINE_FACTORY_H

#include <memory>
#include <optional>
#include <variant>

#include <babylon/babylon_api.h>
#include <babylon/engines/engine_options.h>
#include <babylon/engines/null_engine_options.h>

namespace BABYLON {

class ICanvas;
class ThinEngine;

/**
 * @brief Helper class to create the best engine depending on the current hardware.
 */
struct BABYLON_SHARED_EXPORT EngineFactory {

  using EngineCreateOptions = std::optional<std::variant<EngineOptions, NullEngineOptions>>;

  /**
   * @brief Creates an engine based on the capabilities of the underlying hardware.
   * @param canvas Defines the canvas to use to display the result
   * @param options Defines the options passed to the engine to create the context dependencies
   * @returns a promise that resolves with the created engine
   */
  static std::unique_ptr<ThinEngine> CreateSync(ICanvas* canvas,
                                                const EngineCreateOptions& options = std::nullopt);

}; // end of struct EngineFactory

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_ENGINE_FACTORY_H
