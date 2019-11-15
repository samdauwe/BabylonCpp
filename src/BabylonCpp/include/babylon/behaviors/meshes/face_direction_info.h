#ifndef BABYLON_BEHAVIORS_MESHES_FACE_DIRECTION_INFO_H
#define BABYLON_BEHAVIORS_MESHES_FACE_DIRECTION_INFO_H

#include <babylon/babylon_api.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

/**
 * @brief Hidden.
 */
struct BABYLON_SHARED_EXPORT FaceDirectionInfo {

  FaceDirectionInfo(const Vector3& direction,
                    const Vector3& rotatedDirection = Vector3(),
                    float diff = 0.f, bool ignore = false);
  FaceDirectionInfo(const FaceDirectionInfo& other);
  FaceDirectionInfo(FaceDirectionInfo&& other);
  FaceDirectionInfo& operator=(const FaceDirectionInfo& other);
  FaceDirectionInfo& operator=(FaceDirectionInfo&& other);
  ~FaceDirectionInfo(); // = default
  FaceDirectionInfo copy() const;
  std::unique_ptr<FaceDirectionInfo> clone() const;
  friend std::ostream& operator<<(std::ostream& os,
                                  const FaceDirectionInfo& arc);
  friend bool operator==(const FaceDirectionInfo& lhs,
                         const FaceDirectionInfo& rhs);
  friend bool operator!=(const FaceDirectionInfo& lhs,
                         const FaceDirectionInfo& rhs);

  Vector3 direction;
  Vector3 rotatedDirection;
  float diff;
  bool ignore;

}; // end of struct FaceDirectionInfo

std::ostream& operator<<(std::ostream& os,
                         const FaceDirectionInfo& faceDirectionInfo);
bool operator==(const FaceDirectionInfo& lhs, const FaceDirectionInfo& rhs);
bool operator!=(const FaceDirectionInfo& lhs, const FaceDirectionInfo& rhs);

} // end of namespace BABYLON

#endif // end of BABYLON_BEHAVIORS_MESHES_FACE_DIRECTION_INFO_H
