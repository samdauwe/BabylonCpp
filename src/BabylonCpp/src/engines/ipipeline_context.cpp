#include <babylon/engines/ipipeline_context.h>

namespace BABYLON {

IPipelineContext::IPipelineContext()
    : _handlesSpectorRebuildCallback{nullptr}, _isAsync{false}, _isReady{false}
{
}

IPipelineContext::~IPipelineContext() = default;

bool IPipelineContext::isAsync()
{
  return _isAsync;
}

bool IPipelineContext::isReady()
{
  return _isReady;
}

std::string IPipelineContext::_name() const
{
  return "";
}

} // end of namespace BABYLON
