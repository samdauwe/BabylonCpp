#include <babylon/extensions/recastjs/recastjs_plugin.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/extensions/recastjs/recastjs_crowd.h>
#include <babylon/maths/vector3.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/meshes/vertex_data.h>
#include <babylon/navigation/inav_mesh_parameters.h>

#include "Recast.h"

namespace BABYLON {
namespace Extensions {

RecastJSPlugin::RecastJSPlugin() : INavigationEnginePlugin{}
{
  name = "RecastJSPlugin";
}

RecastJSPlugin::~RecastJSPlugin()
{
  dispose();
}

void RecastJSPlugin::createMavMesh(const std::vector<MeshPtr>& meshes,
                                   const INavMeshParameters& parameters)
{
  rcConfig rc;
  rc.cs                     = parameters.cs;
  rc.ch                     = parameters.ch;
  rc.borderSize             = 0;
  rc.tileSize               = 0;
  rc.walkableSlopeAngle     = parameters.walkableSlopeAngle;
  rc.walkableHeight         = parameters.walkableHeight;
  rc.walkableClimb          = parameters.walkableClimb;
  rc.walkableRadius         = parameters.walkableRadius;
  rc.maxEdgeLen             = parameters.maxEdgeLen;
  rc.maxSimplificationError = parameters.maxSimplificationError;
  rc.minRegionArea          = parameters.minRegionArea;
  rc.mergeRegionArea        = parameters.mergeRegionArea;
  rc.maxVertsPerPoly        = parameters.maxVertsPerPoly;
  rc.detailSampleDist       = parameters.detailSampleDist;
  rc.detailSampleMaxError   = parameters.detailSampleMaxError;

  navMesh = std::make_unique<NavMesh>();

  Int32Array indices;
  Float32Array positions;
  int offset = 0;
  for (const auto& mesh : meshes) {
    if (mesh) {
      const auto meshIndices = mesh->getIndices();
      if (meshIndices.empty()) {
        continue;
      }
      auto meshPositions = mesh->getVerticesData(VertexBuffer::PositionKind, false, false);
      if (meshPositions.empty()) {
        continue;
      }

      const auto wm = mesh->computeWorldMatrix(false);

      for (size_t tri = 0; tri < meshIndices.size(); ++tri) {
        indices.emplace_back(static_cast<int>(meshIndices[tri]) + offset);
      }

      auto transformed = Vector3::Zero();
      auto position    = Vector3::Zero();
      for (unsigned int pt = 0; pt < meshPositions.size(); pt += 3) {
        Vector3::FromArrayToRef(meshPositions, pt, position);
        Vector3::TransformCoordinatesToRef(position, wm, transformed);
        stl_util::concat(positions, {transformed.x, transformed.y, transformed.z});
      }

      offset += meshPositions.size() / 3;
    }
  }

  navMesh->build(positions.data(), offset, indices.data(), static_cast<int>(indices.size()), rc);
}

MeshPtr RecastJSPlugin::createDebugNavMesh(Scene* scene) const
{
  auto debugNavMesh        = navMesh->getDebugNavMesh();
  const auto triangleCount = static_cast<uint32_t>(debugNavMesh.getTriangleCount());

  IndicesArray indices;
  Float32Array positions;
  for (uint32_t tri = 0; tri < triangleCount * 3u; tri++) {
    indices.emplace_back(tri);
  }
  for (uint32_t tri = 0; tri < triangleCount; ++tri) {
    for (int pt = 0; pt < 3; pt++) {
      auto triangle    = debugNavMesh.getTriangle(static_cast<int>(tri));
      const auto point = triangle.getPoint(pt);
      stl_util::concat(positions, {point.x, point.y, point.z});
    }
  }

  auto mesh       = Mesh::New("NavMeshDebug", scene);
  auto vertexData = std::make_unique<VertexData>();

  vertexData->indices   = std::move(indices);
  vertexData->positions = std::move(positions);
  vertexData->applyToMesh(*mesh, false);
  return mesh;
}

Vector3 RecastJSPlugin::getClosestPoint(const Vector3& position)
{
  const Vec3 p(position.x, position.y, position.z);
  const auto ret = navMesh->getClosestPoint(p);
  return Vector3(ret.x, ret.y, ret.z);
}

Vector3 RecastJSPlugin::getRandomPointAround(const Vector3& position, float maxRadius)
{
  const Vec3 p(position.x, position.y, position.z);
  const auto ret = navMesh->getRandomPointAround(p, maxRadius);
  return Vector3(ret.x, ret.y, ret.z);
}

Vector3 RecastJSPlugin::moveAlong(const Vector3& position, const Vector3& destination)
{
  const Vec3 p(position.x, position.y, position.z);
  const Vec3 d(destination.x, destination.y, destination.z);
  const auto ret = navMesh->moveAlong(p, d);
  return Vector3(ret.x, ret.y, ret.z);
}

std::vector<Vector3> RecastJSPlugin::computePath(const Vector3& start, const Vector3& end)
{
  const Vec3 startPos(start.x, start.y, start.z);
  const Vec3 endPos(end.x, end.y, end.z);
  auto navPath    = navMesh->computePath(startPos, endPos);
  auto pointCount = navPath.getPointCount();
  std::vector<Vector3> positions;
  for (int pt = 0; pt < pointCount; ++pt) {
    const auto p = navPath.getPoint(pt);
    positions.emplace_back(Vector3(p.x, p.y, p.z));
  }
  return positions;
}

ICrowdPtr RecastJSPlugin::createCrowd(size_t maxAgents, float maxAgentRadius, Scene* scene)
{
  auto crowd = std::make_shared<RecastJSCrowd>(this, maxAgents, maxAgentRadius, scene);
  return std::static_pointer_cast<ICrowd>(crowd);
}

void RecastJSPlugin::setDefaultQueryExtent(const Vector3& extent)
{
  const Vec3 ext(extent.x, extent.y, extent.z);
  navMesh->setDefaultQueryExtent(ext);
}

Vector3 RecastJSPlugin::getDefaultQueryExtent() const
{
  const auto p = navMesh->getDefaultQueryExtent();
  return Vector3(p.x, p.y, p.z);
}

void RecastJSPlugin::dispose()
{
}

bool RecastJSPlugin::isSupported() const
{
  return true;
}

} // end of namespace Extensions
} // end of namespace BABYLON
