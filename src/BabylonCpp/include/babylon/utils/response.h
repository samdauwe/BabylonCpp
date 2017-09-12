#ifndef response_h
#define response_h

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class Response {
public:
  struct IncorrectStartLineException {
    const std::string startLine;
  };

protected:
  std::string _body;
  std::vector<std::string> _headers;
  std::string _httpVersion;
  int _statusCode;
  std::string _statusDescription;

  static void parseStartLine(
    const std::string& startLine, decltype(_httpVersion)& httpVersion,
    decltype(_statusCode)& statusCode,
    decltype(_statusDescription)& statusDescription) noexcept(false)
  {
    std::stringstream stream(startLine);
    if (stream.good()) {
      stream >> httpVersion;
      if (stream.good()) {
        stream >> statusCode;
        if (stream.good()) {
          while (stream.good()) {
            std::string word;
            stream >> word;
            statusDescription += word + " ";
          }
        }
        else {
          throw IncorrectStartLineException{startLine};
        }
      }
      else {
        throw IncorrectStartLineException{startLine};
      }
    }
    else {
      throw IncorrectStartLineException{startLine};
    }
  }

public:
  Response(const std::string& startLine, decltype(_headers)&& headers,
           decltype(_body)&& body) noexcept(false)
      : _body(::std::move(body)), _headers(::std::move(headers))
  {
    parseStartLine(startLine, _httpVersion, _statusCode, _statusDescription);
  }

  Response(decltype(_statusCode) statusCode_,
           decltype(_statusDescription)&& statusDescription_,
           decltype(_body)&& body_)
      : _body(::std::move(body_))
      , _statusCode(statusCode_)
      , _statusDescription(::std::move(statusDescription_))
  {
  }

  const decltype(_body)& body() const
  {
    return _body;
  }

  const decltype(_headers)& headers() const
  {
    return _headers;
  }

  const decltype(_httpVersion)& httpVersion() const
  {
    return _httpVersion;
  }

  const decltype(_statusCode) statusCode() const
  {
    return _statusCode;
  }

  const decltype(_statusDescription)& statusDescription() const
  {
    return _statusDescription;
  }
};

#endif // end of response_h