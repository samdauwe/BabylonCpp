#ifndef BABYLON_MISC_HIGH_DYNAMIC_RANGE_CUBE_MAP_INFO_H
#define BABYLON_MISC_HIGH_DYNAMIC_RANGE_CUBE_MAP_INFO_H

#include <babylon/babylon_api.h>
#include <babylon/core/array_buffer_view.h>

namespace BABYLON {

/**
 * @brief CubeMap information grouping all the data for each faces as well as the cubemap size.
 */
struct BABYLON_SHARED_EXPORT CubeMapInfo {

  /**
   * The pixel array for the front face.
   * This is stored in RGB, left to right, up to down format.
   */
  ArrayBufferView front;

  /**
   * The pixel array for the back face.
   * This is stored in RGB, left to right, up to down format.
   */
  ArrayBufferView back;

  /**
   * The pixel array for the left face.
   * This is stored in RGB, left to right, up to down format.
   */
  ArrayBufferView left;

  /**
   * The pixel array for the right face.
   * This is stored in RGB, left to right, up to down format.
   */
  ArrayBufferView right;

  /**
   * The pixel array for the up face.
   * This is stored in RGB, left to right, up to down format.
   */
  ArrayBufferView up;

  /**
   * The pixel array for the down face.
   * This is stored in RGB, left to right, up to down format.
   */
  ArrayBufferView down;

  /**
   * The size of the cubemap stored.
   *
   * Each faces will be size * size pixels.
   */
  std::size_t size;

  /**
   * The format of the texture.
   *
   * RGBA, RGB.
   */
  unsigned int format;

  /**
   * The type of the texture data.
   *
   * UNSIGNED_INT, FLOAT.
   */
  unsigned int type;

  /**
   * Specifies whether the texture is in gamma space.
   */
  bool gammaSpace;

  /**
   * @brief Returns the face data by name.
   */
  ArrayBufferView& operator[](const std::string& face);
  const ArrayBufferView& operator[](const std::string& face) const;

}; // end of struct CubeMapInfo

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_HIGH_DYNAMIC_RANGE_CUBE_MAP_INFO_H
