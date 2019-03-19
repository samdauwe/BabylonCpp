/*
   base64.cpp and base64.h

   Copyright (C) 2004-2008 René Nyffenegger

   This source code is provided 'as-is', without any express or implied
   warranty. In no event will the author be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this source code must not be misrepresented; you must not
      claim that you wrote the original source code. If you use this source code
      in a product, an acknowledgment in the product documentation would be
      appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
      misrepresented as being the original source code.

   3. This notice may not be removed or altered from any source distribution.

   René Nyffenegger rene.nyffenegger@adp-gmbh.ch

*/

#ifndef BABYLON_UTILS_BASE64_H
#define BABYLON_UTILS_BASE64_H

#include <iostream>
#include <string>

#include <babylon/babylon_api.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT Base64 {

  static const std::string base64_chars;

  static inline bool is_base64(unsigned char c)
  {
    return (isalnum(c) || (c == '+') || (c == '/'));
  }

  static std::string encode(unsigned char const* bytes_to_encode,
                            unsigned int in_len);

  static std::string decode(std::string const& encoded_string);

  /**
   * @brief Encodes a string in base-64. This method uses the "A-Z", "a-z",
   * "0-9",
   * "+", "/" and "=" characters to encode the string.
   * @param str The string to be encoded.
   * @return A String, representing the base-64 encoded string.
   */
  static std::string btoa(const std::string& str);

  /**
   * @brief Decodes a base-64 encoded string.
   * @param str The string which has been encoded by the btoa() method.
   * @return A String, representing the decoded string.
   */
  static std::string atob(const std::string& str);

}; // end of class Base64

} // end of namespace BABYLON

#endif // end of BABYLON_UTILS_BASE64_H
