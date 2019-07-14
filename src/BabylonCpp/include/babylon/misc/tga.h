#ifndef BABYLON_MISC_TGA_H
#define BABYLON_MISC_TGA_H

#include <array>
#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

class InternalTexture;
using InternalTexturePtr = std::shared_ptr<InternalTexture>;

struct TGAHeader {
  uint32_t id_length;
  uint32_t colormap_type;
  uint32_t image_type;
  uint32_t colormap_index;
  uint32_t colormap_length;
  uint32_t colormap_size;
  std::array<uint32_t, 2> origin;
  int32_t width;
  int32_t height;
  uint32_t pixel_size;
  uint32_t flags;

}; // end of struct TGAHeader

/**
 * @brief Based on jsTGALoader - Javascript loader for TGA file
 * By Vincent Thibault
 * @see http://blog.robrowser.com/javascript-tga-loader.html
 */
class BABYLON_SHARED_EXPORT TGATools {

private:
  // static constexpr unsigned int _TYPE_NO_DATA = 0;
  static constexpr unsigned int _TYPE_INDEXED     = 1;
  static constexpr unsigned int _TYPE_RGB         = 2;
  static constexpr unsigned int _TYPE_GREY        = 3;
  static constexpr unsigned int _TYPE_RLE_INDEXED = 9;
  static constexpr unsigned int _TYPE_RLE_RGB     = 10;
  static constexpr unsigned int _TYPE_RLE_GREY    = 11;
  static constexpr unsigned int _ORIGIN_MASK      = 0x30;
  static constexpr unsigned int _ORIGIN_SHIFT     = 0x04;
  static constexpr unsigned int _ORIGIN_BL        = 0x00;
  static constexpr unsigned int _ORIGIN_BR        = 0x01;
  static constexpr unsigned int _ORIGIN_UL        = 0x02;
  static constexpr unsigned int _ORIGIN_UR        = 0x03;

public:
  /**
   * @brief Gets the header of a TGA file.
   * @param data defines the TGA data
   * @returns the header
   */
  static TGAHeader GetTGAHeader(const Uint8Array& data);

  /**
   * @brief Uploads TGA content to a Babylon Texture.
   * Hidden
   */
  static void UploadContent(const InternalTexturePtr& texture,
                            const Uint8Array& data);

  /**
   * @brief Hidden
   */
  static Uint8Array _getImageData8bits(const TGAHeader& header,
                                       const Uint8Array& palettes,
                                       const Uint8Array& pixel_data,
                                       int32_t y_start, int32_t y_step,
                                       int32_t y_end, int32_t x_start,
                                       int32_t x_step, int32_t x_end);

  /**
   * @brief Hidden
   */
  static Uint8Array _getImageData16bits(const TGAHeader& header,
                                        const Uint8Array& palettes,
                                        const Uint8Array& pixel_data,
                                        int32_t y_start, int32_t y_step,
                                        int32_t y_end, int32_t x_start,
                                        int32_t x_step, int32_t x_end);

  /**
   * @brief Hidden
   */
  static Uint8Array _getImageData24bits(const TGAHeader& header,
                                        const Uint8Array& palettes,
                                        const Uint8Array& pixel_data,
                                        int32_t y_start, int32_t y_step,
                                        int32_t y_end, int32_t x_start,
                                        int32_t x_step, int32_t x_end);

  /**
   * @brief Hidden
   */
  static Uint8Array _getImageData32bits(const TGAHeader& header,
                                        const Uint8Array& palettes,
                                        const Uint8Array& pixel_data,
                                        int32_t y_start, int32_t y_step,
                                        int32_t y_end, int32_t x_start,
                                        int32_t x_step, int32_t x_end);

  /**
   * @brief Hidden
   */
  static Uint8Array _getImageDataGrey8bits(const TGAHeader& header,
                                           const Uint8Array& palettes,
                                           const Uint8Array& pixel_data,
                                           int32_t y_start, int32_t y_step,
                                           int32_t y_end, int32_t x_start,
                                           int32_t x_step, int32_t x_end);

  /**
   * @brief Hidden
   */
  static Uint8Array _getImageDataGrey16bits(const TGAHeader& header,
                                            const Uint8Array& palettes,
                                            const Uint8Array& pixel_data,
                                            int32_t y_start, int32_t y_step,
                                            int32_t y_end, int32_t x_start,
                                            int32_t x_step, int32_t x_end);

}; // end of struct Tools

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_TGA_H
