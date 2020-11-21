#ifndef BABYLON_MATERIALS_TEXTURES_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_TEXTURE_H

#include <variant>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/maths/matrix.h>
#include <babylon/misc/iinspectable.h>
#include <babylon/misc/observable.h>

namespace BABYLON {

FWD_STRUCT_SPTR(LoaderOptions)
FWD_CLASS_SPTR(MirrorTexture)
FWD_CLASS_SPTR(Texture)

/**
 * @brief This represents a texture in babylon. It can be easily loaded from a network, base64 or
 * html input.
 * @see https://doc.babylonjs.com/babylon101/materials#texture
 */
class BABYLON_SHARED_EXPORT Texture : public BaseTexture {

  friend Scene;

public:
  /**
   * Gets or sets a general boolean used to indicate that textures containing direct data (buffers)
   * must be saved as part of the serialization process
   */
  static bool SerializeBuffers;

  /**
   * Gets or sets a general boolean used to indicate that texture buffers must be saved as part of
   * the serialization process. If no buffer exists, one will be created as base64 string from the
   * internal webgl data.
   */
  static bool ForceSerializeBuffers;

  /**
   * @brief Hidden
   */
  static MirrorTexturePtr
  _CreateMirror(const std::string& name,
                const std::variant<int, RenderTargetSize, float>& renderTargetSize, Scene* scene,
                bool generateMipMaps);

  /**
   * Gets or sets a boolean which defines if the texture url must be build from the serialized URL
   * instead of just using the name and loading them side by side with the scene file
   */
  static bool UseSerializedUrlIfAny;

public:
  template <typename... Ts>
  static TexturePtr New(Ts&&... args)
  {
    auto texture = std::shared_ptr<Texture>(new Texture(std::forward<Ts>(args)...));
    texture->addToScene(texture);

    return texture;
  }
  ~Texture() override; // = default

  Type type() const override;

  /**
   * @brief Update the url (and optional buffer) of this texture if url was null during
   * construction.
   * @param url the url of the texture
   * @param buffer the buffer of the texture (defaults to null)
   * @param onLoad callback called when the texture is loaded  (defaults to
   * null)
   */
  void updateURL(
    const std::string& iUrl,
    const std::optional<std::variant<std::string, ArrayBuffer, ArrayBufferView, Image>>& buffer
    = std::nullopt,
    const std::function<void(InternalTexture*, EventState&)>& onLoad = nullptr);

  /**
   * @brief Finish the loading sequence of a texture flagged as delayed load.
   * @hidden
   */
  void delayLoad(const std::string& forcedExtension = "") override;

  /**
   * @brief Checks if the texture has the same transform matrix than another texture.
   * @param texture texture to check against
   * @returns true if the transforms are the same, else false
   */
  bool checkTransformsAreIdentical(const BaseTexturePtr& texture) const override;

  /**
   * @brief Get the current texture matrix which includes the requested offsetting, tiling and
   * rotation components.
   * @returns the transform matrix of the texture.
   */
  Matrix* getTextureMatrix(int uBase = 1) override;

  /**
   * @brief Get the current matrix used to apply reflection. This is useful to rotate an environment
   * texture for instance.
   * @returns The reflection texture transform
   */
  Matrix* getReflectionTextureMatrix() override;

  /**
   * @brief Clones the texture.
   * @returns the cloned texture
   */
  TexturePtr clone() const;

  /**
   * @brief Serialize the texture to a JSON representation we can easily use in the resepective
   * Parse function.
   * @returns The JSON representation of the texture
   */
  json serialize() const;

  /**
   * @brief Get the current class name of the texture useful for serialization or dynamic coding.
   * @returns "Texture"
   */
  const char* getClassName() const;

  /**
   * @brief Dispose the texture and release its associated resources.
   */
  void dispose() override;

  /**
   * @brief Parse the JSON representation of a texture in order to recreate the texture in the given
   * scene.
   * @param parsedTexture Define the JSON representation of the texture
   * @param scene Define the scene the parsed texture should be instantiated in
   * @param rootUrl Define the root url of the parsing sequence in the case of relative dependencies
   * @returns The parsed texture if successful
   */
  static BaseTexturePtr Parse(const json& parsedTexture, Scene* scene, const std::string& rootUrl);

