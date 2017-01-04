#ifndef BABYLON_EXTENSIONS_SHADER_BUILDER_ITEXTURE_H
#define BABYLON_EXTENSIONS_SHADER_BUILDER_ITEXTURE_H

#include <babylon/babylon_global.h>

namespace BABYLON {
namespace Extensions {

struct BABYLON_SHARED_EXPORT ITexture {
  std::string key;
  bool inVertex;
  bool inFragment;
}; // end of struct ITexture

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_SHADER_BUILDER_ITEXTURE_H
