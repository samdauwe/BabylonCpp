#include <babylon/engines/extensions/texture_selector_extension.h>

#include <babylon/engines/engine.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

TextureSelectorExtension::TextureSelectorExtension(Engine* engine)
    : texturesSupported{this, &TextureSelectorExtension::get_texturesSupported}
    , textureFormatInUse{this, &TextureSelectorExtension::get_textureFormatInUse}
    , _this{engine}

{
}

TextureSelectorExtension::~TextureSelectorExtension() = default;

std::string TextureSelectorExtension::transformTextureUrl(Engine* /*engine*/,
                                                          const std::string& url) const
{
  const auto lastDot          = StringTools::lastIndexOf(url, ".");
  const auto lastQuestionMark = StringTools::lastIndexOf(url, "?");
  const auto querystring = lastQuestionMark > -1 ? url.substr(lastQuestionMark, url.size()) : "";
  return (lastDot > -1 ? url.substr(0, lastDot) : url) + _textureFormatInUse + querystring;
}

std::vector<std::string>& TextureSelectorExtension::get_texturesSupported()
{
  if (_texturesSupported.empty()) {
    // Intelligently add supported compressed formats in order to check for.
    // Check for ASTC support first as it is most powerful and to be very cross platform. Next PVRTC
    // & DXT, which are probably superior to ETC1/2. Likely no hardware which supports both PVR &
    // DXT, so order matters little. ETC2 is newer and handles ETC1 (no alpha capability), so check
    // for first.
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

void TextureSelectorExtension::setCompressedTextureExclusions(
  const std::vector<std::string>& skippedFiles)
{
  _excludedCompressedTextures = skippedFiles;
}

std::string
TextureSelectorExtension::setTextureFormatToUse(const std::vector<std::string>& formatsAvailable)
{
  const auto& iTexturesSupported = texturesSupported();
  for (size_t i = 0, len1 = iTexturesSupported.size(); i < len1; ++i) {
    for (size_t j = 0, len2 = formatsAvailable.size(); j < len2; ++j) {
      if (iTexturesSupported[i] == StringTools::toLowerCase(formatsAvailable[j])) {
        _this->_transformTextureUrl = [this](const std::string& url) -> std::string {
          return transformTextureUrl(_this, url);
        };
        return _textureFormatInUse = iTexturesSupported[i];
      }
    }
  }
  // actively set format to nothing, to allow this to be called more than once
  // and possibly fail the 2nd time
  _textureFormatInUse         = "";
  _this->_transformTextureUrl = nullptr;
  return "";
}

} // end of namespace BABYLON
