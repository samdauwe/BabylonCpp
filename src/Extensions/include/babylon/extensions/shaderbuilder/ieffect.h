#ifndef BABYLON_EXTENSIONS_SHADER_BUILDER_IEFFECT_H
#define BABYLON_EXTENSIONS_SHADER_BUILDER_IEFFECT_H

#include <string>

#include <babylon/babylon_api.h>

namespace BABYLON {
namespace Extensions {

struct BABYLON_SHARED_EXPORT IEffect {
  std::string px;
  std::string py;
  std::string pz;
  std::string pw;
  std::string pr;
}; // end of struct IEffect

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_SHADER_BUILDER_IEFFECT_H
