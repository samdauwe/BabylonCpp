#include <babylon/extensions/hexplanetgeneration/terrain/uuid_obj.h>

namespace BABYLON {
namespace Extensions {

UidObj::UidObj(size_t p_id) : id(p_id)
{
}

UidObj::~UidObj() = default;

bool UidObj::operator==(const UidObj& other) const
{
  return id == other.id;
}

} // end of namespace Extensions
} // end of namespace BABYLON
