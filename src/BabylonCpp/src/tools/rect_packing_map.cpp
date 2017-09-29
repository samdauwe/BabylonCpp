#include <babylon/tools/rect_packing_map.h>

namespace BABYLON {

RectPackingMap::RectPackingMap(const Size& size, float margin)
    : PackedRect{nullptr, nullptr, Vector2::Zero(), size}
{
  _margin = margin;
  _root   = this;
}

RectPackingMap::~RectPackingMap()
{
}

PackedRect* RectPackingMap::addRect(const Size& size)
{
  return findAndSplitNode(size);
}

float RectPackingMap::freeSpace()
{
  const size_t freeSize = evalFreeSize(freeSize);
  return freeSize / static_cast<float>(_size.width * _size.height);
}

} // end of namespace BABYLON
