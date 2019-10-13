#include <babylon/misc/guid.h>

#include <array>

#include <babylon/core/random.h>

namespace BABYLON {

std::string GUID::RandomId()
{
  // from
  // http://stackoverflow.com/questions/105034/how-to-create-a-guid-uuid-in-javascript/2117523#answer-2117523
  std::string randomId = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";
  const std::array<unsigned int, 16> yconv{
    {8, 9, 10, 11, 8, 9, 10, 11, 8, 9, 10, 11, 8, 9, 10, 11}};
  const std::array<char, 16> hex{{'0', '1', '2', '3', '4', '5', '6', '7', '8',
                                  '9', 'a', 'b', 'c', 'd', 'e', 'f'}};
  const auto randomFloats = Math::randomList(0.f, 1.f, randomId.size());
  for (unsigned int i = 0; i < randomId.size(); ++i) {
    const char c = randomId[i];
    if (c == 'x' || c == 'y') {
      const unsigned int r = static_cast<unsigned int>(randomFloats[i] * 16);
      randomId[i]          = (c == 'x') ? hex[r] : hex[yconv[r]];
    }
  }
  return randomId;
}

} // end of namespace BABYLON
