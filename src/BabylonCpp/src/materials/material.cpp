#include <babylon/materials/material.h>

#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/material_defines.h>
#include <babylon/materials/multi_material.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/uniform_buffer.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/geometry.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

Material::Material(const string_t& iName, Scene* scene, bool /*doNotAdd*/)
    : id{!iName.empty() ? iName : Tools::RandomId()}
    , name{iName}
    , checkReadyOnEveryCall{false}
    , checkReadyOnlyOnce{false}
    , state{""}
    , alpha{1.f}
    , doNotSerialize{false}
    , storeEffectOnSubMeshes{false}
    , alphaMode{EngineConstants::ALPHA_COMBINE}
    , disableDepthWrite{false}
    , pointSize{1.f}
    , zOffset{0.f}
    , _effect{nullptr}
    , _wasPreviouslyReady{false}
    , _backFaceCulling{true}
    , _uniformBuffer{::std::make_unique<UniformBuffer>(scene->getEngine())}
    , _onDisposeObserver{nullptr}
    , _onBindObserver{nullptr}
    , _fogEnabled{true}
    , _useUBO{false}
    , _scene{scene}
    , _fillMode{Material::TriangleFillMode}
{
  if (_scene->useRightHandedSystem()) {
    sideOrientation = Material::ClockWiseSideOrientation;
  }
  else {
    sideOrientation = Material::CounterClockWiseSideOrientation;
  }

  _useUBO = getScene()->getEngine()->webGLVersion() > 1.f;
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

void Material::addMaterialToScene(unique_ptr_t<Material>&& newMaterial)
{
  _scene->materials.emplace_back(::std::move(newMaterial));
}

void Material::addMultiMaterialToScene(
  unique_ptr_t<MultiMaterial>&& newMultiMaterial)
{
  _scene->multiMaterials.emplace_back(::std::move(newMultiMaterial));
}

bool Material::backFaceCulling() const
{
  return _backFaceCulling;
}

void Material::setBackFaceCulling(bool value)
{
  if (_backFaceCulling == value) {
    return;
  }
  _backFaceCulling = value;
  markAsDirty(Material::TextureDirtyFlag);
}

bool Material::fogEnabled() const
{
  return _fogEnabled;
}

void Material::setFogEnabled(bool value)
{
  if (_fogEnabled == value) {
    return;
  }
  _fogEnabled = value;
  markAsDirty(Material::MiscDirtyFlag);
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
vector_t<Animation*> Material::getAnimations()
{
  return vector_t<Animation*>();
}

// Events
void Material::setOnDispose(const ::std::function<void()>& callback)
{
  if (_onDisposeObserver) {
    onDisposeObservable.remove(_onDisposeObserver);
  }
  _onDisposeObserver = onDisposeObservable.add(callback);
}

void Material::setOnBind(const ::std::function<void()>& callback)
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
  if (_fillMode == value) {
    return;
  }

  _fillMode = value;
  markAsDirty(Material::MiscDirtyFlag);
}

string_t Material::toString(bool fullDetails) const
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

bool Material::isReadyForSubMesh(AbstractMesh* /*mesh*/,
                                 BaseSubMesh* /*subMesh*/,
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
  const ::std::function<void(const Effect* effect)>& /*onCompiled*/,
  const ::std::function<void(const Effect* effect, const string_t& errors)>&
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
  engine->setState(backFaceCulling(), zOffset, false, reverse);
}

void Material::bind(Matrix* /*world*/, Mesh* /*mesh*/)
{
}

void Material::bindForSubMesh(Matrix* /*world*/, Mesh* /*mesh*/,
                              SubMesh* /*subMesh*/)
{
}

void Material::bindOnlyWorldMatrix(Matrix& /*world*/)
{
}

void Material::bindSceneUniformBuffer(Effect* effect, UniformBuffer* sceneUbo)
{
  sceneUbo->bindToEffect(effect, "Scene");
}

void Material::bindView(Effect* effect)
{
  if (!_useUBO) {
    effect->setMatrix("view", getScene()->getViewMatrix());
  }
  else {
    bindSceneUniformBuffer(effect, getScene()->getSceneUniformBuffer());
  }
}

void Material::bindViewProjection(Effect* effect)
{
  if (!_useUBO) {
    effect->setMatrix("viewProjection", getScene()->getTransformMatrix());
  }
  else {
    bindSceneUniformBuffer(effect, getScene()->getSceneUniformBuffer());
  }
}

void Material::_afterBind(Mesh* mesh)
{
  _scene->_cachedMaterial = this;
  if (mesh) {
    _scene->_cachedVisibility = mesh->visibility;
  }
  else {
    _scene->_cachedVisibility = 1.f;
  }

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

vector_t<BaseTexture*> Material::getActiveTextures() const
{
  return {};
}

bool Material::hasTexture(BaseTexture* /*texture*/) const
{
  return false;
}

Material* Material::clone(const string_t& /*name*/,
                          bool /*cloneChildren*/) const
{
  return nullptr;
}

vector_t<AbstractMesh*> Material::getBindedMeshes()
{
  vector_t<AbstractMesh*> result;

  for (auto& mesh : _scene->meshes) {
    if (mesh->material() == this) {
      result.emplace_back(mesh.get());
    }
  }

  return result;
}

void Material::forceCompilation(
  AbstractMesh* mesh,
  const ::std::function<void(Material* material)>& onCompiled,
  Nullable<bool> alphaTest, Nullable<bool> clipPlane)
{
  auto subMesh = ::std::make_unique<BaseSubMesh>();
  auto scene   = getScene();
  auto engine  = scene->getEngine();

  const auto checkReady = [&]() {
    if (!_scene || !_scene->getEngine()) {
      return;
    }

    if (subMesh->_materialDefines) {
      subMesh->_materialDefines->_renderId = -1;
    }

    auto alphaTestState = engine->getAlphaTesting();
    auto clipPlaneState = *scene->clipPlane();

    engine->setAlphaTesting(!alphaTest.isNull() ? *alphaTest :
                                                  needAlphaTesting());

    if (!clipPlane.isNull() && (*clipPlane)) {
      scene->setClipPlane(Plane(0, 0, 0, 1));
    }

    if (storeEffectOnSubMeshes) {
      if (isReadyForSubMesh(mesh, subMesh.get())) {
        if (onCompiled) {
          onCompiled(this);
        }
      }
      else {
        // setTimeout(checkReady, 16);
      }
    }
    else {
      if (isReady(mesh)) {
        if (onCompiled) {
          onCompiled(this);
        }
      }
      else {
        // setTimeout(checkReady, 16);
      }
    }

    engine->setAlphaTesting(alphaTestState);

    if (!clipPlane.isNull() && (*clipPlane)) {
      scene->setClipPlane(clipPlaneState);
    }
  };

  checkReady();
}

void Material::markAsDirty(unsigned int flag)
{
  if (flag & Material::TextureDirtyFlag) {
    _markAllSubMeshesAsTexturesDirty();
  }

  if (flag & Material::LightDirtyFlag) {
    _markAllSubMeshesAsLightsDirty();
  }

  if (flag & Material::FresnelDirtyFlag) {
    _markAllSubMeshesAsFresnelDirty();
  }

  if (flag & Material::AttributesDirtyFlag) {
    _markAllSubMeshesAsAttributesDirty();
  }

  if (flag & Material::MiscDirtyFlag) {
    _markAllSubMeshesAsMiscDirty();
  }

  getScene()->resetCachedMaterial();
}

void Material::_markAllSubMeshesAsDirty(
  const ::std::function<void(MaterialDefines& defines)>& func)
{
  for (auto& mesh : getScene()->meshes) {
    if (mesh->subMeshes.empty()) {
      continue;
    }
    for (auto& subMesh : mesh->subMeshes) {
      if (subMesh->getMaterial() != this) {
        continue;
      }

      if (!subMesh->_materialDefines) {
        continue;
      }

      func(*subMesh->_materialDefines);
    }
  }
}

void Material::_markAllSubMeshesAsImageProcessingDirty()
{
  // _markAllSubMeshesAsDirty(defines => defines.markAsImageProcessingDirty());
}

void Material::_markAllSubMeshesAsTexturesDirty()
{
  // _markAllSubMeshesAsDirty(defines => defines.markAsTexturesDirty());
}

void Material::_markAllSubMeshesAsFresnelDirty()
{
  // _markAllSubMeshesAsDirty(defines => defines.markAsFresnelDirty());
}

void Material::_markAllSubMeshesAsLightsDirty()
{
  // _markAllSubMeshesAsDirty(defines => defines.markAsLightDirty());
}

void Material::_markAllSubMeshesAsAttributesDirty()
{
  // _markAllSubMeshesAsDirty(defines => defines.markAsAttributesDirty());
}

void Material::_markAllSubMeshesAsMiscDirty()
{
  // _markAllSubMeshesAsDirty(defines => defines.markAsMiscDirty());
}

void Material::dispose(bool forceDisposeEffect, bool /*forceDisposeTextures*/)
{
  // Animations
  getScene()->stopAnimation(this);

  // Remove from scene
  _scene->materials.erase(
    ::std::remove_if(_scene->materials.begin(), _scene->materials.end(),
                     [this](const unique_ptr_t<Material>& material) {
                       return material.get() == this;
                     }),
    _scene->materials.end());

  // Remove from meshes
  Mesh* _mesh = nullptr;
  for (auto& mesh : _scene->meshes) {
    if (mesh->material() == this) {
      mesh->setMaterial(nullptr);

      _mesh = static_cast<Mesh*>(mesh.get());
      if (_mesh && _mesh->geometry()) {
        auto geometry = _mesh->geometry();
        if (storeEffectOnSubMeshes) {
          for (auto& subMesh : mesh->subMeshes) {
            geometry->_releaseVertexArrayObject(subMesh->_materialEffect);
          }
        }
        else {
          geometry->_releaseVertexArrayObject(_effect);
        }
      }
    }
  }

  _uniformBuffer->dispose();

  // Shader are kept in cache for further use but we can get rid of this by
  // using forceDisposeEffect
  if (forceDisposeEffect && _effect && _mesh) {
    if (storeEffectOnSubMeshes) {
      for (auto& subMesh : _mesh->subMeshes) {
        _scene->getEngine()->_releaseEffect(subMesh->_materialEffect);
      }
    }
    else {
      _scene->getEngine()->_releaseEffect(_effect);
    }

    _effect = nullptr;
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
  other->setBackFaceCulling(backFaceCulling());
  other->setWireframe(wireframe());
  other->setFogEnabled(fogEnabled());
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
    auto _subMatId = subMatId.get<string_t>();
    if (!_subMatId.empty()) {
      multiMaterial->subMaterials().emplace_back(
        scene->getMaterialByID(_subMatId));
    }
    else {
      multiMaterial->subMaterials().emplace_back(nullptr);
    }
  }

  return multiMaterial;
}

Material* Material::Parse(const Json::value& parsedMaterial, Scene* scene,
                          const string_t& rootUrl)
{
  if (!parsedMaterial.contains("customType")) {
    return StandardMaterial::Parse(parsedMaterial, scene, rootUrl);
  }

  return nullptr;
}

} // end of namespace BABYLON
