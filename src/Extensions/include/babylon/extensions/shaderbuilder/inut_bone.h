#ifndef BABYLON_EXTENSIONS_SHADER_BUILDER_INUT_BONE_H
#define BABYLON_EXTENSIONS_SHADER_BUILDER_INUT_BONE_H

#include <babylon/babylon_global.h>
#include <babylon/extensions/shaderbuilder/ivector3.h>

namespace BABYLON {
namespace Extensions {

struct BABYLON_SHARED_EXPORT INutBone {
  std::string bet;
  std::string center;
  IVector3 rotation;
}; // end of struct INutBone

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_SHADER_BUILDER_INUT_BONE_H
