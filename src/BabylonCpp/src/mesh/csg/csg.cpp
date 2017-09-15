#include <babylon/mesh/csg/csg.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/string.h>
#include <babylon/math/quaternion.h>
#include <babylon/mesh/csg/node.h>
#include <babylon/mesh/csg/polygon.h>
#include <babylon/mesh/csg/vertex.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {

unsigned int CSG::CSG::currentCSGMeshId = 0;

CSG::CSG::CSG()
{
}

CSG::CSG::~CSG()
{
}

CSG::CSG* CSG::CSG::FromMesh(Mesh* mesh)
{
  Vector3 normal;
  Vector2 uv;
  Vector3 position;
  vector_t<Polygon*> polygons;

  Matrix matrix;
  Vector3 meshPosition;
  Vector3 meshRotation;
  Quaternion meshRotationQuaternion;
  Vector3 meshScaling;

  mesh->computeWorldMatrix(true);
  /*matrix       = mesh->getWorldMatrix()->clone();
  meshPosition = mesh->position()->clone();
  meshRotation = mesh->rotation()->clone();
  if (mesh->rotationQuaternion()) {
    meshRotationQuaternion = mesh->rotationQuaternion()->clone();
  }
  meshScaling = mesh->rotation()->clone();*/

  Uint32Array indices    = mesh->getIndices();
  Float32Array positions = mesh->getVerticesData(VertexBuffer::PositionKind);
  Float32Array normals   = mesh->getVerticesData(VertexBuffer::NormalKind);
  Float32Array uvs       = mesh->getVerticesData(VertexBuffer::UVKind);

  unsigned int sm = 0;
  for (auto& subMesh : mesh->subMeshes) {
    for (size_t i  = subMesh->indexStart,
                il = subMesh->indexCount + subMesh->indexStart;
         i < il; i += 3) {
      vector_t<Vertex*> vertices;
      for (unsigned int j = 0; j < 3; ++j) {
        Vector3 sourceNormal(normals[indices[i + j] * 3],
                             normals[indices[i + j] * 3 + 1],
                             normals[indices[i + j] * 3 + 2]);
        Vector2 _uv(uvs[indices[i + j] * 2], uvs[indices[i + j] * 2 + 1]);
        Vector3 sourcePosition(positions[indices[i + j] * 3],
                               positions[indices[i + j] * 3 + 1],
                               positions[indices[i + j] * 3 + 2]);
        position = Vector3::TransformCoordinates(sourcePosition, matrix);
        normal   = Vector3::TransformNormal(sourceNormal, matrix);

        vertices.emplace_back(new Vertex(position, normal, _uv));
      }

      PolygonOptions shared;
      shared.subMeshId     = sm;
      shared.meshId        = currentCSGMeshId;
      shared.materialIndex = subMesh->materialIndex;

      Polygon* polygon = new Polygon(vertices, shared);

      // To handle the case of degenerated triangle
      // polygon.plane == null <=> the polygon does not represent 1 single plane
      // <=> the triangle is degenerated
      if (polygon->hasPlane()) {
        polygons.emplace_back(polygon);
      }
    }
    ++sm;
  }

  CSG* csg = CSG::FromPolygons(polygons);
  /*csg->matrix             = matrix;
  csg->position           = meshPosition;
  csg->rotation           = meshRotation.cloneToNewObject();
  csg->scaling            = meshScaling;
  csg->rotationQuaternion = meshRotationQuaternion.cloneToNewObject();*/
  ++currentCSGMeshId;

  return csg;
}

BABYLON::CSG::CSG*
CSG::CSG::FromPolygons(const vector_t<BABYLON::CSG::Polygon*>& _polygons)
{
  BABYLON::CSG::CSG* csg = new BABYLON::CSG::CSG();
  csg->_polygons         = _polygons;
  return csg;
}

CSG::CSG* CSG::CSG::clone()
{
  CSG* csg = new CSG();
  for (auto& p : _polygons) {
    csg->_polygons.emplace_back(p->cloneToNewObject());
  }
  csg->copyTransformAttributes(this);
  return csg;
}

vector_t<CSG::Polygon*>& CSG::CSG::toPolygons()
{
  return _polygons;
}

CSG::CSG* CSG::CSG::_union(BABYLON::CSG::CSG* csg)
{
  Node* a = new Node(clone()->_polygons);
  Node* b = new Node(csg->clone()->_polygons);
  a->clipTo(b);
  b->clipTo(a);
  b->invert();
  b->clipTo(a);
  b->invert();
  vector_t<BABYLON::CSG::Polygon*> allPolygonsB = b->allPolygons();
  a->build(allPolygonsB);
  return CSG::FromPolygons(a->allPolygons())->copyTransformAttributes(this);
}

