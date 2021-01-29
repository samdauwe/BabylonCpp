#ifndef BABYLON_MISC_ARRAY_TOOLS_H
#define BABYLON_MISC_ARRAY_TOOLS_H

#include <functional>
#include <vector>

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Class containing a set of static utilities functions for arrays.
 */
class BABYLON_SHARED_EXPORT ArrayTools {

  /**
   * @brief Returns an array of the given size filled with element built from the given constructor
   * and the parameters.
   * @param size the number of element to construct and put in the array
   * @param itemBuilder a callback responsible for creating new instance of item. Called once per
   * array entry.
   * @returns a new array filled with new objects
   */
  template <typename T>
  static std::vector<T> BuildArray(size_t size, const std::function<T()>& itemBuilder)
  {
    std::vector<T> a;
    a.reserve(size);
    for (size_t i = 0ull; i < size; ++i) {
      a.emplace_back(itemBuilder());
    }
    return a;
  }

}; // end of struct ArrayTools

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_ARRAY_TOOLS_H
