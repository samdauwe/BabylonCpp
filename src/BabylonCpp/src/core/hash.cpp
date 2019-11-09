/*
Copyright 2017 Google Inc. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS-IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <babylon/core/hash.h>
#include <ctype.h>

namespace BABYLON {

HashValue Hash(const char* str)
{
  const auto npos = -1;
  return Hash(str, npos);
}

HashValue Hash(const char* str, int len)
{
  if (str == nullptr || *str == 0 || len == 0) {
    return 0;
  }

  // A quick good hash, from:
  // https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
  auto count      = 0;
  HashValue value = kHashOffsetBasis;
  while (*str && count < len) {
    value = (value ^ static_cast<unsigned char>(*str++)) * kHashPrimeMultiplier;
    ++count;
  }
  return value;
}

HashValue Hash(const std::string& str)
{
  return Hash(str.data(), static_cast<int>(str.length()));
}

HashValue HashCaseInsensitive(const char* str, size_t len)
{
  if (str == nullptr || *str == 0 || len == 0) {
    return 0;
  }
  size_t count    = 0;
  HashValue value = kHashOffsetBasis;
  while (*str && count < len) {
    value = (value ^ static_cast<unsigned char>(tolower(*str++))) * kHashPrimeMultiplier;
    ++count;
  }
  return value;
}

} // end of namespace BABYLON
