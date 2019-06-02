#include <babylon/misc/highdynamicrange/cube_map_info.h>

namespace BABYLON {

ArrayBufferView& CubeMapInfo::operator[](const std::string& face)
{
  if (face == "front") { // Front face
    return front;
  }
  else if (face == "back") { // Back face
    return back;
  }
  else if (face == "left") { // Left Face
    return left;
  }
  else if (face == "right") { // Right face
    return right;
  }
  else if (face == "up") { // Up face
    return up;
  }
  else if (face == "down") { // Down face
    return down;
  }

  // Return front face by default
  return front;
}

const ArrayBufferView& CubeMapInfo::operator[](const std::string& face) const
{
  if (face == "front") { // Front face
    return front;
  }
  else if (face == "back") { // Back face
    return back;
  }
  else if (face == "left") { // Left Face
    return left;
  }
  else if (face == "right") { // Right face
    return right;
  }
  else if (face == "up") { // Up face
    return up;
  }
  else if (face == "down") { // Down face
    return down;
  }

  // Return front face by default
  return front;
}

} // end of namespace BABYLON
