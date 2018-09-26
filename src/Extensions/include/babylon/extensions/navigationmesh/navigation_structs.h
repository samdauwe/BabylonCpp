#ifndef BABYLON_EXTENSIONS_NAVIGATION_MESH_NAVIGATION_STRUCTS_H
#define BABYLON_EXTENSIONS_NAVIGATION_MESH_NAVIGATION_STRUCTS_H

#include <babylon/babylon_api.h>
#include <babylon/math/vector3.h>

namespace BABYLON {
namespace Extensions {

struct BABYLON_SHARED_EXPORT NavigationPolygon {
  std::size_t id;
  IndicesArray vertexIds;
  Vector3 centroid;
  Vector3 normal;
  std::vector<NavigationPolygon> neighbours;
  std::vector<Uint32Array> portals;
  bool hasGroup;
  size_t group;

  bool operator==(const NavigationPolygon& rhs) const
  {
    return (id == rhs.id) && (vertexIds == rhs.vertexIds)
           && (centroid == rhs.centroid) && (normal == rhs.normal)
           && (neighbours == rhs.neighbours) && (portals == rhs.portals)
           && (hasGroup == rhs.hasGroup) && (group == rhs.group);
  }
}; // end of struct NavigationPolygon

struct BABYLON_SHARED_EXPORT NavigationMesh {
  std::vector<NavigationPolygon> polygons;
  Float32Array vertices;
}; // end of struct NavigationPolygon

struct BABYLON_SHARED_EXPORT NavigationGroup {
  std::size_t id;
  Uint32Array neighbours;
  IndicesArray vertexIds;
  Vector3 centroid;
  std::vector<Uint32Array> portals;
  float cost;

  bool operator==(const NavigationGroup& rhs) const
  {
    return (id == rhs.id) && (neighbours == rhs.neighbours)
           && (vertexIds == rhs.vertexIds) && (centroid == rhs.centroid)
           && (portals == rhs.portals);
  }
}; // end of struct NavigationGroup

struct NavigationGroupGraph {
  using Node           = NavigationGroup;
  using NodeId         = std::size_t;
  using iterator       = std::vector<NavigationGroup>::iterator;
  using const_iterator = std::vector<NavigationGroup>::const_iterator;
  std::vector<NavigationGroup> groups;

  void push(NavigationGroup&& newGroup)
  {
    groups.emplace_back(std::move(newGroup));
  }

  NavigationGroup& operator[](const std::size_t groupId)
  {
    return groups[groupId];
  }

  const NavigationGroup& operator[](const std::size_t groupId) const
  {
    return groups[groupId];
  }

  std::size_t size() const
  {
    return groups.size();
  }

  iterator begin()
  {
    return groups.begin();
  }

  const_iterator begin() const
  {
    return groups.begin();
  }

  iterator end()
  {
    return groups.end();
  }

  const_iterator end() const
  {
    return groups.end();
  }

  const NavigationGroup& group(const std::size_t groupId) const
  {
    return groups[groupId];
  }

  float cost(const std::size_t /*groupId1*/,
             const NavigationGroup& group2) const
  {
    return group(group2.id).cost;
  }

  float heuristicCostEstimate(const NavigationGroup& group1,
                              const NavigationGroup& group2) const
  {
    auto pos1 = group(group1.id).centroid;
    auto pos2 = group(group2.id).centroid;
    return Vector3::DistanceSquared(pos1, pos2);
  }

  std::vector<NavigationGroup> neighbors(const std::size_t groupId) const
  {
    std::vector<NavigationGroup> ret;
    auto& group = groups[groupId];
    for (auto index : group.neighbours) {
      ret.emplace_back(groups[index]);
    }
    return ret;
  }

}; // end of struct NavigationGroupGraph

struct BABYLON_SHARED_EXPORT GroupedNavigationMesh {
  std::vector<NavigationGroupGraph> groups;
  Float32Array vertices;
}; // end of struct GroupedNavigationMesh

struct BABYLON_SHARED_EXPORT Portal {
  Vector3 left;
  Vector3 right;
}; // end of struct Portal

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_NAVIGATION_MESH_NAVIGATION_STRUCTS_H
