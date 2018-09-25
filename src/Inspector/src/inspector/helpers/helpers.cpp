#include <babylon/inspector/helpers/helpers.h>

#include <babylon/core/string.h>

namespace BABYLON {

bool Helpers::IsSystemName(const std::string& name)
{
  if (name.empty()) {
    return false;
  }

  return (String::indexOf(name, "###") == 0)
         && (String::lastIndexOf(name, "###")
             == (static_cast<int>(name.size()) - 3));
}

} // end of namespace BABYLON
