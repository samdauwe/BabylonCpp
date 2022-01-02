#include <babylon/extensions/recastjs/recastjs.h>

#include "DetourCommon.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshBuilder.h"
#include "DetourNavMeshQuery.h"
#include "Recast.h"

#include <algorithm>
#include <cstring>
#include <float.h>
#include <iostream>
#include <math.h>
#include <sstream>
#include <stdio.h>
#include <vector>

namespace BABYLON {
namespace Extensions {

void Log(const char* str)
{
  std::cout << std::string(str) << std::endl;
}

static int g_seed = 1337;
inline int fastrand()
{
  g_seed = (214013 * g_seed + 2531011);
  return (g_seed >> 16) & 0x7FFF;
}

inline float r01()
{
  return (static_cast<float>(fastrand())) * (1.f / 32767.f);
}

struct NavMeshintermediates {
  ~NavMeshintermediates()
  {
    if (m_solid) {
      rcFreeHeightField(m_solid);
    }
    if (m_chf) {
      rcFreeCompactHeightfield(m_chf);
    }
    if (m_cset) {
      rcFreeContourSet(m_cset);
    }
  }

  rcHeightfield* m_solid      = nullptr;
  rcCompactHeightfield* m_chf = nullptr;
  rcContourSet* m_cset        = nullptr;
};

void NavMesh::destroy()
{
  if (m_pmesh) {
    rcFreePolyMesh(m_pmesh);
  }
  if (m_dmesh) {
    rcFreePolyMeshDetail(m_dmesh);
  }
  if (m_navData) {
    dtFree(m_navData);
  }
  dtFreeNavMesh(m_navMesh);
  dtFreeNavMeshQuery(m_navQuery);
}

void NavMesh::build(const float* positions, const int /*positionCount*/, const int* indices,
                    const int indexCount, const rcConfig& config)
{
  if (m_pmesh) {
    rcFreePolyMesh(m_pmesh);
  }
  if (m_dmesh) {
    rcFreePolyMeshDetail(m_dmesh);
  }
  if (m_navData) {
    dtFree(m_navData);
  }

  NavMeshintermediates intermediates;
  std::vector<Vec3> triangleIndices;
  const float* pv = &positions[0];
  const int* t    = &indices[0];

  Vec3 bbMin(FLT_MAX);
  Vec3 bbMax(-FLT_MAX);
  triangleIndices.resize(static_cast<size_t>(indexCount));
  for (unsigned int i = 0; i < static_cast<size_t>(indexCount); i++) {
    int ind = (*t++) * 3;
    Vec3 v(pv[ind], pv[ind + 1], pv[ind + 2]);
    bbMin.isMinOf(v);
    bbMax.isMaxOf(v);
    triangleIndices[i] = v;
  }

  bool keepInterResults = false;

  // Set the area where the navigation will be build.
  // Here the bounds of the input mesh are used, but the
  // area could be specified by an user defined box, etc.
  // float bmin[3] = {-20.f, 0.f, -20.f};
  // float bmax[3] = { 20.f, 1.f,  20.f};
  rcConfig cfg               = config;
  cfg.walkableHeight         = config.walkableHeight;
  cfg.walkableClimb          = config.walkableClimb;
  cfg.walkableRadius         = config.walkableRadius;
  cfg.maxEdgeLen             = config.maxEdgeLen;
  cfg.maxSimplificationError = config.maxSimplificationError;
  cfg.minRegionArea    = static_cast<int>(rcSqr(config.minRegionArea));   // Note: area = size*size
  cfg.mergeRegionArea  = static_cast<int>(rcSqr(config.mergeRegionArea)); // Note: area = size*size
  cfg.maxVertsPerPoly  = static_cast<int>(config.maxVertsPerPoly);
  cfg.detailSampleDist = config.detailSampleDist < 0.9f ? 0 : config.cs * config.detailSampleDist;
  cfg.detailSampleMaxError = config.ch * config.detailSampleMaxError;

  rcVcopy(cfg.bmin, &bbMin.x);
  rcVcopy(cfg.bmax, &bbMax.x);

  rcCalcGridSize(cfg.bmin, cfg.bmax, cfg.cs, &cfg.width, &cfg.height);

  rcContext ctx;

  //
  // Step 2. Rasterize input polygon soup.
  //

  // Allocate voxel heightfield where we rasterize our input data to.
  intermediates.m_solid = rcAllocHeightfield();
  if (!intermediates.m_solid) {
    Log("buildNavigation: Out of memory 'solid'.");
    return;
  }
  if (!rcCreateHeightfield(&ctx, *intermediates.m_solid, cfg.width, cfg.height, cfg.bmin, cfg.bmax,
                           cfg.cs, cfg.ch)) {
    Log("buildNavigation: Could not create solid heightfield.");
    return;
  }

  std::vector<float> verts;
  verts.resize(triangleIndices.size() * 3);
  int nverts = static_cast<int>(triangleIndices.size());
  for (unsigned int i = 0; i < triangleIndices.size(); i++) {
    verts[i * 3 + 0] = triangleIndices[i].x;
    verts[i * 3 + 1] = triangleIndices[i].y;
    verts[i * 3 + 2] = triangleIndices[i].z;
  }
  size_t ntris = triangleIndices.size() / 3;
  std::vector<int> tris;
  tris.resize(triangleIndices.size());
  for (unsigned int i = 0; i < triangleIndices.size(); i++) {
    tris[i] = static_cast<int>(triangleIndices.size() - i - 1);
  }

  // Allocate array that can hold triangle area types.
  // If you have multiple meshes you need to process, allocate
  // and array which can hold the max number of triangles you need to process.
  unsigned char* triareas = static_cast<unsigned char*>(malloc(ntris));

  // Find triangles which are walkable based on their slope and rasterize them.
  // If your input data is multiple meshes, you can transform them here, calculate
  // the are type for each of the meshes and rasterize them.
  std::memset(triareas, RC_WALKABLE_AREA, ntris * sizeof(unsigned char));

  rcRasterizeTriangles(&ctx, verts.data(), nverts, tris.data(), triareas, static_cast<int>(ntris),
                       *intermediates.m_solid, cfg.walkableClimb);

  free(triareas);

  //
  // Step 3. Filter walkables surfaces.
  //

  // Once all geoemtry is rasterized, we do initial pass of filtering to
  // remove unwanted overhangs caused by the conservative rasterization
  // as well as filter spans where the character cannot possibly stand.

  rcFilterLowHangingWalkableObstacles(&ctx, cfg.walkableClimb, *intermediates.m_solid);
  rcFilterLedgeSpans(&ctx, cfg.walkableHeight, cfg.walkableClimb, *intermediates.m_solid);
  rcFilterWalkableLowHeightSpans(&ctx, cfg.walkableHeight, *intermediates.m_solid);

  //
  // Step 4. Partition walkable surface to simple regions.
  //

  // Compact the heightfield so that it is faster to handle from now on.
  // This will result more cache coherent data as well as the neighbours
  // between walkable cells will be calculated.

  intermediates.m_chf = rcAllocCompactHeightfield();
  if (!intermediates.m_chf) {
    Log("buildNavigation: Out of memory 'chf'.");
    return;
  }

  if (!rcBuildCompactHeightfield(&ctx, cfg.walkableHeight, cfg.walkableClimb,
                                 *intermediates.m_solid, *intermediates.m_chf)) {
    Log("buildNavigation: Could not build compact data.");
    return;
  }

  if (!keepInterResults) {
    rcFreeHeightField(intermediates.m_solid);
    intermediates.m_solid = nullptr;
  }

  // Erode the walkable area by agent radius.
  if (!rcErodeWalkableArea(&ctx, cfg.walkableRadius, *intermediates.m_chf)) {
    Log("buildNavigation: Could not erode.");
    return;
  }

  // Prepare for region partitioning, by calculating Distance field along the walkable surface.
  if (!rcBuildDistanceField(&ctx, *intermediates.m_chf)) {
    Log("buildNavigation: Could not build Distance field.");
    return;
  }

  // Partition the walkable surface into simple regions without holes.
  if (!rcBuildRegions(&ctx, *intermediates.m_chf, 0, cfg.minRegionArea, cfg.mergeRegionArea)) {
    Log("buildNavigation: Could not build regions.");
    return;
  }

  //
  // Step 5. Trace and simplify region contours.
  //

  // Create contours.

  intermediates.m_cset = rcAllocContourSet();
  if (!intermediates.m_cset) {
    Log("buildNavigation: Out of memory 'cset'.");
    return;
  }
  if (!rcBuildContours(&ctx, *intermediates.m_chf, cfg.maxSimplificationError, cfg.maxEdgeLen,
                       *intermediates.m_cset)) {
    Log("buildNavigation: Could not create contours.");
    return;
  }

  //
  // Step 6. Build polygons mesh from contours.
  //

  m_pmesh = rcAllocPolyMesh();
  if (!m_pmesh) {
    Log("buildNavigation: Out of memory 'pmesh'.");
    return;
  }
  if (!rcBuildPolyMesh(&ctx, *intermediates.m_cset, cfg.maxVertsPerPoly, *m_pmesh)) {
    Log("buildNavigation: Could not triangulate contours.");
    return;
  }

  //
  // Step 7. Create detail mesh which allows to access approximate height on each polygon.
  //
  m_dmesh = rcAllocPolyMeshDetail();
  if (!m_dmesh) {
    Log("buildNavigation: Out of memory 'pmdtl'.");
    return;
  }

  if (!rcBuildPolyMeshDetail(&ctx, *m_pmesh, *intermediates.m_chf, cfg.detailSampleDist,
                             cfg.detailSampleMaxError, *m_dmesh)) {
    Log("buildNavigation: Could not build detail mesh.");
    return;
  }

  if (!keepInterResults) {
    rcFreeCompactHeightfield(intermediates.m_chf);
    intermediates.m_chf = nullptr;
    rcFreeContourSet(intermediates.m_cset);
    intermediates.m_cset = nullptr;
  }

  //
  // (Optional) Step 8. Create Detour data from Recast poly mesh.
  //

  // Only build the detour navmesh if we do not exceed the limit.
  if (cfg.maxVertsPerPoly <= DT_VERTS_PER_POLYGON) {
    rcPolyMesh* pmesh       = m_pmesh;
    rcPolyMeshDetail* dmesh = m_dmesh;

    int navDataSize = 0;

    // Update poly flags from areas.
    for (int i = 0; i < pmesh->npolys; ++i) {
      if (pmesh->areas[i] == RC_WALKABLE_AREA) {
        pmesh->areas[i] = 0;
      }
      if (pmesh->areas[i] == 0) {
        pmesh->flags[i] = 1;
      }
    }

    dtNavMeshCreateParams params;
    memset(&params, 0, sizeof(params));
    params.verts            = pmesh->verts;
    params.vertCount        = pmesh->nverts;
    params.polys            = pmesh->polys;
    params.polyAreas        = pmesh->areas;
    params.polyFlags        = pmesh->flags;
    params.polyCount        = pmesh->npolys;
    params.nvp              = pmesh->nvp;
    params.detailMeshes     = dmesh->meshes;
    params.detailVerts      = dmesh->verts;
    params.detailVertsCount = dmesh->nverts;
    params.detailTris       = dmesh->tris;
    params.detailTriCount   = dmesh->ntris;
    // optional connection between areas
    params.offMeshConVerts  = nullptr; // geom->getOffMeshConnectionVerts();
    params.offMeshConRad    = nullptr; // geom->getOffMeshConnectionRads();
    params.offMeshConDir    = nullptr; // geom->getOffMeshConnectionDirs();
    params.offMeshConAreas  = nullptr; // geom->getOffMeshConnectionAreas();
    params.offMeshConFlags  = nullptr; // geom->getOffMeshConnectionFlags();
    params.offMeshConUserID = nullptr; // geom->getOffMeshConnectionId();
    params.offMeshConCount  = 0;       // geom->getOffMeshConnectionCount();
    params.walkableHeight   = static_cast<float>(config.walkableHeight);
    params.walkableRadius   = static_cast<float>(config.walkableRadius);
    params.walkableClimb    = static_cast<float>(config.walkableClimb);
    rcVcopy(params.bmin, pmesh->bmin);
    rcVcopy(params.bmax, pmesh->bmax);
    params.cs          = cfg.cs;
    params.ch          = cfg.ch;
    params.buildBvTree = true;

    if (!dtCreateNavMeshData(&params, &m_navData, &navDataSize)) {
      Log("Could not build Detour navmesh.");
      return;
    }

    m_navMesh = dtAllocNavMesh();
    if (!m_navMesh) {
      Log("Could not create Detour navmesh");
      return;
    }

    dtStatus status;

    status = m_navMesh->init(m_navData, navDataSize, DT_TILE_FREE_DATA);
    if (dtStatusFailed(status)) {
      Log("Could not init Detour navmesh");
      return;
    }

    m_navQuery = dtAllocNavMeshQuery();
    if (!m_navQuery) {
      dtFreeNavMesh(m_navMesh);
      m_navMesh = nullptr;
      Log("Could not allocate Navmesh query");
      return;
    }
    status = m_navQuery->init(m_navMesh, 2048);
    if (dtStatusFailed(status)) {
      dtFreeNavMesh(m_navMesh);
      m_navMesh = nullptr;
      Log("Could not init Detour navmesh query");
      return;
    }
  }
  Log("Done");
}

static const int NAVMESHSET_MAGIC   = 'M' << 24 | 'S' << 16 | 'E' << 8 | 'T'; //'MSET';
static const int NAVMESHSET_VERSION = 1;

struct NavMeshSetHeader {
  int magic;
  int version;
  int numTiles;
  dtNavMeshParams params;
};

struct NavMeshTileHeader {
  dtTileRef tileRef;
  int dataSize;
};

void NavMesh::buildFromNavmeshData(NavmeshData* navmeshData)
{
  destroy();
  unsigned char* bits = static_cast<unsigned char*>(navmeshData->dataPointer);

  // Read header.
  NavMeshSetHeader header;
  size_t readLen = sizeof(NavMeshSetHeader);
  memcpy(&header, bits, readLen);
  bits += readLen;

  if (header.magic != NAVMESHSET_MAGIC) {
    return;
  }
  if (header.version != NAVMESHSET_VERSION) {
    return;
  }

  dtNavMesh* mesh = dtAllocNavMesh();
  if (!mesh) {
    return;
  }
  dtStatus status = mesh->init(&header.params);
  if (dtStatusFailed(status)) {
    return;
  }

  // Read tiles.
  for (int i = 0; i < header.numTiles; ++i) {
    NavMeshTileHeader tileHeader;
    readLen = sizeof(tileHeader);
    memcpy(&tileHeader, bits, readLen);
    bits += readLen;

    if (!tileHeader.tileRef || !tileHeader.dataSize) {
      break;
    }

    unsigned char* data = static_cast<unsigned char*>(
      dtAlloc(static_cast<size_t>(tileHeader.dataSize), DT_ALLOC_PERM));
    if (!data) {
      break;
    }

    readLen = static_cast<size_t>(tileHeader.dataSize);
    memcpy(data, bits, readLen);
    bits += readLen;

    mesh->addTile(data, tileHeader.dataSize, DT_TILE_FREE_DATA, tileHeader.tileRef, nullptr);
  }

  m_navMesh = mesh;

  m_navQuery = dtAllocNavMeshQuery();
  if (!m_navQuery) {
    dtFreeNavMesh(m_navMesh);
    m_navMesh = nullptr;
    Log("Load navmesh data: Could not allocate Navmesh query");
    return;
  }
  status = m_navQuery->init(m_navMesh, 2048);
  if (dtStatusFailed(status)) {
    dtFreeNavMesh(m_navMesh);
    m_navMesh = nullptr;
    Log("Load navmesh data: Could not init Detour navmesh query");
    return;
  }
}

NavmeshData NavMesh::getNavmeshData() const
{
  if (!m_navMesh) {
    return {nullptr, 0};
  }
  unsigned char* bits   = nullptr;
  size_t bitsSize       = 0;
  const dtNavMesh* mesh = m_navMesh;

  // Store header.
  NavMeshSetHeader header;
  header.magic    = NAVMESHSET_MAGIC;
  header.version  = NAVMESHSET_VERSION;
  header.numTiles = 0;
  for (int i = 0; i < mesh->getMaxTiles(); ++i) {
    const dtMeshTile* tile = mesh->getTile(i);
    if (!tile || !tile->header || !tile->dataSize)
      continue;
    header.numTiles++;
  }
  memcpy(&header.params, mesh->getParams(), sizeof(dtNavMeshParams));
  bits = static_cast<unsigned char*>(realloc(bits, bitsSize + sizeof(NavMeshSetHeader)));
  memcpy(&bits[bitsSize], &header, sizeof(NavMeshSetHeader));
  bitsSize += sizeof(NavMeshSetHeader);

  // Store tiles.
  for (int i = 0; i < mesh->getMaxTiles(); ++i) {
    const dtMeshTile* tile = mesh->getTile(i);
    if (!tile || !tile->header || !tile->dataSize)
      continue;

    NavMeshTileHeader tileHeader;
    tileHeader.tileRef  = mesh->getTileRef(tile);
    tileHeader.dataSize = tile->dataSize;

    bits = static_cast<unsigned char*>(realloc(bits, bitsSize + sizeof(tileHeader)));
    memcpy(&bits[bitsSize], &tileHeader, sizeof(tileHeader));
    bitsSize += sizeof(tileHeader);

    bits
      = static_cast<unsigned char*>(realloc(bits, bitsSize + static_cast<size_t>(tile->dataSize)));
    memcpy(&bits[bitsSize], tile->data, static_cast<size_t>(tile->dataSize));
    bitsSize += static_cast<size_t>(tile->dataSize);
  }

  NavmeshData navmeshData;
  navmeshData.dataPointer = bits;
  navmeshData.size        = int(bitsSize);
  return navmeshData;
}

void NavMesh::freeNavmeshData(NavmeshData* navmeshData)
{
  free(navmeshData->dataPointer);
}

void NavMesh::navMeshPoly(DebugNavMesh& debugNavMesh, const dtNavMesh& mesh, dtPolyRef ref)
{
  const dtMeshTile* tile = nullptr;
  const dtPoly* poly     = nullptr;
  if (dtStatusFailed(mesh.getTileAndPolyByRef(ref, &tile, &poly)))
    return;

  const unsigned int ip = static_cast<unsigned int>(poly - tile->polys);

  if (poly->getType() == DT_POLYTYPE_OFFMESH_CONNECTION) {
    /*
    If we want to display links (teleport) between navmesh or inside a navmesh
    this code will be usefull for debug output.

    dtOffMeshConnection* con = &tile->offMeshCons[ip - tile->header->offMeshBase];

    dd->begin(DU_DRAW_LINES, 2.0f);

    // Connection arc.
    duAppendArc(dd, con->pos[0],con->pos[1],con->pos[2], con->pos[3],con->pos[4],con->pos[5], 0.25f,
                (con->flags & 1) ? 0.6f : 0.0f, 0.6f, c);

    dd->end();
    */
  }
  else {
    const dtPolyDetail* pd = &tile->detailMeshes[ip];

    for (unsigned int i = 0; i < pd->triCount; ++i) {
      const unsigned char* t = &tile->detailTris[(pd->triBase + i) * 4];
      Triangle triangle;
      float* pf;

      for (int j = 0; j < 3; ++j) {
        if (t[j] < poly->vertCount) {
          pf = &tile->verts[poly->verts[t[j]] * 3];
        }
        else {
          pf = &tile->detailVerts[(pd->vertBase + t[j] - poly->vertCount) * 3];
        }

        triangle.mPoint[2 - j] = Vec3(pf[0], pf[1], pf[2]);
      }
      debugNavMesh.mTriangles.push_back(triangle);
    }
  }
}

void NavMesh::navMeshPolysWithFlags(DebugNavMesh& debugNavMesh, const dtNavMesh& mesh,
                                    const unsigned short polyFlags)
{
  for (int i = 0; i < mesh.getMaxTiles(); ++i) {
    const dtMeshTile* tile = mesh.getTile(i);
    if (!tile->header) {
      continue;
    }
    dtPolyRef base = mesh.getPolyRefBase(tile);

    for (int j = 0; j < tile->header->polyCount; ++j) {
      const dtPoly* p = &tile->polys[j];
      if ((p->flags & polyFlags) == 0) {
        continue;
      }
      navMeshPoly(debugNavMesh, mesh, base | static_cast<dtPolyRef>(j));
    }
  }
}

DebugNavMesh NavMesh::getDebugNavMesh()
{
  DebugNavMesh debugNavMesh;
  navMeshPolysWithFlags(debugNavMesh, *m_navMesh, 0xFFFF);
  return debugNavMesh;
}

Vec3 NavMesh::getClosestPoint(const Vec3& position)
{
  dtQueryFilter filter;
  filter.setIncludeFlags(0xffff);
  filter.setExcludeFlags(0);

  dtPolyRef polyRef;

  Vec3 pos(position.x, position.y, position.z);
  m_navQuery->findNearestPoly(&pos.x, &m_defaultQueryExtent.x, &filter, &polyRef, nullptr);

  bool posOverlay;
  Vec3 resDetour;
  dtStatus status = m_navQuery->closestPointOnPoly(polyRef, &pos.x, &resDetour.x, &posOverlay);

  if (dtStatusFailed(status)) {
    return Vec3(0.f, 0.f, 0.f);
  }
  return Vec3(resDetour.x, resDetour.y, resDetour.z);
}

Vec3 NavMesh::getRandomPointAround(const Vec3& position, float maxRadius)
{
  dtQueryFilter filter;
  filter.setIncludeFlags(0xffff);
  filter.setExcludeFlags(0);

  dtPolyRef polyRef;

  Vec3 pos(position.x, position.y, position.z);

  m_navQuery->findNearestPoly(&pos.x, &m_defaultQueryExtent.x, &filter, &polyRef, nullptr);

  dtPolyRef randomRef;
  Vec3 resDetour;
  dtStatus status = m_navQuery->findRandomPointAroundCircle(polyRef, &position.x, maxRadius,
                                                            &filter, r01, &randomRef, &resDetour.x);
  if (dtStatusFailed(status)) {
    return Vec3(0.f, 0.f, 0.f);
  }

  return Vec3(resDetour.x, resDetour.y, resDetour.z);
}

Vec3 NavMesh::moveAlong(const Vec3& position, const Vec3& destination)
{
  dtQueryFilter filter;
  filter.setIncludeFlags(0xffff);
  filter.setExcludeFlags(0);

  dtPolyRef polyRef;

  Vec3 pos(position.x, position.y, position.z);
  Vec3 dest(destination.x, destination.y, destination.z);

  m_navQuery->findNearestPoly(&pos.x, &m_defaultQueryExtent.x, &filter, &polyRef, nullptr);

  Vec3 resDetour;
  dtPolyRef visitedPoly[128];
  int visitedPolyCount;
  dtStatus status
    = m_navQuery->moveAlongSurface(polyRef, &pos.x, &dest.x, &filter, &resDetour.x, visitedPoly,
                                   &visitedPolyCount, sizeof(visitedPoly) / sizeof(dtPolyRef));
  if (dtStatusFailed(status)) {
    return Vec3(0.f, 0.f, 0.f);
  }
  return Vec3(resDetour.x, resDetour.y, resDetour.z);
}

NavPath NavMesh::computePath(const Vec3& start, const Vec3& end) const
{
  NavPath navpath;
  static const int MAX_POLYS = 256;
  float straightPath[MAX_POLYS * 3];

  dtPolyRef startRef;
  dtPolyRef endRef;

  dtQueryFilter filter;
  filter.setIncludeFlags(0xffff);
  filter.setExcludeFlags(0);

  Vec3 posStart(start.x, start.y, start.z);
  Vec3 posEnd(end.x, end.y, end.z);

  m_navQuery->findNearestPoly(&posStart.x, &m_defaultQueryExtent.x, &filter, &startRef, nullptr);
  m_navQuery->findNearestPoly(&posEnd.x, &m_defaultQueryExtent.x, &filter, &endRef, nullptr);

  dtPolyRef polys[MAX_POLYS];
  int npolys;

  m_navQuery->findPath(startRef, endRef, &posStart.x, &posEnd.x, &filter, polys, &npolys,
                       MAX_POLYS);
  int mNstraightPath = 0;
  if (npolys) {
    unsigned char straightPathFlags[MAX_POLYS];
    dtPolyRef straightPathPolys[MAX_POLYS];
    int straightPathOptions;
    bool posOverPoly;
    Vec3 closestEnd = posEnd;

    if (polys[npolys - 1] != endRef) {
      m_navQuery->closestPointOnPoly(polys[npolys - 1], &end.x, &closestEnd.x, &posOverPoly);
    }
    straightPathOptions = 0;
    m_navQuery->findStraightPath(&posStart.x, &closestEnd.x, polys, npolys, straightPath,
                                 straightPathFlags, straightPathPolys, &mNstraightPath, MAX_POLYS,
                                 straightPathOptions);

    navpath.mPoints.resize(static_cast<size_t>(mNstraightPath));
    for (size_t i = 0; i < static_cast<size_t>(mNstraightPath); i++) {
      navpath.mPoints[i]
        = Vec3(straightPath[i * 3], straightPath[i * 3 + 1], straightPath[i * 3 + 2]);
    }
  }
  return navpath;
}

Crowd::Crowd(const int maxAgents, const float maxAgentRadius, dtNavMesh* nav)
    : m_defaultQueryExtent(1.f)
{
  m_crowd = dtAllocCrowd();
  m_crowd->init(maxAgents, maxAgentRadius, nav);
}

void Crowd::destroy()
{
  if (m_crowd) {
    dtFreeCrowd(m_crowd);
    m_crowd = nullptr;
  }
}

int Crowd::addAgent(const Vec3& pos, const dtCrowdAgentParams* params)
{
  return m_crowd->addAgent(&pos.x, params);
}

void Crowd::removeAgent(const int idx)
{
  m_crowd->removeAgent(idx);
}

void Crowd::update(const float dt)
{
  m_crowd->update(dt, nullptr);
}

Vec3 Crowd::getAgentPosition(int idx)
{
  const dtCrowdAgent* agent = m_crowd->getAgent(idx);
  return Vec3(agent->npos[0], agent->npos[1], agent->npos[2]);
}

Vec3 Crowd::getAgentVelocity(int idx)
{
  const dtCrowdAgent* agent = m_crowd->getAgent(idx);
  return Vec3(agent->vel[0], agent->vel[1], agent->vel[2]);
}

Vec3 Crowd::getAgentNextTargetPath(int idx)
{
  const dtCrowdAgent* agent = m_crowd->getAgent(idx);
  return Vec3(agent->cornerVerts[0], agent->cornerVerts[1], agent->cornerVerts[2]);
}

int Crowd::getAgentState(int idx)
{
  const dtCrowdAgent* agent = m_crowd->getAgent(idx);
  return agent->state;
}

bool Crowd::overOffmeshConnection(int idx)
{
  const dtCrowdAgent* agent = m_crowd->getAgent(idx);
  const float triggerRadius = agent->params.radius * 2.25f;
  if (!agent->ncorners)
    return false;
  const bool offMeshConnection
    = (agent->cornerFlags[agent->ncorners - 1] & DT_STRAIGHTPATH_OFFMESH_CONNECTION) ? true : false;
  if (offMeshConnection) {
    const float distSq = dtVdist2DSqr(agent->npos, &agent->cornerVerts[(agent->ncorners - 1) * 3]);
    if (distSq < triggerRadius * triggerRadius) {
      return true;
    }
  }
  return false;
}

void Crowd::agentGoto(int idx, const Vec3& destination)
{
  dtQueryFilter filter;
  filter.setIncludeFlags(0xffff);
  filter.setExcludeFlags(0);

  dtPolyRef polyRef;

  Vec3 pos(destination.x, destination.y, destination.z);
  m_crowd->getNavMeshQuery()->findNearestPoly(&pos.x, &m_defaultQueryExtent.x, &filter, &polyRef,
                                              nullptr);

  m_crowd->requestMoveTarget(idx, polyRef, &pos.x);
}

void Crowd::agentTeleport(int idx, const Vec3& destination)
{
  if (idx < 0 || idx > m_crowd->getAgentCount()) {
    return;
  }

  dtQueryFilter filter;
  filter.setIncludeFlags(0xffff);
  filter.setExcludeFlags(0);

  dtPolyRef polyRef = 0;

  Vec3 pos(destination.x, destination.y, destination.z);
  m_crowd->getNavMeshQuery()->findNearestPoly(&pos.x, &m_defaultQueryExtent.x, &filter, &polyRef,
                                              nullptr);

  dtCrowdAgent* ag = m_crowd->getEditableAgent(idx);

  float nearest[3];
  dtVcopy(nearest, &pos.x);

  ag->corridor.reset(polyRef, nearest);
  ag->boundary.reset();
  ag->partial = false;

  ag->topologyOptTime  = 0;
  ag->targetReplanTime = 0;
  ag->nneis            = 0;

  dtVset(ag->dvel, 0, 0, 0);
  dtVset(ag->nvel, 0, 0, 0);
  dtVset(ag->vel, 0, 0, 0);
  dtVcopy(ag->npos, nearest);

  ag->desiredSpeed = 0;

  if (polyRef)
    ag->state = DT_CROWDAGENT_STATE_WALKING;
  else
    ag->state = DT_CROWDAGENT_STATE_INVALID;

  ag->targetState = DT_CROWDAGENT_TARGET_NONE;
}

dtCrowdAgentParams Crowd::getAgentParameters(const int idx)
{
  dtCrowdAgentParams params;
  const dtCrowdAgent* agent = m_crowd->getAgent(idx);
  params                    = agent->params;
  return params;
}

void Crowd::setAgentParameters(const int idx, const dtCrowdAgentParams* params)
{
  m_crowd->updateAgentParameters(idx, params);
}

} // end of namespace Extensions
} // end of namespace BABYLON
