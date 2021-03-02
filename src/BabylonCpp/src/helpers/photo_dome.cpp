#include <babylon/helpers/photo_dome.h>

#include <babylon/helpers/texture_dome_options.h>
#include <babylon/materials/textures/texture.h>

namespace BABYLON {

PhotoDome::PhotoDome(
  std::string iName, const std::string& urlOfPhoto, TextureDomeOptions& options, Scene* scene,
  const std::function<void(const std::string& message, const std::string& exception)>& iOnError)
    : TextureDome{iName, urlOfPhoto, options, scene, iOnError}
    , photoTexture{this, &PhotoDome::get_photoTexture, &PhotoDome::set_photoTexture}
    , imageMode{this, &PhotoDome::get_imageMode, &PhotoDome::set_imageMode}
{
}

PhotoDome::~PhotoDome() = default;

TexturePtr& PhotoDome::get_photoTexture()
{
  return texture();
}

void PhotoDome::set_photoTexture(const TexturePtr& value)
{
  texture = value;
}

unsigned int PhotoDome::get_imageMode() const
{
  return textureMode();
}

void PhotoDome::set_imageMode(unsigned int value)
{
  textureMode = value;
}

TexturePtr PhotoDome::_initTexture(const std::string& urlsOrElement, Scene* scene,
                                   const TextureDomeOptions& options)
{
  return Texture::New(
    urlsOrElement, scene, !options.generateMipMaps.value_or(false), !_useDirectMapping,
    TextureConstants::TRILINEAR_SAMPLINGMODE,
    [this]() -> void { onLoadObservable.notifyObservers(); },
    [this](const std::string& message, const std::string& exception) -> void {
      auto iMessage = !message.empty() ? message : "Unknown error occured";
      onLoadErrorObservable.notifyObservers(&iMessage);

      if (onError) {
        onError(message, exception);
      }
    });
}

} // end of namespace BABYLON
