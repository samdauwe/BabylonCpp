///
/// anax
/// An open source C++ entity system.
///
/// anax
/// An open source C++ entity system.
///
/// Copyright (C) 2013-2014 Miguel Martin (miguel@miguel-martin.com)
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to
/// deal in the Software without restriction, including without limitation the
/// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
/// sell copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
/// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
/// IN THE SOFTWARE.
///

#ifndef BABYLON_EXTENSIONS_ENTITY_COMPONENT_SYSTEM_DETAIL_ANAX_ASSERT_H
#define BABYLON_EXTENSIONS_ENTITY_COMPONENT_SYSTEM_DETAIL_ANAX_ASSERT_H

#ifndef ANAX_TEST_CASE_BUILD

#include <cassert>
#define ANAX_ASSERT(condition, failMessage) assert((condition) && failMessage)

#else

#include <stdexcept>
#include <string>

namespace BABYLON {
namespace Extensions {
namespace ECS {

using TestException = std::runtime_error;

} // end of namespace ECS
} // end of namespace Extensions
} // end of namespace BABYLON

// assertion failed: <file>:line: <function> <failMessage>
#define ANAX_ASSERT(condition, failMessage)                                    \
  if (!(condition))                                                            \
    throw anax::TestException{                                                 \
      std::string{"assertion failed: "} + std::string{__FILE__}                \
      + std::string{": "} + std::to_string(__LINE__) + " "                     \
      + std::string{__func__} + " " + std::string{failMessage}};
#endif // ANAX_TEST_CASES

#endif // BABYLON_EXTENSIONS_ENTITY_COMPONENT_SYSTEM_DETAIL_ANAX_ASSERT_H
