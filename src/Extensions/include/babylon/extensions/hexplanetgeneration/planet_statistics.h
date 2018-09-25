#ifndef BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_PLANET_STATISTICS_H
#define BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_PLANET_STATISTICS_H

#include <limits>
#include <map>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {
namespace Extensions {

template <typename T>
struct ValueStats {
  ValueStats()
  {
    reset();
  }
  T min, avg, max;
  void reset()
  {
    min = std::numeric_limits<T>::max();
    max = std::numeric_limits<T>::lowest();
    avg = 0;
  }
};

template <typename T>
void updateMinMaxAvg(ValueStats<T>& stats, T value)
{
  stats.min = std::min(stats.min, value);
  stats.max = std::max(stats.max, value);
  stats.avg += value;
}

struct PlanetStatistics {
  struct Corners {
    size_t count;
    ValueStats<float> airCurrent;
    ValueStats<float> elevation;
    ValueStats<float> temperature;
    ValueStats<float> moisture;
    ValueStats<float> distanceToPlateBoundary;
    ValueStats<float> distanceToPlateRoot;
    ValueStats<float> pressure;
    ValueStats<float> shear;
    size_t doublePlateBoundaryCount;
    size_t triplePlateBoundaryCount;
    size_t innerLandBoundaryCount;
    size_t outerLandBoundaryCount;
  };
  Corners corners;

  struct Borders {
    size_t count;
    ValueStats<float> length;
    size_t plateBoundaryCount;
    float plateBoundaryPercentage;
    size_t landBoundaryCount;
    float landBoundaryPercentage;
  };
  Borders borders;

  struct Tiles {
    size_t count;
    float totalArea;
    ValueStats<float> area;
    ValueStats<float> elevation;
    ValueStats<float> temperature;
    ValueStats<float> moisture;
    ValueStats<float> plateMovement;
    std::map<std::string, size_t> biomeCounts;
    std::map<std::string, float> biomeAreas;
    size_t pentagonCount;
    size_t hexagonCount;
    size_t heptagonCount;
  };
  Tiles tiles;

  struct Plates {
    size_t count;
    ValueStats<size_t> tileCount;
    ValueStats<float> area;
    ValueStats<float> boundaryElevation;
    ValueStats<size_t> boundaryBorders;
    ValueStats<float> circumference;
  };
  Plates plates;
};

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_PLANET_STATISTICS_H
