#include <babylon/extensions/hexplanetgeneration/world.h>

#include <cmath>

#include <babylon/core/logging.h>
#include <babylon/extensions/hexplanetgeneration/icosphere.h>
#include <babylon/extensions/hexplanetgeneration/planet.h>
#include <babylon/extensions/hexplanetgeneration/planet_statistics.h>
#include <babylon/extensions/hexplanetgeneration/render_data.h>
#include <babylon/extensions/hexplanetgeneration/terrain/distance_corner.h>
#include <babylon/extensions/hexplanetgeneration/terrain/elevation_border.h>
#include <babylon/extensions/hexplanetgeneration/terrain/plate.h>
#include <babylon/extensions/hexplanetgeneration/terrain/spatial_partition.h>
#include <babylon/extensions/hexplanetgeneration/terrain/stress.h>
#include <babylon/extensions/hexplanetgeneration/terrain/topology.h>
#include <babylon/extensions/hexplanetgeneration/terrain/whorl.h>
#include <babylon/extensions/hexplanetgeneration/utils/matrix3.h>
#include <babylon/extensions/hexplanetgeneration/utils/tools.h>
#include <babylon/extensions/hexplanetgeneration/utils/xor_shift_128.h>
#include <babylon/math/color4.h>
#include <babylon/misc/tools.h>

