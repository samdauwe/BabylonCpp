#ifndef BABYLON_MESH_GROUND_MESH_H
#define BABYLON_MESH_GROUND_MESH_H

#include <babylon/babylon_global.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector2.h>
#include <babylon/math/vector4.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {

struct Quad {
  Vector2 slope;
  Vector4 facet1;
  Vector4 facet2;
}; // end of struct Quad

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT GroundMesh : public Mesh {

public:
  template <typename... Ts>
  static GroundMesh* New(Ts&&... args)
  {
    auto mesh = new GroundMesh(::std::forward<Ts>(args)...);
    mesh->addToScene(static_cast<unique_ptr_t<AbstractMesh>>(mesh));

    return mesh;
  }
  ~GroundMesh();

  /**
   * @brief Returns the string "GroundMesh"
   */
  const char* getClassName() const override;

  IReflect::Type type() const override;

  size_t subdivisions() const;
  size_t subdivisionsX() const;
  size_t subdivisionsY() const;
  void optimize(size_t chunksCount, size_t octreeBlocksSize = 32);

  /**
   * @brief Returns a height (y) value in the Worl system :
   * the ground altitude at the coordinates (x, z) expressed in the World
   * system.
   * Returns the ground y position if (x, z) are outside the ground
   * surface.
   */
  float getHeightAtCoordinates(float x, float z);

  /**
   * @brief Returns a normalized vector (Vector3) orthogonal to the ground
   * at the ground coordinates (x, z) expressed in the World system.
   * Returns Vector3(0, 1, 0) if (x, z) are outside the ground surface.
   * Not pertinent if the ground is rotated.
   */
  Vector3 getNormalAtCoordinates(float x, float z);

  /**
   * @brief Updates the Vector3 passed a reference with a normalized vector
   * orthogonal to the ground
   * at the ground coordinates (x, z) expressed in the World system.
   * Doesn't uptade the reference Vector3 if (x, z) are outside the ground
   * surface.
   * @returns The GroundMesh.
   */
  GroundMesh& getNormalAtCoordinatesToRef(float x, float z, Vector3& ref);

  /**
   * @brief Force the heights to be recomputed for getHeightAtCoordinates() or
   * getNormalAtCoordinates()
   * if the ground has been updated.
   * This can be used in the render loop
   * @returns The GroundMesh.
   */
  GroundMesh& updateCoordinateHeights();

  Json::object serialize(Json::object& serializationObject) const;

  static GroundMesh* Parse(const Json::value& parsedMesh, Scene* scene);

private:
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
  GroundMesh(const string_t& name, Scene* scene);

public:
  bool generateOctree;
  size_t _subdivisionsX;
  size_t _subdivisionsY;
  float _width;
  float _height;
  float _minX;
  float _maxX;
  float _minZ;
  float _maxZ;

private:
  Matrix _worldInverse;
  vector_t<Quad> _heightQuads;

}; // end of class GroundMesh

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_GROUND_MESH_H
