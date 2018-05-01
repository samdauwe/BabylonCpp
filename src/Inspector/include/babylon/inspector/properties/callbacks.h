#ifndef BABYLON_INSPECTOR_PROPERTIES_CALLBACKS_H
#define BABYLON_INSPECTOR_PROPERTIES_CALLBACKS_H

#include <babylon/babylon_global.h>

namespace BABYLON {

// Boolean property callbacks
using TBoolGetter = ::std::function<bool()>;
using TBoolSetter = ::std::function<void(const bool)>;

// Float property callbacks
using TFloatGetter = ::std::function<float()>;
using TFloatSetter = ::std::function<void(const float)>;

// String property callbacks
using TStringGetter = ::std::function<string_t()>;
using TStringSetter = ::std::function<void(const string_t)>;

// Color3 property callbacks
using TColor3Getter = ::std::function<Color3&()>;
using TColor3Setter = ::std::function<void(const Color3&)>;

} // end of namespace BABYLON

#endif // end of BABYLON_INSPECTOR_PROPERTIES_CALLBACKS_H
