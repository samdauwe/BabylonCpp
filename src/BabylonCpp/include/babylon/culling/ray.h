#ifndef BABYLON_CULLING_RAY_H
#define BABYLON_CULLING_RAY_H

#include <babylon/babylon_api.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class AbstractMesh;
class BoundingBox;
class BoundingSphere;
class IntersectionInfo;
class PickingInfo;

/**
 * @brief Represents a Ray.
 */
class BABYLON_SHARED_EXPORT Ray {

public:
  static const float smallnum;
  static const float rayl;

public:
  Ray();
  Ray(const Vector3& origin, const Vector3& direction,
      float length = std::numeric_limits<float>::max());
  Ray(const Ray& otherRay);
  Ray(Ray&& otherRay);
  Ray& operator=(const Ray& otherRay);
  Ray& operator=(Ray&& otherRay);
  ~Ray();
  std::unique_ptr<Ray> clone() const;
  friend std::ostream& operator<<(std::ostream& os, const Ray& ray);

  /** Methods **/
  bool intersectsBoxMinMax(const Vector3& minimum,
                           const Vector3& maximum) const;
  bool intersectsBox(const BoundingBox& box) const;
  bool intersectsSphere(const BoundingSphere& sphere) const;
  std::optional<IntersectionInfo> intersectsTriangle(const Vector3& vertex0,
                                                     const Vector3& vertex1,
                                                     const Vector3& vertex2);
  std::optional<float> intersectsPlane(const Plane& plane);
  PickingInfo intersectsMesh(AbstractMesh* mesh, bool fastCheck = false);
  std::vector<PickingInfo> intersectsMeshes(std::vector<AbstractMesh*>& meshes,
                                            bool fastCheck,
                                            std::vector<PickingInfo>& results);

  /**
   * Intersection test between the ray and a given segment whithin a given
   * tolerance (threshold)
   * @param sega the first point of the segment to test the intersection
   * against
   * @param segb the second point of the segment to test the intersection
   * against
   * @param threshold the tolerance margin, if the ray doesn't intersect the
   * segment but is close to the given threshold, the intersection is
   * successful
   * @return the distance from the ray origin to the intersection point if
   * there's intersection, or -1 if there's no intersection
   */
  float intersectionSegment(const Vector3& sega, const Vector3& segb,
                            float threshold) const;

  Ray& update(float x, float y, float viewportWidth, float viewportHeight,
              Matrix& world, Matrix& view, Matrix& projection);

  /** Statics **/
  static Ray Zero();
  static Ray CreateNew(float x, float y, float viewportWidth,
                       float viewportHeight, Matrix& world, Matrix& view,
                       Matrix& projection);

  /**
   * Function will create a new transformed ray starting from origin and ending
   * at the end point. Ray's length will be set, and ray will be
   * transformed to the given world matrix.
   * @param origin The origin point
   * @param end The end point
   * @param world a matrix to transform the ray to. Default is the identity
   * matrix.
   */
  static Ray CreateNewFromTo(const Vector3& origin, const Vector3& end,
                             const Matrix& world = Matrix::Identity());

  static Ray Transform(const Ray& ray, const Matrix& matrix);

  static void TransformToRef(const Ray& ray, const Matrix& matrix, Ray& result);

private:
  static int _comparePickingInfo(const PickingInfo& pickingInfoA,
                                 const PickingInfo& pickingInfoB);

public:
  Vector3 origin;
  Vector3 direction;
  float length;

private:
  bool _vectorsSet;
  Vector3 _edge1;
  Vector3 _edge2;
  Vector3 _pvec;
  Vector3 _tvec;
  Vector3 _qvec;

  std::unique_ptr<Ray> _tmpRay;

}; // end of class Ray

std::ostream& operator<<(std::ostream& os, const Ray& ray);

} // end of namespace BABYLON

#endif // end of BABYLON_CULLING_RAY_H
