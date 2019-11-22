#ifndef BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_UTIL_GRADIENT_H
#define BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_UTIL_GRADIENT_H

#include <cmath>
#include <utility>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {
namespace Extensions {

template <class T>
class Gradient {

public:
  using Stop = std::pair<float, T>;

public:
  Gradient(float resolution)
      : _resolution{resolution}, _stops{{}}, _gradient{{}}
  {
  }

  ~Gradient() = default;

  void addStop(float stop, const T& value)
  {
    auto newStop = std::make_pair(stop, value);
    _stops.emplace_back(newStop);
  }

  T& getValue(size_t index)
  {
    const auto idx = std::floor(index * (_resolution - 1.f));
    return _gradient[static_cast<size_t>(idx)];
  }

  void calculate()
  {
    if (_stops.size() < 2) {
      return;
    }

    for (size_t stopIdx = 1; stopIdx < _stops.size(); ++stopIdx) {
      const auto& currentStop = _stops[stopIdx - 1];
      const auto& nextStop    = _stops[stopIdx];

      auto totalSteps
        = std::ceil((nextStop.first - currentStop.first) * _resolution);
      auto step = (nextStop.second - currentStop.second) / (totalSteps - 1);
      
      _gradient.resize(static_cast<size_t>(totalSteps + std::ceil(currentStop.first * _resolution)));

      for (size_t i = 0; i < totalSteps; ++i) {
        auto gradIdx
          = static_cast<size_t>(i + std::ceil(currentStop.first * _resolution));
        auto val           = currentStop.second + (step * i);
        _gradient[gradIdx] = val;
      }
    }
  }

public:
  float _resolution;

private:
  std::vector<Stop> _stops;
  std::vector<T> _gradient;

}; // end of class Gradient

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_UTIL_GRADIENT_H