void CSG::CSG::unionInPlace(BABYLON::CSG::CSG* csg)
{
  Node* a = new Node(_polygons);
  Node* b = new Node(csg->_polygons);

  a->clipTo(b);
  b->clipTo(a);
  b->invert();
  b->clipTo(a);
  b->invert();
  vector_t<BABYLON::CSG::Polygon*> allPolygonsB = b->allPolygons();
  a->build(allPolygonsB);

  _polygons = a->allPolygons();
}

CSG::CSG* CSG::CSG::subtract(BABYLON::CSG::CSG* csg)
{
  Node* a = new Node(clone()->_polygons);
  Node* b = new Node(csg->clone()->_polygons);
  a->invert();
  a->clipTo(b);
  b->clipTo(a);
  b->invert();
  b->clipTo(a);
  b->invert();
  vector_t<BABYLON::CSG::Polygon*> allPolygonsB = b->allPolygons();
  a->build(allPolygonsB);
  a->invert();
  return CSG::FromPolygons(a->allPolygons())->copyTransformAttributes(this);
}

void CSG::CSG::subtractInPlace(BABYLON::CSG::CSG* csg)
{
  Node* a = new Node(_polygons);
  Node* b = new Node(csg->_polygons);

  a->invert();
  a->clipTo(b);
  b->clipTo(a);
  b->invert();
  b->clipTo(a);
  b->invert();
  vector_t<BABYLON::CSG::Polygon*> allPolygonsB = b->allPolygons();
  a->build(allPolygonsB);
  a->invert();

  _polygons = a->allPolygons();
}

CSG::CSG* CSG::CSG::intersect(BABYLON::CSG::CSG* csg)
{
  Node* a = new Node(clone()->_polygons);
  Node* b = new Node(csg->clone()->_polygons);
  a->invert();
  b->clipTo(a);
  b->invert();
  a->clipTo(b);
  b->clipTo(a);
  vector_t<BABYLON::CSG::Polygon*> allPolygonsB = b->allPolygons();
  a->build(allPolygonsB);
  a->invert();
  return CSG::FromPolygons(a->allPolygons())->copyTransformAttributes(this);
}

void CSG::CSG::intersectInPlace(BABYLON::CSG::CSG* csg)
{
  Node* a = new Node(_polygons);
  Node* b = new Node(csg->_polygons);

  a->invert();
  b->clipTo(a);
  b->invert();
  a->clipTo(b);
  b->clipTo(a);
  vector_t<BABYLON::CSG::Polygon*> allPolygonsB = b->allPolygons();
  a->build(allPolygonsB);
  a->invert();

  _polygons = a->allPolygons();
}

CSG::CSG* CSG::CSG::inverse()
{
  BABYLON::CSG::CSG* csg = clone();
  csg->inverseInPlace();
  return csg;
}

void CSG::CSG::inverseInPlace()
{
  for (auto& p : _polygons) {
    p->flip();
  }
}

CSG::CSG* CSG::CSG::copyTransformAttributes(BABYLON::CSG::CSG* csg)
{
  matrix             = csg->matrix;
  position           = csg->position;
  rotation           = csg->rotation;
  scaling            = csg->scaling;
  rotationQuaternion = csg->rotationQuaternion;

  return this;
}

