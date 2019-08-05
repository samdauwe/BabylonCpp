#include <babylon/actions/abstract_action_manager.h>

namespace BABYLON {

AbstractActionManager::AbstractActionManager()
    : hoverCursor{""}
    , isRecursive{false}
    , hasPointerTriggers{this, &AbstractActionManager::get_hasPointerTriggers}
    , hasPickTriggers{this, &AbstractActionManager::get_hasPickTriggers}
{
}

AbstractActionManager::~AbstractActionManager()
{
}

bool AbstractActionManager::HasTriggers()
{
  return false;
}

bool AbstractActionManager::HasPickTriggers()
{
  return false;
}

bool AbstractActionManager::HasSpecificTrigger(unsigned int /*trigger*/)
{
  return false;
}

} // end of namespace BABYLON
