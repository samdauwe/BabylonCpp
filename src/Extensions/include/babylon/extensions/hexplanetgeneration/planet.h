#ifndef BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_PLANET_H
#define BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_PLANET_H

#include <babylon/babylon_global.h>

#include <babylon/extensions/hexplanetgeneration/planet_statistics.h>
#include <babylon/extensions/hexplanetgeneration/render_data.h>
#include <babylon/extensions/hexplanetgeneration/terrain/spatial_partition.h>
#include <babylon/extensions/hexplanetgeneration/terrain/topology.h>

namespace BABYLON {
namespace Extensions {

struct Plate;

struct Planet {
  Planet();

  unsigned long seed;
  unsigned long originalSeed;
  Topology topology;

  std::vector<Plate> plates;
  SpatialPartition partition;
  RenderData renderData;
  PlanetStatistics statistics;
};

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_PLANET_H
