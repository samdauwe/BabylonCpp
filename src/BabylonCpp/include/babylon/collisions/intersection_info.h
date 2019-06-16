#ifndef BABYLON_COLLISIONS_INTERSECTION_INFO_H
#define BABYLON_COLLISIONS_INTERSECTION_INFO_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

/**
 * @brief Hidden
 */
class BABYLON_SHARED_EXPORT IntersectionInfo {

public:
  IntersectionInfo();
  IntersectionInfo(float distance);
  IntersectionInfo(float bu, float bv, float distance);
  IntersectionInfo(const IntersectionInfo& other);
  IntersectionInfo(IntersectionInfo&& other);
  IntersectionInfo& operator=(const IntersectionInfo& other);
  IntersectionInfo& operator=(IntersectionInfo&& other);
  ~IntersectionInfo();

public:
  std::optional<float> bu;
  std::optional<float> bv;
  float distance;
  size_t faceId;
  size_t subMeshId;

}; // end of class IntersectionInfo

} // end of namespace BABYLON

#endif // end of BABYLON_COLLISIONS_INTERSECTION_INFO_H
