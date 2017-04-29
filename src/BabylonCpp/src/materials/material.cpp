#include <babylon/materials/material.h>

#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/multi_material.h>
#include <babylon/materials/standard_material.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {

Material::Material(const std::string& iName, Scene* scene, bool /*doNotAdd*/)
    : id{iName}
    , name{iName}
    , checkReadyOnEveryCall{false}
    , checkReadyOnlyOnce{false}
    , state{""}
    , alpha{1.f}
    , backFaceCulling{true}
    , doNotSerialize{false}
    , storeEffectOnSubMeshes{false}
    , alphaMode{Engine::ALPHA_COMBINE}
    , disableDepthWrite{false}
    , fogEnabled{true}
    , pointSize{1.f}
    , zOffset{0.f}
    , _effect{nullptr}
    , _wasPreviouslyReady{false}
    , _onDisposeObserver{nullptr}
    , _onBindObserver{nullptr}
    , _scene{scene}
    , _fillMode{Material::TriangleFillMode}
{
  if (_scene->useRightHandedSystem) {
    sideOrientation = Material::ClockWiseSideOrientation;
  }
  else {
    sideOrientation = Material::CounterClockWiseSideOrientation;
  }
}

Material::~Material()
{
}

const char* Material::getClassName() const
{
  return "Material";
}

IReflect::Type Material::type() const
{
  return IReflect::Type::MATERIAL;
}

void Material::addMaterialToScene(std::unique_ptr<Material>&& newMaterial)
{
  _scene->materials.emplace_back(std::move(newMaterial));
}

void Material::addMultiMaterialToScene(
  std::unique_ptr<MultiMaterial>&& newMultiMaterial)
{
  _scene->multiMaterials.emplace_back(std::move(newMultiMaterial));
}

void Material::setAmbientColor(const Color3& /*color*/)
{
}

void Material::setDiffuseColor(const Color3& /*color*/)
{
}

void Material::setSpecularColor(const Color3& /*color*/)
{
}
void Material::setEmissiveColor(const Color3& /*color*/)
{
}

// Methods
std::vector<Animation*> Material::getAnimations()
{
  return std::vector<Animation*>();
}

// Events
void Material::setOnDispose(const std::function<void()>& callback)
{
  if (_onDisposeObserver) {
    onDisposeObservable.remove(_onDisposeObserver);
  }
  _onDisposeObserver = onDisposeObservable.add(callback);
}

void Material::setOnBind(const std::function<void()>& callback)
{
  if (_onBindObserver) {
    onBindObservable.remove(_onBindObserver);
  }
  _onBindObserver = onBindObservable.add(callback);
}

bool Material::wireframe() const
{
  return _fillMode == Material::WireFrameFillMode;
}

void Material::setWireframe(bool value)
{
  _fillMode
    = (value ? Material::WireFrameFillMode : Material::TriangleFillMode);
}

bool Material::pointsCloud() const
{
  return _fillMode == Material::PointFillMode;
}

void Material::setPointsCloud(bool value)
{
  _fillMode = (value ? Material::PointFillMode : Material::TriangleFillMode);
}

unsigned int Material::fillMode() const
{
  return _fillMode;
}

void Material::setFillMode(unsigned int value)
{
  _fillMode = value;
}

std::string Material::toString(bool fullDetails) const
{
  std::ostringstream oss;
  oss << "Name: " << name;
  if (fullDetails) {
  }
  return oss.str();
}

bool Material::isFrozen() const
{
  return checkReadyOnlyOnce;
}

void Material::freeze()
{
  checkReadyOnlyOnce = true;
}

void Material::unfreeze()
{
  checkReadyOnlyOnce = false;
}

bool Material::isReady(AbstractMesh* /*mesh*/, bool /*useInstances*/)
{
  return true;
}

bool Material::isReadyForSubMesh(AbstractMesh* /*mesh*/, SubMesh* /*subMesh*/,
                                 bool /*useInstances*/)
{
  return false;
}

Effect* Material::getEffect()
{
  return _effect;
}

Scene* Material::getScene() const
{
  return _scene;
}

bool Material::needAlphaBlending()
{
  return (alpha < 1.f);
}

bool Material::needAlphaTesting()
{
  return false;
}

BaseTexture* Material::getAlphaTestTexture()
{
  return nullptr;
}

void Material::trackCreation(
  const std::function<void(const Effect* effect)>& /*onCompiled*/,
  const std::function<void(const Effect* effect, const std::string& errors)>&
  /*onError*/)
{
}

void Material::markDirty()
{
  _wasPreviouslyReady = false;
}

