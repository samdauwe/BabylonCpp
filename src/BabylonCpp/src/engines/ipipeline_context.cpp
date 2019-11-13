#include <babylon/engines/ipipeline_context.h>

namespace BABYLON {

IPipelineContext::IPipelineContext()
    : isAsync{this, &IPipelineContext::get_isAsync}
    , isReady{this, &IPipelineContext::get_isReady}
    , _handlesSpectorRebuildCallback{nullptr}
    , _isAsync{false}
    , _isReady{false}
{
}

IPipelineContext::~IPipelineContext() = default;

bool IPipelineContext::get_isAsync() const
{
  return _isAsync;
}

bool IPipelineContext::get_isReady() const
{
  return _isReady;
}

} // end of namespace BABYLON
