#ifndef BABYLON_INSPECTOR_PROPERTIES_CALLBACKS_H
#define BABYLON_INSPECTOR_PROPERTIES_CALLBACKS_H

#include <functional>

#include <babylon/babylon_api.h>

namespace BABYLON {

// Primitive property callbacks
template <typename T>
using TPrimitiveGetter = std::function<T()>;
template <typename T>
using TPrimitiveSetter = std::function<void(const T)>;

// Babylon property callbacks
template <typename T>
using TBabylonGetter = std::function<T const&()>;
template <typename T>
using TBabylonNonConstGetter = std::function<T&()>;
template <typename T>
using TBabylonSetter = std::function<void(const T&)>;

} // end of namespace BABYLON

#endif // end of BABYLON_INSPECTOR_PROPERTIES_CALLBACKS_H