  /**
   * @brief Creates a texture from its base 64 representation.
   * @param data Define the base64 payload without the data: prefix
   * @param name Define the name of the texture in the scene useful fo caching purpose for instance
   * @param scene Define the scene the texture should belong to
   * @param noMipmap Forces the texture to not create mip map information if true
   * @param invertY define if the texture needs to be inverted on the y axis during loading
   * @param samplingMode define the sampling mode we want for the texture while fectching from it
   * (Texture.NEAREST_SAMPLINGMODE...)
   * @param onLoad define a callback triggered when the texture has been loaded
   * @param onError define a callback triggered when an error occurred during the loading session
   * @param format define the format of the texture we are trying to load
   * (Engine.TEXTUREFORMAT_RGBA...)
   * @returns the created texture
   */
  static TexturePtr CreateFromBase64String(
    const std::string& data, const std::string& name, Scene* scene, bool noMipmap = false,
    bool invertY = false, unsigned int samplingMode = TextureConstants::TRILINEAR_SAMPLINGMODE,
    const std::function<void()>& onLoad = nullptr,
    const std::function<void(const std::string& message, const std::string& exception)>& onError
    = nullptr,
    unsigned int format = Constants::TEXTUREFORMAT_RGBA);

  /**
   * @brief Creates a texture from its data: representation. (data: will be added in case only the
   * payload has been passed in).
   * @param data Define the base64 payload without the data: prefix
   * @param name Define the name of the texture in the scene useful fo caching purpose for instance
   * @param buffer define the buffer to load the texture from in case the texture is loaded from a
   * buffer representation
   * @param scene Define the scene the texture should belong to
   * @param deleteBuffer define if the buffer we are loading the texture from should be deleted
   * after load
   * @param noMipmap Forces the texture to not create mip map information if true
   * @param invertY define if the texture needs to be inverted on the y axis during loading
   * @param samplingMode define the sampling mode we want for the texture while fectching from it
   * (Texture.NEAREST_SAMPLINGMODE...)
   * @param onLoad define a callback triggered when the texture has been loaded
   * @param onError define a callback triggered when an error occurred during the loading session
   * @param format define the format of the texture we are trying to load
   * (Engine.TEXTUREFORMAT_RGBA...)
   * @returns the created texture
   */
  static TexturePtr LoadFromDataString(
    const std::string& name,
    const std::optional<std::variant<std::string, ArrayBuffer, ArrayBufferView, Image>>& buffer,
    Scene* scene, bool deleteBuffer = false, bool noMipmap = false, bool invertY = true,
    unsigned int samplingMode           = Constants::TEXTURE_TRILINEAR_SAMPLINGMODE,
    const std::function<void()>& onLoad = nullptr,
    const std::function<void(const std::string& message, const std::string& exception)>& onError
    = nullptr,
    unsigned int format = Constants::TEXTUREFORMAT_RGBA);

protected:
  //  clang-format off
  /**
   * @brief Instantiates a new texture.
   * This represents a texture in babylon. It can be easily loaded from a network, base64 or html
   * input.
   * @see https://doc.babylonjs.com/babylon101/materials#texture
   * @param url defines the url of the picture to load as a texture
   * @param sceneOrEngine defines the scene or engine the texture will belong to
   * @param noMipmap defines if the texture will require mip maps or not
   * @param invertY defines if the texture needs to be inverted on the y axis during loading
   * @param samplingMode defines the sampling mode we want for the texture while fectching from it
   * (Texture.NEAREST_SAMPLINGMODE...)
   * @param onLoad defines a callback triggered when the texture has been loaded
   * @param onError defines a callback triggered when an error occurred during the loading session
   * @param buffer defines the buffer to load the texture from in case the texture is loaded from a
   * buffer representation
   * @param deleteBuffer defines if the buffer we are loading the texture from should be deleted
   * after load
   * @param format defines the format of the texture we are trying to load
   * (Engine.TEXTUREFORMAT_RGBA...)
   * @param mimeType defines an optional mime type information
   * @param loaderOptions options to be passed to the loader
   */
  //  clang-format on
  Texture(
    const std::string& url, const std::optional<std::variant<Scene*, ThinEngine*>>& sceneOrEngine,
    bool noMipmap = false, const std::optional<bool>& invertY = true,
    unsigned int samplingMode           = TextureConstants::TRILINEAR_SAMPLINGMODE,
    const std::function<void()>& onLoad = nullptr,
    const std::function<void(const std::string& message, const std::string& exception)>& onError
    = nullptr,
    const std::optional<std::variant<std::string, ArrayBuffer, ArrayBufferView, Image>>& buffer
    = std::nullopt,
    bool deleteBuffer = false, const std::optional<unsigned int>& format = std::nullopt,
    const std::string& mimeType = "", const LoaderOptionsPtr& loaderOptions = nullptr);

  bool get_noMipmap() const override;

  /**
   * @brief Is the texture preventing material to render while loading.
   * If false, a default texture will be used instead of the loading one during the preparation
   * step.
   */
  void set_isBlocking(bool value) override;
  bool get_isBlocking() const override;

  /**
   * @brief Get the current sampling mode associated with the texture.
   */
  unsigned int get_samplingMode() const;

