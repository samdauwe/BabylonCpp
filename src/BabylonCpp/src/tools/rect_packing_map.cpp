#include <babylon/tools/rect_packing_map.h>

namespace BABYLON {

RectPackingMap::RectPackingMap(const Size& size)
    : PackedRect(nullptr, nullptr, Vector2::Zero(), size)
{
  _root = this;
}

RectPackingMap::~RectPackingMap()
{
}

PackedRect* RectPackingMap::addRect(const Size& size)
{
  return findAndSplitNode(size);
}

size_t RectPackingMap::freeSpace()
{
  const size_t freeSize = evalFreeSize(freeSize);
  return freeSize / static_cast<size_t>(_size.width * _size.height);
}

} // end of namespace BABYLON
