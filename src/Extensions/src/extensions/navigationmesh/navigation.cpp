#include <babylon/extensions/navigationmesh/navigation.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/extensions/navigationmesh/channel.h>
#include <babylon/math/plane.h>
#include <babylon/math/vector3.h>
#include <babylon/mesh/geometry.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/vertex_buffer.h>

#include <babylon/extensions/pathfinding/a_star_search.h>

namespace BABYLON {
namespace Extensions {

Navigation::Navigation()
{
}

Navigation::~Navigation()
{
}

GroupedNavigationMesh Navigation::buildNodes(Mesh* mesh)
{
  auto navigationMesh = _buildNavigationMesh(mesh->geometry());
  return _groupNavMesh(navigationMesh);
}

void Navigation::setZoneData(const std::string& zone,
                             const GroupedNavigationMesh& data)
{
  _zoneNodes[zone] = data;
}

int Navigation::getGroup(const std::string& zone, const Vector3& position)
{
  int closestNodeGroup = -1;
  if (!stl_util::contains(_zoneNodes, zone)) {
    return closestNodeGroup;
  }

  float measuredDistance = 0.f;
  float distance         = std::numeric_limits<float>::infinity();
  for (size_t index = 0; index < _zoneNodes[zone].groups.size(); ++index) {
    for (auto& node : _zoneNodes[zone].groups[index]) {
      measuredDistance = Vector3::DistanceSquared(node.centroid, position);
      if (measuredDistance < distance) {
        closestNodeGroup = static_cast<int>(index);
        distance         = measuredDistance;
      }
    }
  }
  return closestNodeGroup;
}

std::vector<Vector3> Navigation::findPath(const Vector3& startPosition,
                                          const Vector3& targetPosition,
                                          const std::string& zone,
                                          std::size_t group)
{
  auto& allNodes = _zoneNodes[zone].groups[group];
  auto& vertices = _zoneNodes[zone].vertices;

  const auto getNodeIndex = [&allNodes](const Vector3& position) {
    int nodeIndex          = -1;
    float distance         = std::numeric_limits<float>::infinity();
    int index              = 0;
    float measuredDistance = 0.f;
    for (auto& node : allNodes) {
      measuredDistance = Vector3::DistanceSquared(node.centroid, position);
      if (measuredDistance < distance) {
        nodeIndex = index;
        distance  = measuredDistance;
      }
      ++index;
    }
    return nodeIndex;
  };

  // If we can't find any node, just go straight to the target
  auto closestNodeIndex  = getNodeIndex(startPosition);
  auto farthestNodeIndex = getNodeIndex(targetPosition);
  if ((closestNodeIndex == -1) || (farthestNodeIndex == -1)) {
    return std::vector<Vector3>();
  }

  auto& closestNode  = allNodes[static_cast<std::size_t>(closestNodeIndex)];
  auto& farthestNode = allNodes[static_cast<std::size_t>(farthestNodeIndex)];
  auto pathIds       = AStarSearch(allNodes, closestNode, farthestNode);

  const auto getPortalFromTo
    = [](const NavigationGroup& a, const NavigationGroup& b) -> Uint32Array {
    for (size_t i = 0; i < a.neighbours.size(); ++i) {
      if (a.neighbours[i] == b.id) {
        return a.portals[i];
      }
    }
    return Uint32Array();
  };

  // We got the corridor
  // Now pull the rope
  Channel channel;
  channel.push(startPosition);
  for (size_t i = 0; i < pathIds.size() - 1; ++i) {
    const auto& polygon     = allNodes[pathIds[i]];
    const auto& nextPolygon = allNodes[pathIds[i + 1]];
    auto portals            = getPortalFromTo(polygon, nextPolygon);
    if (!portals.empty()) {
      channel.push(getVectorFrom(vertices, portals[0]),
                   getVectorFrom(vertices, portals[1]));
    }
  }
  channel.push(targetPosition);
  channel.stringPull();

  std::vector<Vector3> vectors;
  // We don't need the first one, as we already know our start position
  std::copy(channel.path.begin() + 1, channel.path.end(),
            std::back_inserter(vectors));

  return vectors;
}

bool Navigation::_isPointInPoly(const std::vector<Vector3>& poly,
                                const Vector3& pt) const
{
  bool c = false;
  for (std::size_t i = 0, l = poly.size(), j = l - 1; i++ < l; j = i) {
    ((poly[i].z <= pt.z && pt.z < poly[j].z)
     || (poly[j].z <= pt.z && pt.z < poly[i].z))
      && pt.x < (poly[j].x - poly[i].x) * (pt.z - poly[i].z)
                    / (poly[j].z - poly[i].z)
                  + poly[i].x
      && (c = !c);
  }
  return c;
}

bool Navigation::_isVectorInPolygon(const Vector3& vector,
                                    const NavigationGroup& polygon,
                                    const Float32Array& vertices)
{
  // Reference point will be the centroid of the polygon. We need to rotate the
  // vector as well as all the points which the polygon uses.
  float lowestPoint  = 100000.f;
  float highestPoint = -100000.f;

  std::vector<Vector3> polygonVertices;

  for (auto& vId : polygon.vertexIds) {
    auto point   = getVectorFrom(vertices, vId);
    lowestPoint  = std::min(point.y, lowestPoint);
    highestPoint = std::max(point.y, highestPoint);
    polygonVertices.emplace_back(point);
  }

  if (vector.y < highestPoint + 0.5f && vector.y > lowestPoint - 0.5f
      && _isPointInPoly(polygonVertices, vector)) {
    return true;
  }

  return false;
}

void Navigation::_computeCentroids(Geometry* geometry)
{
  size_t f, fl;
  std::vector<Vector3> centroids;
  auto indices  = geometry->getIndices();
  auto vertices = geometry->getVerticesData(VertexBuffer::PositionKind);
  auto c        = Vector3::Zero();

  for (f = 0, fl = indices.size(); f < fl; f += 3) {
    auto p1 = getVectorFrom(vertices, indices[f]);
    auto p2 = getVectorFrom(vertices, indices[f + 1]);
    auto p3 = getVectorFrom(vertices, indices[f + 2]);

    c.copyFromFloats(0, 0, 0);

    c.addInPlace(p1);
    c.addInPlace(p2);
    c.addInPlace(p3);

    c.scaleInPlace(1.f / 3.f);

    centroids.emplace_back(c.copy());
  }
  geometry->centroids = centroids;
}

float Navigation::_roundNumber(float number, unsigned int decimals)
{
  float f = std::pow(10.f, static_cast<float>(decimals));
  return std::floor(number * f + 0.5f) / f;
}

void Navigation::_setPolygonCentroid(NavigationPolygon& polygon,
                                     const NavigationMesh& navigationMesh)
{
  Vector3 sum;

  const auto& vertices = navigationMesh.vertices;

  for (auto& vId : polygon.vertexIds) {
    sum.x += vertices[vId * 3];
    sum.y += vertices[vId * 3 + 1];
    sum.z += vertices[vId * 3 + 2];
  };

  sum.scaleInPlace(1.f / static_cast<float>(polygon.vertexIds.size()));

  polygon.centroid.copyFrom(sum);
}

Vector3 Navigation::getVectorFrom(const Float32Array& vertices, unsigned int id)
{
  return Vector3(vertices[id * 3], vertices[id * 3 + 1], vertices[id * 3 + 2]);
}

std::vector<std::vector<NavigationPolygon>>
Navigation::_buildPolygonGroups(NavigationMesh& navigationMesh)
{
  auto& polygons = navigationMesh.polygons;

  std::vector<std::vector<NavigationPolygon>> polygonGroups;
  size_t groupCount = 0;

  std::function<void(NavigationPolygon & polygon)> spreadGroupId
    = [&](NavigationPolygon& polygon) {
        for (auto& neighbour : polygon.neighbours) {
          if (!neighbour.hasGroup) {
            neighbour.group    = polygon.group;
            neighbour.hasGroup = true;
            spreadGroupId(neighbour);
          }
        }
      };

  for (auto& polygon : polygons) {
    if (!polygon.hasGroup) {
      polygon.group    = groupCount++;
      polygon.hasGroup = true;
      // Spread it
      spreadGroupId(polygon);
    }

    if (polygon.group >= polygonGroups.size()) {
      polygonGroups.resize(polygon.group + 1);
    }

    polygonGroups[polygon.group].emplace_back(polygon);
  }

  return polygonGroups;
}

Uint32Array Navigation::_array_intersect(const Uint32Array& a,
                                         const Uint32Array& b)
{
  Uint32Array aList, bList;
  aList.assign(a.begin(), a.end());
  bList.assign(b.begin(), b.end());

  Uint32Array intersection;
  std::sort(aList.begin(), aList.end());
  std::sort(bList.begin(), bList.end());
  std::set_intersection(aList.begin(), aList.end(), bList.begin(), bList.end(),
                        std::back_inserter(intersection));

  return intersection;
}

void Navigation::_buildPolygonNeighbours(NavigationPolygon& polygon,
                                         NavigationMesh& navigationMesh)
{
  polygon.neighbours.clear();

  // All other nodes that contain at least two of our vertices are our
  // neighbours
  for (auto& navMeshPolygon : navigationMesh.polygons) {
    if (polygon.id == navMeshPolygon.id) {
      continue;
    }

    // Don't check polygons that are too far, since the intersection tests take
    // a long time
    if (Vector3::DistanceSquared(polygon.centroid, navMeshPolygon.centroid)
        > 100 * 100) {
      continue;
    }

    auto matches
      = _array_intersect(polygon.vertexIds, navMeshPolygon.vertexIds);

    if (matches.size() >= 2) {
      polygon.neighbours.emplace_back(navMeshPolygon);
    }
  }
}

NavigationMesh Navigation::_buildPolygonsFromGeometry(Geometry* geometry)
{
  std::vector<NavigationPolygon> polygons;
  auto vertices  = geometry->getVerticesData(VertexBuffer::PositionKind);
  auto indices   = geometry->getIndices();
  auto polygonId = 1u;

  // Convert the faces into a custom format that supports more than 3 vertices
  for (size_t i = 0; i < indices.size(); i += 3) {

    auto a      = getVectorFrom(vertices, indices[i]);
    auto b      = getVectorFrom(vertices, indices[i + 1]);
    auto c      = getVectorFrom(vertices, indices[i + 2]);
    auto normal = Vector3::Cross(b.subtract(a), b.subtract(c)).normalize();

    polygons.emplace_back(NavigationPolygon{
      polygonId++,                                  // id
      {indices[i], indices[i + 1], indices[i + 2]}, // vertexIds
      geometry->centroids[i / 3],                   // centroid
      normal,                                       // normal
      {},                                           // neighbours
      {},                                           // portals
      false,                                        // hasGroup
      0                                             // group
    });
  }

  NavigationMesh navigationMesh{polygons, vertices};

  // Build a list of adjacent polygons
  for (auto& polygon : polygons) {
    _buildPolygonNeighbours(polygon, navigationMesh);
  }

  return navigationMesh;
}

NavigationMesh Navigation::_buildNavigationMesh(Geometry* geometry)
{
  // Prepare geometry
  _computeCentroids(geometry);
  _mergeVertices(geometry);

  return _buildPolygonsFromGeometry(geometry);
}

size_t Navigation::_mergeVertices(Geometry* geometry)
{
  // Hashmap for looking up vertices by position coordinates (and making sure
  // they are unique)
  std::unordered_map<std::string, size_t> verticesMap;
  std::vector<Vector3> unique;
  Uint32Array changes;

  // number of decimal points, e.g. 4 for epsilon of 0.0001
  auto precisionPoints = 4u;
  auto precision       = static_cast<float>(std::pow(10, precisionPoints));
  auto ind             = geometry->getIndices();
  auto vert            = geometry->getVerticesData(VertexBuffer::PositionKind);

  for (size_t i = 0, il = vert.size(); i < il; i += 3) {

    Vector3 v(vert[i], vert[i + 1], vert[i + 2]);

    std::stringstream ss;
    ss << std::fixed;
    ss.precision(precisionPoints);
    ss << std::round(v.x * precision) << "_" << std::round(v.y * precision)
       << "_" << std::round(v.z * precision);
    std::string key = ss.str();

    if (!stl_util::contains(verticesMap, key)) {
      verticesMap[key] = i / 3;
      unique.emplace_back(v);
      changes[i / 3] = static_cast<std::uint32_t>(unique.size()) - 1;
    }
    else {
      changes[i / 3] = changes[verticesMap[key]];
    }
  }

  // if faces are completely degenerate after merging vertices, we
  // have to remove them from the geometry.
  std::vector<size_t> faceIndicesToRemove;

  for (size_t i = 0, il = ind.size(); i < il; i += 3) {

    ind[i]     = changes[ind[i]];
    ind[i + 1] = changes[ind[i + 1]];
    ind[i + 2] = changes[ind[i + 2]];

    Uint32Array indices{ind[i], ind[i + 1], ind[i + 2]};

    // if any duplicate vertices are found in a Face3
    // we have to remove the face as nothing can be saved
    for (unsigned int n = 0; n < 3; ++n) {
      if (indices[n] == indices[(n + 1) % 3]) {
        faceIndicesToRemove.emplace_back(i);
        break;
      }
    }
  }

  for (size_t i = faceIndicesToRemove.size(); i-- > 0;) {
    stl_util::splice(ind, faceIndicesToRemove[i], 3);
  }

  // Use unique set of vertices
  size_t diff = vert.size() / 3 - unique.size();
  vert.clear();
  for (auto& vec : unique) {
    stl_util::concat(vert, {vec.x, vec.y, vec.z});
  }

  geometry->setIndices(ind);
  geometry->setVerticesData(VertexBuffer::PositionKind, vert);

  return diff;
}

Uint32Array Navigation::_getSharedVerticesInOrder(NavigationPolygon& a,
                                                  NavigationPolygon& b)
{
  auto& aList = a.vertexIds;
  auto& bList = b.vertexIds;

  auto sharedVertices = _array_intersect(aList, bList);

  if (sharedVertices.size() < 2) {
    return Uint32Array();
  }

  if (stl_util::contains(sharedVertices, aList[0])
      && stl_util::contains(sharedVertices, aList.back())) {
    // Vertices on both edges are bad, so shift them once to the left
    std::vector<Uint32Array::value_type>(aList.begin() + 1, aList.end())
      .swap(aList);
  }

  if (stl_util::contains(sharedVertices, bList[0])
      && stl_util::contains(sharedVertices, bList.back())) {
    // Vertices on both edges are bad, so shift them once to the left
    std::vector<Uint32Array::value_type>(bList.begin() + 1, bList.end())
      .swap(bList);
  }

  return _array_intersect(aList, bList);
}

GroupedNavigationMesh Navigation::_groupNavMesh(NavigationMesh& navigationMesh)
{
  GroupedNavigationMesh groupedNavMesh;

  for (size_t i = 0; i < navigationMesh.vertices.size(); ++i) {
    float v                    = navigationMesh.vertices[i];
    navigationMesh.vertices[i] = _roundNumber(v, 2);
  }

  groupedNavMesh.vertices = navigationMesh.vertices;

  auto groups = _buildPolygonGroups(navigationMesh);

  groupedNavMesh.groups.clear();

  const auto findPolygonIndex = [](const std::vector<NavigationPolygon>& group,
                                   const NavigationPolygon& p) -> int {
    return stl_util::index_of(group, p);
  };

  for (auto& group : groups) {
    NavigationGroupGraph newGroup;

    for (auto& p : group) {
      Uint32Array neighbours;
      for (auto& n : p.neighbours) {
        neighbours.emplace_back(findPolygonIndex(group, n));
      }

      // Build a portal list to each neighbour
      std::vector<Uint32Array> portals;
      for (auto& n : p.neighbours) {
        portals.emplace_back(_getSharedVerticesInOrder(p, n));
      }

      p.centroid.x = _roundNumber(p.centroid.x, 2);
      p.centroid.y = _roundNumber(p.centroid.y, 2);
      p.centroid.z = _roundNumber(p.centroid.z, 2);

      const auto polygonIndex = findPolygonIndex(group, p);
      if (polygonIndex >= 0) {
        newGroup.push(NavigationGroup{
          static_cast<std::size_t>(polygonIndex), // id
          neighbours,                             // neighbours
          p.vertexIds,                            // vertexIds
          p.centroid,                             // centroid
          portals,                                // portals
          1.f                                     // cost
        });
      }
    }

    groupedNavMesh.groups.emplace_back(newGroup);
  }

  return groupedNavMesh;
}

} // end of namespace Extensions
} // end of namespace BABYLON
