#pragma once
#include <recastnavigation/Detour/Include/DetourNavMesh.h>
#include <recastnavigation/DetourCrowd/Include/DetourCrowd.h>
#include <vector>

class dtNavMeshQuery;
class dtNavMesh;
class MeshLoader;
class NavMesh;
class rcPolyMesh;
class rcPolyMeshDetail;
struct rcConfig;

namespace BABYLON {
namespace Extensions {

struct Vec3 {
  Vec3()
  {
  }
  Vec3(float v) : x(v), y(v), z(v)
  {
  }
  Vec3(float x, float y, float z) : x(x), y(y), z(z)
  {
  }
  void isMinOf(const Vec3& v)
  {
    x = std::min(x, v.x);
    y = std::min(y, v.y);
    z = std::min(z, v.z);
  }
  void isMaxOf(const Vec3& v)
  {
    x = std::max(x, v.x);
    y = std::max(y, v.y);
    z = std::max(z, v.z);
  }
  float operator[](int index)
  {
    return (static_cast<float*>(&x))[index];
  }
  float x, y, z;
};

struct Triangle {
  Triangle()
  {
  }
  const Vec3& getPoint(long n)
  {
    if (n < 2) {
      return mPoint[n];
    }
    return mPoint[2];
  }
  Vec3 mPoint[3];
};

struct DebugNavMesh {
  int getTriangleCount()
  {
    return int(mTriangles.size());
  }
  const Triangle& getTriangle(int n)
  {
    if (n < static_cast<int>(mTriangles.size())) {
      return mTriangles[static_cast<size_t>(n)];
    }
    return mTriangles.back();
  }
  std::vector<Triangle> mTriangles;
};

struct NavPath {
  int getPointCount()
  {
    return int(mPoints.size());
  }
  const Vec3& getPoint(int n)
  {
    if (n < static_cast<int>(mPoints.size())) {
      return mPoints[static_cast<size_t>(n)];
    }
    return mPoints.back();
  }
  std::vector<Vec3> mPoints;
};

class NavMesh {
public:
  NavMesh()
      : m_navQuery(nullptr)
      , m_navMesh(nullptr)
      , m_pmesh(nullptr)
      , m_dmesh(nullptr)
      , m_navData(nullptr)
      , m_defaultQueryExtent(1.f)
  {
  }
  void destroy();
  void build(const float* positions, const int positionCount, const int* indices,
             const int indexCount, const rcConfig& config);

  DebugNavMesh getDebugNavMesh();
  Vec3 getClosestPoint(const Vec3& position);
  Vec3 getRandomPointAround(const Vec3& position, float maxRadius);
  Vec3 moveAlong(const Vec3& position, const Vec3& destination);
  dtNavMesh* getNavMesh()
  {
    return m_navMesh;
  }
  NavPath computePath(const Vec3& start, const Vec3& end) const;
  void setDefaultQueryExtent(const Vec3& extent)
  {
    m_defaultQueryExtent = extent;
  }
  Vec3 getDefaultQueryExtent() const
  {
    return m_defaultQueryExtent;
  }

protected:
  dtNavMeshQuery* m_navQuery;
  dtNavMesh* m_navMesh;
  rcPolyMesh* m_pmesh;
  rcPolyMeshDetail* m_dmesh;
  unsigned char* m_navData;
  Vec3 m_defaultQueryExtent;

  void navMeshPoly(DebugNavMesh& debugNavMesh, const dtNavMesh& mesh, dtPolyRef ref);
  void navMeshPolysWithFlags(DebugNavMesh& debugNavMesh, const dtNavMesh& mesh,
                             const unsigned short polyFlags);
};

class Crowd {
public:
  Crowd(const int maxAgents, const float maxAgentRadius, dtNavMesh* nav);
  void destroy();
  int addAgent(const Vec3& pos, const dtCrowdAgentParams* params);
  void removeAgent(const int idx);
  void update(const float dt);
  Vec3 getAgentPosition(int idx);
  Vec3 getAgentVelocity(int idx);
  void agentGoto(int idx, const Vec3& destination);
  void setDefaultQueryExtent(const Vec3& extent)
  {
    m_defaultQueryExtent = extent;
  }
  Vec3 getDefaultQueryExtent() const
  {
    return m_defaultQueryExtent;
  }

protected:
  dtCrowd* m_crowd;
  Vec3 m_defaultQueryExtent;
};

} // end of namespace Extensions
} // end of namespace BABYLON
