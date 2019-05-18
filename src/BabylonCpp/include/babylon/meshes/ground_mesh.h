#ifndef BABYLON_MESHES_GROUND_MESH_H
#define BABYLON_MESHES_GROUND_MESH_H

#include <babylon/babylon_api.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector2.h>
#include <babylon/math/vector4.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {

struct Quad {
  Vector2 slope;
  Vector4 facet1;
  Vector4 facet2;
}; // end of struct Quad

/**
 * @brief Mesh representing the ground.
 */
class BABYLON_SHARED_EXPORT GroundMesh : public Mesh {

public:
  template <typename... Ts>
  static GroundMeshPtr New(Ts&&... args)
  {
    auto mesh
      = std::shared_ptr<GroundMesh>(new GroundMesh(std::forward<Ts>(args)...));
    mesh->addToScene(mesh);

    return mesh;
  }
  ~GroundMesh() override;

  /**
   * @brief "GroundMesh"
   * @returns "GroundMesh"
   */
  const std::string getClassName() const override;

  Type type() const override;

  /**
   * @brief This function will update an octree to help to select the right
   * submeshes for rendering, picking and collision computations. Please note
   * that you must have a decent number of submeshes to get performance
   * improvements when using an octree
   * @param chunksCount the number of subdivisions for x and y
   * @param octreeBlocksSize (Default: 32)
   */
  void optimize(size_t chunksCount, size_t octreeBlocksSize = 32);

  /**
   * @brief Returns a height (y) value in the Worl system :
   * the ground altitude at the coordinates (x, z) expressed in the World
   * system.
   * @param x x coordinate
   * @param z z coordinate
   * @returns the ground y position if (x, z) are outside the ground surface.
   */
  float getHeightAtCoordinates(float x, float z);

  /**
   * @brief Returns a normalized vector (Vector3) orthogonal to the ground
   * at the ground coordinates (x, z) expressed in the World system.
   * @param x x coordinate
   * @param z z coordinate
   * @returns Vector3(0.0, 1.0, 0.0) if (x, z) are outside the ground surface.
   */
  Vector3 getNormalAtCoordinates(float x, float z);

  /**
   * @brief Updates the Vector3 passed a reference with a normalized vector
   * orthogonal to the ground at the ground coordinates (x, z) expressed in the
   * World system. Doesn't uptade the reference Vector3 if (x, z) are outside
   * the ground surface.
   * @param x x coordinate
   * @param z z coordinate
   * @param ref vector to store the result
   * @returns the GroundMesh.
   */
  GroundMesh& getNormalAtCoordinatesToRef(float x, float z, Vector3& ref);

  /**
   * @brief Force the heights to be recomputed for getHeightAtCoordinates() or
   * getNormalAtCoordinates() if the ground has been updated. This can be used
   * in the render loop.
   * @returns the GroundMesh.
   */
  GroundMesh& updateCoordinateHeights();

  /**
   * @brief Serializes this ground mesh.
   * @param serializationObject object to write serialization to
   */
  json serialize(json& serializationObject) const;

  /**
   * @brief Parses a serialized ground mesh.
   * @param parsedMesh the serialized mesh
   * @param scene the scene to create the ground mesh in
   * @returns the created ground mesh
   */
  static GroundMeshPtr Parse(const json& parsedMesh, Scene* scene);

private:
  size_t get_subdivisions() const;
  size_t get_subdivisionsX() const;
  size_t get_subdivisionsY() const;

  /**
   * @brief Returns the element "facet" from the heightQuads array relative to
   * (x, z) local coordinates
   */
  Vector4 _getFacetAt(float x, float z);

  /**
   * @brief Creates and populates the heightMap array with "facet" elements:
   * a quad is two triangular facets separated by a slope, so a "facet" element
   * is 1 slope + 2 facets
   * slope : Vector2(c, h) = 2D diagonal line equation setting appart two
   * triangular facets in a quad : z = cx + h
   * facet1 : Vector4(a, b, c, d) = first facet 3D plane equation : ax + by + cz
   * + d = 0
   * facet2 :  Vector4(a, b, c, d) = second facet 3D plane equation : ax + by +
   * cz + d = 0
   * facet2 :  Vector4(a, b, c, d) = second facet 3D plane equation : ax + by +
   * cz + d = 0
   * @returns The GroundMesh.
   */
  GroundMesh& _initHeightQuads();

  /**
   * @brief Compute each quad element values and update the the heightMap array:
   * slope : Vector2(c, h) = 2D diagonal line equation setting appart two
   * triangular facets in a quad : z = cx + h
   * facet1 : Vector4(a, b, c, d) = first facet 3D plane equation : ax + by + cz
   * + d = 0
   * facet2 :  Vector4(a, b, c, d) = second facet 3D plane equation : ax + by +
   * cz + d = 0
   * facet2 :  Vector4(a, b, c, d) = second facet 3D plane equation : ax + by +
   * cz + d = 0
   * @returns The GroundMesh.
   */
  GroundMesh& _computeHeightQuads();

protected:
  GroundMesh(const std::string& name, Scene* scene);

public:
  /** If octree should be generated */
  bool generateOctree;
  /** Hidden */
  size_t _subdivisionsX;
  /** Hidden */
  size_t _subdivisionsY;
  /** Hidden */
  float _width;
  /** Hidden */
  float _height;
  /** Hidden */
  float _minX;
  /** Hidden */
  float _maxX;
  /** Hidden */
  float _minZ;
  /** Hidden */
  float _maxZ;

  /**
   * The minimum of x and y subdivisions
   */
  ReadOnlyProperty<GroundMesh, size_t> subdivisions;

  /**
   * X subdivisions
   */
  ReadOnlyProperty<GroundMesh, size_t> subdivisionsX;

  /**
   * Y subdivisions
   */
  ReadOnlyProperty<GroundMesh, size_t> subdivisionsY;

private:
  std::vector<Quad> _heightQuads;

}; // end of class GroundMesh

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_GROUND_MESH_H
