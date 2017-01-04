#ifndef BABYLON_EXTENSIONS_SHADER_BUILDER_IREFLECT_MAP_H
#define BABYLON_EXTENSIONS_SHADER_BUILDER_IREFLECT_MAP_H

#include <babylon/babylon_global.h>
#include <babylon/extensions/shaderbuilder/ivector3.h>

namespace BABYLON {
namespace Extensions {

struct BABYLON_SHARED_EXPORT IReflectMap {
  std::string path;
  float x;
  float y;
  float scaleX;
  float scaleY;
  bool equirectangular;
  IVector3 rotation;
  bool useInVertex;
  std::string uv;
  std::string normal;
  std::string normalLevel;
  std::string bias;
  bool alpha;
  bool revers;
  std::string reflectMap;
  bool refract;
  std::string refractMap;
}; // end of struct IReflectMap

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_SHADER_BUILDER_IREFLECT_MAP_H
