#ifndef BABYLON_EXTENSIONS_SHADER_BUILDER_INUT_H
#define BABYLON_EXTENSIONS_SHADER_BUILDER_INUT_H

#include <babylon/babylon_api.h>
#include <babylon/extensions/shaderbuilder/inut_bone.h>

namespace BABYLON {
namespace Extensions {

struct BABYLON_SHARED_EXPORT INut {
  std::vector<INutBone> bones;
  std::string frame;
  std::vector<std::string> array;
}; // end of struct INut

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_SHADER_BUILDER_INUT_H