Mesh* CSG::CSG::buildMeshGeometry(const string_t& name, Scene* scene,
                                  bool keepSubMeshes)
{
  Matrix _matrix = matrix;
  _matrix.invert();

  using SubMeshObj = array_t<unsigned int, 3>;

  auto mesh = Mesh::New(name, scene);
  Float32Array vertices;
  Uint32Array indices;
  Float32Array normals;
  Float32Array uvs;
  Vector3 vertex              = Vector3::Zero();
  Vector3 normal              = Vector3::Zero();
  Vector2 uv                  = Vector2::Zero();
  vector_t<Polygon*> polygons = _polygons;
  array_t<unsigned int, 3> polygonIndices{{0, 0, 0}};
  unordered_map_t<string_t, size_t> vertice_dict;
  bool vertexIdxDefined     = false;
  size_t vertex_idx         = 0;
  unsigned int currentIndex = 0;
  unordered_map_t<unsigned int, unordered_map_t<unsigned int, SubMeshObj>>
    subMesh_dict;
  SubMeshObj subMesh_obj;

  if (keepSubMeshes) {
    // Sort Polygons, since subMeshes are indices range
    std::sort(polygons.begin(), polygons.end(),
              [](const Polygon* a, const Polygon* b) {
                if (a->shared.meshId == b->shared.meshId) {
                  return a->shared.subMeshId - b->shared.subMeshId;
                }
                else {
                  return a->shared.meshId - b->shared.meshId;
                }
              });
  }

  for (auto& polygon : polygons) {
    // Building SubMeshes
    if (subMesh_dict.find(polygon->shared.meshId) == subMesh_dict.end()) {
      subMesh_dict[polygon->shared.meshId]
        = unordered_map_t<unsigned int, SubMeshObj>();
    }
    if (subMesh_dict[polygon->shared.meshId].find(polygon->shared.subMeshId)
        == subMesh_dict[polygon->shared.meshId].end()) {
      // IndexStart
      subMesh_dict[polygon->shared.meshId][polygon->shared.subMeshId][0]
        = std::numeric_limits<unsigned int>::max();
      // IndexEnd
      subMesh_dict[polygon->shared.meshId][polygon->shared.subMeshId][1]
        = std::numeric_limits<unsigned int>::min();
      // materialIndex
      subMesh_dict[polygon->shared.meshId][polygon->shared.subMeshId][2]
        = polygon->shared.materialIndex;
    }
    subMesh_obj
      = subMesh_dict[polygon->shared.meshId][polygon->shared.subMeshId];

    for (unsigned int j = 2; j < polygon->vertices.size(); ++j) {

      polygonIndices[0] = 0;
      polygonIndices[1] = j - 1;
      polygonIndices[2] = j;

      for (unsigned int k = 0; k < 3; ++k) {
        vertex.copyFrom(polygon->vertices[polygonIndices[k]]->pos);
        normal.copyFrom(polygon->vertices[polygonIndices[k]]->normal);
        uv.copyFrom(polygon->vertices[polygonIndices[k]]->uv);
        Vector3 localVertex = Vector3::TransformCoordinates(vertex, matrix);
        Vector3 localNormal = Vector3::TransformNormal(normal, matrix);

        string_t vertexId = String::concat(localVertex.x, ",", localVertex.y,
                                           ",", localVertex.z);

        if (stl_util::contains(vertice_dict, vertexId)) {
          vertex_idx       = vertice_dict[vertexId];
          vertexIdxDefined = true;
        }

        // Check if 2 points can be merged
        if (!(vertexIdxDefined
              && stl_util::almost_equal(normals[vertex_idx * 3], localNormal.x)
              && stl_util::almost_equal(normals[vertex_idx * 3 + 1],
                                        localNormal.y)
              && stl_util::almost_equal(normals[vertex_idx * 3 + 2],
                                        localNormal.z)
              && stl_util::almost_equal(uvs[vertex_idx * 2], uv.x)
              && stl_util::almost_equal(uvs[vertex_idx * 2 + 1], uv.y))) {
          stl_util::concat(vertices,
                           {localVertex.x, localVertex.y, localVertex.z});
          stl_util::concat(uvs, {uv.x, uv.y});
          stl_util::concat(normals, {normal.x, normal.y, normal.z});
          vertex_idx             = (vertices.size() / 3) - 1;
          vertice_dict[vertexId] = vertex_idx;
          vertexIdxDefined       = true;
        }

        indices.emplace_back(vertex_idx);

        // indexStart
        subMesh_obj[0] = std::min(currentIndex, subMesh_obj[0]);
        subMesh_obj[1] = std::max(currentIndex, subMesh_obj[1]);
        subMesh_dict[polygon->shared.meshId][polygon->shared.subMeshId]
          = subMesh_obj;
        ++currentIndex;
      }
    }
  }

  mesh->setVerticesData(VertexBuffer::PositionKind, vertices);
  mesh->setVerticesData(VertexBuffer::NormalKind, normals);
  mesh->setVerticesData(VertexBuffer::UVKind, uvs);
  mesh->setIndices(indices);

  if (keepSubMeshes) {
    // We offset the materialIndex by the previous number of materials in the
    // CSG mixed meshes
    unsigned int materialIndexOffset = 0;
    int materialMaxIndex;

    mesh->subMeshes.clear();

    for (auto& m : subMesh_dict) {
      materialMaxIndex = -1;
      for (auto& sm : m.second) {
        subMesh_obj = sm.second;
        SubMesh::CreateFromIndices(subMesh_obj[2] + materialIndexOffset,
                                   subMesh_obj[0],
                                   subMesh_obj[1] - subMesh_obj[0] + 1, mesh);
        materialMaxIndex
          = std::max(static_cast<int>(subMesh_obj[2]), materialMaxIndex);
      }
      materialIndexOffset += (++materialMaxIndex);
    }
  }

  return mesh;
}

Mesh* CSG::CSG::toMesh(const string_t& name, Material* material, Scene* scene,
                       bool keepSubMeshes)
{
  Mesh* mesh = buildMeshGeometry(name, scene, keepSubMeshes);

  mesh->setMaterial(material);

  // mesh->position->copyFrom(position);
  mesh->rotation().copyFrom(*rotation);
  if (rotationQuaternion) {
    // mesh->setRotationQuaternion(rotationQuaternion->cloneToNewObject());
  }
  mesh->scaling().copyFrom(scaling);
  mesh->computeWorldMatrix(true);

  return mesh;
}

} // end of namespace BABYLON
