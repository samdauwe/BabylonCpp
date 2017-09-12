#include <babylon/tools/hdr/file_face_orientation.h>

namespace BABYLON {
namespace Internals {

FileFaceOrientation::FileFaceOrientation(const std::string& iName,
                                         const Vector3& _worldAxisForNormal,
                                         const Vector3& _worldAxisForFileX,
                                         const Vector3& _worldAxisForFileY)
    : name{iName}
    , worldAxisForNormal{_worldAxisForNormal}
    , worldAxisForFileX{_worldAxisForFileX}
    , worldAxisForFileY{_worldAxisForFileY}
{
}

FileFaceOrientation::FileFaceOrientation(const FileFaceOrientation& other)
    : name{other.name}
    , worldAxisForNormal{other.worldAxisForNormal}
    , worldAxisForFileX{other.worldAxisForFileX}
    , worldAxisForFileY{other.worldAxisForFileY}
{
}

FileFaceOrientation::FileFaceOrientation(FileFaceOrientation&& other)
    : name{other.name}
    , worldAxisForNormal{::std::move(other.worldAxisForNormal)}
    , worldAxisForFileX{::std::move(other.worldAxisForFileX)}
    , worldAxisForFileY{::std::move(other.worldAxisForFileY)}
{
}

FileFaceOrientation& FileFaceOrientation::
operator=(const FileFaceOrientation& other)
{
  if (&other != this) {
    name               = other.name;
    worldAxisForNormal = other.worldAxisForNormal;
    worldAxisForFileX  = other.worldAxisForFileX;
    worldAxisForFileY  = other.worldAxisForFileY;
  }

  return *this;
}

FileFaceOrientation& FileFaceOrientation::operator=(FileFaceOrientation&& other)
{
  if (&other != this) {
    name               = ::std::move(other.name);
    worldAxisForNormal = ::std::move(other.worldAxisForNormal);
    worldAxisForFileX  = ::std::move(other.worldAxisForFileX);
    worldAxisForFileY  = ::std::move(other.worldAxisForFileY);
  }

  return *this;
}

FileFaceOrientation::~FileFaceOrientation()
{
}

} // end of namespace Internals
} // end of namespace BABYLON
