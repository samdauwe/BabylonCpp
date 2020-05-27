#ifndef BABYLON_ENGINES_EXTENSIONS_TEXTURE_SELECTOR_EXTENSION_H
#define BABYLON_ENGINES_EXTENSIONS_TEXTURE_SELECTOR_EXTENSION_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

class Engine;

/**
 * @brief Hidden
 */
class BABYLON_SHARED_EXPORT TextureSelectorExtension {

public:
  TextureSelectorExtension(Engine* engine);
  ~TextureSelectorExtension(); // = default;

  /**
   * @brief Set the compressed texture extensions or file names to skip.
   *
   * @param skippedFiles defines the list of those texture files you want to skip
   * Example: [".dds", ".env", "myfile.png"]
   */
  void setCompressedTextureExclusions(const std::vector<std::string>& skippedFiles);

  /**
   * @brief Set the compressed texture format to use, based on the formats you have, and the formats
   * supported by the hardware / browser.
   *
   * Khronos Texture Container (.ktx) files are used to support this.  This format has the
   * advantage of being specifically designed for OpenGL.  Header elements directly correspond
   * to API arguments needed to compressed textures.  This puts the burden on the container
   * generator to house the arcane code for determining these for current & future formats.
   *
   * for description see https://www.khronos.org/opengles/sdk/tools/KTX/
   * for file layout see https://www.khronos.org/opengles/sdk/tools/KTX/file_format_spec/
   *
   * Note: The result of this call is not taken into account when a texture is base64.
   *
   * @param formatsAvailable defines the list of those format families you have created
   * on your server.  Syntax: '-' + format family + '.ktx'.  (Case and order do not matter.)
   *
   * Current families are astc, dxt, pvrtc, etc2, & etc1.
   * @returns The extension selected.
   */
  std::string setTextureFormatToUse(const std::vector<std::string>& formatsAvailable);

protected:
  /**
   * @brief Gets the list of texture formats supported.
   */
  std::vector<std::string>& get_texturesSupported();

  /**
   * @brief Gets the texture format in use.
   */
  std::string get_textureFormatInUse() const;

private:
  std::string transformTextureUrl(Engine* engine, const std::string& url) const;

public:
  /**
   * Gets the list of texture formats supported
   */
  ReadOnlyProperty<TextureSelectorExtension, std::vector<std::string>> texturesSupported;

  /**
   * Gets the texture format in use
   */
  ReadOnlyProperty<TextureSelectorExtension, std::string> textureFormatInUse;

private:
  Engine* _this;

  /** @hidden */
  std::vector<std::string> _excludedCompressedTextures;

  /** @hidden */
  std::string _textureFormatInUse;

  /** @hidden */
  std::vector<std::string> _texturesSupported;

}; // end of class TextureSelectorExtension

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_EXTENSIONS_TEXTURE_SELECTOR_EXTENSION_H
