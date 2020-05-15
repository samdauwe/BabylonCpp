#ifndef BABYLON_CULLING_RAY_H
#define BABYLON_CULLING_RAY_H

#include <babylon/babylon_api.h>
#include <babylon/maths/matrix.h>
#include <babylon/maths/vector3.h>

namespace BABYLON {

class AbstractMesh;
class BoundingBox;
class BoundingSphere;
class IntersectionInfo;
class PickingInfo;

/**
 * @brief Class representing a ray with position and direction.
 */
class BABYLON_SHARED_EXPORT Ray {

public:
  static const float smallnum;
  static const float rayl;

public:
  Ray();

  /**
   * @brief Creates a new ray.
   * @param origin origin point
   * @param direction direction
   * @param length length of the ray
   */
  Ray(const Vector3& origin, const Vector3& direction,
      float length = (std::numeric_limits<float>::max)());
  Ray(const Ray& otherRay);
  Ray(Ray&& otherRay);
  Ray& operator=(const Ray& otherRay);
  Ray& operator=(Ray&& otherRay);
  ~Ray(); // = default
  [[nodiscard]] std::unique_ptr<Ray> clone() const;
  friend std::ostream& operator<<(std::ostream& os, const Ray& ray);

  /** Methods **/
  /**
   * @brief Checks if the ray intersects a box.
   * This does not account for the ray lenght by design to improve perfs.
   * @param minimum bound of the box
   * @param maximum bound of the box
   * @param intersectionTreshold extra extend to be added to the box in all direction
   * @returns if the box was hit
   */
  [[nodiscard]] bool intersectsBoxMinMax(const Vector3& minimum, const Vector3& maximum,
                                         float intersectionTreshold = 0.f) const;

  /**
   * @brief Checks if the ray intersects a box.
   * This does not account for the ray length by design to improve perfs.
   * @param box the bounding box to check
   * @param intersectionTreshold extra extend to be added to the BoundingBox in all direction
   * @returns if the box was hit
   */
  [[nodiscard]] bool intersectsBox(const BoundingBox& box, float intersectionTreshold = 0.f) const;

  /**
   * @brief If the ray hits a sphere.
   * @param sphere the bounding sphere to check
   * @returns true if it hits the sphere
   */
  [[nodiscard]] bool intersectsSphere(const BoundingSphere& sphere,
                                      float intersectionTreshold = 0.f) const;

  /**
   * @brief If the ray hits a triange.
   * @param vertex0 triangle vertex
   * @param vertex1 triangle vertex
   * @param vertex2 triangle vertex
   * @returns intersection information if hit
   */
  std::optional<IntersectionInfo> intersectsTriangle(const Vector3& vertex0, const Vector3& vertex1,
                                                     const Vector3& vertex2);

  /**
   * @brief Checks if ray intersects a plane.
   * @param plane the plane to check
   * @returns the distance away it was hit
   */
  std::optional<float> intersectsPlane(const Plane& plane);

  /**
   * @brief Calculate the intercept of a ray on a given axis.
   * @param axis to check 'x' | 'y' | 'z'
   * @param offset from axis interception (i.e. an offset of 1y is intercepted
   * above ground)
   * @returns a vector containing the coordinates where 'axis' is equal to zero
   * (else offset), or null if there is no intercept.
   */
  std::optional<Vector3> intersectsAxis(const std::string& axis, float offset = 0.f);

  /**
   * @brief Checks if ray intersects a mesh.
   * @param mesh the mesh to check
   * @param fastCheck defines if the first intersection will be used (and not the closest)
   * @returns picking info of the intersecton
   */
  PickingInfo intersectsMesh(AbstractMesh* mesh,
                             const std::optional<bool>& fastCheck = std::nullopt);

  /**
   * @brief Checks if ray intersects a mesh.
   * @param meshes the meshes to check
   * @param fastCheck defines if the first intersection will be used (and not the closest)
   * @returns Array of picking infos
   */
  std::vector<PickingInfo> intersectsMeshes(std::vector<AbstractMesh*>& meshes,
                                            const std::optional<bool>& fastCheck = std::nullopt);

