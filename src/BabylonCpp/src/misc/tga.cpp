#include <babylon/misc/tga.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/logging.h>
#include <babylon/engines/engine.h>
#include <babylon/materials/textures/internal_texture.h>

namespace BABYLON {

TGAHeader TGATools::GetTGAHeader(const Uint8Array& data)
{
  auto offset = 0u;

  TGAHeader header{
    data[offset++], // id_length
    data[offset++], // colormap_type
    data[offset++], // image_type
    static_cast<uint32_t>(data[offset++]
                          | data[offset++] << 8), // colormap_index
    static_cast<uint32_t>(data[offset++]
                          | data[offset++] << 8), // colormap_length
    data[offset++],                               // colormap_size
    {
      static_cast<uint32_t>(data[offset++] | data[offset++] << 8), //
      static_cast<uint32_t>(data[offset++] | data[offset++] << 8)  //
    },                                                             // origin
    data[offset++] | data[offset++] << 8,                          // width
    data[offset++] | data[offset++] << 8,                          // height
    data[offset++],                                                // pixel_size
    data[offset++]                                                 // flags
  };

  return header;
}

void TGATools::UploadContent(const InternalTexturePtr& texture,
                             const Uint8Array& data)
{
  // Not enough data to contain header ?
  if (data.size() < 19) {
    BABYLON_LOG_ERROR(
      "TGATools", "Unable to load TGA file - Not enough data to contain header")
    return;
  }

  // Read Header
  auto offset = 18u;
  auto header = TGATools::GetTGAHeader(data);

  // Assume it's a valid Targa file.
  if (header.id_length + offset > data.size()) {
    BABYLON_LOG_ERROR("Unable to load TGA file - Not enough data")
    return;
  }

  // Skip not needed data
  offset += header.id_length;

  auto use_rle  = false;
  auto use_pal  = false;
  auto use_grey = false;

  // Get some informations.
  switch (header.image_type) {
    case TGATools::_TYPE_RLE_INDEXED:
      use_rle = true;
    case TGATools::_TYPE_INDEXED:
      use_pal = true;
      break;

    case TGATools::_TYPE_RLE_RGB:
      use_rle = true;
    case TGATools::_TYPE_RGB:
      // use_rgb = true;
      break;

    case TGATools::_TYPE_RLE_GREY:
      use_rle = true;
    case TGATools::_TYPE_GREY:
      use_grey = true;
      break;
    default:
      break;
  }

  Uint8Array pixel_data;

  // var numAlphaBits = header.flags & 0xf;
  auto pixel_size = header.pixel_size >> 3;
  auto pixel_total
    = static_cast<uint32_t>(header.width * header.height) * pixel_size;

  // Read palettes
  Uint8Array palettes;

  if (use_pal) {
    const auto start = static_cast<int>(offset);
    offset += header.colormap_length * (header.colormap_size >> 3);
    const auto end = static_cast<int>(offset);
    palettes       = stl_util::subarray(data, start, end);
  }

  // Read LRE
  if (use_rle) {
    pixel_data = Uint8Array(pixel_total);

    uint32_t c, count, i;
    uint32_t localOffset = 0;
    auto pixels          = Uint8Array(pixel_size);

    while (offset < pixel_total && localOffset < pixel_total) {
      c     = data[offset++];
      count = (c & 0x7f) + 1;

      // RLE pixels
      if (c & 0x80) {
        // Bind pixel tmp array
        for (i = 0; i < pixel_size; ++i) {
          pixels[i] = data[offset++];
        }

        // Copy pixel array
        for (i = 0; i < count; ++i) {
          const auto offset = static_cast<int>(localOffset + i * pixel_size);
          stl_util::array_set(pixel_data, pixels, offset);
        }

        localOffset += pixel_size * count;
      }
      // Raw pixels
      else {
        count *= pixel_size;
        for (i = 0; i < count; ++i) {
          pixel_data[localOffset + i] = data[offset++];
        }
        localOffset += count;
      }
    }
  }
  // RAW Pixels
  else {
    const auto start = static_cast<int>(offset);
    offset += (use_pal ? static_cast<uint32_t>(header.width * header.height) :
                         pixel_total);
    const auto end = static_cast<int>(offset);
    pixel_data     = stl_util::subarray(data, start, end);
  }

  // Load to texture
  int32_t x_start, y_start, x_step, y_step, x_end, y_end;

  switch ((header.flags & TGATools::_ORIGIN_MASK) >> TGATools::_ORIGIN_SHIFT) {
    default:
    case TGATools::_ORIGIN_UL:
      x_start = 0;
      x_step  = 1;
      x_end   = static_cast<int>(header.width);
      y_start = 0;
      y_step  = 1;
      y_end   = static_cast<int>(header.height);
      break;

    case TGATools::_ORIGIN_BL:
      x_start = 0;
      x_step  = 1;
      x_end   = header.width;
      y_start = header.height - 1;
      y_step  = -1;
      y_end   = -1;
      break;

    case TGATools::_ORIGIN_UR:
      x_start = header.width - 1;
      x_step  = -1;
      x_end   = -1;
      y_start = 0;
      y_step  = 1;
      y_end   = static_cast<int>(header.height);
      break;

    case TGATools::_ORIGIN_BR:
      x_start = header.width - 1;
      x_step  = -1;
      x_end   = -1;
      y_start = header.height - 1;
      y_step  = -1;
      y_end   = -1;
      break;
  }

  // Load the specify method
  Uint8Array imageData;
  if (use_grey) {
    if (header.pixel_size == 8) {
      imageData = _getImageDataGrey8bits(header, palettes, pixel_data, y_start,
                                         y_step, y_end, x_start, x_step, x_end);
    }
    else if (header.pixel_size == 16) {
      imageData
        = _getImageDataGrey16bits(header, palettes, pixel_data, y_start, y_step,
                                  y_end, x_start, x_step, x_end);
    }
  }
  else {
    if (header.pixel_size == 8) {
      imageData = _getImageDataGrey8bits(header, palettes, pixel_data, y_start,
                                         y_step, y_end, x_start, x_step, x_end);
    }
    else if (header.pixel_size == 16) {
      imageData = _getImageData16bits(header, palettes, pixel_data, y_start,
                                      y_step, y_end, x_start, x_step, x_end);
    }
    else if (header.pixel_size == 24) {
      imageData = _getImageData24bits(header, palettes, pixel_data, y_start,
                                      y_step, y_end, x_start, x_step, x_end);
    }
    else if (header.pixel_size == 32) {
      imageData = _getImageData32bits(header, palettes, pixel_data, y_start,
                                      y_step, y_end, x_start, x_step, x_end);
    }
  }

  auto engine = texture->getEngine();
  engine->_uploadDataToTextureDirectly(texture, imageData);
}

Uint8Array TGATools::_getImageData8bits(const TGAHeader& header,
                                        const Uint8Array& palettes,
                                        const Uint8Array& pixel_data,
                                        int32_t y_start, int32_t y_step,
                                        int32_t y_end, int32_t x_start,
                                        int32_t x_step, int32_t x_end)
{
  const auto& image    = pixel_data;
  const auto& colormap = palettes;
  auto width = header.width, height = header.height;
  uint8_t color = 0;
  auto i = 0, x = 0, y = 0;

  auto imageData = Uint8Array(static_cast<uint32_t>(width * height * 4));

  for (y = y_start; y != y_end; y += y_step) {
    for (x = x_start; x != x_end; x += x_step, i++) {
      const auto xwy         = static_cast<uint32_t>(x + width * y);
      const auto _i          = static_cast<uint32_t>(i);
      color                  = image[_i];
      imageData[xwy * 4 + 3] = 255;
      imageData[xwy * 4 + 2] = colormap[(color * 3) + 0];
      imageData[xwy * 4 + 1] = colormap[(color * 3) + 1];
      imageData[xwy * 4 + 0] = colormap[(color * 3) + 2];
    }
  }

  return imageData;
}

Uint8Array TGATools::_getImageData16bits(const TGAHeader& header,
                                         const Uint8Array& /*palettes*/,
                                         const Uint8Array& pixel_data,
                                         int32_t y_start, int32_t y_step,
                                         int32_t y_end, int32_t x_start,
                                         int32_t x_step, int32_t x_end)
{
  const auto& image = pixel_data;
  auto width = header.width, height = header.height;
  uint8_t color = 0;
  auto i = 0, x = 0, y = 0;

  auto imageData = Uint8Array(static_cast<uint32_t>(width * height * 4));

  for (y = y_start; y != y_end; y += y_step) {
    for (x = x_start; x != x_end; x += x_step, i += 2) {
      const auto xwy = static_cast<uint32_t>(x + width * y);
      const auto _i  = static_cast<uint32_t>(i);
      color          = static_cast<uint8_t>(image[_i + 0]
                                   + (image[_i + 1] << 8)); // Inversed ?
      uint8_t r      = (((color & 0x7C00) >> 10) * 255) / 0x1F | 0;
      uint8_t g      = (((color & 0x03E0) >> 5) * 255) / 0x1F | 0;
      uint8_t b      = ((color & 0x001F) * 255) / 0x1F | 0;

      imageData[xwy * 4 + 0] = r;
      imageData[xwy * 4 + 1] = g;
      imageData[xwy * 4 + 2] = b;
      imageData[xwy * 4 + 3] = (color & 0x8000) ? 0 : 255;
    }
  }

  return imageData;
}

Uint8Array TGATools::_getImageData24bits(const TGAHeader& header,
                                         const Uint8Array& /*palettes*/,
                                         const Uint8Array& pixel_data,
                                         int32_t y_start, int32_t y_step,
                                         int32_t y_end, int32_t x_start,
                                         int32_t x_step, int32_t x_end)
{
  const auto& image = pixel_data;
  auto width = header.width, height = header.height;
  auto i = 0, x = 0, y = 0;

  auto imageData = Uint8Array(static_cast<uint32_t>(width * height * 4));

  for (y = y_start; y != y_end; y += y_step) {
    for (x = x_start; x != x_end; x += x_step, i += 3) {
      const auto xwy         = static_cast<uint32_t>(x + width * y);
      const auto _i          = static_cast<uint32_t>(i);
      imageData[xwy * 4 + 3] = 255;
      imageData[xwy * 4 + 2] = image[_i + 0];
      imageData[xwy * 4 + 1] = image[_i + 1];
      imageData[xwy * 4 + 0] = image[_i + 2];
    }
  }

  return imageData;
}

Uint8Array TGATools::_getImageData32bits(const TGAHeader& header,
                                         const Uint8Array& /*palettes*/,
                                         const Uint8Array& pixel_data,
                                         int32_t y_start, int32_t y_step,
                                         int32_t y_end, int32_t x_start,
                                         int32_t x_step, int32_t x_end)
{
  const auto& image = pixel_data;
  auto width = header.width, height = header.height;
  auto i = 0, x = 0, y = 0;

  auto imageData = Uint8Array(static_cast<uint32_t>(width * height * 4));

  for (y = y_start; y != y_end; y += y_step) {
    for (x = x_start; x != x_end; x += x_step, i += 4) {
      const auto xwy         = static_cast<uint32_t>(x + width * y);
      const auto _i          = static_cast<uint32_t>(i);
      imageData[xwy * 4 + 2] = image[_i + 0];
      imageData[xwy * 4 + 1] = image[_i + 1];
      imageData[xwy * 4 + 0] = image[_i + 2];
      imageData[xwy * 4 + 3] = image[_i + 3];
    }
  }

  return imageData;
}

Uint8Array TGATools::_getImageDataGrey8bits(const TGAHeader& header,
                                            const Uint8Array& /*palettes*/,
                                            const Uint8Array& pixel_data,
                                            int32_t y_start, int32_t y_step,
                                            int32_t y_end, int32_t x_start,
                                            int32_t x_step, int32_t x_end)
{
  const auto& image = pixel_data;
  auto width = header.width, height = header.height;
  uint8_t color = 0;
  auto i = 0, x = 0, y = 0;

  auto imageData = Uint8Array(static_cast<uint32_t>(width * height * 4));

  for (y = y_start; y != y_end; y += y_step) {
    for (x = x_start; x != x_end; x += x_step, i++) {
      const auto xwy         = static_cast<uint32_t>(x + width * y);
      const auto _i          = static_cast<uint32_t>(i);
      color                  = image[_i];
      imageData[xwy * 4 + 0] = color;
      imageData[xwy * 4 + 1] = color;
      imageData[xwy * 4 + 2] = color;
      imageData[xwy * 4 + 3] = 255;
    }
  }

  return imageData;
}

Uint8Array TGATools::_getImageDataGrey16bits(const TGAHeader& header,
                                             const Uint8Array& /*palettes*/,
                                             const Uint8Array& pixel_data,
                                             int32_t y_start, int32_t y_step,
                                             int32_t y_end, int32_t x_start,
                                             int32_t x_step, int32_t x_end)
{
  const auto& image = pixel_data;
  auto width = header.width, height = header.height;
  auto i = 0, x = 0, y = 0;

  auto imageData = Uint8Array(static_cast<uint32_t>(width * height * 4));

  for (y = y_start; y != y_end; y += y_step) {
    for (x = x_start; x != x_end; x += x_step, i += 2) {
      const auto xwy         = static_cast<uint32_t>(x + width * y);
      const auto _i          = static_cast<uint32_t>(i);
      imageData[xwy * 4 + 0] = image[_i + 0];
      imageData[xwy * 4 + 1] = image[_i + 0];
      imageData[xwy * 4 + 2] = image[_i + 0];
      imageData[xwy * 4 + 3] = image[_i + 1];
    }
  }

  return imageData;
}

} // end of namespace BABYLON
