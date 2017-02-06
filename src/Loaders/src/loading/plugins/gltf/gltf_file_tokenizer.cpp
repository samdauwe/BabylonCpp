#include <babylon/loading/plugins/gltf/gltf_file_tokenizer.h>

namespace BABYLON {

Tokenizer::Tokenizer(const std::string& toParse)
    : _toParse{toParse}, _maxPos{toParse.size()}
{
}

Tokenizer::~Tokenizer()
{
}

ETokenType Tokenizer::getNextToken()
{
  if (isEnd()) {
    return ETokenType::END_OF_INPUT;
  }

  currentString = read();
  currentToken  = ETokenType::UNKNOWN;

  if (currentString == '_' || std::isalnum(currentString)) {
    currentToken      = ETokenType::IDENTIFIER;
    currentIdentifier = currentString;
    while (!isEnd()) {
      currentString = peek();
      if (std::isalnum(currentString) || currentString == '_') {
        currentIdentifier += currentString;
        forward();
      }
    }
  }

  return currentToken;
}

char Tokenizer::peek() const
{
  if (!isEnd()) {
    return ' ';
  }

  return _toParse.at(_pos);
}

char Tokenizer::read()
{
  return _toParse.at(_pos++);
}

void Tokenizer::forward()
{
  _pos++;
}

bool Tokenizer::isEnd() const
{
  return _pos >= _maxPos;
}

} // end of namespace BABYLON