namespace BABYLON {
namespace Extensions {

const size_t World::UdefIdx = std::numeric_limits<size_t>::max();

World::World() = default;

World::~World() = default;

void World::generatePlanet(Planet*& planet, unsigned long originalSeed, unsigned long seed,
                           size_t icosahedronSubdivision, float topologyDistortionRate,
                           size_t plateCount, float oceanicRate, float heatLevel,
                           float moistureLevel)
{
  XorShift128 random(seed);

  generatePlanet(icosahedronSubdivision, topologyDistortionRate, plateCount, oceanicRate, heatLevel,
                 moistureLevel, random, *planet);
  planet->seed         = seed;
  planet->originalSeed = originalSeed;
}

void World::generatePlanet(size_t icosahedronSubdivision, float topologyDistortionRate,
                           size_t plateCount, float oceanicRate, float heatLevel,
                           float moistureLevel, IRandomFunction& random, Planet& planet)
{
  auto mesh
    = Icosphere::generateIcosahedronMesh(icosahedronSubdivision, topologyDistortionRate, random);

  generatePlanetTopology(mesh, planet.topology);

  generatePlanetPartition(planet.topology.tiles, planet.partition);
  generatePlanetTerrain(planet, plateCount, oceanicRate, heatLevel, moistureLevel, random);
  generatePlanetRenderData(planet.topology, random, planet.renderData);
  generatePlanetStatistics(planet.topology, planet.plates, planet.statistics);
}

bool World::generatePlanetTopology(const IcosahedronMesh& mesh, Topology& ret)
{
  auto& corners = ret.corners;
  auto& borders = ret.borders;
  auto& tiles   = ret.tiles;

  corners.reserve(mesh.faces.size());
  for (size_t i = 0; i < mesh.faces.size(); ++i) {
    const auto& face = mesh.faces[i];
    corners.emplace_back(
      Corner(i, face.centroid * 1000, face.e.size(), face.e.size(), face.n.size()));
  }

  borders.reserve(mesh.edges.size());
  for (size_t i = 0; i < mesh.edges.size(); ++i) {
    borders.emplace_back(Border(i, 2, 4, 2));
  }

  tiles.reserve(mesh.nodes.size());
  for (size_t i = 0; i < mesh.nodes.size(); ++i) {
    const auto& node = mesh.nodes[i];
    tiles.emplace_back(Tile(i, node.p * 1000, node.f.size(), node.e.size(), node.e.size()));
  }

  for (size_t i = 0; i < corners.size(); ++i) {
    auto& corner = corners[i];
    auto& face   = mesh.faces[i];
    for (size_t j = 0; j < face.e.size(); ++j) {
      corner.borders[j] = &borders[face.e[j]];
    }
    for (size_t j = 0; j < face.n.size(); ++j) {
      corner.tiles[j] = &tiles[face.n[j]];
    }
  }

  for (size_t i = 0; i < borders.size(); ++i) {
    auto& border = borders[i];
    auto& edge   = mesh.edges[i];
    Vector3 averageCorner(0.f, 0.f, 0.f);
    size_t n = 0;
    for (size_t j = 0; j < edge.f.size(); ++j) {
      auto& corner = corners[edge.f[j]];
      averageCorner += corner.position;
      border.corners[j] = &corner;
      for (auto& pcornerBorder : corner.borders) {
        if (pcornerBorder != &border) {
          border.borders[n++] = pcornerBorder;
        }
      }
    }
    border.midpoint = averageCorner * (1.f / static_cast<float>(border.corners.size()));
    for (size_t j = 0; j < edge.n.size(); ++j) {
      border.tiles[j] = &tiles[edge.n[j]];
    }
  }

  for (auto& corner : corners) {
    for (size_t j = 0; j < corner.borders.size(); ++j) {
      corner.corners[j] = &corner.borders[j]->oppositeCorner(corner);
    }
  }

  for (size_t i = 0; i < tiles.size(); ++i) {
    auto& tile = tiles[i];
    auto& node = mesh.nodes[i];
    for (size_t j = 0; j < node.f.size(); ++j) {
      tile.corners[j] = &corners[node.f[j]];
    }
    for (size_t j = 0; j < node.e.size(); ++j) {
      auto& border = borders[node.e[j]];
      if (border.tiles[0] == &tile) {
        for (size_t k = 0; k < tile.corners.size(); ++k) {
          auto& corner0 = *tile.corners[k];
          auto& corner1 = *tile.corners[(k + 1) % tile.corners.size()];
          if (border.corners[1] == &corner0 && border.corners[0] == &corner1) {
            border.corners[0] = &corner0;
            border.corners[1] = &corner1;
          }
          else if (border.corners[0] != &corner0 || border.corners[1] != &corner1) {
            continue;
          }
          tile.borders[k] = &border;
          tile.tiles[k]   = &border.oppositeTile(tile);
          break;
        }
      }
      else {
        for (size_t k = 0; k < tile.corners.size(); ++k) {
          auto& corner0 = *tile.corners[k];
          auto& corner1 = *tile.corners[(k + 1) % tile.corners.size()];
          if (border.corners[0] == &corner0 && border.corners[1] == &corner1) {
            border.corners[1] = &corner0;
            border.corners[0] = &corner1;
          }
          else if (border.corners[1] != &corner0 || border.corners[0] != &corner1) {
            continue;
          }
          tile.borders[k] = &border;
          tile.tiles[k]   = &border.oppositeTile(tile);
          break;
        }
      }
    }

    tile.averagePosition = Vector3(0.f, 0.f, 0.f);
    for (size_t j = 0; j < tile.corners.size(); ++j) {
      tile.averagePosition += tile.corners[j]->position;
    }
    tile.averagePosition *= (1.f / static_cast<float>(tile.corners.size()));

    float maxDistanceToCorner = 0.f;
    for (size_t j = 0; j < tile.corners.size(); ++j) {
      maxDistanceToCorner = std::max(
        maxDistanceToCorner, Vector3::Distance(tile.corners[j]->position, tile.averagePosition));
    }

    float area = 0;
    for (size_t j = 0; j < tile.borders.size(); ++j) {
      area += Tools::calculateTriangleArea(tile.position, tile.borders[j]->corners[0]->position,
                                           tile.borders[j]->corners[1]->position);
    }
    tile.area   = area;
    tile.normal = tile.position;
    tile.normal.normalize();
    tile.boundingSphere
      = BoundingSphere(tile.averagePosition, Vector3(maxDistanceToCorner, 0.f, 0.f));
  }

  for (auto& corner : corners) {
    corner.area = 0.f;
    for (size_t j = 0; j < corner.tiles.size(); ++j) {
      corner.area += corner.tiles[j]->area / static_cast<float>(corner.tiles[j]->corners.size());
    }
  }

  return true;
}

void World::generatePlanetPartition(std::vector<Tile>& tiles, SpatialPartition& rootPartition)
{
  IcosahedronMesh icosahedron;
  Icosphere::generateIcosahedron(icosahedron);

  for (auto& face : icosahedron.faces) {
    auto p0             = icosahedron.nodes[face.n[0]].p * 1000;
    auto p1             = icosahedron.nodes[face.n[1]].p * 1000;
    auto p2             = icosahedron.nodes[face.n[2]].p * 1000;
    auto center         = (p0 + p1 + p2) / 3.;
    auto radius         = std::max(Vector3::Distance(center, p0),
                           std::max(Vector3::Distance(center, p2), Vector3::Distance(center, p2)));
    face.boundingSphere = BoundingSphere(center, Vector3(radius, 0.f, 0.f));
  }

  std::vector<Tile*> unparentedTiles;
  float maxDistanceFromOrigin = 0.f;
  for (auto& tile : tiles) {
    maxDistanceFromOrigin = std::max(maxDistanceFromOrigin, tile.boundingSphere.center.length()
                                                              + tile.boundingSphere.radius);

    bool parentFound = false;
    for (auto& face : icosahedron.faces) {
      float distance = Vector3::Distance(tile.boundingSphere.center, face.boundingSphere.center)
                       + tile.boundingSphere.radius;
      if (distance < face.boundingSphere.radius) {
        face.children.emplace_back(&tile);
        parentFound = true;
        break;
      }
    }
    if (!parentFound) {
      unparentedTiles.emplace_back(&tile);
    }
  }

  rootPartition.boundingSphere
    = BoundingSphere(Vector3(0.f, 0.f, 0.f), Vector3(maxDistanceFromOrigin, 0.f, 0.f));
  rootPartition.tiles = unparentedTiles;
  for (auto& face : icosahedron.faces) {
    rootPartition.partitions.emplace_back(SpatialPartition(face.boundingSphere, {}, face.children));
  }
}

void World::generatePlanetTerrain(Planet& planet, size_t plateCount, float oceanicRate,
                                  float heatLevel, float moistureLevel, IRandomFunction& random)
{
  generatePlanetTectonicPlates(planet.topology, plateCount, oceanicRate, random, planet.plates);
  generatePlanetElevation(planet.topology, planet.plates);
  generatePlanetWeather(planet.topology, planet.partition, heatLevel, moistureLevel, random);
  generatePlanetBiomes(planet.topology.tiles, 1000);
}

void World::generatePlanetTectonicPlates(Topology& topology, size_t plateCount, float oceanicRate,
                                         IRandomFunction& random, std::vector<Plate>& plates)
{
  struct Plateless {
    Tile* tile;
    Plate* plate;
  };
  std::vector<Plateless> plateless;

  plates.reserve(plateCount);

  auto failedCount = 0;
  while (plates.size() < plateCount && failedCount < 10000) {
    auto& corner = topology.corners[random.integerExclusive(0, topology.corners.size())];
    bool adjacentToExistingPlate = false;
    for (size_t i = 0; i < corner.tiles.size(); ++i) {
      if (corner.tiles[i]->plate) {
        adjacentToExistingPlate = true;
        failedCount += 1;
        break;
      }
    }
    if (adjacentToExistingPlate) {
      continue;
    }

    failedCount = 0;

    bool oceanic = (random.unit() < oceanicRate);
    plates.emplace_back(Plate(
      Color3(random.realInclusive(0, 1), random.realInclusive(0, 1), random.realInclusive(0, 1)),
      Tools::randomUnitVector(random), random.realInclusive(-Math::PI / 30.f, Math::PI / 30.f),
      random.realInclusive(-Math::PI / 30.f, Math::PI / 30.f),
      oceanic ? random.realInclusive(-0.8f, -0.3f) : random.realInclusive(0.1f, 0.5f), oceanic,
      corner));
    auto pplate = &plates.back();
    auto& plate = plates.back();

    for (size_t i = 0; i < corner.tiles.size(); ++i) {
      corner.tiles[i]->plate = &plate;
      plate.tiles.emplace_back(corner.tiles[i]);
    }

    for (size_t i = 0; i < corner.tiles.size(); ++i) {
      auto& tile = *corner.tiles[i];
      for (size_t j = 0; j < tile.tiles.size(); ++j) {
        auto& adjacentTile = *tile.tiles[j];
        if (!adjacentTile.plate) {
          plateless.emplace_back(Plateless{&adjacentTile, pplate});
        }
      }
    }
  }

  while (plateless.size() > 0) {
    size_t index = static_cast<size_t>(std::floor(std::pow(random.unit(), 2.f) * plateless.size()));
    auto indexit = getAdvancedIt(plateless, index);
    auto ptile   = indexit->tile;
    auto pplate  = indexit->plate;
    auto& tile   = *ptile;
    auto& plate  = *pplate;
    plateless.erase(indexit);
    if (!tile.plate) {
      tile.plate = &plate;
      plate.tiles.emplace_back(ptile);
      for (size_t j = 0; j < tile.tiles.size(); ++j) {
        if (!tile.tiles[j]->plate) {
          plateless.emplace_back(Plateless{tile.tiles[j], &plate});
        }
      }
    }
  }

  calculateCornerDistancesToPlateRoot(plates);
}

void World::calculateCornerDistancesToPlateRoot(std::vector<Plate>& plates)
{
  std::vector<DistanceCorner> distanceCornerQueue;
  for (size_t i = 0; i < plates.size(); ++i) {
    auto& corner               = *plates[i].root;
    corner.distanceToPlateRoot = 0;
    for (size_t j = 0; j < corner.corners.size(); ++j) {
      distanceCornerQueue.emplace_back(
        DistanceCorner{corner.corners[j], corner.borders[j]->length()});
    }
  }

  while (distanceCornerQueue.size() > 0) {
    size_t iEnd = distanceCornerQueue.size();
    for (size_t i = 0; i < iEnd; ++i) {
      auto& front              = getAdvancedItVal(distanceCornerQueue, i);
      auto& corner             = *front.corner;
      auto distanceToPlateRoot = front.distanceToPlateRoot;
      if (corner.distanceToPlateRoot == 0.f || corner.distanceToPlateRoot > distanceToPlateRoot) {
        corner.distanceToPlateRoot = distanceToPlateRoot;
        for (size_t j = 0; j < corner.corners.size(); ++j) {
          distanceCornerQueue.emplace_back(
            DistanceCorner{corner.corners[j], distanceToPlateRoot + corner.borders[j]->length()});
        }
      }
    }
    auto itend = distanceCornerQueue.begin();
    std::advance(itend, iEnd);
    distanceCornerQueue.erase(distanceCornerQueue.begin(), itend);

    std::sort(distanceCornerQueue.begin(), distanceCornerQueue.end(),
              [](DistanceCorner const& left, DistanceCorner const& right) {
                return left.distanceToPlateRoot < right.distanceToPlateRoot;
              });
  }
}

void World::generatePlanetElevation(Topology& topology, std::vector<Plate>& /*plates*/)
{
  std::vector<Corner*> boundaryCorners;
  std::vector<size_t> boundaryCornerInnerBorderIndexes;
  std::vector<ElevationBorder> elevationBorderQueue;
  std::function<bool(const ElevationBorder&, const ElevationBorder&)> elevationBorderQueueSorter
    = [](const ElevationBorder& left, const ElevationBorder& right) {
        return left.distanceToPlateBoundary < right.distanceToPlateBoundary;
      };

  identifyBoundaryBorders(topology.borders);
  collectBoundaryCorners(topology.corners, boundaryCorners);
  calculatePlateBoundaryStress(boundaryCorners, boundaryCornerInnerBorderIndexes);
  blurPlateBoundaryStress(boundaryCorners, 3, 0.4f);
  populateElevationBorderQueue(boundaryCorners, boundaryCornerInnerBorderIndexes,
                               elevationBorderQueue);
  processElevationBorderQueue(elevationBorderQueue, elevationBorderQueueSorter);
  calculateTileAverageElevations(topology.tiles);
}

void World::identifyBoundaryBorders(std::vector<Border>& borders)
{
  for (auto& border : borders) {
    if (border.tiles[0]->plate != border.tiles[1]->plate) {
      border.betweenPlates             = true;
      border.corners[0]->betweenPlates = true;
      border.corners[1]->betweenPlates = true;
      border.tiles[0]->plate->boundaryBorders.emplace_back(&border);
      border.tiles[1]->plate->boundaryBorders.emplace_back(&border);
    }
  }
}

void World::collectBoundaryCorners(std::vector<Corner>& corners,
                                   std::vector<Corner*>& boundaryCorners)
{
  for (auto& corner : corners) {
    if (corner.betweenPlates) {
      boundaryCorners.emplace_back(&corner);
      corner.tiles[0]->plate->boundaryCorners.emplace_back(&corner);
      if (corner.tiles[1]->plate != corner.tiles[0]->plate) {
        corner.tiles[1]->plate->boundaryCorners.emplace_back(&corner);
      }
      if (corner.tiles[2]->plate != corner.tiles[0]->plate
          && corner.tiles[2]->plate != corner.tiles[1]->plate) {
        corner.tiles[2]->plate->boundaryCorners.emplace_back(&corner);
      }
    }
  }
}

void World::calculatePlateBoundaryStress(const std::vector<Corner*>& boundaryCorners,
                                         std::vector<size_t>& boundaryCornerInnerBorderIndexes)
{
  boundaryCornerInnerBorderIndexes.resize(boundaryCorners.size());
  size_t i = 0;
  for (auto pcorner : boundaryCorners) {
    auto& corner                   = *pcorner;
    corner.distanceToPlateBoundary = 0;

    Border* innerBorder     = nullptr;
    size_t innerBorderIndex = 0;
    for (size_t j = 0; j < corner.borders.size(); ++j) {
      auto pborder = corner.borders[j];
      auto& border = *pborder;
      if (!border.betweenPlates) {
        innerBorder      = &border;
        innerBorderIndex = j;
        break;
      }
    }

    if (innerBorder) {
      boundaryCornerInnerBorderIndexes[i] = innerBorderIndex;
      auto& outerBorder0 = *corner.borders[(innerBorderIndex + 1) % corner.borders.size()];
      auto& outerBorder1 = *corner.borders[(innerBorderIndex + 2) % corner.borders.size()];
      auto& farCorner0   = outerBorder0.oppositeCorner(corner);
      auto& farCorner1   = outerBorder1.oppositeCorner(corner);
      auto& plate0       = *innerBorder->tiles[0]->plate;
      auto& plate1 = *(outerBorder0.tiles[0]->plate != &plate0 ? outerBorder0.tiles[0]->plate :
                                                                 outerBorder0.tiles[1]->plate);
      auto boundaryVector = farCorner0.vectorTo(farCorner1);
      auto boundaryNormal = Vector3::Cross(boundaryVector, corner.position);
      Stress stress;
      calculateStress(plate0.calculateMovement(corner.position),
                      plate1.calculateMovement(corner.position), boundaryVector, boundaryNormal,
                      stress);
      corner.pressure = stress.pressure;
      corner.shear    = stress.shear;
    }
    else {
      boundaryCornerInnerBorderIndexes[i] = UdefIdx;
      auto& plate0                        = *corner.tiles[0]->plate;
      auto& plate1                        = *corner.tiles[1]->plate;
      auto& plate2                        = *corner.tiles[2]->plate;
      auto boundaryVector0                = corner.corners[0]->vectorTo(corner);
      auto boundaryVector1                = corner.corners[1]->vectorTo(corner);
      auto boundaryVector2                = corner.corners[2]->vectorTo(corner);
      auto boundaryNormal0                = Vector3::Cross(boundaryVector0, corner.position);
      auto boundaryNormal1                = Vector3::Cross(boundaryVector1, corner.position);
      auto boundaryNormal2                = Vector3::Cross(boundaryVector2, corner.position);
      Stress stress0, stress1, stress2;
      calculateStress(plate0.calculateMovement(corner.position),
                      plate1.calculateMovement(corner.position), boundaryVector0, boundaryNormal0,
                      stress0);
      calculateStress(plate1.calculateMovement(corner.position),
                      plate2.calculateMovement(corner.position), boundaryVector1, boundaryNormal1,
                      stress1);
      calculateStress(plate2.calculateMovement(corner.position),
                      plate0.calculateMovement(corner.position), boundaryVector2, boundaryNormal2,
                      stress2);
      corner.pressure = (stress0.pressure + stress1.pressure + stress2.pressure) / 3;
      corner.shear    = (stress0.shear + stress1.shear + stress2.shear) / 3;
    }

    ++i;
  }
}

void World::calculateStress(const Vector3& movement0, const Vector3& movement1,
                            const Vector3& boundaryVector, const Vector3& boundaryNormal,
                            Stress& stress)
{
  Vector3 relativeMovement = movement0 - movement1;
  auto pressureVector      = Tools::projectOnVector(relativeMovement, boundaryNormal);
  float pressure           = pressureVector.length();
  if (Vector3::Dot(pressureVector, boundaryNormal) > 0) {
    pressure = -pressure;
  }
  float shear = Tools::projectOnVector(relativeMovement, boundaryVector).length();

  stress.pressure = 2.f / (1.f + std::exp(-pressure / 30.f)) - 1.f;
  stress.shear    = 2.f / (1.f + std::exp(-shear / 30.f)) - 1.f;
}

void World::blurPlateBoundaryStress(const std::vector<Corner*>& boundaryCorners,
                                    size_t stressBlurIterations, float stressBlurCenterWeighting)
{
  Float32Array newCornerPressure(boundaryCorners.size());
  Float32Array newCornerShear(boundaryCorners.size());
  for (size_t i = 0; i < stressBlurIterations; ++i) {
    for (size_t j = 0; j < boundaryCorners.size(); ++j) {
      auto& corner          = *boundaryCorners[j];
      float averagePressure = 0.f;
      float averageShear    = 0.f;
      size_t neighborCount  = 0;
      for (size_t k = 0; k < corner.corners.size(); ++k) {
        auto& neighbor = *corner.corners[k];
        if (neighbor.betweenPlates) {
          averagePressure += neighbor.pressure;
          averageShear += neighbor.shear;
          ++neighborCount;
        }
      }
      newCornerPressure[j] = corner.pressure * stressBlurCenterWeighting
                             + (averagePressure / neighborCount) * (1 - stressBlurCenterWeighting);
      newCornerShear[j] = corner.shear * stressBlurCenterWeighting
                          + (averageShear / neighborCount) * (1 - stressBlurCenterWeighting);
    }

    for (size_t j = 0; j < boundaryCorners.size(); ++j) {
      auto& corner = *boundaryCorners[j];
      if (corner.betweenPlates) {
        corner.pressure = newCornerPressure[j];
        corner.shear    = newCornerShear[j];
      }
    }
  }
}

void World::populateElevationBorderQueue(
  const std::vector<Corner*>& boundaryCorners,
  const std::vector<size_t>& boundaryCornerInnerBorderIndexes,
  std::vector<ElevationBorder>& elevationBorderQueue)
{
  size_t i = 0;
  for (auto pcorner : boundaryCorners) {
    auto& corner = *pcorner;

    size_t innerBorderIndex = boundaryCornerInnerBorderIndexes[i];
    if (innerBorderIndex != UdefIdx) {
      auto& innerBorder  = *corner.borders[innerBorderIndex];
      auto& outerBorder0 = *corner.borders[(innerBorderIndex + 1) % corner.borders.size()];
      auto& plate0       = *innerBorder.tiles[0]->plate;
      auto& plate1 = *(outerBorder0.tiles[0]->plate != &plate0 ? outerBorder0.tiles[0]->plate :
                                                                 outerBorder0.tiles[1]->plate);

      using namespace std::placeholders;
      ElevationBorderOrigin::CalculateElevationFunc calculateElevation;

      if (corner.pressure > 0.3f) {
        corner.elevation = std::max(plate0.elevation, plate1.elevation) + corner.pressure;
        if (plate0.oceanic == plate1.oceanic) {
          calculateElevation = std::bind(&World::calculateCollidingElevation, _1, _2, _3, _4);
        }
        else if (plate0.oceanic) {
          calculateElevation = std::bind(&World::calculateSubductingElevation, _1, _2, _3, _4);
        }
        else {
          calculateElevation
            = std::bind(&World::calculateSuperductingElevation, _1, _2, _3, _4, _5);
        }
      }
      else if (corner.pressure < -0.3f) {
        corner.elevation   = std::max(plate0.elevation, plate1.elevation) - corner.pressure / 4.f;
        calculateElevation = std::bind(&World::calculateDivergingElevation, _1, _2, _3, _4);
      }
      else if (corner.shear > 0.3f) {
        corner.elevation   = std::max(plate0.elevation, plate1.elevation) + corner.shear / 8.f;
        calculateElevation = std::bind(&World::calculateShearingElevation, _1, _2, _3, _4);
      }
      else {
        corner.elevation   = (plate0.elevation + plate1.elevation) / 2.f;
        calculateElevation = std::bind(&World::calculateDormantElevation, _1, _2, _3, _4);
      }

      auto& nextCorner = innerBorder.oppositeCorner(corner);
      if (!nextCorner.betweenPlates) {
        elevationBorderQueue.emplace_back(
          ElevationBorder(ElevationBorderOrigin(&corner, corner.pressure, corner.shear, &plate0,
                                                calculateElevation),
                          &innerBorder, &corner, &nextCorner, innerBorder.length()));
      }
    }
    else {
      auto& plate0 = *corner.tiles[0]->plate;
      auto& plate1 = *corner.tiles[1]->plate;
      auto& plate2 = *corner.tiles[2]->plate;

      if (corner.pressure > 0.3f) {
        corner.elevation = std::max(plate0.elevation, std::max(plate1.elevation, plate2.elevation))
                           + corner.pressure;
      }
      else if (corner.pressure < -0.3f) {
        corner.elevation = std::max(plate0.elevation, std::max(plate1.elevation, plate2.elevation))
                           + corner.pressure / 4;
      }
      else if (corner.shear > 0.3f) {
        corner.elevation = std::max(plate0.elevation, std::max(plate1.elevation, plate2.elevation))
                           + corner.shear / 8;
      }
      else {
        corner.elevation = (plate0.elevation + plate1.elevation + plate2.elevation) / 3;
      }
    }

    ++i;
  }
}

float World::calculateCollidingElevation(float distanceToPlateBoundary, float distanceToPlateRoot,
                                         float boundaryElevation, float plateElevation)
{
  float t = distanceToPlateBoundary / (distanceToPlateBoundary + distanceToPlateRoot);
  if (t < 0.5f) {
    t = t / 0.5f;
    return plateElevation + std::pow(t - 1.f, 2.f) * (boundaryElevation - plateElevation);
  }
  else {
    return plateElevation;
  }
}

float World::calculateSuperductingElevation(float distanceToPlateBoundary,
                                            float distanceToPlateRoot, float boundaryElevation,
                                            float plateElevation, float pressure)
{
  float t = distanceToPlateBoundary / (distanceToPlateBoundary + distanceToPlateRoot);
  if (t < 0.2f) {
    t = t / 0.2f;
    return boundaryElevation + t * (plateElevation - boundaryElevation + pressure / 2.f);
  }
  else if (t < 0.5f) {
    t = (t - 0.2f) / 0.3f;
    return plateElevation + std::pow(t - 1.f, 2.f) * pressure / 2.f;
  }
  else {
    return plateElevation;
  }
}

float World::calculateSubductingElevation(float distanceToPlateBoundary, float distanceToPlateRoot,
                                          float boundaryElevation, float plateElevation)
{
  float t = distanceToPlateBoundary / (distanceToPlateBoundary + distanceToPlateRoot);
  return plateElevation + std::pow(t - 1.f, 2.f) * (boundaryElevation - plateElevation);
}

float World::calculateDivergingElevation(float distanceToPlateBoundary, float distanceToPlateRoot,
                                         float boundaryElevation, float plateElevation)
{
  float t = distanceToPlateBoundary / (distanceToPlateBoundary + distanceToPlateRoot);
  if (t < 0.3f) {
    t = t / 0.3f;
    return plateElevation + std::pow(t - 1.f, 2.f) * (boundaryElevation - plateElevation);
  }
  else {
    return plateElevation;
  }
}

float World::calculateShearingElevation(float distanceToPlateBoundary, float distanceToPlateRoot,
                                        float boundaryElevation, float plateElevation)
{
  float t = distanceToPlateBoundary / (distanceToPlateBoundary + distanceToPlateRoot);
  if (t < 0.2f) {
    t = t / 0.2f;
    return plateElevation + std::pow(t - 1.f, 2.f) * (boundaryElevation - plateElevation);
  }
  else {
    return plateElevation;
  }
}

float World::calculateDormantElevation(float distanceToPlateBoundary, float distanceToPlateRoot,
                                       float boundaryElevation, float plateElevation)
{
  float t = distanceToPlateBoundary / (distanceToPlateBoundary + distanceToPlateRoot);
  float elevationDifference = boundaryElevation - plateElevation;
  return t * t * elevationDifference * (2.f * t - 3.f) + boundaryElevation;
}

void World::processElevationBorderQueue(
  std::vector<ElevationBorder>& elevationBorderQueue,
  const std::function<bool(const ElevationBorder&, const ElevationBorder&)>&
    elevationBorderQueueSorter)
{
  while (elevationBorderQueue.size() > 0) {
    size_t iEnd = elevationBorderQueue.size();
    auto it     = elevationBorderQueue.begin();
    for (size_t i = 0; i < iEnd; ++i) {
      auto& front = *it;
      ++it;
      auto& corner = *front.nextCorner;
      if (corner.elevation == 0.f) {
        corner.distanceToPlateBoundary = front.distanceToPlateBoundary;
        corner.elevation               = front.origin.calculateElevation(
          corner.distanceToPlateBoundary, corner.distanceToPlateRoot,
          front.origin.corner->elevation, front.origin.plate->elevation, front.origin.pressure,
          front.origin.shear);

        for (size_t j = 0; j < corner.borders.size(); ++j) {
          auto& border = *corner.borders[j];
          if (!border.betweenPlates) {
            auto& nextCorner              = *corner.corners[j];
            float distanceToPlateBoundary = corner.distanceToPlateBoundary + border.length();
            if (nextCorner.distanceToPlateBoundary == 0.f
                || nextCorner.distanceToPlateBoundary > distanceToPlateBoundary) {
              elevationBorderQueue.emplace_back(ElevationBorder(
                front.origin, &border, &corner, &nextCorner, distanceToPlateBoundary));
            }
          }
        }
      }
    }
    // elevationBorderQueue.splice(0, iEnd);
    auto itend = elevationBorderQueue.begin();
    std::advance(itend, iEnd);
    elevationBorderQueue.erase(elevationBorderQueue.begin(), itend);

    std::sort(elevationBorderQueue.begin(), elevationBorderQueue.end(),
              [&elevationBorderQueueSorter](const ElevationBorder& a, const ElevationBorder& b) {
                return elevationBorderQueueSorter(a, b);
              });
  }
}

void World::calculateTileAverageElevations(std::vector<Tile>& tiles)
{
  for (auto& tile : tiles) {
    float elevation = 0;
    for (size_t j = 0; j < tile.corners.size(); ++j) {
      elevation += tile.corners[j]->elevation;
    }
    tile.elevation = elevation / tile.corners.size();
  }
}

void World::generatePlanetWeather(Topology& topology, SpatialPartition& /*partition*/,
                                  float heatLevel, float moistureLevel, IRandomFunction& random)
{
  float planetRadius = 1000.f;
  std::vector<Whorl> whorls;
  std::vector<Corner*> activeCorners;
  float totalHeat;
  float remainingHeat;
  float totalMoisture;
  float remainingMoisture;

  generateAirCurrentWhorls(planetRadius, random, whorls);
  calculateAirCurrents(topology.corners, whorls, planetRadius);

  initializeAirHeat(topology.corners, heatLevel, activeCorners, totalHeat);
  remainingHeat = totalHeat;
  float consumedHeat;
  do {
    consumedHeat = processAirHeat(activeCorners);
    remainingHeat -= consumedHeat;
  } while (remainingHeat > 0 && consumedHeat >= 0.0001f);

  calculateTemperature(topology.corners, topology.tiles, planetRadius);

  initializeAirMoisture(topology.corners, moistureLevel, activeCorners, totalMoisture);
  remainingMoisture = totalMoisture;
  float consumedMoisture;
  do {
    consumedMoisture = processAirMoisture(activeCorners);
    remainingMoisture -= consumedMoisture;
  } while (remainingMoisture > 0 && consumedMoisture >= 0.0001f);

  calculateMoisture(topology.corners, topology.tiles);
}

void World::generateAirCurrentWhorls(float planetRadius, IRandomFunction& random,
                                     std::vector<Whorl>& whorls)
{
  float direction       = random.integer(0, 1) ? 1.f : -1.f;
  size_t layerCount     = random.integer(4, 7);
  float circumference   = Math::PI2 * planetRadius;
  float fullRevolution  = Math::PI2;
  float baseWhorlRadius = circumference / (2.f * (layerCount - 1.f));

  whorls.reserve(
    1
    + layerCount * static_cast<size_t>(std::ceil(planetRadius * fullRevolution / baseWhorlRadius))
        / 2
    + 1);

  Matrix3 m1, m2;
  m1.FromAngleAxis(Vector3(1.f, 0.f, 0.f), BABYLON::Tools::ToRadians(random.realInclusive(
                                             0, fullRevolution / (2 * (layerCount + 4)))));
  m2.FromAngleAxis(Vector3(0.f, 1.f, 0.f),
                   BABYLON::Tools::ToRadians(random.real(0, fullRevolution)));
  whorls.emplace_back(
    Whorl{m1 * m2 * Vector3(0, planetRadius, 0),
          random.realInclusive(fullRevolution / 36, fullRevolution / 24) * direction,
          random.realInclusive(baseWhorlRadius * 0.8f, baseWhorlRadius * 1.2f)});

  for (size_t i = 1; i < layerCount - 1; ++i) {
    direction              = -direction;
    float baseTilt         = i / (layerCount - 1) * fullRevolution / 2;
    size_t layerWhorlCount = static_cast<size_t>(
      std::ceil((std::sin(baseTilt) * planetRadius * fullRevolution) / baseWhorlRadius));
    Matrix3 m3, m4;
    m3.FromAngleAxis(Vector3(1, 0, 0), BABYLON::Tools::ToRadians(baseTilt));
    for (size_t j = 0; j < layerWhorlCount; ++j) {
      m4.FromAngleAxis(Vector3(0, 1, 0), BABYLON::Tools::ToRadians(
                                           fullRevolution * (j + (i % 2) / 2) / layerWhorlCount));
      whorls.emplace_back(
        Whorl{m1 * m2 * m3 * m4 * Vector3(0, planetRadius, 0),
              random.realInclusive(fullRevolution / 48, fullRevolution / 32) * direction,
              random.realInclusive(baseWhorlRadius * 0.8f, baseWhorlRadius * 1.2f)});
    }
  }

  direction = -direction;
  Matrix3 m3;
  m3.FromAngleAxis(Vector3(1, 0, 0), BABYLON::Tools::ToRadians(fullRevolution / 2));
  whorls.emplace_back(
    Whorl{m1 * m2 * m3 * Vector3(0, planetRadius, 0),
          random.realInclusive(fullRevolution / 36, fullRevolution / 24) * direction,
          random.realInclusive(baseWhorlRadius * 0.8f, baseWhorlRadius * 1.2f)});
}

void World::calculateAirCurrents(std::vector<Corner>& corners, const std::vector<Whorl>& whorls,
                                 float planetRadius)
{
  for (auto& corner : corners) {
    Vector3 airCurrent(0.f, 0.f, 0.f);
    float weight = 0;
    for (auto& whorl : whorls) {
      auto angle     = Tools::angleBetween(whorl.center, corner.position);
      float distance = BABYLON::Tools::ToRadians(angle) * planetRadius;
      if (distance < whorl.radius) {
        float normalizedDistance = distance / whorl.radius;
        float whorlWeight        = 1.f - normalizedDistance;
        float whorlStrength      = planetRadius * whorl.strength * whorlWeight * normalizedDistance;
        Vector3 whorlCurrent
          = Tools::setLength(Vector3::Cross(whorl.center, corner.position), whorlStrength);
        airCurrent += whorlCurrent;
        weight += whorlWeight;
      }
    }
    airCurrent /= weight;
    corner.airCurrent      = airCurrent;
    corner.airCurrentSpeed = airCurrent.length(); // kilometers per hour

    corner.airCurrentOutflows.reserve(corner.borders.size());
    auto airCurrentDirection = airCurrent.normalize();
    float outflowSum         = 0.f;
    for (auto pcornerCorner : corner.corners) {
      auto vector = corner.vectorTo(*pcornerCorner).normalize();
      auto dot    = Vector3::Dot(vector, airCurrentDirection);
      if (dot > 0.f) {
        corner.airCurrentOutflows.emplace_back(dot);
        outflowSum += dot;
      }
      else {
        corner.airCurrentOutflows.emplace_back(0.f);
      }
    }

    if (outflowSum > 0.f) {
      for (size_t j = 0; j < corner.borders.size(); ++j) {
        corner.airCurrentOutflows[j] /= outflowSum;
      }
    }
  }
}

void World::initializeAirHeat(std::vector<Corner>& corners, float heatLevel,
                              std::vector<Corner*>& activeCorners, float& airHeat)
{
  activeCorners.clear();
  airHeat = 0.f;
  for (auto& corner : corners) {
    corner.airHeat    = corner.area * heatLevel;
    corner.newAirHeat = 0.f;
    corner.heat       = 0.f;

    corner.heatAbsorption
      = 0.1f * corner.area / std::max(0.1f, std::min(corner.airCurrentSpeed, 1.f));
    if (corner.elevation <= 0.f) {
      corner.maxHeat = corner.area;
    }
    else {
      corner.maxHeat = corner.area;
      corner.heatAbsorption *= 2.f;
    }

    activeCorners.emplace_back(&corner);
    airHeat += corner.airHeat;
  }
}

float World::processAirHeat(std::vector<Corner*>& activeCorners)
{
  float consumedHeat     = 0.f;
  auto activeCornerCount = activeCorners.size();

  auto it = activeCorners.begin();
  for (size_t i = 0; i < activeCornerCount; ++i, ++it) {
    auto& corner = *(*it);
    if (corner.airHeat == 0.f) {
      continue;
    }

    float heatChange = std::max(
      0.f, std::min(corner.airHeat, corner.heatAbsorption * (1.f - corner.heat / corner.maxHeat)));
    corner.heat += heatChange;
    consumedHeat += heatChange;
    float heatLoss = corner.area * (corner.heat / corner.maxHeat) * 0.02f;
    heatChange     = std::min(corner.airHeat, heatChange + heatLoss);

    float remainingCornerAirHeat = corner.airHeat - heatChange;
    corner.airHeat               = 0;

    for (size_t j = 0; j < corner.corners.size(); ++j) {
      const auto& outflow = corner.airCurrentOutflows[j];
      if (outflow > 0) {
        corner.corners[j]->newAirHeat += remainingCornerAirHeat * outflow;
        activeCorners.emplace_back(corner.corners[j]);
      }
    }
  }

  activeCorners.erase(activeCorners.begin(), it);

  for (auto& pcorner : activeCorners) {
    pcorner->airHeat    = pcorner->newAirHeat;
    pcorner->newAirHeat = 0;
  }

  return consumedHeat;
}

void World::calculateTemperature(std::vector<Corner>& corners, std::vector<Tile>& tiles,
                                 float planetRadius)
{
  for (auto& corner : corners) {
    float latitudeEffect = std::sqrt(1.f - std::abs(corner.position.y) / planetRadius);
    float elevationEffect
      = 1.f - std::pow(std::max(0.f, std::min(corner.elevation * 0.8f, 1.f)), 2.f);
    float normalizedHeat = corner.heat / corner.area;
    corner.temperature
      = (latitudeEffect * elevationEffect * 0.7f + normalizedHeat * 0.3f) * 5.f / 3.f - 2.f / 3.f;
  }

  for (auto& tile : tiles) {
    tile.temperature = 0.f;
    for (const auto& pcorner : tile.corners) {
      tile.temperature += pcorner->temperature;
    }
    tile.temperature /= tile.corners.size();
  }
}

void World::initializeAirMoisture(std::vector<Corner>& corners, float moistureLevel,
                                  std::vector<Corner*>& activeCorners, float& airMoisture)
{
  airMoisture = 0.f;
  for (auto& corner : corners) {
    corner.airMoisture = (corner.elevation > 0.f) ?
                           0.f :
                           corner.area * moistureLevel
                             * std::max(0.f, std::min(0.5f + corner.temperature * 0.5f, 1.f));
    corner.newAirMoisture = 0.f;
    corner.precipitation  = 0.f;
    corner.precipitationRate
      = 0.0075f * corner.area / std::max(0.1f, std::min(corner.airCurrentSpeed, 1.f));
    corner.precipitationRate
      *= 1.f + (1.f - std::max(0.f, std::min(corner.temperature, 1.f))) * 0.1f;
    if (corner.elevation > 0.f) {
      corner.precipitationRate *= 1.f + corner.elevation * 0.5f;
      corner.maxPrecipitation
        = corner.area * (0.25f + std::max(0.f, std::min(corner.elevation, 1.f)) * 0.25f);
    }
    else {
      corner.maxPrecipitation = corner.area * 0.25f;
    }

    activeCorners.emplace_back(&corner);
    airMoisture += corner.airMoisture;
  }
}

float World::processAirMoisture(std::vector<Corner*>& activeCorners)
{
  float consumedMoisture = 0.f;
  auto activeCornerCount = activeCorners.size();
  auto it                = activeCorners.begin();
  for (size_t i = 0; i < activeCornerCount; ++i, ++it) {
    auto& corner = *(*it);
    if (corner.airMoisture == 0.f) {
      continue;
    }

    float moistureChange
      = std::max(0.f, std::min(corner.airMoisture,
                               corner.precipitationRate
                                 * (1.f - corner.precipitation / corner.maxPrecipitation)));
    corner.precipitation += moistureChange;
    consumedMoisture += moistureChange;
    float moistureLoss = corner.area * (corner.precipitation / corner.maxPrecipitation) * 0.02f;
    moistureChange     = std::min(corner.airMoisture, moistureChange + moistureLoss);

    float remainingCornerAirMoisture = corner.airMoisture - moistureChange;
    corner.airMoisture               = 0.f;

    for (size_t j = 0; j < corner.corners.size(); ++j) {
      float outflow = corner.airCurrentOutflows[j];
      if (outflow > 0.f) {
        corner.corners[j]->newAirMoisture += remainingCornerAirMoisture * outflow;
        activeCorners.emplace_back(corner.corners[j]);
      }
    }
  }

  activeCorners.erase(activeCorners.begin(), it);

  for (auto& pcorner : activeCorners) {
    pcorner->airMoisture    = pcorner->newAirMoisture;
    pcorner->newAirMoisture = 0.f;
  }

  return consumedMoisture;
}

void World::calculateMoisture(std::vector<Corner>& corners, std::vector<Tile>& tiles)
{
  for (auto& corner : corners) {
    corner.moisture = corner.precipitation / corner.area / 0.5f;
  }

  for (auto& tile : tiles) {
    tile.moisture = 0.f;
    for (auto pcorner : tile.corners) {
      tile.moisture += pcorner->temperature;
    }
    tile.moisture /= tile.corners.size();
  }
}

void World::generatePlanetBiomes(std::vector<Tile>& tiles, float /*planetRadius*/)
{
  for (auto& tile : tiles) {
    auto elevation = std::max(0.f, tile.elevation);
    // auto latitude = std::abs(tile.position.y / planetRadius);
    auto temperature = tile.temperature;
    auto moisture    = tile.moisture;

    if (elevation <= 0) {
      if (temperature > 0.f) {
        tile.biome = "ocean";
      }
      else {
        tile.biome = "oceanGlacier";
      }
    }
    else if (elevation < 0.6f) {
      if (temperature > 0.75f) {
        if (moisture < 0.25f) {
          tile.biome = "desert";
        }
        else {
          tile.biome = "rainForest";
        }
      }
      else if (temperature > 0.5f) {
        if (moisture < 0.25f) {
          tile.biome = "rocky";
        }
        else if (moisture < 0.50f) {
          tile.biome = "plains";
        }
        else {
          tile.biome = "swamp";
        }
      }
      else if (temperature > 0.f) {
        if (moisture < 0.25f) {
          tile.biome = "plains";
        }
        else if (moisture < 0.50f) {
          tile.biome = "grassland";
        }
        else {
          tile.biome = "deciduousForest";
        }
      }
      else {
        if (moisture < 0.25f) {
          tile.biome = "tundra";
        }
        else {
          tile.biome = "landGlacier";
        }
      }
    }
    else if (elevation < 0.8f) {
      if (temperature > 0.f) {
        if (moisture < 0.25f) {
          tile.biome = "tundra";
        }
        else {
          tile.biome = "coniferForest";
        }
      }
      else {
        tile.biome = "tundra";
      }
    }
    else {
      if (temperature > 0.f || moisture < 0.25f) {
        tile.biome = "mountain";
      }
      else {
        tile.biome = "snowyMountain";
      }
    }
  }
}

void World::generatePlanetRenderData(Topology& topology, IRandomFunction& random,
                                     RenderData& renderData)
{
  buildSurfaceRenderObject(topology.tiles, random, renderData.surface);
  buildPlateBoundariesRenderObject(topology.borders, renderData.plateBoundaries);
  buildPlateMovementsRenderObject(topology.tiles, renderData.plateMovements);
  buildAirCurrentsRenderObject(topology.corners, renderData.airCurrents);
}

void World::doBuildTileWedge(RenderObject& ro, size_t b, size_t s, size_t t)
{
  ro.triangle(b + s + 2, b, b + t + 2);
  ro.triangle(b + s + 1, b + t + 2, b + t + 1);
  ro.triangle(b + s + 1, b + s + 2, b + t + 2);
}

void World::buildSurfaceRenderObject(std::vector<Tile>& tiles, IRandomFunction& random,
                                     RenderObject& ro)
{
  size_t baseIndex = 0;
  for (auto& tile : tiles) {
    auto colorDeviance = Color4(random.unit(), random.unit(), random.unit());
    Color4 terrainColor;
    if (tile.elevation <= 0) {
      if (tile.biome == "ocean") {
        terrainColor
          = lerp(lerp(Tools::ocv(0x0066FF), Tools::ocv(0x0044BB), std::min(-tile.elevation, 1.f)),
                 colorDeviance, 0.10f);
      }
      else if (tile.biome == "oceanGlacier") {
        terrainColor = lerp(Tools::ocv(0xDDEEFF), colorDeviance, 0.10f);
      }
      else
        terrainColor = Tools::ocv(0xFF00FF);
    }
    else if (tile.elevation < 0.6f) {
      auto normalizedElevation = tile.elevation / 0.6f;
      if (tile.biome == "desert") {
        terrainColor = lerp(lerp(Tools::ocv(0xDDDD77), Tools::ocv(0xBBBB55), normalizedElevation),
                            colorDeviance, 0.10f);
      }
      else if (tile.biome == "rainForest") {
        terrainColor = lerp(lerp(Tools::ocv(0x44DD00), Tools::ocv(0x229900), normalizedElevation),
                            colorDeviance, 0.20f);
      }
      else if (tile.biome == "rocky") {
        terrainColor = lerp(lerp(Tools::ocv(0xAA9977), Tools::ocv(0x887755), normalizedElevation),
                            colorDeviance, 0.15f);
      }
      else if (tile.biome == "plains") {
        terrainColor = lerp(lerp(Tools::ocv(0x99BB44), Tools::ocv(0x667722), normalizedElevation),
                            colorDeviance, 0.10f);
      }
      else if (tile.biome == "grassland") {
        terrainColor = lerp(lerp(Tools::ocv(0x77CC44), Tools::ocv(0x448822), normalizedElevation),
                            colorDeviance, 0.15f);
      }
      else if (tile.biome == "swamp") {
        terrainColor = lerp(lerp(Tools::ocv(0x77AA44), Tools::ocv(0x446622), normalizedElevation),
                            colorDeviance, 0.25f);
      }
      else if (tile.biome == "deciduousForest") {
        terrainColor = lerp(lerp(Tools::ocv(0x33AA22), Tools::ocv(0x116600), normalizedElevation),
                            colorDeviance, 0.10f);
      }
      else if (tile.biome == "tundra") {
        terrainColor = lerp(lerp(Tools::ocv(0x9999AA), Tools::ocv(0x777788), normalizedElevation),
                            colorDeviance, 0.15f);
      }
      else if (tile.biome == "landGlacier") {
        terrainColor = lerp(Tools::ocv(0xDDEEFF), colorDeviance, 0.10f);
      }
      else {
        terrainColor = Tools::ocv(0xFF00FF);
      }
    }
    else if (tile.elevation < 0.8f) {
      auto normalizedElevation = (tile.elevation - 0.6f) / 0.2f;
      if (tile.biome == "tundra") {
        terrainColor = lerp(lerp(Tools::ocv(0x777788), Tools::ocv(0x666677), normalizedElevation),
                            colorDeviance, 0.10f);
      }
      else if (tile.biome == "coniferForest") {
        terrainColor = lerp(lerp(Tools::ocv(0x338822), Tools::ocv(0x116600), normalizedElevation),
                            colorDeviance, 0.10f);
      }
      else if (tile.biome == "snow") {
        terrainColor = lerp(lerp(Tools::ocv(0xEEEEEE), Tools::ocv(0xDDDDDD), normalizedElevation),
                            colorDeviance, 0.10f);
      }
      else if (tile.biome == "mountain") {
        terrainColor = lerp(lerp(Tools::ocv(0x555544), Tools::ocv(0x444433), normalizedElevation),
                            colorDeviance, 0.05f);
      }
      else {
        terrainColor = Tools::ocv(0xFF00FF);
      }
    }
    else {
      auto normalizedElevation = std::min((tile.elevation - 0.8f) / 0.5f, 1.f);
      if (tile.biome == "mountain") {
        terrainColor = lerp(lerp(Tools::ocv(0x444433), Tools::ocv(0x333322), normalizedElevation),
                            colorDeviance, 0.05f);
      }
      else if (tile.biome == "snowyMountain") {
        terrainColor = lerp(lerp(Tools::ocv(0xDDDDDD), Tools::ocv(0xFFFFFF), normalizedElevation),
                            colorDeviance, 0.10f);
      }
      else {
        terrainColor = Tools::ocv(0xFF00FF);
      }
    }

    Color4 elevationColor;
    if (tile.elevation <= 0) {
      elevationColor
        = lerp(Tools::ocv(0x224488), Tools::ocv(0xAADDFF),
               std::max(0.f, std::min((tile.elevation + 3.f / 4.f) / (3.f / 4.f), 1.f)));
    }
    else if (tile.elevation < 0.75f) {
      elevationColor = lerp(Tools::ocv(0x997755), Tools::ocv(0x553311),
                            std::max(0.f, std::min((tile.elevation) / (3.f / 4.f), 1.f)));
    }
    else {
      elevationColor
        = lerp(Tools::ocv(0x553311), Tools::ocv(0x222222),
               std::max(0.f, std::min((tile.elevation - 3.f / 4.f) / (1.f / 2.f), 1.f)));
    }

    Color4 temperatureColor;
    if (tile.temperature <= 0) {
      temperatureColor
        = lerp(Tools::ocv(0x0000FF), Tools::ocv(0xBBDDFF),
               std::max(0.f, std::min((tile.temperature + 2.f / 3.f) / (2.f / 3.f), 1.f)));
    }
    else {
      temperatureColor = lerp(Tools::ocv(0xFFFF00), Tools::ocv(0xFF0000),
                              std::max(0.f, std::min((tile.temperature) / (3.f / 3.f), 1.f)));
    }

    ro.position(tile.averagePosition);
    ro.normal(tile.normal);
    ro.colour(terrainColor);
    for (size_t j = 0; j < tile.corners.size(); ++j) {
      auto& cornerPosition = tile.corners[j]->position;

      ro.position(cornerPosition);
      ro.normal(tile.normal);
      ro.colour(terrainColor);

      ro.position((tile.averagePosition - cornerPosition) * 0.1f + cornerPosition);
      ro.normal(tile.normal);
      ro.colour(terrainColor * 0.5f);

      size_t i0 = j * 2;
      size_t i1 = ((j + 1) % tile.corners.size()) * 2;
      doBuildTileWedge(ro, baseIndex, i0, i1);
    }

    baseIndex += 1 + tile.corners.size() * 2;
  }
}

void World::buildPlateBoundariesRenderObject(std::vector<Border>& borders, RenderObject& ro)
{
  auto baseIndex = ro.getCurrentVertexCount();
  for (auto& border : borders) {
    if (border.betweenPlates) {
      auto normal = border.midpoint.normalize();
      auto offset = normal * 1;

      auto& borderPoint0 = border.corners[0]->position;
      auto& borderPoint1 = border.corners[1]->position;
      auto& tilePoint0   = border.tiles[0]->averagePosition;
      auto& tilePoint1   = border.tiles[1]->averagePosition;
      auto nl            = (border.tiles[0]->normal + border.tiles[1]->normal) / 2.f;

      auto pressure = std::max(
        -1.f, std::min((border.corners[0]->pressure + border.corners[1]->pressure) / 2.f, 1.f));
      auto shear
        = std::max(0.f, std::min((border.corners[0]->shear + border.corners[1]->shear) / 2.f, 1.f));
      auto innerColor
        = (pressure <= 0) ? Color4(1.f + pressure, 1.f, 0.f) : Color4(1.f, 1.f - pressure, 0.f);
      auto outerColor = Color4(0.f, shear / 2.f, shear);

      ro.position(borderPoint0 + offset);
      ro.normal(nl);
      ro.colour(innerColor);

      ro.position(borderPoint1 + offset);
      ro.normal(nl);
      ro.colour(innerColor);

      ro.position((tilePoint0 - borderPoint0) * 0.2f + borderPoint0 + offset);
      ro.normal(nl);
      ro.colour(outerColor);

      ro.position((tilePoint0 - borderPoint1) * 0.2f + borderPoint1 + offset);
      ro.normal(nl);
      ro.colour(outerColor);

      ro.position((tilePoint1 - borderPoint0) * 0.2f + borderPoint0 + offset);
      ro.normal(nl);
      ro.colour(outerColor);

      ro.position((tilePoint1 - borderPoint1) * 0.2f + borderPoint1 + offset);
      ro.normal(nl);
      ro.colour(outerColor);

      ro.triangle(baseIndex + 0, baseIndex + 2, baseIndex + 1);
      ro.triangle(baseIndex + 1, baseIndex + 2, baseIndex + 3);
      ro.triangle(baseIndex + 1, baseIndex + 5, baseIndex + 0);
      ro.triangle(baseIndex + 0, baseIndex + 5, baseIndex + 4);

      baseIndex += 6;
    }
  }
}

void World::buildPlateMovementsRenderObject(std::vector<Tile>& tiles, RenderObject& ro)
{
  for (auto& tile : tiles) {
    auto& plate             = *tile.plate;
    auto movement           = plate.calculateMovement(tile.position);
    auto plateMovementColor = Color4(1.f - plate.color.r, 1.f - plate.color.g, 1.f - plate.color.b);

    buildArrow(ro, tile.position * 1.002f, movement * 0.5f, tile.position.normalize(),
               std::min(movement.length(), 4.f), plateMovementColor);

    tile.plateMovement = movement;
  }
}

void World::buildAirCurrentsRenderObject(std::vector<Corner>& corners, RenderObject& ro)
{
  for (auto& corner : corners) {
    buildArrow(ro, corner.position * 1.002f, corner.airCurrent * 0.5f, corner.position.normalize(),
               std::min(corner.airCurrent.length(), 4.f), Color4(1.f, 1.f, 1.f, 1.f));
  }
}

void World::buildArrow(RenderObject& ro, const Vector3& position, const Vector3& direction,
                       const Vector3& normal, float baseWidth, const Color4& color)
{
  if (direction.lengthSquared() == 0.f) {
    return;
  }

  auto sideOffset = Tools::setLength(Vector3::Cross(direction, normal), baseWidth / 2.f);
  auto baseIndex  = ro.getCurrentVertexCount();

  ro.position(position + sideOffset);
  ro.normal(normal);
  ro.colour(color);

  ro.position(position + direction);
  ro.normal(normal);
  ro.colour(color);

  ro.position(position - sideOffset);
  ro.normal(normal);
  ro.colour(color);

  ro.triangle(baseIndex, baseIndex + 1, baseIndex + 2);
}

void World::generatePlanetStatistics(Topology& topology, std::vector<Plate>& plates,
                                     PlanetStatistics& planetStatistics)
{
  auto& statistics = planetStatistics;

  statistics.corners.count = topology.corners.size();
  statistics.corners.airCurrent.reset();
  statistics.corners.elevation.reset();
  statistics.corners.temperature.reset();
  statistics.corners.moisture.reset();
  statistics.corners.distanceToPlateBoundary.reset();
  statistics.corners.distanceToPlateRoot.reset();
  statistics.corners.pressure.reset();
  statistics.corners.shear.reset();
  statistics.corners.doublePlateBoundaryCount = 0;
  statistics.corners.triplePlateBoundaryCount = 0;
  statistics.corners.innerLandBoundaryCount   = 0;
  statistics.corners.outerLandBoundaryCount   = 0;

  for (auto& corner : topology.corners) {
    updateMinMaxAvg(statistics.corners.airCurrent, corner.airCurrent.length());
    updateMinMaxAvg(statistics.corners.elevation, corner.elevation);
    updateMinMaxAvg(statistics.corners.temperature, corner.temperature);
    updateMinMaxAvg(statistics.corners.moisture, corner.moisture);
    updateMinMaxAvg(statistics.corners.distanceToPlateBoundary, corner.distanceToPlateBoundary);
    updateMinMaxAvg(statistics.corners.distanceToPlateRoot, corner.distanceToPlateRoot);
    if (corner.betweenPlates) {
      updateMinMaxAvg(statistics.corners.pressure, corner.pressure);
      updateMinMaxAvg(statistics.corners.shear, corner.shear);
      if (!corner.borders[0]->betweenPlates || !corner.borders[1]->betweenPlates
          || !corner.borders[2]->betweenPlates) {
        statistics.corners.doublePlateBoundaryCount += 1;
      }
      else {
        statistics.corners.triplePlateBoundaryCount += 1;
      }
    }
    auto landCount = ((corner.tiles[0]->elevation > 0) ? 1 : 0)
                     + ((corner.tiles[1]->elevation > 0) ? 1 : 0)
                     + ((corner.tiles[2]->elevation > 0) ? 1 : 0);
    if (landCount == 2) {
      statistics.corners.innerLandBoundaryCount += 1;
    }
    else if (landCount == 1) {
      statistics.corners.outerLandBoundaryCount += 1;
    }
    if (corner.corners.size() != 3) {
      BABYLON_LOG_ERROR("World", "Corner has as invalid number of neighboring corners.")
      return;
    }
    if (corner.borders.size() != 3) {
      BABYLON_LOG_ERROR("World", "Corner has as invalid number of borders.")
      return;
    }
    if (corner.tiles.size() != 3) {
      BABYLON_LOG_ERROR("World", "Corner has as invalid number of tiles.")
      return;
    }
  }

  statistics.corners.airCurrent.avg /= statistics.corners.count;
  statistics.corners.elevation.avg /= statistics.corners.count;
  statistics.corners.temperature.avg /= statistics.corners.count;
  statistics.corners.moisture.avg /= statistics.corners.count;
  statistics.corners.distanceToPlateBoundary.avg /= statistics.corners.count;
  statistics.corners.distanceToPlateRoot.avg /= statistics.corners.count;
  statistics.corners.pressure.avg
    /= (statistics.corners.doublePlateBoundaryCount + statistics.corners.triplePlateBoundaryCount);
  statistics.corners.shear.avg
    /= (statistics.corners.doublePlateBoundaryCount + statistics.corners.triplePlateBoundaryCount);

  statistics.borders.count = topology.borders.size();
  statistics.borders.length.reset();
  statistics.borders.plateBoundaryCount      = 0;
  statistics.borders.plateBoundaryPercentage = 0;
  statistics.borders.landBoundaryCount       = 0;
  statistics.borders.landBoundaryPercentage  = 0;

  for (auto& border : topology.borders) {
    auto length = border.length();
    updateMinMaxAvg(statistics.borders.length, length);
    if (border.betweenPlates) {
      statistics.borders.plateBoundaryCount += 1;
      statistics.borders.plateBoundaryPercentage += length;
    }
    if (border.isLandBoundary()) {
      statistics.borders.landBoundaryCount += 1;
      statistics.borders.landBoundaryPercentage += length;
    }
    if (border.corners.size() != 2) {
      BABYLON_LOG_ERROR("World", "Border has as invalid number of corners.")
      return;
    }
    if (border.borders.size() != 4) {
      BABYLON_LOG_ERROR("World", "Border has as invalid number of neighboring borders.")
      return;
    }
    if (border.tiles.size() != 2) {
      BABYLON_LOG_ERROR("World", "Border has as invalid number of tiles.")
      return;
    }
  }

  statistics.borders.plateBoundaryPercentage /= statistics.borders.length.avg;
  statistics.borders.landBoundaryPercentage /= statistics.borders.length.avg;
  statistics.borders.length.avg /= statistics.borders.count;

  statistics.tiles.count     = topology.tiles.size();
  statistics.tiles.totalArea = 0;
  statistics.tiles.area.reset();
  statistics.tiles.elevation.reset();
  statistics.tiles.temperature.reset();
  statistics.tiles.moisture.reset();
  statistics.tiles.plateMovement.reset();
  statistics.tiles.biomeCounts.clear();
  statistics.tiles.biomeAreas.clear();
  statistics.tiles.pentagonCount = 0;
  statistics.tiles.hexagonCount  = 0;
  statistics.tiles.heptagonCount = 0;

  for (auto& tile : topology.tiles) {
    updateMinMaxAvg(statistics.tiles.area, tile.area);
    updateMinMaxAvg(statistics.tiles.elevation, tile.elevation);
    updateMinMaxAvg(statistics.tiles.temperature, tile.temperature);
    updateMinMaxAvg(statistics.tiles.moisture, tile.moisture);
    updateMinMaxAvg(statistics.tiles.plateMovement, tile.plateMovement.length());
    if (!statistics.tiles.biomeCounts[tile.biome]) {
      statistics.tiles.biomeCounts[tile.biome] = 0;
    }
    statistics.tiles.biomeCounts[tile.biome] += 1;
    if (statistics.tiles.biomeAreas[tile.biome] == 0.f) {
      statistics.tiles.biomeAreas[tile.biome] = 0;
    }
    statistics.tiles.biomeAreas[tile.biome] += tile.area;
    if (tile.tiles.size() == 5) {
      statistics.tiles.pentagonCount += 1;
    }
    else if (tile.tiles.size() == 6) {
      statistics.tiles.hexagonCount += 1;
    }
    else if (tile.tiles.size() == 7) {
      statistics.tiles.heptagonCount += 1;
    }
    else {
      BABYLON_LOG_ERROR("World", "Tile has an invalid number of neighboring tiles.")
      return;
    }
    if (tile.tiles.size() != tile.borders.size()) {
      BABYLON_LOG_ERROR("World", "Tile has a neighbor and border count that do not match.")
      return;
    }
    if (tile.tiles.size() != tile.corners.size()) {
      BABYLON_LOG_ERROR("World", "Tile has a neighbor and corner count that do not match.")
      return;
    }
  }

  statistics.tiles.totalArea = statistics.tiles.area.avg;
  statistics.tiles.area.avg /= statistics.tiles.count;
  statistics.tiles.elevation.avg /= statistics.tiles.count;
  statistics.tiles.temperature.avg /= statistics.tiles.count;
  statistics.tiles.moisture.avg /= statistics.tiles.count;
  statistics.tiles.plateMovement.avg /= statistics.tiles.count;

  statistics.plates.count = plates.size();
  statistics.plates.tileCount.reset();
  statistics.plates.area.reset();
  statistics.plates.boundaryElevation.reset();
  statistics.plates.boundaryBorders.reset();
  statistics.plates.circumference.reset();

  for (auto& plate : plates) {
    updateMinMaxAvg(statistics.plates.tileCount, plate.tiles.size());
    plate.area = 0.f;
    for (auto ptile : plate.tiles) {
      auto& tile = *ptile;
      plate.area += tile.area;
    }
    updateMinMaxAvg(statistics.plates.area, plate.area);
    float elevation = 0.f;
    for (auto pcorner : plate.boundaryCorners) {
      auto& corner = *pcorner;
      elevation += corner.elevation;
    }
    updateMinMaxAvg(statistics.plates.boundaryElevation, elevation / plate.boundaryCorners.size());
    updateMinMaxAvg(statistics.plates.boundaryBorders, plate.boundaryBorders.size());
    plate.circumference = 0.f;
    for (auto pborder : plate.boundaryBorders) {
      auto& border = *pborder;
      plate.circumference += border.length();
    }
    updateMinMaxAvg(statistics.plates.circumference, plate.circumference);
  }

  statistics.plates.tileCount.avg /= statistics.plates.count;
  statistics.plates.area.avg /= statistics.plates.count;
  statistics.plates.boundaryElevation.avg /= statistics.plates.count;
  statistics.plates.boundaryBorders.avg /= statistics.plates.count;
  statistics.plates.circumference.avg /= statistics.plates.count;
}

} // end of namespace Extensions
} // end of namespace BABYLON
