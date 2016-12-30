#ifndef BABYLON_TOOLS_HDR_FILE_FACE_ORIENTATION_H
#define BABYLON_TOOLS_HDR_FILE_FACE_ORIENTATION_H

#include <babylon/babylon_global.h>
#include <babylon/math/vector3.h>

namespace BABYLON {
namespace Internals {

struct BABYLON_SHARED_EXPORT FileFaceOrientation {

  FileFaceOrientation(const std::string& name,
                      const Vector3& worldAxisForNormal,
                      const Vector3& worldAxisForFileX,
                      const Vector3& worldAxisForFileY);
  FileFaceOrientation(const FileFaceOrientation& other);
  FileFaceOrientation(FileFaceOrientation&& other);
  FileFaceOrientation& operator=(const FileFaceOrientation& other);
  FileFaceOrientation& operator=(FileFaceOrientation&& other);
  ~FileFaceOrientation();

  std::string name;
  // The world axis corresponding to the normal to the face
  Vector3 worldAxisForNormal;
  // The world axis corresponding to texture right x-axis in file
  Vector3 worldAxisForFileX;
  // The world axis corresponding to texture down y-axis in file
  Vector3 worldAxisForFileY;

}; // end of struct FileFaceOrientation

} // end of namespace Internals
} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_HDR_FILE_FACE_ORIENTATION_H
