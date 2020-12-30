#include <babylon/misc/unique_id_generator.h>

namespace BABYLON {

size_t UniqueIdGenerator::_UniqueIdCounter = 0ull;

size_t UniqueIdGenerator::UniqueId()
{
  const auto result = _UniqueIdCounter;
  ++_UniqueIdCounter;
  return result;
}

} // end of namespace BABYLON
