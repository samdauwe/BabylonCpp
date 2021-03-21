#include <babylon/engines/engine_factory.h>

#include <babylon/engines/engine.h>
#include <babylon/engines/null_engine.h>

namespace BABYLON {

std::unique_ptr<ThinEngine> EngineFactory::CreateSync(ICanvas* canvas,
                                                      const EngineCreateOptions& iOptions)
{
  if (Engine::IsSupported()) {
    return Engine::New(canvas,
                       (iOptions.has_value() && std::holds_alternative<EngineOptions>(*iOptions)) ?
                         std::get<EngineOptions>(*iOptions) :
                         EngineOptions{});
  }
  return NullEngine::New(
    (iOptions.has_value() && std::holds_alternative<NullEngineOptions>(*iOptions)) ?
      std::get<NullEngineOptions>(*iOptions) :
      NullEngineOptions{});
}

} // end of namespace BABYLON
