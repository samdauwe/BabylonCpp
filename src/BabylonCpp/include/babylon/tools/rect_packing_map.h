#ifndef BABYLON_TOOLS_RECT_PACKED_MAP_H
#define BABYLON_TOOLS_RECT_PACKED_MAP_H

#include <babylon/babylon_global.h>
#include <babylon/tools/packed_rect.h>

namespace BABYLON {

/**
 * @brief The purpose of this class is to pack several Rectangles into a big
 * map, while trying to fit everything as optimally as possible.
 * This class is typically used to build lightmaps, sprite map or to pack
 * several little textures into a big one.
 * Note that this class allows allocated Rectangles to be freed: that is the map
 * is dynamically maintained so you can add/remove rectangle based on their
 * life-cycle.
 */
struct BABYLON_SHARED_EXPORT RectPackingMap : public PackedRect {

  /**
   * Create an instance of the object with a dimension using the given size
   * @param size The dimension of the rectangle that will contain all the sub
   * ones.
   */
  RectPackingMap(const Size& size);
  ~RectPackingMap();

  /**
   * Add a rectangle, finding the best location to store it into the map
   * @param size the dimension of the rectangle to store
   * @return the Node containing the rectangle information, or null if we
   * couldn't find a free spot
   */
  PackedRect* addRect(const Size& size);

  /**
   * Return the current space free normalized between [0;1]
   * @returns {}
   */
  size_t freeSpace();

}; // end of struct RectPackingMap

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_RECT_PACKED_MAP_H
