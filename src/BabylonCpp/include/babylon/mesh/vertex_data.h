#ifndef BABYLON_MESH_VERTEX_DATA_H
#define BABYLON_MESH_VERTEX_DATA_H

#include <babylon/babylon_global.h>
#include <babylon/math/vector4.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT VertexData {

public:
  VertexData();
  ~VertexData();

  void set(const Float32Array& data, unsigned int kind);

  /**
   * @brief Associates the vertexData to the passed Mesh.
   * Sets it as updatable or not (default `false`).
   * @returns The VertexData.
   */
  VertexData& applyToMesh(Mesh* mesh, bool updatable = false);

  /**
   * @brief Associates the vertexData to the passed Geometry.
   * Sets it as updatable or not (default `false`).
   * @returns The VertexData.
   */
  VertexData& applyToGeometry(Geometry* geometry, bool updatable = false);

  /**
   * @brief Updates the associated mesh.
   * @returns The VertexData.
   */
  VertexData& updateMesh(Mesh* mesh, bool updateExtends = false,
                         bool makeItUnique = false);

  /**
   * @brief Updates the associated geometry.
   * @returns The VertexData.
   */
  VertexData& updateGeometry(Geometry* geometry, bool updateExtends = false,
                             bool makeItUnique = false);

  /**
   * @brief Transforms each position and each normal of the vertexData according
   * to the passed Matrix.
   * @returns The VertexData.
   */
  VertexData& transform(const Matrix& matrix);

  /**
   * @brief Merges the passed VertexData into the current one.
   * @returns The modified VertexData.
   */
  VertexData& merge(VertexData* other);

  /**
   * @brief Serializes the VertexData.
   * @returns The serialized object.
   */
  Json::object serialize() const;

  /** Statics **/

  /**
   * @brief Returns the object VertexData associated to the passed mesh.
   */
  static std::unique_ptr<VertexData>
  ExtractFromMesh(Mesh* mesh, bool copyWhenShared = false,
                  bool forceCopy = false);

  /**
   * @brief Returns the object VertexData associated to the passed geometry.
   */
  static std::unique_ptr<VertexData>
  ExtractFromGeometry(Geometry* geometry, bool copyWhenShared,
                      bool forceCopy = false);

  /**
   * @brief Creates the vertexData of the Ribbon.
   */
  static std::unique_ptr<VertexData> CreateRibbon(RibbonOptions& options);

  /**
   * @brief Creates the VertexData of the Box.
   */
  static std::unique_ptr<VertexData> CreateBox(BoxOptions& options);

  /**
   * @brief Creates the VertexData of the Sphere.
   */
  static std::unique_ptr<VertexData> CreateSphere(SphereOptions& options);

  /**
   * @brief Creates the VertexData of the Cylinder or Cone.
   */
  static std::unique_ptr<VertexData> CreateCylinder(CylinderOptions& options);

  /**
   * @brief Creates the VertexData of the Torus.
   */
  static std::unique_ptr<VertexData> CreateTorus(TorusOptions& options);

  /**
   * @brief Creates the VertexData of the LineSystem.
   */
  static std::unique_ptr<VertexData>
  CreateLineSystem(LineSystemOptions& options);

  /**
   * @brief Create the VertexData of the DashedLines.
   */
  static std::unique_ptr<VertexData>
  CreateDashedLines(DashedLinesOptions& options);

  /**
   * @brief Creates the VertexData of the Ground.
   */
  static std::unique_ptr<VertexData> CreateGround(GroundOptions& options);

  /**
   * @brief Creates the VertexData of the TiledGround.
   */
  static std::unique_ptr<VertexData>
  CreateTiledGround(TiledGroundOptions& options);

  /**
   * @brief Creates the VertexData of the Ground designed from a heightmap.
   */
  static std::unique_ptr<VertexData>
  CreateGroundFromHeightMap(GroundFromHeightMapOptions& options);

  /**
   * @brief Creates the VertexData of the Plane.
   */
  static std::unique_ptr<VertexData> CreatePlane(PlaneOptions& options);

  /**
   * @brief Creates the VertexData of the Disc or regular Polygon.
   */
  static std::unique_ptr<VertexData> CreateDisc(DiscOptions& options);

  /**
   * @brief Re-creates the VertexData of the Polygon for sideOrientation.
   */
  static std::unique_ptr<VertexData>
  CreatePolygon(Mesh* polygon, unsigned int sideOrientation,
                const vector_t<Vector4>& fUV, const vector_t<Color4>& fColors,
                Vector4& frontUVs, Vector4& backUVs);

  /**
   * @brief Creates the VertexData of the IcoSphere.
   */
  static std::unique_ptr<VertexData> CreateIcoSphere(IcoSphereOptions& options);

  /**
   * @brief Creates the VertexData of the Polyhedron.
   */
  static std::unique_ptr<VertexData>
  CreatePolyhedron(PolyhedronOptions& options);

  /**
   * @brief Creates the VertexData of the Torus Knot.
   */
  static std::unique_ptr<VertexData> CreateTorusKnot(TorusKnotOptions& options);

  /** Tools **/

  /**
   * @brief Computes the normals of the vertex data object.
   */
  static void ComputeNormals(const Float32Array& positions,
                             const Uint32Array& indices, Float32Array& normals);

  /**
   * @brief Computes the normals of the vertex data object.
   */
  static void ComputeNormals(const Float32Array& positions,
                             const Uint32Array& indices, Float32Array& normals,
                             FacetParameters& options);

  /**
   * @brief Creates a new VertexData from the imported parameters.
   */
  static void ImportVertexData(const Json::value& parsedVertexData,
                               Geometry* geometry);

  static void
  _ComputeSides(unsigned int sideOrientation, Float32Array& positions,
                Uint32Array& indices, Float32Array& normals, Float32Array& uvs,
                const Vector4& frontUVs = Vector4(0.f, 0.f, 1.f, 1.f),
                const Vector4& backUVs  = Vector4(0.f, 0.f, 1.f, 1.f));

private:
  VertexData& _applyTo(IGetSetVerticesData* meshOrGeometry,
                       bool updatable = false);
  VertexData& _update(IGetSetVerticesData* meshOrGeometry,
                      bool updateExtends = false, bool makeItUnique = false);
  static std::unique_ptr<VertexData>
  _ExtractFrom(IGetSetVerticesData* meshOrGeometry, bool copyWhenShared = false,
               bool forceCopy = false);

public:
  Float32Array positions;
  Float32Array normals;
  Float32Array tangents;
  Float32Array uvs;
  Float32Array uvs2;
  Float32Array uvs3;
  Float32Array uvs4;
  Float32Array uvs5;
  Float32Array uvs6;
  Float32Array colors;
  Float32Array matricesIndices;
  Float32Array matricesWeights;
  Float32Array matricesIndicesExtra;
  Float32Array matricesWeightsExtra;
  IndicesArray indices;

public:
  Uint32Array _idx;

}; // end of class VertexData

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_VERTEX_DATA_H
