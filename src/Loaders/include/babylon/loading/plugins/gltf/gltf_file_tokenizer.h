#ifndef BABYLON_LOADING_PLUGINS_GLTF_GLTF_FILE_TOKENIZER_H
#define BABYLON_LOADING_PLUGINS_GLTF_GLTF_FILE_TOKENIZER_H

#include <babylon/babylon_global.h>
#include <babylon/loading/plugins/gltf/gltf_file_loader_enums.h>

namespace BABYLON {

/**
 * @brief Tokenizer. Used for shaders compatibility. Automatically map world,
 * view, projection, worldViewProjection, attributes and so on
 */
class BABYLON_SHARED_EXPORT Tokenizer {

public:
  Tokenizer(const std::string& toParse);
  ~Tokenizer();

  ETokenType getNextToken();
  char peek() const;
  char read();
  void forward();
  bool isEnd() const;

public:
  ETokenType currentToken;
  std::string currentIdentifier;
  char currentString;

private:
  std::string _toParse;
  size_t _pos;
  size_t _maxPos;

}; // end of class Tokenizer

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_PLUGINS_GLTF_GLTF_FILE_TOKENIZER_H
