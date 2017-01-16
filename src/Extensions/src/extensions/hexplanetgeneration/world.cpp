#include <babylon/extensions/hexplanetgeneration/world.h>

#include <babylon/extensions/hexplanetgeneration/icosphere.h>
#include <babylon/extensions/hexplanetgeneration/planet.h>
#include <babylon/extensions/hexplanetgeneration/render_data.h>
#include <babylon/extensions/hexplanetgeneration/terrain/distance_corner.h>
#include <babylon/extensions/hexplanetgeneration/terrain/elevation_border.h>
#include <babylon/extensions/hexplanetgeneration/terrain/plate.h>
#include <babylon/extensions/hexplanetgeneration/terrain/spatial_partition.h>
#include <babylon/extensions/hexplanetgeneration/terrain/stress.h>
#include <babylon/extensions/hexplanetgeneration/terrain/topology.h>
#include <babylon/extensions/hexplanetgeneration/terrain/whorl.h>
#include <babylon/extensions/hexplanetgeneration/utils/tools.h>
#include <babylon/math/color3.h>

namespace BABYLON {
namespace Extensions {

const size_t World::UdefIdx = std::numeric_limits<size_t>::max();

World::World()
{
}

World::~World()
{
}

bool generatePlanetTopology(const IcosahedronMesh& mesh, Topology& ret)
{
  auto& corners = ret.corners;
  auto& borders = ret.borders;
  auto& tiles   = ret.tiles;
  size_t i;

  i = 0;
  corners.reserve(mesh.faces.size());
  for (const auto& face : mesh.faces) {
    corners.emplace_back(Corner(i, face.centroid * 1000, face.e.size(),
                                face.e.size(), face.n.size()));
    ++i;
  }

  i = 0;
  borders.reserve(mesh.edges.size());
  for (i = 0; i < mesh.edges.size(); ++i) {
    borders.emplace_back(Border(i, 2, 4, 2));
  }

  i = 0;
  tiles.reserve(mesh.nodes.size());
  for (const auto& node : mesh.nodes) {
    tiles.emplace_back(
      Tile(i, node.p * 1000, node.f.size(), node.e.size(), node.e.size()));
    ++i;
  }

  i = 0;
  for (auto& corner : corners) {
    auto& face = mesh.faces[i];
    for (size_t j = 0; j < face.e.size(); ++j) {
      corner.borders[j] = &borders[face.e[j]];
    }
    for (size_t j = 0; j < face.n.size(); ++j) {
      corner.tiles[j] = &tiles[face.n[j]];
    }
    ++i;
  }

  i = 0;
  for (auto& border : borders) {
    auto& edge = mesh.edges[i];
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
    border.midpoint
      = averageCorner * (1.f / static_cast<float>(border.corners.size()));
    for (size_t j = 0; j < edge.n.size(); ++j) {
      border.tiles[j] = &tiles[edge.n[j]];
    }
    ++i;
  }

  for (auto& corner : corners) {
    for (size_t j = 0; j < corner.borders.size(); ++j) {
      corner.corners[j] = &corner.borders[j]->oppositeCorner(corner);
    }
  }

  i = 0;
  for (auto& tile : tiles) {
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
          else if (border.corners[0] != &corner0
                   || border.corners[1] != &corner1) {
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
          else if (border.corners[1] != &corner0
                   || border.corners[0] != &corner1) {
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
        maxDistanceToCorner,
        Vector3::Distance(tile.corners[j]->position, tile.averagePosition));
    }

    float area = 0;
    for (size_t j = 0; j < tile.borders.size(); ++j) {
      area += Tools::calculateTriangleArea(
        tile.position, tile.borders[j]->corners[0]->position,
        tile.borders[j]->corners[1]->position);
    }
    tile.area   = area;
    tile.normal = tile.position;
    tile.normal.normalize();
    tile.boundingSphere
      = BoundingSphere(tile.averagePosition, maxDistanceToCorner);
    ++i;
  }

  for (auto& corner : corners) {
    corner.area = 0.f;
    for (size_t j = 0; j < corner.tiles.size(); ++j) {
      corner.area += corner.tiles[j]->area
                     / static_cast<float>(corner.tiles[j]->corners.size());
    }
  }

  return true;
}

void World::generatePlanetPartition(std::vector<Tile>& tiles,
                                    SpatialPartition& rootPartition)
{
  Icosphere icosphere;
  icosphere.generateIcosahedron();

  auto& icosahedron = icosphere.icosahedron();

  for (auto& face : icosahedron.faces) {
    auto p0     = icosahedron.nodes[face.n[0]].p * 1000;
    auto p1     = icosahedron.nodes[face.n[1]].p * 1000;
    auto p2     = icosahedron.nodes[face.n[2]].p * 1000;
    auto center = (p0 + p1 + p2) / 3.;
    auto radius = std::max(
      Vector3::Distance(center, p0),
      std::max(Vector3::Distance(center, p2), Vector3::Distance(center, p2)));
    face.boundingSphere = BoundingSphere(center, radius);
  }

  std::vector<Tile*> unparentedTiles;
  float maxDistanceFromOrigin = 0.f;
  for (auto& tile : tiles) {
    maxDistanceFromOrigin
      = std::max(maxDistanceFromOrigin, tile.boundingSphere.center.length()
                                          + tile.boundingSphere.radius);

    bool parentFound = false;
    for (auto& face : icosahedron.faces) {
      float distance = Vector3::Distance(tile.boundingSphere.center,
                                         face.boundingSphere.center)
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
    = BoundingSphere(Vector3(0.f, 0.f, 0.f), maxDistanceFromOrigin);
  rootPartition.tiles = unparentedTiles;
  for (auto& face : icosahedron.faces) {
    rootPartition.partitions.emplace_back(
      SpatialPartition(face.boundingSphere, {}, face.children));
  }
}

void World::generatePlanetTerrain(Planet& planet, size_t plateCount,
                                  float oceanicRate, float heatLevel,
                                  float moistureLevel, IRandomFunction& random)
{
  generatePlanetTectonicPlates(planet.topology, plateCount, oceanicRate, random,
                               planet.plates);
  generatePlanetElevation(planet.topology, planet.plates);
  generatePlanetWeather(planet.topology, planet.partition, heatLevel,
                        moistureLevel, random);
  generatePlanetBiomes(planet.topology.tiles, 1000);
}

void World::generatePlanetTectonicPlates(Topology& topology, size_t plateCount,
                                         float oceanicRate,
                                         IRandomFunction& random,
                                         std::vector<Plate>& plates)
{
  struct Plateless {
    Tile* tile;
    Plate* plate;
  };
  std::vector<Plateless> plateless;

  plates.reserve(plateCount);

  auto failedCount = 0;
  while (plates.size() < plateCount && failedCount < 10000) {
    auto& corner
      = topology.corners[random.integerExclusive(0, topology.corners.size())];
    bool adjacentToExistingPlate = false;
    for (size_t i = 0; i < corner.tiles.size(); ++i) {
      if (corner.tiles[i]->plate) {
        adjacentToExistingPlate = true;
        failedCount += 1;
        break;
      }
    }
    if (adjacentToExistingPlate)
      continue;

    failedCount = 0;

    bool oceanic = (random.unit() < oceanicRate);
    plates.push_back(
      Plate(Color3(random.realInclusive(0, 1), random.realInclusive(0, 1),
                   random.realInclusive(0, 1)),
            Tools::randomUnitVector(random),
            random.realInclusive(-Math::PI / 30.f, Math::PI / 30.f),
            random.realInclusive(-Math::PI / 30.f, Math::PI / 30.f),
            oceanic ? random.realInclusive(-0.8f, -0.3f) :
                      random.realInclusive(0.1f, 0.5f),
            oceanic, corner));
    auto pplate = &plates.back();
    auto& plate = plates.back();

    for (size_t i = 0; i < corner.tiles.size(); ++i) {
      corner.tiles[i]->plate = &plate;
      plate.tiles.push_back(corner.tiles[i]);
    }

    for (size_t i = 0; i < corner.tiles.size(); ++i) {
      auto& tile = *corner.tiles[i];
      for (size_t j = 0; j < tile.tiles.size(); ++j) {
        auto& adjacentTile = *tile.tiles[j];
        if (!adjacentTile.plate) {
          plateless.push_back(Plateless{&adjacentTile, pplate});
        }
      }
    }
  }

  while (plateless.size() > 0) {
    size_t index = static_cast<size_t>(
      std::floor(std::pow(random.unit(), 2.f) * plateless.size()));
    auto indexit = getAdvancedIt(plateless, index);
    auto ptile   = indexit->tile;
    auto pplate  = indexit->plate;
    auto& tile   = *ptile;
    auto& plate  = *pplate;
    plateless.erase(indexit);
    if (!tile.plate) {
      tile.plate = &plate;
      plate.tiles.push_back(ptile);
      for (size_t j = 0; j < tile.tiles.size(); ++j) {
        if (!tile.tiles[j]->plate) {
          plateless.push_back(Plateless{tile.tiles[j], &plate});
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
      distanceCornerQueue.push_back(
        DistanceCorner{corner.corners[j], corner.borders[j]->length()});
    }
  }

  while (distanceCornerQueue.size() > 0) {
    size_t iEnd = distanceCornerQueue.size();
    for (size_t i = 0; i < iEnd; ++i) {
      auto& front              = getAdvancedItVal(distanceCornerQueue, i);
      auto& corner             = *front.corner;
      auto distanceToPlateRoot = front.distanceToPlateRoot;
      if (corner.distanceToPlateRoot == 0.f
          || corner.distanceToPlateRoot > distanceToPlateRoot) {
        corner.distanceToPlateRoot = distanceToPlateRoot;
        for (size_t j = 0; j < corner.corners.size(); ++j) {
          distanceCornerQueue.push_back(
            DistanceCorner{corner.corners[j],
                           distanceToPlateRoot + corner.borders[j]->length()});
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

void World::generatePlanetElevation(Topology& topology,
                                    std::vector<Plate>& /*plates*/)
{
  std::vector<Corner*> boundaryCorners;
  std::vector<size_t> boundaryCornerInnerBorderIndexes;
  std::vector<ElevationBorder> elevationBorderQueue;
  std::function<bool(const ElevationBorder&, const ElevationBorder&)>
    elevationBorderQueueSorter
    = [](const ElevationBorder& left, const ElevationBorder& right) {
        return left.distanceToPlateBoundary < right.distanceToPlateBoundary;
      };

  identifyBoundaryBorders(topology.borders);
  collectBoundaryCorners(topology.corners, boundaryCorners);
  calculatePlateBoundaryStress(boundaryCorners,
                               boundaryCornerInnerBorderIndexes);
  blurPlateBoundaryStress(boundaryCorners, 3, 0.4f);
  populateElevationBorderQueue(
    boundaryCorners, boundaryCornerInnerBorderIndexes, elevationBorderQueue);
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
      border.tiles[0]->plate->boundaryBorders.push_back(&border);
      border.tiles[1]->plate->boundaryBorders.push_back(&border);
    }
  }
}

void World::collectBoundaryCorners(std::vector<Corner>& corners,
                                   std::vector<Corner*>& boundaryCorners)
{
  for (auto& corner : corners) {
    if (corner.betweenPlates) {
      boundaryCorners.push_back(&corner);
      corner.tiles[0]->plate->boundaryCorners.push_back(&corner);
      if (corner.tiles[1]->plate != corner.tiles[0]->plate) {
        corner.tiles[1]->plate->boundaryCorners.push_back(&corner);
      }
      if (corner.tiles[2]->plate != corner.tiles[0]->plate
          && corner.tiles[2]->plate != corner.tiles[1]->plate) {
        corner.tiles[2]->plate->boundaryCorners.push_back(&corner);
      }
    }
  }
}

void World::calculatePlateBoundaryStress(
  const std::vector<Corner*>& boundaryCorners,
  std::vector<size_t>& boundaryCornerInnerBorderIndexes)
{
  boundaryCornerInnerBorderIndexes.resize(boundaryCorners.size());
  size_t i = 0;
  for (auto pcorner : boundaryCorners) {
    auto& corner                   = *pcorner;
    corner.distanceToPlateBoundary = 0;

    Border* innerBorder = nullptr;
    size_t innerBorderIndex;
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
      auto& outerBorder0
        = *corner.borders[(innerBorderIndex + 1) % corner.borders.size()];
      auto& outerBorder1
        = *corner.borders[(innerBorderIndex + 2) % corner.borders.size()];
      auto& farCorner0 = outerBorder0.oppositeCorner(corner);
      auto& farCorner1 = outerBorder1.oppositeCorner(corner);
      auto& plate0     = *innerBorder->tiles[0]->plate;
      auto& plate1     = *(outerBorder0.tiles[0]->plate != &plate0 ?
                         outerBorder0.tiles[0]->plate :
                         outerBorder0.tiles[1]->plate);
      auto boundaryVector = farCorner0.vectorTo(farCorner1);
      auto boundaryNormal = Vector3::Cross(boundaryVector, corner.position);
      Stress stress;
      calculateStress(plate0.calculateMovement(corner.position),
                      plate1.calculateMovement(corner.position), boundaryVector,
                      boundaryNormal, stress);
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
      auto boundaryNormal0 = Vector3::Cross(boundaryVector0, corner.position);
      auto boundaryNormal1 = Vector3::Cross(boundaryVector1, corner.position);
      auto boundaryNormal2 = Vector3::Cross(boundaryVector2, corner.position);
      Stress stress0, stress1, stress2;
      calculateStress(plate0.calculateMovement(corner.position),
                      plate1.calculateMovement(corner.position),
                      boundaryVector0, boundaryNormal0, stress0);
      calculateStress(plate1.calculateMovement(corner.position),
                      plate2.calculateMovement(corner.position),
                      boundaryVector1, boundaryNormal1, stress1);
      calculateStress(plate2.calculateMovement(corner.position),
                      plate0.calculateMovement(corner.position),
                      boundaryVector2, boundaryNormal2, stress2);
      corner.pressure
        = (stress0.pressure + stress1.pressure + stress2.pressure) / 3;
      corner.shear = (stress0.shear + stress1.shear + stress2.shear) / 3;
    }

    ++i;
  }
}

void World::calculateStress(const Vector3& movement0, const Vector3& movement1,
                            const Vector3& boundaryVector,
                            const Vector3& boundaryNormal, Stress& stress)
{
  Vector3 relativeMovement = movement0 - movement1;
  auto pressureVector
    = Tools::projectOnVector(relativeMovement, boundaryNormal);
  float pressure = pressureVector.length();
  if (Vector3::Dot(pressureVector, boundaryNormal) > 0) {
    pressure = -pressure;
  }
  float shear
    = Tools::projectOnVector(relativeMovement, boundaryVector).length();

  stress.pressure = 2.f / (1.f + std::exp(-pressure / 30.f)) - 1.f;
  stress.shear    = 2.f / (1.f + std::exp(-shear / 30.f)) - 1.f;
}

void World::blurPlateBoundaryStress(const std::vector<Corner*>& boundaryCorners,
                                    size_t stressBlurIterations,
                                    float stressBlurCenterWeighting)
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
      newCornerPressure[j]
        = corner.pressure * stressBlurCenterWeighting
          + (averagePressure / neighborCount) * (1 - stressBlurCenterWeighting);
      newCornerShear[j]
        = corner.shear * stressBlurCenterWeighting
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
  const std::vector<Corner*>& /*boundaryCorners*/,
  const std::vector<size_t>& /*boundaryCornerInnerBorderIndexes*/,
  std::vector<ElevationBorder>& /*elevationBorderQueue*/)
{
}

float World::calculateCollidingElevation(float distanceToPlateBoundary,
                                         float distanceToPlateRoot,
                                         float boundaryElevation,
                                         float plateElevation) const
{
  float t
    = distanceToPlateBoundary / (distanceToPlateBoundary + distanceToPlateRoot);
  if (t < 0.5f) {
    t = t / 0.5f;
    return plateElevation
           + std::pow(t - 1.f, 2.f) * (boundaryElevation - plateElevation);
  }
  else {
    return plateElevation;
  }
}

float World::calculateSuperductingElevation(float distanceToPlateBoundary,
                                            float distanceToPlateRoot,
                                            float boundaryElevation,
                                            float plateElevation,
                                            float pressure) const
{
  float t
    = distanceToPlateBoundary / (distanceToPlateBoundary + distanceToPlateRoot);
  if (t < 0.2f) {
    t = t / 0.2f;
    return boundaryElevation
           + t * (plateElevation - boundaryElevation + pressure / 2.f);
  }
  else if (t < 0.5f) {
    t = (t - 0.2f) / 0.3f;
    return plateElevation + std::pow(t - 1.f, 2.f) * pressure / 2.f;
  }
  else {
    return plateElevation;
  }
}

float World::calculateSubductingElevation(float distanceToPlateBoundary,
                                          float distanceToPlateRoot,
                                          float boundaryElevation,
                                          float plateElevation) const
{
  float t
    = distanceToPlateBoundary / (distanceToPlateBoundary + distanceToPlateRoot);
  return plateElevation
         + std::pow(t - 1.f, 2.f) * (boundaryElevation - plateElevation);
}

float World::calculateDivergingElevation(float distanceToPlateBoundary,
                                         float distanceToPlateRoot,
                                         float boundaryElevation,
                                         float plateElevation) const
{
  float t
    = distanceToPlateBoundary / (distanceToPlateBoundary + distanceToPlateRoot);
  if (t < 0.3f) {
    t = t / 0.3f;
    return plateElevation
           + std::pow(t - 1.f, 2.f) * (boundaryElevation - plateElevation);
  }
  else {
    return plateElevation;
  }
}

float World::calculateShearingElevation(float distanceToPlateBoundary,
                                        float distanceToPlateRoot,
                                        float boundaryElevation,
                                        float plateElevation) const
{
  float t
    = distanceToPlateBoundary / (distanceToPlateBoundary + distanceToPlateRoot);
  if (t < 0.2f) {
    t = t / 0.2f;
    return plateElevation
           + std::pow(t - 1.f, 2.f) * (boundaryElevation - plateElevation);
  }
  else {
    return plateElevation;
  }
}

float World::calculateDormantElevation(float distanceToPlateBoundary,
                                       float distanceToPlateRoot,
                                       float boundaryElevation,
                                       float plateElevation) const
{
  float t
    = distanceToPlateBoundary / (distanceToPlateBoundary + distanceToPlateRoot);
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
          front.origin.corner->elevation, front.origin.plate->elevation,
          front.origin.pressure, front.origin.shear);

        for (size_t j = 0; j < corner.borders.size(); ++j) {
          auto& border = *corner.borders[j];
          if (!border.betweenPlates) {
            auto& nextCorner = *corner.corners[j];
            float distanceToPlateBoundary
              = corner.distanceToPlateBoundary + border.length();
            if (nextCorner.distanceToPlateBoundary == 0.f
                || nextCorner.distanceToPlateBoundary
                     > distanceToPlateBoundary) {
              elevationBorderQueue.push_back(
                ElevationBorder(front.origin, &border, &corner, &nextCorner,
                                distanceToPlateBoundary));
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
              elevationBorderQueueSorter);
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

void World::generatePlanetWeather(Topology& topology,
                                  SpatialPartition& /*partition*/,
                                  float heatLevel, float moistureLevel,
                                  IRandomFunction& random)
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

  initializeAirMoisture(topology.corners, moistureLevel, activeCorners,
                        totalMoisture);
  remainingMoisture = totalMoisture;
  float consumedMoisture;
  do {
    consumedMoisture = processAirMoisture(activeCorners);
    remainingMoisture -= consumedMoisture;
  } while (remainingMoisture > 0 && consumedMoisture >= 0.0001f);

  calculateMoisture(topology.corners, topology.tiles);
}

void World::generateAirCurrentWhorls(float /*planetRadius*/,
                                     IRandomFunction& /*random*/,
                                     std::vector<Whorl>& /*whorls*/)
{
}

void World::calculateAirCurrents(std::vector<Corner>& /*corners*/,
                                 const std::vector<Whorl>& /*whorls*/,
                                 float /*planetRadius*/)
{
}

void World::initializeAirHeat(std::vector<Corner>& corners, float heatLevel,
                              std::vector<Corner*>& activeCorners,
                              float& airHeat)
{
  activeCorners.clear();
  airHeat = 0.f;
  for (auto& corner : corners) {
    corner.airHeat    = corner.area * heatLevel;
    corner.newAirHeat = 0.f;
    corner.heat       = 0.f;

    corner.heatAbsorption
      = 0.1f * corner.area
        / std::max(0.1f, std::min(corner.airCurrentSpeed, 1.f));
    if (corner.elevation <= 0.f) {
      corner.maxHeat = corner.area;
    }
    else {
      corner.maxHeat = corner.area;
      corner.heatAbsorption *= 2.f;
    }

    activeCorners.push_back(&corner);
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
      0.f, std::min(corner.airHeat, corner.heatAbsorption
                                      * (1.f - corner.heat / corner.maxHeat)));
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
        activeCorners.push_back(corner.corners[j]);
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

void World::calculateTemperature(std::vector<Corner>& corners,
                                 std::vector<Tile>& tiles, float planetRadius)
{
  for (auto& corner : corners) {
    float latitudeEffect
      = std::sqrt(1.f - std::abs(corner.position.y) / planetRadius);
    float elevationEffect
      = 1.f
        - std::pow(std::max(0.f, std::min(corner.elevation * 0.8f, 1.f)), 2.f);
    float normalizedHeat = corner.heat / corner.area;
    corner.temperature
      = (latitudeEffect * elevationEffect * 0.7f + normalizedHeat * 0.3f) * 5.f
          / 3.f
        - 2.f / 3.f;
  }

  for (auto& tile : tiles) {
    tile.temperature = 0.f;
    for (const auto& pcorner : tile.corners) {
      tile.temperature += pcorner->temperature;
    }
    tile.temperature /= tile.corners.size();
  }
}

void World::initializeAirMoisture(std::vector<Corner>& corners,
                                  float moistureLevel,
                                  std::vector<Corner*>& activeCorners,
                                  float& airMoisture)
{
  airMoisture = 0;
  for (auto& corner : corners) {
    corner.airMoisture
      = (corner.elevation > 0.f) ?
          0.f :
          corner.area * moistureLevel
            * std::max(0.f, std::min(0.5f + corner.temperature * 0.5f, 1.f));
    corner.newAirMoisture = 0.f;
    corner.precipitation  = 0.f;
    corner.precipitationRate
      = 0.0075f * corner.area
        / std::max(0.1f, std::min(corner.airCurrentSpeed, 1.f));
    corner.precipitationRate
      *= 1.f + (1.f - std::max(0.f, std::min(corner.temperature, 1.f))) * 0.1f;
    if (corner.elevation > 0.f) {
      corner.precipitationRate *= 1.f + corner.elevation * 0.5f;
      corner.maxPrecipitation
        = corner.area
          * (0.25f + std::max(0.f, std::min(corner.elevation, 1.f)) * 0.25f);
    }
    else {
      corner.maxPrecipitation = corner.area * 0.25f;
    }

    activeCorners.push_back(&corner);
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

    float moistureChange = std::max(
      0.f,
      std::min(corner.airMoisture,
               corner.precipitationRate
                 * (1.f - corner.precipitation / corner.maxPrecipitation)));
    corner.precipitation += moistureChange;
    consumedMoisture += moistureChange;
    float moistureLoss
      = corner.area * (corner.precipitation / corner.maxPrecipitation) * 0.02f;
    moistureChange
      = std::min(corner.airMoisture, moistureChange + moistureLoss);

    float remainingCornerAirMoisture = corner.airMoisture - moistureChange;
    corner.airMoisture               = 0.f;

    for (size_t j = 0; j < corner.corners.size(); ++j) {
      float outflow = corner.airCurrentOutflows[j];
      if (outflow > 0.f) {
        corner.corners[j]->newAirMoisture
          += remainingCornerAirMoisture * outflow;
        activeCorners.push_back(corner.corners[j]);
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

void World::calculateMoisture(std::vector<Corner>& corners,
                              std::vector<Tile>& tiles)
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

void World::generatePlanetBiomes(std::vector<Tile>& tiles,
                                 float /*planetRadius*/)
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

void World::generatePlanetRenderData(Topology& /*topology*/,
                                     IRandomFunction& /*random*/,
                                     RenderData& /*renderData*/)
{
  /*buildSurfaceRenderObject(topology.tiles, random, *renderData.surface);
  buildPlateBoundariesRenderObject(topology.borders,
                                   *renderData.plateBoundaries);
  buildPlateMovementsRenderObject(topology.tiles, *renderData.plateMovements);
  buildAirCurrentsRenderObject(topology.corners, *renderData.airCurrents)*/;
}

void World::doBuildTileWedge(RenderObject& ro, size_t b, size_t s, size_t t)
{
  ro.triangle(b + s + 2, b, b + t + 2);
  ro.triangle(b + s + 1, b + t + 2, b + t + 1);
  ro.triangle(b + s + 1, b + s + 2, b + t + 2);
}

} // end of namespace Extensions
} // end of namespace BABYLON
