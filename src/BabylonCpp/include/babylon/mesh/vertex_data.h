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
  void applyToMesh(Mesh* mesh, bool updatable = false);
  void applyToGeometry(Geometry* geometry, bool updatable = false);
  void updateMesh(Mesh* mesh, bool updateExtends = false,
                  bool makeItUnique = false);
  void updateGeometry(Geometry* geometry, bool updateExtends = false,
                      bool makeItUnique = false);
  void transform(const Matrix& matrix);
  void merge(VertexData* other);
  Json::object serialize() const;

  /** Statics **/
  static std::unique_ptr<VertexData>
  ExtractFromMesh(Mesh* mesh, bool copyWhenShared = false);
  static std::unique_ptr<VertexData> ExtractFromGeometry(Geometry* geometry,
                                                         bool copyWhenShared);
  static std::unique_ptr<VertexData> CreateRibbon(RibbonOptions& options);
  static std::unique_ptr<VertexData> CreateBox(BoxOptions& options);
  static std::unique_ptr<VertexData> CreateSphere(SphereOptions& options);
  // Cylinder and cone
  static std::unique_ptr<VertexData> CreateCylinder(CylinderOptions& options);
  static std::unique_ptr<VertexData> CreateTorus(TorusOptions& options);
  static std::unique_ptr<VertexData>
  CreateLineSystem(LineSystemOptions& options);
  static std::unique_ptr<VertexData>
  CreateDashedLines(DashedLinesOptions& options);
  static std::unique_ptr<VertexData> CreateGround(GroundOptions& options);
  static std::unique_ptr<VertexData>
  CreateTiledGround(TiledGroundOptions& options);
  static std::unique_ptr<VertexData>
  CreateGroundFromHeightMap(GroundFromHeightMapOptions& options);
  static std::unique_ptr<VertexData> CreatePlane(PlaneOptions& options);
  static std::unique_ptr<VertexData> CreateDisc(DiscOptions& options);
  static std::unique_ptr<VertexData> CreateIcoSphere(IcoSphereOptions& options);
  static std::unique_ptr<VertexData>
  CreatePolyhedron(PolyhedronOptions& options);
  static std::unique_ptr<VertexData> CreateTorusKnot(TorusKnotOptions& options);

  // Tools
  /**
   * @param {any} - positions (number[] or Float32Array)
   * @param {any} - indices   (number[] or Uint32Array)
   * @param {any} - normals   (number[] or Float32Array)
   */
  static void ComputeNormals(const Float32Array& positions,
                             const Uint32Array& indices, Float32Array& normals);
  static void ImportVertexData(const Json::value& parsedVertexData,
                               Geometry* geometry);

private:
  void _applyTo(IGetSetVerticesData* meshOrGeometry, bool updatable = false);
  void _update(IGetSetVerticesData* meshOrGeometry, bool updateExtends = false,
               bool makeItUnique = false);
  static std::unique_ptr<VertexData>
  _ExtractFrom(IGetSetVerticesData* meshOrGeometry, bool copyWhenShared);
  static void _ComputeSides(unsigned int sideOrientation,
                            Float32Array& positions, Uint32Array& indices,
                            Float32Array& normals, Float32Array& uvs);

public:
  Float32Array positions;
  Float32Array normals;
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
  Uint32Array indices;

public:
  Uint32Array _idx;

}; // end of class VertexData

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_VERTEX_DATA_H
