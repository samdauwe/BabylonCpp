#ifndef BABYLON_EXTENSIONS_SHADER_BUILDER_IMAP_H
#define BABYLON_EXTENSIONS_SHADER_BUILDER_IMAP_H

#include <babylon/babylon_api.h>
#include <babylon/extensions/shaderbuilder/ivector3.h>

namespace BABYLON {
namespace Extensions {

struct BABYLON_SHARED_EXPORT IMap {
  std::string path;
  float x;
  float y;
  float scaleX;
  float scaleY;
  IVector3 rotation;
  bool useInVertex;
  std::string uv;
  std::string normal;
  std::string normalLevel;
  std::string bias;
  bool alpha;
  int rowIndex;
  int columnIndex;
  int indexCount;
  bool tiled;
  bool animation;
  float animationSpeed;
  float animationFrameEnd;
  float animationFrameStart;
  std::string index;
}; // end of struct IMap

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_SHADER_BUILDER_IMAP_H
