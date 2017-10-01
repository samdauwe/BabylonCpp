#include <babylon/extensions/hexplanetgeneration/icosphere.h>

#include <babylon/extensions/hexplanetgeneration/utils/irandom_function.h>
#include <babylon/extensions/hexplanetgeneration/utils/tools.h>

namespace BABYLON {
namespace Extensions {

// -----------------------------------------------------------------------------
// IcoNode
// -----------------------------------------------------------------------------

IcoNode::IcoNode(const Vector3& _p) : p{_p}
{
}

IcoNode::IcoNode(const Vector3& _p, const std::vector<size_t>& _e)
    : p{_p}, e{_e}
{
}

IcoNode::IcoNode(const IcoNode& other) : p{other.p}, e{other.e}, f{other.f}
{
}

IcoNode::IcoNode(IcoNode&& other)
    : p{std::move(other.p)}, e{std::move(other.e)}, f{std::move(other.f)}
{
}

IcoNode& IcoNode::operator=(const IcoNode& other)
{
  if (&other != this) {
    p = other.p;
    e = other.e;
    f = other.f;
  }

  return *this;
}

IcoNode& IcoNode::operator=(IcoNode&& other)
{
  if (&other != this) {
    p = std::move(other.p);
    e = std::move(other.e);
    f = std::move(other.f);
  }

  return *this;
}

// -----------------------------------------------------------------------------
// IcoEdge
// -----------------------------------------------------------------------------

IcoEdge::IcoEdge(const std::vector<size_t>& _n) : n{_n}
{
}

IcoEdge::IcoEdge(const IcoEdge& other)
    : n{other.n}
    , f{other.f}
    , subdivided_n{other.subdivided_n}
    , subdivided_e{other.subdivided_e}
{
}

IcoEdge::IcoEdge(IcoEdge&& other)
    : n{std::move(other.n)}
    , f{std::move(other.f)}
    , subdivided_n{std::move(other.subdivided_n)}
    , subdivided_e{std::move(other.subdivided_e)}
{
}

IcoEdge& IcoEdge::operator=(const IcoEdge& other)
{
  if (&other != this) {
    n            = other.n;
    f            = other.f;
    subdivided_n = other.subdivided_n;
    subdivided_e = other.subdivided_e;
  }

  return *this;
}

IcoEdge& IcoEdge::operator=(IcoEdge&& other)
{
  if (&other != this) {
    n            = std::move(other.n);
    f            = std::move(other.f);
    subdivided_n = std::move(other.subdivided_n);
    subdivided_e = std::move(other.subdivided_e);
  }

  return *this;
}

// -----------------------------------------------------------------------------
// IcoFace
// -----------------------------------------------------------------------------

IcoFace::IcoFace(const std::vector<size_t>& _n, const std::vector<size_t>& _e)
    : n{_n}, e{_e}, boundingSphere{Vector3::Zero(), Vector3::Zero()}
{
}

IcoFace::IcoFace(const IcoFace& other)
    : n{other.n}
    , e{other.e}
    , centroid{other.centroid}
    , boundingSphere{other.boundingSphere}
    , children{other.children}
{
}

IcoFace::IcoFace(IcoFace&& other)
    : n{std::move(other.n)}
    , e{std::move(other.e)}
    , centroid{std::move(other.centroid)}
    , boundingSphere{std::move(other.boundingSphere)}
    , children{std::move(other.children)}
{
}

IcoFace& IcoFace::operator=(const IcoFace& other)
{
  if (&other != this) {
    n              = other.n;
    e              = other.e;
    centroid       = other.centroid;
    boundingSphere = other.boundingSphere;
    children       = other.children;
  }

  return *this;
}

IcoFace& IcoFace::operator=(IcoFace&& other)
{
  if (&other != this) {
    n              = std::move(other.n);
    e              = std::move(other.e);
    centroid       = std::move(other.centroid);
    boundingSphere = std::move(other.boundingSphere);
    children       = std::move(other.children);
  }

  return *this;
}

// -----------------------------------------------------------------------------
// IcosahedronMesh
// -----------------------------------------------------------------------------

IcosahedronMesh::IcosahedronMesh()
{
}

IcosahedronMesh::IcosahedronMesh(const IcosahedronMesh& other)
    : nodes{other.nodes}, edges{other.edges}, faces{other.faces}
{
}

IcosahedronMesh::IcosahedronMesh(IcosahedronMesh&& other)
    : nodes{std::move(other.nodes)}
    , edges{std::move(other.edges)}
    , faces{std::move(other.faces)}
{
}

IcosahedronMesh& IcosahedronMesh::operator=(const IcosahedronMesh& other)
{
  if (&other != this) {
    nodes = other.nodes;
    edges = other.edges;
    faces = other.faces;
  }

  return *this;
}

IcosahedronMesh& IcosahedronMesh::operator=(IcosahedronMesh&& other)
{
  if (&other != this) {
    nodes = std::move(other.nodes);
    edges = std::move(other.edges);
    faces = std::move(other.faces);
  }

  return *this;
}

// -----------------------------------------------------------------------------
// Icosphere
// -----------------------------------------------------------------------------

IcosahedronMesh
Icosphere::generateIcosahedronMesh(size_t icosahedronSubdivision,
                                   float topologyDistortionRate,
                                   IRandomFunction& random)
{
  IcosahedronMesh mesh;
  generateSubdividedIcosahedron(icosahedronSubdivision, mesh);

  // Distorting Triangle Mesh
  float totalDistortion = std::ceil(mesh.edges.size() * topologyDistortionRate);
  for (size_t remainingIterations = 6; remainingIterations > 0;) {
    float iterationDistortion
      = std::floor(totalDistortion / remainingIterations);
    totalDistortion -= iterationDistortion;
    distortMesh(mesh, static_cast<size_t>(iterationDistortion), random);
    relaxMesh(mesh, 0.5f);
    --remainingIterations;
  }

  // Relaxing Triangle Mesh
  float averageNodeRadius = std::sqrt(4 * Math::PI / mesh.nodes.size());
  float minShiftDelta     = averageNodeRadius / 50000 * mesh.nodes.size();

  float priorShift = relaxMesh(mesh, 0.5f);
  for (int i = 0; i < 300; ++i) {
    float currentShift = relaxMesh(mesh, 0.5f);
    float shiftDelta   = std::abs(currentShift - priorShift);
    if (shiftDelta < minShiftDelta) {
      break;
    }
    priorShift = currentShift;
  }

  // Calculating Triangle Centroids
  for (auto& face : mesh.faces) {
    auto& p0      = mesh.nodes[face.n[0]].p;
    auto& p1      = mesh.nodes[face.n[1]].p;
    auto& p2      = mesh.nodes[face.n[2]].p;
    face.centroid = calculateFaceCentroid(p0, p1, p2);
    face.centroid.normalize();
  }

  // Reordering Triangle Nodes
  unsigned int i = 0;
  for (auto& node : mesh.nodes) {
    auto faceIndex = node.f[0];
    for (size_t j = 1; j < node.f.size() - 1; ++j) {
      faceIndex = findNextFaceIndex(mesh, i, faceIndex);
      size_t k  = 0;
      if (findIndex(node.f, faceIndex, k)) {
        node.f[k] = node.f[j];
        node.f[j] = faceIndex;
      }
    }
    ++i;
  }

  return mesh;
}

void Icosphere::generateIcosahedron(IcosahedronMesh& mesh)
{
  float phi = (1.f + std::sqrt(5.f)) / 2.f;
  float du  = 1.f / std::sqrt(phi * phi + 1.f);
  float dv  = phi * du;

  mesh.nodes = {IcoNode(Vector3(0, +dv, +du)), IcoNode(Vector3(0, +dv, -du)),
                IcoNode(Vector3(0, -dv, +du)), IcoNode(Vector3(0, -dv, -du)),
                IcoNode(Vector3(+du, 0, +dv)), IcoNode(Vector3(-du, 0, +dv)),
                IcoNode(Vector3(+du, 0, -dv)), IcoNode(Vector3(-du, 0, -dv)),
                IcoNode(Vector3(+dv, +du, 0)), IcoNode(Vector3(+dv, -du, 0)),
                IcoNode(Vector3(-dv, +du, 0)), IcoNode(Vector3(-dv, -du, 0))};

  mesh.edges
    = {IcoEdge({0, 1}),  IcoEdge({0, 4}),  IcoEdge({0, 5}),  IcoEdge({0, 8}),
       IcoEdge({0, 10}), IcoEdge({1, 6}),  IcoEdge({1, 7}),  IcoEdge({1, 8}),
       IcoEdge({1, 10}), IcoEdge({2, 3}),  IcoEdge({2, 4}),  IcoEdge({2, 5}),
       IcoEdge({2, 9}),  IcoEdge({2, 11}), IcoEdge({3, 6}),  IcoEdge({3, 7}),
       IcoEdge({3, 9}),  IcoEdge({3, 11}), IcoEdge({4, 5}),  IcoEdge({4, 8}),
       IcoEdge({4, 9}),  IcoEdge({5, 10}), IcoEdge({5, 11}), IcoEdge({6, 7}),
       IcoEdge({6, 8}),  IcoEdge({6, 9}),  IcoEdge({7, 10}), IcoEdge({7, 11}),
       IcoEdge({8, 9}),  IcoEdge({10, 11})};

  mesh.faces = {
    IcoFace({0, 1, 8}, {0, 7, 3}),     IcoFace({0, 4, 5}, {1, 18, 2}),
    IcoFace({0, 5, 10}, {2, 21, 4}),   IcoFace({0, 8, 4}, {3, 19, 1}),
    IcoFace({0, 10, 1}, {4, 8, 0}),    IcoFace({1, 6, 8}, {5, 24, 7}),
    IcoFace({1, 7, 6}, {6, 23, 5}),    IcoFace({1, 10, 7}, {8, 26, 6}),
    IcoFace({2, 3, 11}, {9, 17, 13}),  IcoFace({2, 4, 9}, {10, 20, 12}),
    IcoFace({2, 5, 4}, {11, 18, 10}),  IcoFace({2, 9, 3}, {12, 16, 9}),
    IcoFace({2, 11, 5}, {13, 22, 11}), IcoFace({3, 6, 7}, {14, 23, 15}),
    IcoFace({3, 7, 11}, {15, 27, 17}), IcoFace({3, 9, 6}, {16, 25, 14}),
    IcoFace({4, 8, 9}, {19, 28, 20}),  IcoFace({5, 11, 10}, {22, 29, 21}),
    IcoFace({6, 9, 8}, {25, 28, 24}),  IcoFace({7, 10, 11}, {26, 29, 27}),
  };

  mesh.nodes.reserve(mesh.edges.size() * 3);
  for (size_t i = 0; i < mesh.edges.size(); ++i) {
    for (size_t j = 0; j < mesh.edges[i].n.size(); ++j) {
      mesh.nodes[j].e.emplace_back(i);
    }
  }

  for (size_t i = 0; i < mesh.faces.size(); ++i) {
    for (size_t j = 0; j < mesh.faces[i].n.size(); ++j) {
      mesh.nodes[j].f.emplace_back(i);
    }
  }

  mesh.edges.reserve(mesh.faces.size() * 3);
  for (size_t i = 0; i < mesh.faces.size(); ++i) {
    for (size_t j = 0; j < mesh.faces[i].e.size(); ++j) {
      mesh.edges[j].f.emplace_back(i);
    }
  }
}

void Icosphere::generateSubdividedIcosahedron(size_t degree,
                                              IcosahedronMesh& mesh)
{
  auto& nodes = mesh.nodes;
  auto& edges = mesh.edges;
  auto& faces = mesh.faces;

  IcosahedronMesh icosahedron;
  generateIcosahedron(icosahedron);

  // Ico nodes
  nodes.reserve(icosahedron.nodes.size()
                + icosahedron.edges.size() * (degree - 1)
                + icosahedron.faces.size() * (degree - 1) * (degree - 1) / 2);
  for (auto& node : icosahedron.nodes) {
    nodes.emplace_back(IcoNode(node.p));
  }

  // Ico edges
  edges.reserve(
    icosahedron.edges.size() + icosahedron.edges.size() * (degree - 1)
    + 3 * icosahedron.faces.size() * (degree - 1) * (degree - 1) / 2);
  for (auto& edge : icosahedron.edges) {
    edge.subdivided_n.clear();
    edge.subdivided_e.clear();
    auto& n0 = icosahedron.nodes[edge.n[0]];
    auto& n1 = icosahedron.nodes[edge.n[1]];
    auto& p0 = n0.p;
    auto& p1 = n1.p;
    nodes[edge.n[0]].e.emplace_back(edges.size());
    size_t priorNodeIndex = edge.n[0];
    for (unsigned int s = 1; s < degree; ++s) {
      size_t edgeIndex = edges.size();
      size_t nodeIndex = nodes.size();
      edge.subdivided_e.emplace_back(edgeIndex);
      edge.subdivided_n.emplace_back(nodeIndex);
      edges.emplace_back(IcoEdge({priorNodeIndex, nodeIndex}));
      priorNodeIndex = nodeIndex;
      nodes.emplace_back(
        IcoNode(Vector3::Lerp(
                  p0, p1, static_cast<float>(s) / static_cast<float>(degree)),
                {edgeIndex, edgeIndex + 1}));
    }
    edge.subdivided_e.emplace_back(edges.size());
    nodes[edge.n[1]].e.emplace_back(edges.size());
    edges.emplace_back(IcoEdge({priorNodeIndex, edge.n[1]}));
  }

  // Ico faces
  faces.reserve(2 * icosahedron.faces.size() * (degree - 1) * (degree - 1) / 2);
  for (auto& face : icosahedron.faces) {
    auto& edge0 = icosahedron.edges[face.e[0]];
    auto& edge1 = icosahedron.edges[face.e[1]];
    auto& edge2 = icosahedron.edges[face.e[2]];

    auto getEdgeNode0 = [&face, &edge0, &degree](size_t k) {
      if (face.n[0] == edge0.n[0]) {
        return edge0.subdivided_n[k];
      }
      else {
        return edge0.subdivided_n[degree - 2 - k];
      }
    };

    auto getEdgeNode1 = [&face, &edge1, &degree](size_t k) {
      if (face.n[1] == edge1.n[0]) {
        return edge1.subdivided_n[k];
      }
      else {
        return edge1.subdivided_n[degree - 2 - k];
      }
    };

    auto getEdgeNode2 = [&face, &edge2, &degree](size_t k) {
      if (face.n[0] == edge2.n[0]) {
        return edge2.subdivided_n[k];
      }
      else {
        return edge2.subdivided_n[degree - 2 - k];
      }
    };

    // Face nodes
    std::vector<size_t> faceNodes;
    faceNodes.reserve(1 + edge0.subdivided_n.size() + 1
                      + (degree - 1) * (degree - 1) / 2 + 1);
    faceNodes.emplace_back(face.n[0]);
    for (unsigned int j = 0; j < edge0.subdivided_n.size(); ++j) {
      faceNodes.emplace_back(getEdgeNode0(j));
    }
    faceNodes.emplace_back(face.n[1]);
    for (unsigned int s = 1; s < degree; ++s) {
      faceNodes.emplace_back(getEdgeNode2(s - 1));
      auto& p0 = nodes[getEdgeNode2(s - 1)].p;
      auto& p1 = nodes[getEdgeNode1(s - 1)].p;
      for (unsigned int t = 1; t < degree - s; ++t) {
        faceNodes.emplace_back(nodes.size());
        nodes.emplace_back(IcoNode(Vector3::Lerp(
          p0, p1, static_cast<float>(t) / static_cast<float>(degree - s))));
      }
      faceNodes.emplace_back(getEdgeNode1(s - 1));
    }
    faceNodes.emplace_back(face.n[2]);

    auto getEdgeEdge0 = [&face, &edge0, &degree](size_t k) {
      if (face.n[0] == edge0.n[0]) {
        return edge0.subdivided_e[k];
      }
      else {
        return edge0.subdivided_e[degree - 1 - k];
      }
    };

    auto getEdgeEdge1 = [&face, &edge1, &degree](size_t k) {
      if (face.n[1] == edge1.n[0]) {
        return edge1.subdivided_e[k];
      }
      else {
        return edge1.subdivided_e[degree - 1 - k];
      }
    };

    auto getEdgeEdge2 = [&face, &edge2, &degree](size_t k) {
      if (face.n[0] == edge2.n[0]) {
        return edge2.subdivided_e[k];
      }
      else {
        return edge2.subdivided_e[degree - 1 - k];
      }
    };

    // Face edges 0
    std::vector<size_t> faceEdges0;
    faceEdges0.reserve(degree + (degree - 1) * (degree - 1) / 2);
    for (unsigned int j = 0; j < degree; ++j) {
      faceEdges0.emplace_back(getEdgeEdge0(j));
    }
    auto nodeIndex = degree + 1;
    for (unsigned int s = 1; s < degree; ++s) {
      for (unsigned int t = 0; t < degree - s; ++t) {
        faceEdges0.emplace_back(edges.size());
        IcoEdge edge({faceNodes[nodeIndex], faceNodes[nodeIndex + 1]});
        nodes[edge.n[0]].e.emplace_back(edges.size());
        nodes[edge.n[1]].e.emplace_back(edges.size());
        edges.emplace_back(edge);
        ++nodeIndex;
      }
      ++nodeIndex;
    }

    // Face edges 1
    std::vector<size_t> faceEdges1;
    faceEdges1.reserve(degree * (degree - 1) / 2);
    nodeIndex = 1;
    for (unsigned int s = 0; s < degree; ++s) {
      for (unsigned int t = 1; t < degree - s; ++t) {
        faceEdges1.emplace_back(edges.size());
        IcoEdge edge({faceNodes[nodeIndex], faceNodes[nodeIndex + degree - s]});
        nodes[edge.n[0]].e.emplace_back(edges.size());
        nodes[edge.n[1]].e.emplace_back(edges.size());
        edges.emplace_back(edge);
        ++nodeIndex;
      }
      faceEdges1.emplace_back(getEdgeEdge1(s));
      nodeIndex += 2;
    }

    // Face edges 2
    std::vector<size_t> faceEdges2;
    faceEdges2.reserve(degree * (degree - 1) / 2);
    nodeIndex = 1;
    for (unsigned int s = 0; s < degree; ++s) {
      faceEdges2.emplace_back(getEdgeEdge2(s));
      for (unsigned int t = 1; t < degree - s; ++t) {
        faceEdges2.emplace_back(edges.size());
        IcoEdge edge(
          {faceNodes[nodeIndex], faceNodes[nodeIndex + degree - s + 1]});
        nodes[edge.n[0]].e.emplace_back(edges.size());
        nodes[edge.n[1]].e.emplace_back(edges.size());
        edges.emplace_back(edge);
        ++nodeIndex;
      }
      nodeIndex += 2;
    }

    nodeIndex        = 0;
    size_t edgeIndex = 0;
    for (unsigned int s = 0; s < degree; ++s) {
      for (unsigned int t = 1; t < degree - s + 1; ++t) {
        IcoFace subFace({faceNodes[nodeIndex], faceNodes[nodeIndex + 1],
                         faceNodes[nodeIndex + degree - s + 1]},
                        {faceEdges0[edgeIndex], faceEdges1[edgeIndex],
                         faceEdges2[edgeIndex]});
        nodes[subFace.n[0]].f.emplace_back(faces.size());
        nodes[subFace.n[1]].f.emplace_back(faces.size());
        nodes[subFace.n[2]].f.emplace_back(faces.size());
        edges[subFace.e[0]].f.emplace_back(faces.size());
        edges[subFace.e[1]].f.emplace_back(faces.size());
        edges[subFace.e[2]].f.emplace_back(faces.size());
        faces.emplace_back(subFace);
        ++nodeIndex;
        ++edgeIndex;
      }
      ++nodeIndex;
    }

    nodeIndex = 1;
    edgeIndex = 0;
    for (unsigned int s = 1; s < degree; ++s) {
      for (unsigned int t = 1; t < degree - s + 1; ++t) {
        IcoFace subFace(
          {faceNodes[nodeIndex], faceNodes[nodeIndex + degree - s + 2],
           faceNodes[nodeIndex + degree - s + 1]},
          {faceEdges2[edgeIndex + 1], faceEdges0[edgeIndex + degree - s + 1],
           faceEdges1[edgeIndex]});
        nodes[subFace.n[0]].f.emplace_back(faces.size());
        nodes[subFace.n[1]].f.emplace_back(faces.size());
        nodes[subFace.n[2]].f.emplace_back(faces.size());
        edges[subFace.e[0]].f.emplace_back(faces.size());
        edges[subFace.e[1]].f.emplace_back(faces.size());
        edges[subFace.e[2]].f.emplace_back(faces.size());
        faces.emplace_back(subFace);
        ++nodeIndex;
        ++edgeIndex;
      }
      nodeIndex += 2;
      edgeIndex += 1;
    }
  }
}

size_t Icosphere::getEdgeOppositeFaceIndex(const IcoEdge& edge,
                                           size_t faceIndex)
{
  if (edge.f[0] == faceIndex) {
    return edge.f[1];
  }
  if (edge.f[1] == faceIndex) {
    return edge.f[0];
  }
  return 0;
}

size_t Icosphere::getFaceOppositeNodeIndex(const IcoFace& face,
                                           const IcoEdge& edge)
{
  if (face.n[0] != edge.n[0] && face.n[0] != edge.n[1]) {
    return 0;
  }
  if (face.n[1] != edge.n[0] && face.n[1] != edge.n[1]) {
    return 1;
  }
  if (face.n[2] != edge.n[0] && face.n[2] != edge.n[1]) {
    return 2;
  }
  return 0;
}

size_t Icosphere::findNextFaceIndex(const IcosahedronMesh& mesh,
                                    size_t nodeIndex, size_t faceIndex)
{
  const auto& face = mesh.faces[faceIndex];
  size_t nodeFaceIndex;
  bool ok = findIndex(face.n, nodeIndex, nodeFaceIndex);
  if (!ok) {
    return 0;
  }
  const auto& edge = mesh.edges[face.e[(nodeFaceIndex + 2) % 3]];
  return getEdgeOppositeFaceIndex(edge, faceIndex);
}

bool Icosphere::conditionalRotateEdge(IcosahedronMesh& mesh, size_t edgeIndex,
                                      RotationPredicateType& predicate)
{
  auto& edge  = mesh.edges[edgeIndex];
  auto& face0 = mesh.faces[edge.f[0]];
  auto& face1 = mesh.faces[edge.f[1]];
  size_t farNodeFaceIndex0;
  size_t farNodeFaceIndex1;
  farNodeFaceIndex0  = getFaceOppositeNodeIndex(face0, edge);
  farNodeFaceIndex1  = getFaceOppositeNodeIndex(face1, edge);
  auto newNodeIndex0 = face0.n[farNodeFaceIndex0];
  auto oldNodeIndex0 = face0.n[(farNodeFaceIndex0 + 1) % 3];
  auto newNodeIndex1 = face1.n[farNodeFaceIndex1];
  auto oldNodeIndex1 = face1.n[(farNodeFaceIndex1 + 1) % 3];
  auto& oldNode0     = mesh.nodes[oldNodeIndex0];
  auto& oldNode1     = mesh.nodes[oldNodeIndex1];
  auto& newNode0     = mesh.nodes[newNodeIndex0];
  auto& newNode1     = mesh.nodes[newNodeIndex1];
  auto newEdgeIndex0 = face1.e[(farNodeFaceIndex1 + 2) % 3];
  auto newEdgeIndex1 = face0.e[(farNodeFaceIndex0 + 2) % 3];
  auto& newEdge0     = mesh.edges[newEdgeIndex0];
  auto& newEdge1     = mesh.edges[newEdgeIndex1];

  if (!predicate(oldNode0, oldNode1, newNode0, newNode1)) {
    return false;
  }

  removeIfFindIndex(oldNode0.e, edgeIndex);
  removeIfFindIndex(oldNode1.e, edgeIndex);
  newNode0.e.push_back(edgeIndex);
  newNode1.e.push_back(edgeIndex);

  edge.n[0] = newNodeIndex0;
  edge.n[1] = newNodeIndex1;

  removeIfFindIndex(newEdge0.f, edge.f[1]);
  removeIfFindIndex(newEdge1.f, edge.f[0]);
  newEdge0.f.push_back(edge.f[0]);
  newEdge1.f.push_back(edge.f[1]);

  removeIfFindIndex(oldNode0.f, edge.f[1]);
  removeIfFindIndex(oldNode1.f, edge.f[0]);
  newNode0.f.push_back(edge.f[1]);
  newNode1.f.push_back(edge.f[0]);

  face0.n[(farNodeFaceIndex0 + 2) % 3] = newNodeIndex1;
  face1.n[(farNodeFaceIndex1 + 2) % 3] = newNodeIndex0;

  face0.e[(farNodeFaceIndex0 + 1) % 3] = newEdgeIndex0;
  face1.e[(farNodeFaceIndex1 + 1) % 3] = newEdgeIndex1;
  face0.e[(farNodeFaceIndex0 + 2) % 3] = edgeIndex;
  face1.e[(farNodeFaceIndex1 + 2) % 3] = edgeIndex;

  return true;
}

Vector3 Icosphere::calculateFaceCentroid(const Vector3& pa, const Vector3& pb,
                                         const Vector3& pc)
{
  auto centroid = pa + pb + pc;
  centroid /= 3.f;
  return centroid;
}

bool Icosphere::distortMesh(IcosahedronMesh& mesh, size_t degree,
                            IRandomFunction& random)
{
  auto rotationPredicate = RotationPredicateType(
    [&](const IcoNode& oldNode0, const IcoNode& oldNode1,
        const IcoNode& newNode0, const IcoNode& newNode1) -> bool {
      if (newNode0.f.size() >= 7 || newNode1.f.size() >= 7
          || oldNode0.f.size() <= 5 || oldNode1.f.size() <= 5) {
        return false;
      }
      float oldEdgeLength = Vector3::Distance(oldNode0.p, oldNode1.p);
      if (oldEdgeLength == 0.f) {
        return false;
      }
      float newEdgeLength = Vector3::Distance(newNode0.p, newNode1.p);
      auto ratio          = oldEdgeLength / newEdgeLength;
      if (ratio >= 2.f || ratio <= 0.5f) {
        return false;
      }
      auto v0 = (oldNode1.p - oldNode0.p) / oldEdgeLength;
      auto v1 = (newNode0.p - oldNode0.p);
      v1.normalize();
      auto v2 = (newNode1.p - oldNode0.p);
      v2.normalize();
      if (Vector3::Dot(v0, v1) < 0.2f || Vector3::Dot(v0, v2) < 0.2f) {
        return false;
      }
      v0 *= -1.f;
      auto v3 = (newNode0.p - oldNode1.p);
      v3.normalize();
      auto v4 = (newNode1.p - oldNode1.p);
      v4.normalize();
      if (Vector3::Dot(v0, v3) < 0.2f || Vector3::Dot(v0, v4) < 0.2f) {
        return false;
      }
      return true;
    });

  for (size_t i = 0; i < degree; i++) {
    size_t consecutiveFailedAttempts = 0;
    auto edgeIndex = random.integerExclusive(0, mesh.edges.size());
    while (!conditionalRotateEdge(mesh, edgeIndex, rotationPredicate)) {
      if (++consecutiveFailedAttempts >= mesh.edges.size()) {
        return false;
      }
      edgeIndex = (edgeIndex + 1) % mesh.edges.size();
    }
  };

  return true;
}

float Icosphere::relaxMesh(IcosahedronMesh& mesh, float multiplier)
{
  float totalSurfaceArea = 4.f * Math::PI;
  float idealFaceArea
    = totalSurfaceArea / static_cast<float>(mesh.faces.size());
  float idealEdgeLength = std::sqrt(idealFaceArea * 4.f / std::sqrt(3.f));
  float idealDistanceToCentroid = idealEdgeLength * std::sqrt(3.f) / 3.f * 0.9f;

  std::vector<Vector3> pointShifts(mesh.nodes.size());
  std::fill(pointShifts.begin(), pointShifts.end(), Vector3(0.f, 0.f, 0.f));

  size_t i = 0;
  for (auto& face : mesh.faces) {
    auto& n0      = mesh.nodes[face.n[0]];
    auto& n1      = mesh.nodes[face.n[1]];
    auto& n2      = mesh.nodes[face.n[2]];
    auto& p0      = n0.p;
    auto& p1      = n1.p;
    auto& p2      = n2.p;
    auto centroid = calculateFaceCentroid(p0, p1, p2);
    centroid.normalize();
    auto v0      = centroid - p0;
    auto v1      = centroid - p1;
    auto v2      = centroid - p2;
    auto length0 = v0.length();
    auto length1 = v1.length();
    auto length2 = v2.length();
    v0 *= (multiplier * (length0 - idealDistanceToCentroid) / length0);
    v1 *= (multiplier * (length1 - idealDistanceToCentroid) / length1);
    v2 *= (multiplier * (length2 - idealDistanceToCentroid) / length2);
    pointShifts[face.n[0]] += v0;
    pointShifts[face.n[1]] += v1;
    pointShifts[face.n[2]] += v2;
    ++i;
  }

  for (size_t i = 0; i < mesh.nodes.size(); ++i) {
    pointShifts[i] = mesh.nodes[i].p
                     + Tools::projectOnPlane(pointShifts[i], mesh.nodes[i].p);
    pointShifts[i].normalize();
  }

  Float32Array rotationSupressions(mesh.nodes.size());
  std::fill(rotationSupressions.begin(), rotationSupressions.end(), 0.f);

  for (auto& edge : mesh.edges) {
    auto& oldPoint0 = mesh.nodes[edge.n[0]].p;
    auto& oldPoint1 = mesh.nodes[edge.n[1]].p;
    auto& newPoint0 = pointShifts[edge.n[0]];
    auto& newPoint1 = pointShifts[edge.n[1]];
    auto oldVector  = oldPoint1 - oldPoint0;
    oldVector.normalize();
    auto newVector = newPoint1 - newPoint0;
    newVector.normalize();
    auto suppression = (1.f - Vector3::Dot(oldVector, newVector)) * 0.5f;
    rotationSupressions[edge.n[0]]
      = std::max(rotationSupressions[edge.n[0]], suppression);
    rotationSupressions[edge.n[1]]
      = std::max(rotationSupressions[edge.n[1]], suppression);
  }

  float totalShift = 0.f;
  i                = 0;
  for (auto& node : mesh.nodes) {
    auto& point = node.p;
    auto delta  = point;
    point       = Vector3::Lerp(point, pointShifts[i],
                          1.f - std::sqrt(rotationSupressions[i]));
    point.normalize();
    delta -= point;
    totalShift += delta.length();
    ++i;
  }

  return totalShift;
}

} // end of namespace Extensions
} // end of namespace BABYLON
