#include <babylon/meshes/builders/polygon_builder.h>

#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/polygonmesh/polygon_mesh_builder.h>
#include <babylon/meshes/vertex_data.h>

namespace BABYLON {

MeshPtr PolygonBuilder::CreatePolygon(const std::string& name, PolygonOptions& options,
                                      Scene* scene)
{
  options.sideOrientation    = Mesh::_GetDefaultSideOrientation(options.sideOrientation);
  const auto& shape          = options.shape;
  const auto& holes          = options.holes;
  const auto depth           = options.depth.value_or(0.f);
  const auto updatable       = options.updatable.value_or(false);
  const auto sideOrientation = options.sideOrientation.value_or(VertexData::DEFAULTSIDE);
  std::vector<Vector2> contours(shape.size());
  std::vector<Vector2> hole;

  for (std::size_t i = 0; i < shape.size(); ++i) {
    contours[i] = Vector2(shape[i].x, shape[i].z);
  }
  const auto epsilon = 0.00000001f;
  if (contours[0].equalsWithEpsilon(contours.back(), epsilon)) {
    contours.pop_back();
  }

  PolygonMeshBuilder polygonTriangulation(name, contours, scene);
  for (std::size_t hNb = 0; hNb < holes.size(); ++hNb) {
    hole.clear();
    for (std::size_t hPoint = 0; hPoint < holes[hNb].size(); ++hPoint) {
      hole.emplace_back(Vector2(holes[hNb][hPoint].x, holes[hNb][hPoint].z));
    }
    polygonTriangulation.addHole(hole);
  }
  auto polygon                             = polygonTriangulation.build(updatable, depth);
  polygon->_originalBuilderSideOrientation = sideOrientation;
  auto vertexData
    = VertexData::CreatePolygon(polygon.get(), sideOrientation, options.faceUV, options.faceColors,
                                options.frontUVs, options.backUVs, options.wrap);
  vertexData->applyToMesh(*polygon, options.updatable);

  return polygon;
}

MeshPtr PolygonBuilder::ExtrudePolygon(const std::string& name, PolygonOptions& options,
                                       Scene* scene, const std::optional<bool>& /*wrap*/)
{
  return PolygonBuilder::CreatePolygon(name, options, scene);
}

} // end of namespace BABYLON