void Material::_preBind(Effect* effect)
{
  auto engine = _scene->getEngine();

  const bool reverse = sideOrientation == Material::ClockWiseSideOrientation;

  engine->enableEffect(effect ? effect : _effect);

  engine->setState(backFaceCulling, zOffset, false, reverse);
}

void Material::bind(Matrix* /*world*/, Mesh* mesh)
{
  _scene->_cachedMaterial = this;

  onBindObservable.notifyObservers(mesh);

  if (disableDepthWrite) {
    auto engine            = _scene->getEngine();
    _cachedDepthWriteState = engine->getDepthWrite();
    engine->setDepthWrite(false);
  }
}

void Material::bindForSubMesh(Matrix* /*world*/, Mesh* /*mesh*/,
                              SubMesh* /*subMesh*/)
{
}

void Material::bindOnlyWorldMatrix(Matrix& /*world*/)
{
}

void Material::_afterBind(Mesh* mesh)
{
  _scene->_cachedMaterial = this;

  onBindObservable.notifyObservers(mesh);

  if (disableDepthWrite) {
    auto engine            = _scene->getEngine();
    _cachedDepthWriteState = engine->getDepthWrite();
    engine->setDepthWrite(false);
  }
}

void Material::unbind()
{
  onUnBindObservable.notifyObservers(this);

  if (disableDepthWrite) {
    auto engine = _scene->getEngine();
    engine->setDepthWrite(_cachedDepthWriteState);
  }
}

Material* Material::clone(const std::string& /*name*/,
                          bool /*cloneChildren*/) const
{
  return nullptr;
}

std::vector<AbstractMesh*> Material::getBindedMeshes()
{
  std::vector<AbstractMesh*> result;

  for (auto& mesh : _scene->meshes) {
    if (mesh->material() == this) {
      result.emplace_back(mesh.get());
    }
  }

  return result;
}

void Material::dispose(bool forceDisposeEffect, bool /*forceDisposeTextures*/)
{
  // Animations
  getScene()->stopAnimation(this);

  // Remove from scene
  _scene->materials.erase(
    std::remove_if(_scene->materials.begin(), _scene->materials.end(),
                   [this](const std::unique_ptr<Material>& material) {
                     return material.get() == this;
                   }),
    _scene->materials.end());

  // Shader are kept in cache for further use but we can get rid of this by
  // using forceDisposeEffect
  if (forceDisposeEffect && _effect) {
    _scene->getEngine()->_releaseEffect(_effect);
    _effect = nullptr;
  }

  // Remove from meshes
  for (auto& mesh : _scene->meshes) {
    if (mesh->material() == this) {
      mesh->setMaterial(nullptr);
    }
  }

  // Callback
  onDisposeObservable.notifyObservers(this);

  onDisposeObservable.clear();
  onBindObservable.clear();
  onUnBindObservable.clear();
}

void Material::copyTo(Material* other) const
{
  other->checkReadyOnlyOnce    = checkReadyOnlyOnce;
  other->checkReadyOnEveryCall = checkReadyOnEveryCall;
  other->alpha                 = alpha;
  other->setFillMode(fillMode());
  other->backFaceCulling = backFaceCulling;
  other->setWireframe(wireframe());
  other->fogEnabled        = fogEnabled;
  other->zOffset           = zOffset;
  other->alphaMode         = alphaMode;
  other->sideOrientation   = sideOrientation;
  other->disableDepthWrite = disableDepthWrite;
  other->pointSize         = pointSize;
  other->setPointsCloud(pointsCloud());
}

Json::object Material::serialize() const
{
  return Json::object();
}

MultiMaterial*
Material::ParseMultiMaterial(const Json::value& parsedMultiMaterial,
                             Scene* scene)
{
  auto multiMaterial
    = MultiMaterial::New(Json::GetString(parsedMultiMaterial, "name"), scene);

  multiMaterial->id = Json::GetString(parsedMultiMaterial, "id");

  // Tags.AddTagsTo(multiMaterial, parsedMultiMaterial.tags);

  for (auto& subMatId : Json::GetArray(parsedMultiMaterial, "materials")) {
    auto _subMatId = subMatId.get<std::string>();
    if (!_subMatId.empty()) {
      multiMaterial->subMaterials.emplace_back(
        scene->getMaterialByID(_subMatId));
    }
    else {
      multiMaterial->subMaterials.emplace_back(nullptr);
    }
  }

  return multiMaterial;
}

Material* Material::Parse(const Json::value& parsedMaterial, Scene* scene,
                          const std::string& rootUrl)
{
  if (!parsedMaterial.contains("customType")) {
    return StandardMaterial::Parse(parsedMaterial, scene, rootUrl);
  }

  return nullptr;
}

} // end of namespace BABYLON
