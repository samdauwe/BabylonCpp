#include <babylon/misc/code_string_parsing_tools.h>

#include <babylon/misc/string_tools.h>

namespace BABYLON {

int CodeStringParsingTools::ExtractBetweenMarkers(char markerOpen, char markerClose,
                                                  const std::string& block, size_t startIndex)
{
  auto currPos            = startIndex;
  auto openMarkers        = 0;
  std::string waitForChar = "";

  while (currPos < block.size()) {
    auto currChar = block[currPos];

    if (waitForChar.empty()) {
      if (currChar == markerOpen) {
        ++openMarkers;
      }
      else if (currChar == markerClose) {
        --openMarkers;
      }
      else if ((currChar == '"') || (currChar == '\'') || (currChar == '`')) {
        waitForChar = std::to_string(currChar);
      }
      else if (currChar == '/') {
        if (currPos + 1 < block.size()) {
          const auto nextChar = block[currPos + 1];
          if (nextChar == '/') {
            waitForChar = '\n';
          }
          else if (nextChar == '*') {
            waitForChar = "*/";
          }
        }
      }
    }
    else {
      if (std::to_string(currChar) == waitForChar) {
        if (waitForChar == "\"" || waitForChar == "\'") {
          if (block[currPos - 1] != '\\') {
            waitForChar = "";
          }
        }
        else {
          waitForChar = "";
        }
      }
      else if (waitForChar == "*/" && currChar == '*' && currPos + 1 < block.size()) {
        if (block[currPos + 1] == '/') {
          waitForChar = "";
        }
        if (waitForChar == "") {
          ++currPos;
        }
      }
    }

    ++currPos;
    if (openMarkers == 0) {
      break;
    }
  }

  return openMarkers == 0 ? static_cast<int>(currPos) - 1 : -1;
}

size_t CodeStringParsingTools::SkipWhitespaces(const std::string& s, size_t index)
{
  while (index < s.size()) {
    const auto c = s[index];
    if (c != ' ' && c != '\n' && c != '\r' && c != '\t' && c != '\u000a' /* && c != '\u00a0' */) {
      break;
    }
    ++index;
  }

  return index;
}

bool CodeStringParsingTools::IsIdentifierChar(const std::string& c)
{
  const int v = static_cast<int>(c[0]);
  return (v >= 48 && v <= 57) ||  // 0-9
         (v >= 65 && v <= 90) ||  // A-Z
         (v >= 97 && v <= 122) || // a-z
         (v == 95);               // _
}

std::string CodeStringParsingTools::RemoveComments(const std::string& block)
{
  auto currPos            = 0ull;
  std::string waitForChar = "";
  auto inComments         = false;
  std::vector<char> s{};

  while (currPos < block.size()) {
    auto currChar = block[currPos];

    if (waitForChar.empty()) {
      if ((currChar == '"') || (currChar == '\'') || (currChar == '`')) {
        waitForChar = std::to_string(currChar);
      }
      else if (currChar == '/') {
        if (currPos + 1 < block.size()) {
          const auto nextChar = block[currPos + 1];
          if (nextChar == '/') {
            waitForChar = '\n';
            inComments  = true;
          }
          else if (nextChar == '*') {
            waitForChar = "*/";
            inComments  = true;
          }
        }
      }
      if (!inComments) {
        s.emplace_back(currChar);
      }
    }
    else {
      if (std::to_string(currChar) == waitForChar) {
        if (waitForChar == "\"" || waitForChar == "\'") {
          if (block[currPos - 1] != '\\') {
            waitForChar = "";
          }
          s.emplace_back(currChar);
        }
        else {
          waitForChar = "";
          inComments  = false;
        }
      }
      else if (waitForChar == "*/" && currChar == '*' && currPos + 1 < block.size()) {
        if (block[currPos + 1] == '/') {
          waitForChar = "";
        }
        if (waitForChar == "") {
          inComments = false;
          ++currPos;
        }
      }
      else {
        if (!inComments) {
          s.emplace_back(currChar);
        }
      }
    }

    ++currPos;
  }

  return StringTools::join(s, "");
}

int CodeStringParsingTools::FindBackward(const std::string& s, int index, char c)
{
  while (index >= 0 && s[index] != c) {
    --index;
  }

  return index;
}

std::string CodeStringParsingTools::EscapeRegExp(const std::string& s)
{
  return StringTools::regexReplace(s, R"([.*+?^%s()|[\]\\])", R"(\\$&)");
}

} // end of namespace BABYLON
