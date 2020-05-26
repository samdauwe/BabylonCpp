#include <babylon/engines/extensions/texture_selector_extension.h>

#include <babylon/engines/engine.h>

namespace BABYLON {

TextureSelectorExtension::TextureSelectorExtension(Engine* engine)
    : texturesSupported{this, &TextureSelectorExtension::get_texturesSupported}
    , textureFormatInUse{this, &TextureSelectorExtension::get_textureFormatInUse}
    , _this{engine}

{
}

TextureSelectorExtension::~TextureSelectorExtension() = default;

std::vector<std::string>& TextureSelectorExtension::get_texturesSupported()
{
  if (_texturesSupported.empty()) {
    // Intelligently add supported compressed formats in order to check for.
    // Check for ASTC support first as it is most powerful and to be very cross platform.
    // Next PVRTC & DXT, which are probably superior to ETC1/2.
    // Likely no hardware which supports both PVR & DXT, so order matters little.
    // ETC2 is newer and handles ETC1 (no alpha capability), so check for first.
    if (_this->_caps.astc) {
      _texturesSupported.emplace_back("-astc.ktx");
    }
    if (_this->_caps.s3tc) {
      _texturesSupported.emplace_back("-dxt.ktx");
    }
    if (_this->_caps.pvrtc) {
      _texturesSupported.emplace_back("-pvrtc.ktx");
    }
    if (_this->_caps.etc2) {
      _texturesSupported.emplace_back("-etc2.ktx");
    }
    if (_this->_caps.etc1) {
      _texturesSupported.emplace_back("-etc1.ktx");
    }
  }

  return _texturesSupported;
}

std::string TextureSelectorExtension::get_textureFormatInUse() const
{
  return _textureFormatInUse;
}

void setCompressedTextureExclusions(const std::vector<std::string>& /*skippedFiles*/)
{
}

std::string setTextureFormatToUse(const std::vector<std::string>& /*formatsAvailable*/)
{
  return "";
}

} // end of namespace BABYLON
