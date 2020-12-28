#ifndef BABYLON_MISC_MESH_EXPLODER_H
#define BABYLON_MISC_MESH_EXPLODER_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/maths/vector3.h>

namespace BABYLON {

FWD_CLASS_SPTR(Mesh)

/**
 * @brief Class used to explode meshes (ie. to have a center and move them away from that center to
 * better see the overall organization)
 */
class BABYLON_SHARED_EXPORT MeshExploder {

public:
  /**
   * @brief Explodes meshes from a center mesh.
   * @param meshes The meshes to explode.
   * @param centerMesh The mesh to be center of explosion.
   */
  MeshExploder(const std::vector<MeshPtr>& meshes, const MeshPtr& _centerMesh = nullptr);
  ~MeshExploder(); // = default

  /**
   * @brief Get class name.
   * @returns "MeshExploder"
   */
  [[nodiscard]] std::string getClassName() const;

  /**
   * @brief "Exploded meshes"
   * @returns Array of meshes with the centerMesh at index 0.
   */
  [[nodiscard]] std::vector<MeshPtr> getMeshes() const;

  /**
   * @brief Explodes meshes giving a specific direction.
   * @param direction Number to multiply distance of each mesh's origin from center. Use a negative
   * number to implode, or zero to reset.
   */
  void explode(float direction = 1.f);

private:
  void _setCenterMesh();

private:
  MeshPtr _centerMesh;
  std::vector<MeshPtr> _meshes;
  std::vector<Vector3> _meshesOrigins;
  std::vector<Vector3> _toCenterVectors;
  Vector3 _scaledDirection;
  Vector3 _newPosition;
  Vector3 _centerPosition;

}; // end of class MeshExploder

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_MESH_EXPLODER_H