  /**
   * @brief Gets a boolean indicating if the texture needs to be inverted on the y axis during
   * loading.
   */
  bool get_invertY() const;

  /**
   * @brief Returns the texture mime type if it was defined by a loader (undefined else).
   */
  std::string get_mimeType() const;

  /**
   * @brief Gets the observable triggered once the texture has been loaded.
   */
  Observable<Texture>& get_onLoadObservable();

private:
  void _prepareRowForTextureGeneration(float x, float y, float z, Vector3& t);

public:
  /**
   * Define the url of the texture.
   */
  std::string url;

  /**
   * Define an offset on the texture to offset the u coordinates of the UVs
   * @see https://doc.babylonjs.com/how_to/more_materials#offsetting
   */
  float uOffset;

  /**
   * Define an offset on the texture to offset the v coordinates of the UVs
   * @see https://doc.babylonjs.com/how_to/more_materials#offsetting
   */
  float vOffset;

  /**
   * Define an offset on the texture to scale the u coordinates of the UVs
   * @see https://doc.babylonjs.com/how_to/more_materials#tiling
   */
  float uScale;

  /**
   * Define an offset on the texture to scale the v coordinates of the UVs
   * @see https://doc.babylonjs.com/how_to/more_materials#tiling
   */
  float vScale;

  /**
   * Define an offset on the texture to rotate around the u coordinates of the UVs
   * @see https://doc.babylonjs.com/how_to/more_materials
   */
  float uAng;

  /**
   * Define an offset on the texture to rotate around the v coordinates of the UVs
   * @see https://doc.babylonjs.com/how_to/more_materials
   */
  float vAng;

  /**
   * Define an offset on the texture to rotate around the w coordinates of the UVs (in case of 3d
   * texture)
   * @see https://doc.babylonjs.com/how_to/more_materials
   */
  float wAng;

  /**
   * Defines the center of rotation (U)
   */
  float uRotationCenter;

  /**
   * Defines the center of rotation (V)
   */
  float vRotationCenter;

  /**
   * Defines the center of rotation (W)
   */
  float wRotationCenter;

  /**
   * Sets this property to true to avoid deformations when rotating the texture with non-uniform
   * scaling
   */
  bool homogeneousRotationInUVTransform;

  /**
   * @brief List of inspectable custom properties (used by the Inspector).
   * @see https://doc.babylonjs.com/how_to/debug_layer#extensibility
   */
  std::vector<IInspectable> inspectableCustomProperties;

  /**
   * Hidden
   */
  bool _invertY;

  /**
   * Get the current sampling mode associated with the texture.
   */
  ReadOnlyProperty<Texture, unsigned int> samplingMode;

  /**
   * Gets a boolean indicating if the texture needs to be inverted on the y axis during loading
   */
  ReadOnlyProperty<Texture, bool> invertY;

  /**
   * Returns the texture mime type if it was defined by a loader (undefined else).
   */
  ReadOnlyProperty<Texture, std::string> mimeType;

  /**
   * Observable triggered once the texture has been loaded.
   */
  ReadOnlyProperty<Texture, Observable<Texture>> onLoadObservable;

protected:
  /**
   * Get the current sampling mode associated with the texture.
   */
  unsigned int _initialSamplingMode;

  std::optional<unsigned int> _format;
  Observable<Texture> _onLoadObservable;
  bool _isBlocking;

private:
  bool _noMipmap;
  std::unique_ptr<Matrix> _rowGenerationMatrix;
  std::unique_ptr<Matrix> _cachedTextureMatrix;
  std::unique_ptr<Matrix> _projectionModeMatrix;
  std::unique_ptr<Vector3> _t0;
  std::unique_ptr<Vector3> _t1;
  std::unique_ptr<Vector3> _t2;

  float _cachedUOffset;
  float _cachedVOffset;
  float _cachedUScale;
  float _cachedVScale;
  float _cachedUAng;
  float _cachedVAng;
  float _cachedWAng;
  int _cachedProjectionMatrixId;
  int _cachedURotationCenter;
  int _cachedVRotationCenter;
  int _cachedWRotationCenter;
  bool _cachedHomogeneousRotationInUVTransform;
  int _cachedCoordinatesMode;
  std::optional<std::variant<std::string, ArrayBuffer, ArrayBufferView, Image>> _buffer;
  bool _deleteBuffer;
  std::function<void(InternalTexture*, EventState&)> _delayedOnLoad;
  std::function<void(const std::string& message, const std::string& exception)> _delayedOnError;
  std::string _mimeType;
  LoaderOptionsPtr _loaderOptions;

  std::function<void()> _onLoad;
  std::function<void(InternalTexture*, EventState&)> _load;

}; // end of class Texture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_TEXTURE_H
