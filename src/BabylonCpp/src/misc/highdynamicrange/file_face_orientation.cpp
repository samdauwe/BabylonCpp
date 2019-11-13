#include <babylon/misc/highdynamicrange/file_face_orientation.h>

namespace BABYLON {

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

FileFaceOrientation::FileFaceOrientation(const FileFaceOrientation& other) = default;
FileFaceOrientation::FileFaceOrientation(FileFaceOrientation&& other) = default;

FileFaceOrientation& FileFaceOrientation::operator=(const FileFaceOrientation& other) = default;
FileFaceOrientation& FileFaceOrientation::operator=(FileFaceOrientation&& other) = default;

FileFaceOrientation::~FileFaceOrientation() = default;

} // end of namespace BABYLON
