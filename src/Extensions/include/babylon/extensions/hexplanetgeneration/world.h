#ifndef BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_WORLD_H
#define BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_WORLD_H

#include <babylon/babylon_global.h>

namespace BABYLON {
namespace Extensions {

struct Border;
struct Corner;
struct ElevationBorder;
struct IcoNode;
struct IcosahedronMesh;
struct IRandomFunction;
struct Planet;
struct PlanetStatistics;
struct Plate;
struct RenderData;
struct RenderObject;
struct SpatialPartition;
struct Stress;
struct Tile;
struct Topology;
struct Whorl;

class BABYLON_SHARED_EXPORT World {

public:
  World();
  ~World();

  bool generatePlanetTopology(const IcosahedronMesh& mesh, Topology& ret);
  void generatePlanetPartition(std::vector<Tile>& tiles,
                               SpatialPartition& rootPartition);
  void generatePlanetTerrain(Planet& planet, size_t plateCount,
                             float oceanicRate, float heatLevel,
                             float moistureLevel, IRandomFunction& random);
  void generatePlanetTectonicPlates(Topology& topology, size_t plateCount,
                                    float oceanicRate, IRandomFunction& random,
                                    std::vector<Plate>& plates);
  void calculateCornerDistancesToPlateRoot(std::vector<Plate>& plates);
  void generatePlanetElevation(Topology& topology, std::vector<Plate>& plates);
  void identifyBoundaryBorders(std::vector<Border>& borders);
  void collectBoundaryCorners(std::vector<Corner>& corners,
                              std::vector<Corner*>& boundaryCorners);
  void calculatePlateBoundaryStress(
    const std::vector<Corner*>& boundaryCorners,
    std::vector<size_t>& boundaryCornerInnerBorderIndexes);
  void calculateStress(const Vector3& movement0, const Vector3& movement1,
                       const Vector3& boundaryVector,
                       const Vector3& boundaryNormal, Stress& stress);
  void blurPlateBoundaryStress(const std::vector<Corner*>& boundaryCorners,
                               size_t stressBlurIterations,
                               float stressBlurCenterWeighting);
  void populateElevationBorderQueue(
    const std::vector<Corner*>& boundaryCorners,
    const std::vector<size_t>& boundaryCornerInnerBorderIndexes,
    std::vector<ElevationBorder>& elevationBorderQueue);
  float calculateCollidingElevation(float distanceToPlateBoundary,
                                    float distanceToPlateRoot,
                                    float boundaryElevation,
                                    float plateElevation) const;
  float calculateSuperductingElevation(float distanceToPlateBoundary,
                                       float distanceToPlateRoot,
                                       float boundaryElevation,
                                       float plateElevation,
                                       float pressure) const;
  float calculateSubductingElevation(float distanceToPlateBoundary,
                                     float distanceToPlateRoot,
                                     float boundaryElevation,
                                     float plateElevation) const;
  float calculateDivergingElevation(float distanceToPlateBoundary,
                                    float distanceToPlateRoot,
                                    float boundaryElevation,
                                    float plateElevation) const;
  float calculateShearingElevation(float distanceToPlateBoundary,
                                   float distanceToPlateRoot,
                                   float boundaryElevation,
                                   float plateElevation) const;
  float calculateDormantElevation(float distanceToPlateBoundary,
                                  float distanceToPlateRoot,
                                  float boundaryElevation,
                                  float plateElevation) const;
  void processElevationBorderQueue(
    std::vector<ElevationBorder>& elevationBorderQueue,
    const std::function<bool(const ElevationBorder&, const ElevationBorder&)>&
      elevationBorderQueueSorter);
  void calculateTileAverageElevations(std::vector<Tile>& tiles);
  void generatePlanetWeather(Topology& topology, SpatialPartition& partition,
                             float heatLevel, float moistureLevel,
                             IRandomFunction& random);
  void generateAirCurrentWhorls(float planetRadius, IRandomFunction& random,
                                std::vector<Whorl>& whorls);
  void calculateAirCurrents(std::vector<Corner>& corners,
                            const std::vector<Whorl>& whorls,
                            float planetRadius);
  void initializeAirHeat(std::vector<Corner>& corners, float heatLevel,
                         std::vector<Corner*>& activeCorners, float& airHeat);
  float processAirHeat(std::vector<Corner*>& activeCorners);
  void calculateTemperature(std::vector<Corner>& corners,
                            std::vector<Tile>& tiles, float planetRadius);
  void initializeAirMoisture(std::vector<Corner>& corners, float moistureLevel,
                             std::vector<Corner*>& activeCorners,
                             float& airMoisture);
  float processAirMoisture(std::vector<Corner*>& activeCorners);
  void calculateMoisture(std::vector<Corner>& corners,
                         std::vector<Tile>& tiles);
  void generatePlanetBiomes(std::vector<Tile>& tiles, float planetRadius);
  void generatePlanetRenderData(Topology& topology, IRandomFunction& random,
                                RenderData& renderData);
  void doBuildTileWedge(RenderObject& ro, size_t b, size_t s, size_t t);
  void buildSurfaceRenderObject(std::vector<Tile>& tiles,
                                IRandomFunction& random, RenderObject& ro);
  void buildPlateBoundariesRenderObject(std::vector<Border>& borders,
                                        RenderObject& ro);
  void buildPlateMovementsRenderObject(std::vector<Tile>& tiles,
                                       RenderObject& ro);
  void buildAirCurrentsRenderObject(std::vector<Corner>& corners,
                                    RenderObject& ro);
  void buildArrow(RenderObject& ro, const Vector3& position,
                  const Vector3& direction, const Vector3& normal,
                  float baseWidth, const Color3& color);
  void generatePlanetStatistics(Topology& topology, std::vector<Plate>& plates,
                                PlanetStatistics& planetStatistics);

private:
  static const size_t UdefIdx;
  using RotationPredicateType = std::function<bool(
    const IcoNode&, const IcoNode&, const IcoNode&, const IcoNode&)>;

}; // end of class World

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_WORLD_H
