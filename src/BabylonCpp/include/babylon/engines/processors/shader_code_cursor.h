#ifndef BABYLON_ENGINES_PROCESSORS_SHADER_CODE_CURSOR_H
#define BABYLON_ENGINES_PROCESSORS_SHADER_CODE_CURSOR_H

#include <string>
#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

/**
 * @brief Hidden
 */
class BABYLON_SHARED_EXPORT ShaderCodeCursor {

public:
  ShaderCodeCursor();
  ShaderCodeCursor(const ShaderCodeCursor& other);
  ShaderCodeCursor(ShaderCodeCursor&& other);
  ShaderCodeCursor& operator=(const ShaderCodeCursor& other);
  ShaderCodeCursor& operator=(ShaderCodeCursor&& other);
  ~ShaderCodeCursor(); // = default

protected:
  std::string get_currentLine() const;
  bool get_canRead() const;
  void set_lines(const std::vector<std::string>& value);

public:
  int lineIndex;
  ReadOnlyProperty<ShaderCodeCursor, std::string> currentLine;
  ReadOnlyProperty<ShaderCodeCursor, bool> canRead;
  WriteOnlyProperty<ShaderCodeCursor, std::vector<std::string>> lines;

private:
  std::vector<std::string> _lines;

}; // end of class ShaderCodeCursor

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_PROCESSORS_SHADER_CODE_CURSOR_H