  /**
   * @brief Checks if ray intersects a mesh.
   * @param meshes the meshes to check
   * @param fastCheck defines if the first intersection will be used (and not the closest)
   * @param results array to store result in
   * @returns Array of picking infos
   */
  std::vector<PickingInfo> intersectsMeshes(std::vector<AbstractMesh*>& meshes,
                                            const std::optional<bool>& fastCheck,
                                            std::vector<PickingInfo>& results);

  /**
   * @brief Intersection test between the ray and a given segment whithin a
   * given tolerance (threshold).
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
  [[nodiscard]] float intersectionSegment(const Vector3& sega, const Vector3& segb,
                                          float threshold) const;

  /**
   * @brief Update the ray from viewport position.
   * @param x position
   * @param y y position
   * @param viewportWidth viewport width
   * @param viewportHeight viewport height
   * @param world world matrix
   * @param view view matrix
   * @param projection projection matrix
   * @returns this ray updated
   */
  Ray& update(float x, float y, float viewportWidth, float viewportHeight, Matrix& world,
              Matrix& view, Matrix& projection);

  /**
   * @brief Unproject a ray from screen space to object space.
   * @param sourceX defines the screen space x coordinate to use
   * @param sourceY defines the screen space y coordinate to use
   * @param viewportWidth defines the current width of the viewport
   * @param viewportHeight defines the current height of the viewport
   * @param world defines the world matrix to use (can be set to Identity to go
   * to world space)
   * @param view defines the view matrix to use
   * @param projection defines the projection matrix to use
   */
  void unprojectRayToRef(float sourceX, float sourceY, float viewportWidth, float viewportHeight,
                         Matrix& world, const Matrix& view, const Matrix& projection);

  /** Statics **/
  /**
   * @brief Creates a ray with origin and direction of 0,0,0.
   * @returns the new ray
   */
  static Ray Zero();

  /**
   * @brief Creates a new ray from screen space and viewport.
   * @param x position
   * @param y y position
   * @param viewportWidth viewport width
   * @param viewportHeight viewport height
   * @param world world matrix
   * @param view view matrix
   * @param projection projection matrix
   * @returns new ray
   */
  static Ray CreateNew(float x, float y, float viewportWidth, float viewportHeight, Matrix& world,
                       Matrix& view, Matrix& projection);

  /**
   * @brief Function will create a new transformed ray starting from origin and
   * ending at the end point. Ray's length will be set, and ray will be
   * transformed to the given world matrix.
   * @param origin The origin point
   * @param end The end point
   * @param world a matrix to transform the ray to. Default is the identity
   * matrix.
   * @returns the new ray
   */
  static Ray CreateNewFromTo(const Vector3& origin, const Vector3& end,
                             const Matrix& world = Matrix::Identity());

  /**
   * @brief Transforms a ray by a matrix.
   * @param ray ray to transform
   * @param matrix matrix to apply
   * @returns the resulting new ray
   */
  static Ray Transform(const Ray& ray, const Matrix& matrix);

  /**
   * @brief Transforms a ray by a matrix.
   * @param ray ray to transform
   * @param matrix matrix to apply
   * @param result ray to store result in
   */
  static void TransformToRef(const Ray& ray, const Matrix& matrix, Ray& result);

private:
  static int _comparePickingInfo(const PickingInfo& pickingInfoA, const PickingInfo& pickingInfoB);

public:
  /**
   * origin point
   */
  Vector3 origin;

  /**
   * direction
   */
  Vector3 direction;

  /**
   * length of the ray
   */
  float length;

private:
  static std::array<Vector3, 6> TmpVector3;
  std::unique_ptr<Ray> _tmpRay;

}; // end of class Ray

std::ostream& operator<<(std::ostream& os, const Ray& ray);

} // end of namespace BABYLON

#endif // end of BABYLON_CULLING_RAY_H
