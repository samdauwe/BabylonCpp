#include <babylon/loading/glTF/gltf_file_loader.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/json_util.h>
#include <babylon/core/logging.h>
#include <babylon/core/string.h>
#include <babylon/engines/asset_container.h>
#include <babylon/engines/scene.h>
#include <babylon/loading/glTF/2.0/gltf_loader.h>
#include <babylon/loading/glTF/2.0/gltf_loader_interfaces.h>
#include <babylon/loading/glTF/binary_reader.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/tools/tools.h>

namespace BABYLON {
namespace GLTF2 {

const std::string GLTFFileLoader::_logSpaces
  = "                                ";
bool GLTFFileLoader::IncrementalLoading     = true;
bool GLTFFileLoader::HomogeneousCoordinates = false;

void GLTFFileLoader::RegisterAsSceneLoaderPlugin()
{
  SceneLoader::RegisterPlugin(std::make_shared<GLTFFileLoader>());
}

IGLTFLoaderPtr GLTFFileLoader::_CreateGLTF2Loader(GLTFFileLoader& parent)
{
  return GLTF2::GLTFLoader::New(parent);
}

GLTFFileLoader::GLTFFileLoader()
    : onParsed{this, &GLTFFileLoader::set_onParsed}
    , coordinateSystemMode{GLTFLoaderCoordinateSystemMode::AUTO}
    , animationStartMode{GLTFLoaderAnimationStartMode::FIRST}
    , compileMaterials{false}
    , useClipPlane{false}
    , compileShadowGenerators{false}
    , transparencyAsCoverage{false}
    , preprocessUrlAsync{nullptr}
    , onMeshLoaded{this, &GLTFFileLoader::set_onMeshLoaded}
    , onTextureLoaded{this, &GLTFFileLoader::set_onTextureLoaded}
    , onMaterialLoaded{this, &GLTFFileLoader::set_onMaterialLoaded}
    , onCameraLoaded{this, &GLTFFileLoader::set_onCameraLoaded}
    , onComplete{this, &GLTFFileLoader::set_onComplete}
    , onError{this, &GLTFFileLoader::set_onError}
    , onDispose{this, &GLTFFileLoader::set_onDispose}
    , onExtensionLoaded{this, &GLTFFileLoader::set_onExtensionLoaded}
    , loggingEnabled{this, &GLTFFileLoader::get_loggingEnabled,
                     &GLTFFileLoader::set_loggingEnabled}
    , capturePerformanceCounters{this,
                                 &GLTFFileLoader::
                                   get_capturePerformanceCounters,
                                 &GLTFFileLoader::
                                   set_capturePerformanceCounters}
    , validate{false}
    , onValidated{this, &GLTFFileLoader::set_onValidated}
    , _onParsedObserver{nullptr}
    , _onMeshLoadedObserver{nullptr}
    , _onTextureLoadedObserver{nullptr}
    , _onMaterialLoadedObserver{nullptr}
    , _onCameraLoadedObserver{nullptr}
    , _onCompleteObserver{nullptr}
    , _onErrorObserver{nullptr}
    , _onDisposeObserver{nullptr}
    , _onExtensionLoadedObserver{nullptr}
    , _onValidatedObserver{nullptr}
    , _loader{nullptr}
    , _logIndentLevel{0}
    , _loggingEnabled{false}
    , _capturePerformanceCounters{false}
{
  // Name of the loader ("gltf")
  factoryName = name = "gltf";
  // Supported file extensions of the loader (.gltf, .glb)
  ISceneLoaderPluginExtensions supportedFileExtensions;
  supportedFileExtensions.mapping = {
    {".gltf", false}, // .gltf
    {".glb", true}    // ".glb"
  };
  extensions = supportedFileExtensions;
  // If the data string can be loaded directly
  factoryCanDirectLoad = canDirectLoad = [](const std::string& data) {
    return String::contains(data, "scene") && String::contains(data, "node");
  };

  preprocessUrlAsync
    = [](const std::string& url) -> std::string { return url; };

  _log = [this](const std::string& message) { _logDisabled(message); };

  _startPerformanceCounter = [this](const std::string& counterName) {
    _startPerformanceCounterDisabled(counterName);
  };
  _endPerformanceCounter = [this](const std::string& counterName) {
    _endPerformanceCounterDisabled(counterName);
  };
}

GLTFFileLoader::~GLTFFileLoader()
{
}

void GLTFFileLoader::set_onParsed(
  const std::function<void(IGLTFLoaderData* loaderData, EventState& es)>&
    callback)
{
  if (_onParsedObserver) {
    onParsedObservable.remove(_onParsedObserver);
  }
  _onParsedObserver = onParsedObservable.add(callback);
}

void GLTFFileLoader::set_onMeshLoaded(
  const std::function<void(AbstractMesh* callback, EventState& es)>& callback)
{
  if (_onMeshLoadedObserver) {
    onMeshLoadedObservable.remove(_onMeshLoadedObserver);
  }
  _onMeshLoadedObserver = onMeshLoadedObservable.add(callback);
}

void GLTFFileLoader::set_onTextureLoaded(
  const std::function<void(BaseTexture* texture, EventState& es)>& callback)
{
  if (_onTextureLoadedObserver) {
    onTextureLoadedObservable.remove(_onTextureLoadedObserver);
  }
  _onTextureLoadedObserver = onTextureLoadedObservable.add(callback);
}

void GLTFFileLoader::set_onMaterialLoaded(
  const std::function<void(Material* material, EventState& es)>& callback)
{
  if (_onMaterialLoadedObserver) {
    onMaterialLoadedObservable.remove(_onMaterialLoadedObserver);
  }
  _onMaterialLoadedObserver = onMaterialLoadedObservable.add(callback);
}

void GLTFFileLoader::set_onCameraLoaded(
  const std::function<void(Camera* camera, EventState& es)>& callback)
{
  if (_onCameraLoadedObserver) {
    onCameraLoadedObservable.remove(_onCameraLoadedObserver);
  }
  _onCameraLoadedObserver = onCameraLoadedObservable.add(callback);
}

void GLTFFileLoader::set_onComplete(
  const std::function<void(void*, EventState& es)>& callback)
{
  if (_onCompleteObserver) {
    onCompleteObservable.remove(_onCompleteObserver);
  }
  _onCompleteObserver = onCompleteObservable.add(callback);
}

void GLTFFileLoader::set_onError(
  const std::function<void(std::string* reason, EventState& es)>& callback)
{
  if (_onErrorObserver) {
    onErrorObservable.remove(_onErrorObserver);
  }
  _onErrorObserver = onErrorObservable.add(callback);
}

void GLTFFileLoader::set_onDispose(
  const std::function<void(void*, EventState& es)>& callback)
{
  if (_onDisposeObserver) {
    onDisposeObservable.remove(_onDisposeObserver);
  }
  _onDisposeObserver = onDisposeObservable.add(callback);
}

void GLTFFileLoader::set_onExtensionLoaded(
  const std::function<void(IGLTFLoaderExtension* extension, EventState& es)>&
    callback)
{
  if (_onExtensionLoadedObserver) {
    onExtensionLoadedObservable.remove(_onExtensionLoadedObserver);
  }
  _onExtensionLoadedObserver = onExtensionLoadedObservable.add(callback);
}

bool GLTFFileLoader::get_loggingEnabled() const
{
  return _loggingEnabled;
}

void GLTFFileLoader::set_loggingEnabled(bool value)
{
  if (_loggingEnabled == value) {
    return;
  }

  _loggingEnabled = value;

  if (_loggingEnabled) {
    _log = [this](const std::string& message) { _logEnabled(message); };
  }
  else {
    _log = [this](const std::string& message) { _logDisabled(message); };
  }
}

bool GLTFFileLoader::get_capturePerformanceCounters() const
{
  return _capturePerformanceCounters;
}

void GLTFFileLoader::set_capturePerformanceCounters(bool value)
{
  if (_capturePerformanceCounters == value) {
    return;
  }

  _capturePerformanceCounters = value;

  if (_capturePerformanceCounters) {
    _startPerformanceCounter = [this](const std::string& counterName) {
      _startPerformanceCounterEnabled(counterName);
    };
    _endPerformanceCounter = [this](const std::string& counterName) {
      _endPerformanceCounterEnabled(counterName);
    };
  }
  else {
    _startPerformanceCounter = [this](const std::string& counterName) {
      _startPerformanceCounterDisabled(counterName);
    };
    _endPerformanceCounter = [this](const std::string& counterName) {
      _endPerformanceCounterDisabled(counterName);
    };
  }
}

void GLTFFileLoader::set_onValidated(
  const std::function<void(IGLTFValidationResults* results, EventState& es)>&
    callback)
{
  if (_onValidatedObserver) {
    onValidatedObservable.remove(_onValidatedObserver);
  }
  _onValidatedObserver = onValidatedObservable.add(callback);
}

void GLTFFileLoader::dispose(bool /*doNotRecurse*/,
                             bool /*disposeMaterialAndTextures*/)
{
  if (_loader) {
    _loader->dispose();
    _loader = nullptr;
  }

  _clear();

  onDisposeObservable.notifyObservers(nullptr);
  onDisposeObservable.clear();
}

void GLTFFileLoader::_clear()
{
  preprocessUrlAsync = nullptr;

  onMeshLoadedObservable.clear();
  onTextureLoadedObservable.clear();
  onMaterialLoadedObservable.clear();
  onCameraLoadedObservable.clear();
  onCompleteObservable.clear();
  onExtensionLoadedObservable.clear();
}

ImportedMeshes GLTFFileLoader::importMeshAsync(
  const std::vector<std::string>& meshesNames, Scene* scene,
  const std::string& data, const std::string& rootUrl,
  const std::function<void(const SceneLoaderProgressEvent& event)>& onProgress,
  const std::string& fileName)
{
  auto loaderData = _parseAsync(scene, data, rootUrl, fileName);
  _log(String::printf("Loading %s", fileName.c_str()));
  _loader = _getLoader(loaderData);
  return _loader->importMeshAsync(meshesNames, scene, loaderData, rootUrl,
                                  onProgress, fileName);
}

void GLTFFileLoader::loadAsync(
  Scene* scene, const std::string& data, const std::string& rootUrl,
  const std::function<void(const SceneLoaderProgressEvent& event)>& onProgress,
  const std::string& fileName)
{
  auto loaderData = _parseAsync(scene, data, rootUrl, fileName);
  _log(String::printf("Loading %s", fileName.c_str()));
  _loader = _getLoader(loaderData);
  _loader->loadAsync(scene, loaderData, rootUrl, onProgress, fileName);
}

AssetContainerPtr GLTFFileLoader::loadAssetContainerAsync(
  Scene* scene, const std::string& data, const std::string& rootUrl,
  const std::function<void(const SceneLoaderProgressEvent& event)>& onProgress,
  const std::string& fileName)
{
  auto loaderData = _parseAsync(scene, data, rootUrl, fileName);
  _log(String::printf("Loading %s", fileName.c_str()));
  _loader        = _getLoader(loaderData);
  auto result    = _loader->importMeshAsync({}, scene, loaderData, rootUrl,
                                         onProgress, fileName);
  auto container = AssetContainer::New(scene);
  stl_util::concat(container->meshes, result.meshes);
  stl_util::concat(container->particleSystems, result.particleSystems);
  stl_util::concat(container->skeletons, result.skeletons);
  stl_util::concat(container->animationGroups, result.animationGroups);
  container->removeAllFromScene();
  return container;
}

std::variant<ISceneLoaderPluginPtr, ISceneLoaderPluginAsyncPtr>
GLTFFileLoader::createPlugin()
{
  return std::make_shared<GLTFFileLoader>();
}

std::optional<GLTFLoaderState> GLTFFileLoader::loaderState()
{
  return _loader ? _loader->state : std::nullopt;
}

void GLTFFileLoader::whenCompleteAsync()
{
}

IGLTFLoaderData GLTFFileLoader::_parseAsync(
  Scene* scene, const std::variant<std::string, ArrayBuffer>& data,
  const std::string& rootUrl, const std::string& fileName)
{
  UnpackedBinary unpacked;
  if (std::holds_alternative<ArrayBuffer>(data)) {
    unpacked = _unpackBinary(std::get<ArrayBuffer>(data));
  }
  else if (std::holds_alternative<std::string>(data)) {
    unpacked.json = std::get<std::string>(data);
    unpacked.bin  = std::nullopt;
  }

  _validateAsync(scene, unpacked.json, rootUrl, fileName);
  _startPerformanceCounter("Parse JSON");
  _log(String::printf("JSON length: %ld", unpacked.json.size()));

  IGLTFLoaderData loaderData{
    json::parse(unpacked.json), // json
    unpacked.bin                // bin
  };

  _endPerformanceCounter("Parse JSON");

  onParsedObservable.notifyObservers(&loaderData);
  onParsedObservable.clear();

  return loaderData;
}

void GLTFFileLoader::_validateAsync(Scene* /*scene*/,
                                    const std::string& /*json*/,
                                    const std::string& /*rootUrl*/,
                                    const std::string& /*fileName*/)
{
}

IGLTFLoaderPtr GLTFFileLoader::_getLoader(const IGLTFLoaderData& loaderData)
{
  if (!json_util::has_key(loaderData.jsonObject, "asset")) {
    return nullptr;
  }

  auto asset           = loaderData.jsonObject["asset"];
  auto assetVersion    = json_util::get_string(asset, "version");
  auto assetMinVersion = json_util::get_string(asset, "minVersion");
  if (!assetMinVersion.empty()) {
    _log(String::printf("Asset minimum version: %s", assetMinVersion.c_str()));
  }
  auto assetGenerator = json_util::get_string(asset, "generator");
  if (!assetGenerator.empty()) {
    _log(String::printf("Asset generator: %s", assetGenerator.c_str()));
  }

  auto version = GLTFFileLoader::_parseVersion(assetVersion);
  if (!version.has_value()) {
    throw std::runtime_error("Invalid version: " + assetVersion);
  }

  if (!assetMinVersion.empty()) {
    const auto minVersion = GLTFFileLoader::_parseVersion(assetMinVersion);
    if (!minVersion.has_value()) {
      throw std::runtime_error("Invalid minimum version: " + assetMinVersion);
    }

    if (GLTFFileLoader::_compareVersion(*minVersion,
                                        Version{
                                          2u, // major
                                          0u, // minor
                                        })
        > 0) {
      throw std::runtime_error("Incompatible minimum version: "
                               + assetMinVersion);
    }
  }

  std::unordered_map<unsigned int,
                     std::function<IGLTFLoaderPtr(GLTFFileLoader & parent)>>
    createLoaders;
  createLoaders[2] = [](GLTFFileLoader& parent) -> IGLTFLoaderPtr {
    return GLTFFileLoader::_CreateGLTF2Loader(parent);
  };

  if (!stl_util::contains(createLoaders, version->major)) {
    throw std::runtime_error("Unsupported version: " + assetVersion);
  }

  return createLoaders[version->major](*this);
}

UnpackedBinary GLTFFileLoader::_unpackBinary(const ArrayBuffer& data)
{
  _startPerformanceCounter("Unpack binary");
  _log(String::printf("Binary length: %ld", data.size()));

  static const unsigned int Binary_Magic = 0x46546C67;

  BinaryReader binaryReader(data);

  const auto magic = binaryReader.readUint32();
  if (magic != Binary_Magic) {
    throw std::runtime_error(String::printf("Unexpected magic: %d", magic));
  }

  const auto version = binaryReader.readUint32();

  if (loggingEnabled) {
    _log(String::printf("Binary version", version));
  }

  UnpackedBinary unpacked;
  switch (version) {
    case 1: {
      unpacked = _unpackBinaryV1(binaryReader);
      break;
    }
    case 2: {
      unpacked = _unpackBinaryV2(binaryReader);
      break;
    }
    default: {
      throw std::runtime_error(
        String::printf("Unsupported version:: %d", version));
    }
  }

  _endPerformanceCounter("Unpack binary");
  return unpacked;
}

UnpackedBinary GLTFFileLoader::_unpackBinaryV1(BinaryReader& binaryReader) const
{
  static const unsigned int ContentFormat_JSON = 0;

  const auto length = binaryReader.readUint32();
  if (length != binaryReader.getLength()) {
    auto errorMessage = String::printf(
      "Length in header does not match actual data length: %ld != %ld", length,
      binaryReader.getLength());
    throw std::runtime_error(errorMessage);
  }

  const auto contentLength = binaryReader.readUint32();
  const auto contentFormat = binaryReader.readUint32();

  std::string content;
  switch (contentFormat) {
    case ContentFormat_JSON: {
      content = GLTFFileLoader::_decodeBufferToText(
        binaryReader.readUint8Array(contentLength));
      break;
    }
    default: {
      throw std::runtime_error("Unexpected content format: "
                               + std::to_string(contentFormat));
    }
  }

  const auto bytesRemaining
    = binaryReader.getLength() - binaryReader.getPosition();
  const auto body = binaryReader.readUint8Array(bytesRemaining);

  return UnpackedBinary{
    content, // json
    body     // bin
  };
}

UnpackedBinary GLTFFileLoader::_unpackBinaryV2(BinaryReader& binaryReader) const
{
  static const unsigned int ChunkFormat_JSON = 0x4E4F534A;
  static const unsigned int ChunkFormat_BIN  = 0x004E4942;

  const auto length = binaryReader.readUint32();
  if (length != binaryReader.getLength()) {
    auto errorMessage = String::printf(
      "Length in header does not match actual data length: %ld != %ld", length,
      binaryReader.getLength());
    throw std::runtime_error(errorMessage);
  }

  // JSON chunk
  const auto chunkLength = binaryReader.readUint32();
  const auto chunkFormat = binaryReader.readUint32();
  if (chunkFormat != ChunkFormat_JSON) {
    throw std::runtime_error("First chunk format is not JSON");
  }
  const auto json = GLTFFileLoader::_decodeBufferToText(
    binaryReader.readUint8Array(chunkLength));

  // Look for BIN chunk
  Uint8Array bin;
  while (binaryReader.getPosition() < binaryReader.getLength()) {
    const auto chunkLength = binaryReader.readUint32();
    const auto chunkFormat = binaryReader.readUint32();
    switch (chunkFormat) {
      case ChunkFormat_JSON: {
        throw std::runtime_error("Unexpected JSON chunk");
      }
      case ChunkFormat_BIN: {
        bin = binaryReader.readUint8Array(chunkLength);
        break;
      }
      default: {
        // ignore unrecognized chunkFormat
        binaryReader.skipBytes(chunkLength);
        break;
      }
    }
  }

  return UnpackedBinary{
    json, // json
    bin   // bin
  };
}

std::optional<Version> GLTFFileLoader::_parseVersion(const std::string& version)
{
  if (version == "1.0" || version == "1.0.1") {
    return Version{
      1, // major
      0  // minor
    };
  }

  const std::regex regex("(\\d+)\\.(\\d+)", std::regex::optimize);
  auto match = String::regexMatch(version, regex);
  if (match.size() < 2) {
    return std::nullopt;
  }

  return Version{
    String::toNumber<unsigned int>(match[0]), // major
    String::toNumber<unsigned int>(match[1])  // minor
  };
}

int GLTFFileLoader::_compareVersion(const Version& a, const Version& b)
{
  if (a.major > b.major) {
    return 1;
  }
  if (a.major < b.major) {
    return -1;
  }
  if (a.minor > b.minor) {
    return 1;
  }
  if (a.minor < b.minor) {
    return -1;
  }
  return 0;
}

std::string GLTFFileLoader::_decodeBufferToText(const Uint8Array& buffer)
{
  std::ostringstream result;
  const auto length = buffer.size();

  for (size_t i = 0; i < length; ++i) {
    result << String::fromCharCode(buffer[i]);
  }

  return result.str();
}

void GLTFFileLoader::_logOpen(const std::string& message)
{
  _log(message);
  ++_logIndentLevel;
}

void GLTFFileLoader::_logClose()
{
  --_logIndentLevel;
}

void GLTFFileLoader::_logEnabled(const std::string& message)
{
  const auto spaces = GLTFFileLoader::_logSpaces.substr(0, _logIndentLevel * 2);
  BABYLON_LOGF_INFO("GLTFFileLoader", "%s%s", spaces.c_str(), message.c_str());
}

void GLTFFileLoader::_logDisabled(const std::string& /*message*/)
{
}

void GLTFFileLoader::_startPerformanceCounterEnabled(
  const std::string& counterName)
{
  Tools::StartPerformanceCounter(counterName);
}

void GLTFFileLoader::_startPerformanceCounterDisabled(
  const std::string& /*counterName*/)
{
}

void GLTFFileLoader::_endPerformanceCounterEnabled(
  const std::string& counterName)
{
  Tools::EndPerformanceCounter(counterName);
}

void GLTFFileLoader::_endPerformanceCounterDisabled(
  const std::string& /*counterName*/)
{
}

} // end of namespace GLTF2
} // end of namespace BABYLON
