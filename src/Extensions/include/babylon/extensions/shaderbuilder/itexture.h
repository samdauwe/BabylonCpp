#ifndef BABYLON_EXTENSIONS_SHADER_BUILDER_ITEXTURE_H
#define BABYLON_EXTENSIONS_SHADER_BUILDER_ITEXTURE_H

#include <string>

#include <babylon/babylon_api.h>

namespace BABYLON {
namespace Extensions {

struct BABYLON_SHARED_EXPORT ITexture {
  ITexture(const std::string iKey, bool iInVertex, bool iInFragment)
      : key(iKey), inVertex{iInVertex}, inFragment{iInFragment}
  {
  }
  ~ITexture()
  {
  }

  std::string key;
  bool inVertex;
  bool inFragment;
}; // end of struct ITexture

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_SHADER_BUILDER_ITEXTURE_H
