#include <babylon/engines/processors/shader_code_cursor.h>

#include <babylon/core/string.h>

namespace BABYLON {

ShaderCodeCursor::ShaderCodeCursor()
    : lineIndex{-1}
    , currentLine{this, &ShaderCodeCursor::get_currentLine}
    , canRead{this, &ShaderCodeCursor::get_canRead}
    , lines{this, &ShaderCodeCursor::set_lines}
{
}

ShaderCodeCursor::ShaderCodeCursor(const ShaderCodeCursor& other)
    : lineIndex{other.lineIndex}
    , currentLine{this, &ShaderCodeCursor::get_currentLine}
    , canRead{this, &ShaderCodeCursor::get_canRead}
    , lines{this, &ShaderCodeCursor::set_lines}
    , _lines{other._lines}
{
}

ShaderCodeCursor::ShaderCodeCursor(ShaderCodeCursor&& other)
    : lineIndex{std::move(other.lineIndex)}
    , currentLine{this, &ShaderCodeCursor::get_currentLine}
    , canRead{this, &ShaderCodeCursor::get_canRead}
    , lines{this, &ShaderCodeCursor::set_lines}
    , _lines{std::move(other._lines)}
{
}

ShaderCodeCursor& ShaderCodeCursor::operator=(const ShaderCodeCursor& other)
{
  if (&other != this) {
    lineIndex = other.lineIndex;
    _lines    = other._lines;
  }

  return *this;
}

ShaderCodeCursor& ShaderCodeCursor::operator=(ShaderCodeCursor&& other)
{
  if (&other != this) {
    lineIndex = std::move(other.lineIndex);
    _lines    = std::move(other._lines);
  }

  return *this;
}

ShaderCodeCursor::~ShaderCodeCursor() = default;

std::string ShaderCodeCursor::get_currentLine() const
{
  if (lineIndex >= 0 && lineIndex < static_cast<int>(_lines.size())) {
    return _lines[static_cast<size_t>(lineIndex)];
  }

  return "";
}

bool ShaderCodeCursor::get_canRead() const
{
  return lineIndex < static_cast<int>(_lines.size()) - 1;
}

void ShaderCodeCursor::set_lines(const std::vector<std::string>& value)
{
  _lines = {};

  for (const auto& line : value) {
    // Prevent removing line break in macros.
    if (line[0] == '#') {
      _lines.emplace_back(line);
      continue;
    }

    auto split = String::split(line, ';');

    for (size_t index = 0; index < split.size(); ++index) {
      auto& subLine = split[index];
      subLine       = String::trim(subLine);

      if (subLine.empty()) {
        continue;
      }

      _lines.emplace_back(subLine + (index != split.size() - 1 ? ";" : ""));
    }
  }
}

} // end of namespace BABYLON
