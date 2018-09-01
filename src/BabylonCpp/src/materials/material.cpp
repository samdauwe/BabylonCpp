#include <babylon/materials/material.h>

#include <babylon/babylon_stl_util.h>
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

Material::Material(const string_t& iName, Scene* scene, bool doNotAdd)
    : id{!iName.empty() ? iName : Tools::RandomId()}
    , name{iName}
    , checkReadyOnEveryCall{false}
    , checkReadyOnlyOnce{false}
    , alpha{this, &Material::get_alpha, &Material::set_alpha}
    , backFaceCulling{this, &Material::get_backFaceCulling,
                      &Material::set_backFaceCulling}
    , doNotSerialize{false}
    , storeEffectOnSubMeshes{false}
    , onDispose{this, &Material::set_onDispose}
    , onBindObservable{this, &Material::get_onBindObservable}
    , onBind{this, &Material::set_onBind}
    , onUnBindObservable{this, &Material::get_onUnBindObservable}
    , alphaMode{this, &Material::get_alphaMode, &Material::set_alphaMode}
    , needDepthPrePass{this, &Material::get_needDepthPrePass,
                       &Material::set_needDepthPrePass}
    , disableDepthWrite{false}
    , forceDepthWrite{false}
    , separateCullingPass{false}
    , fogEnabled{this, &Material::get_fogEnabled, &Material::set_fogEnabled}
    , pointSize{1.f}
    , zOffset{0.f}
    , wireframe{this, &Material::get_wireframe, &Material::set_wireframe}
    , pointsCloud{this, &Material::get_pointsCloud, &Material::set_pointsCloud}
    , fillMode{this, &Material::get_fillMode, &Material::set_fillMode}
    , _effect{nullptr}
    , _wasPreviouslyReady{false}
    , useLogarithmicDepth{this, &Material::get_useLogarithmicDepth,
                          &Material::set_useLogarithmicDepth}
    , _alpha{1.f}
    , _backFaceCulling{true}
    , _uniformBuffer{::std::make_unique<UniformBuffer>(scene->getEngine())}
    , _onDisposeObserver{nullptr}
    , _onUnBindObservable{nullptr}
    , _onBindObserver{nullptr}
    , _alphaMode{EngineConstants::ALPHA_COMBINE}
    , _needDepthPrePass{false}
    , _fogEnabled{true}
    , _useUBO{false}
    , _scene{scene ? scene : Engine::LastCreatedScene()}
    , _fillMode{Material::TriangleFillMode()}
{
  uniqueId = _scene->getUniqueId();

  if (_scene->useRightHandedSystem()) {
    sideOrientation = Material::ClockWiseSideOrientation();
  }
  else {
    sideOrientation = Material::CounterClockWiseSideOrientation();
  }

  _useUBO = getScene()->getEngine()->supportsUniformBuffers();

  if (!doNotAdd) {
    // _scene->materials.emplace_back(this);
  }
}

Material::~Material()
{
}

void Material::set_alpha(float value)
{
  if (stl_util::almost_equal(_alpha, value)) {
    return;
  }
  _alpha = value;
  markAsDirty(Material::MiscDirtyFlag());
}

float Material::get_alpha() const
{
  return _alpha;
}

void Material::set_backFaceCulling(bool value)
{
  if (_backFaceCulling == value) {
    return;
  }
  _backFaceCulling = value;
  markAsDirty(Material::TextureDirtyFlag());
}

bool Material::get_backFaceCulling() const
{
  return _backFaceCulling;
}

IReflect::Type Material::type() const
{
  return IReflect::Type::MATERIAL;
}

void Material::addMaterialToScene(const MaterialPtr& newMaterial)
{
  _scene->materials.emplace_back(newMaterial);
}

void Material::addMultiMaterialToScene(const MultiMaterialPtr& newMultiMaterial)
{
  _scene->multiMaterials.emplace_back(newMultiMaterial);
}

unsigned int Material::get_alphaMode() const
{
  return _alphaMode;
}

void Material::set_alphaMode(unsigned int value)
{
  if (_alphaMode == value) {
    return;
  }
  _alphaMode = value;
  markAsDirty(Material::TextureDirtyFlag());
}

bool Material::get_needDepthPrePass() const
{
  return _needDepthPrePass;
}

void Material::set_needDepthPrePass(bool value)
{
  if (_needDepthPrePass == value) {
    return;
  }
  _needDepthPrePass = value;
  if (_needDepthPrePass) {
    checkReadyOnEveryCall = true;
  }
}

bool Material::get_fogEnabled() const
{
  return _fogEnabled;
}

void Material::set_fogEnabled(bool value)
{
  if (_fogEnabled == value) {
    return;
  }
  _fogEnabled = value;
  markAsDirty(Material::MiscDirtyFlag());
}

bool Material::get_useLogarithmicDepth() const
{
  return false;
}

void Material::set_useLogarithmicDepth(bool /*value*/)
{
}

// Methods
vector_t<AnimationPtr> Material::getAnimations()
{
  return vector_t<AnimationPtr>();
}

// Events
void Material::set_onDispose(
  const ::std::function<void(Material*, EventState&)>& callback)
{
  if (_onDisposeObserver) {
    onDisposeObservable.remove(_onDisposeObserver);
  }
  _onDisposeObserver = onDisposeObservable.add(callback);
}

Observable<AbstractMesh>& Material::get_onBindObservable()
{
  return _onBindObservable;
}

void Material::set_onBind(
  const ::std::function<void(AbstractMesh*, EventState&)>& callback)
{
  if (_onBindObserver) {
    onBindObservable().remove(_onBindObserver);
  }
  _onBindObserver = onBindObservable().add(callback);
}

Observable<Material>& Material::get_onUnBindObservable()
{
  return _onUnBindObservable;
}

void Material::set_wireframe(bool value)
{
  fillMode
    = value ? Material::WireFrameFillMode() : Material::TriangleFillMode();
}

bool Material::get_wireframe() const
{
  switch (_fillMode) {
    case Material::WireFrameFillMode():
    case Material::LineListDrawMode():
    case Material::LineLoopDrawMode():
    case Material::LineStripDrawMode():
      return true;
  }

  return _scene->forceWireframe();
}

void Material::set_pointsCloud(bool value)
{
  fillMode = value ? Material::PointFillMode() : Material::TriangleFillMode();
}

bool Material::get_pointsCloud() const
{
  switch (_fillMode) {
    case Material::PointFillMode():
    case Material::PointListDrawMode():
      return true;
  }

  return _scene->forcePointsCloud();
}

void Material::set_fillMode(unsigned int value)
{
  if (_fillMode == value) {
    return;
  }

  _fillMode = value;
  markAsDirty(Material::MiscDirtyFlag());
}

unsigned int Material::get_fillMode() const
{
  return _fillMode;
}

string_t Material::toString(bool fullDetails) const
{
  std::ostringstream oss;
  oss << "Name: " << name;
  if (fullDetails) {
  }
  return oss.str();
}

const string_t Material::getClassName() const
{
  return "Material";
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

bool Material::needAlphaBlending() const
{
  return (_alpha < 1.f);
}

bool Material::needAlphaBlendingForMesh(const AbstractMesh& mesh) const
{
  return needAlphaBlending() || (mesh.visibility() < 1.f)
         || mesh.hasVertexAlpha();
}

bool Material::needAlphaTesting() const
{
  return false;
}

BaseTexturePtr Material::getAlphaTestTexture()
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

bool Material::_preBind(Effect* effect,
                        nullable_t<unsigned int> overrideOrientation)
{
  auto engine = _scene->getEngine();

  auto orientation = (!overrideOrientation.has_value()) ?
                       static_cast<unsigned>(sideOrientation) :
                       *overrideOrientation;
  const bool reverse = orientation == Material::ClockWiseSideOrientation();

  engine->enableEffect(effect ? effect : _effect);
  engine->setState(backFaceCulling(), zOffset, false, reverse);

  return reverse;
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

bool Material::_shouldTurnAlphaTestOn(AbstractMesh* mesh) const
{
  return (!needAlphaBlendingForMesh(*mesh) && needAlphaTesting());
}

void Material::_afterBind(Mesh* mesh)
{
  _scene->_cachedMaterial = this;
  if (mesh) {
    _scene->_cachedVisibility = mesh->visibility();
  }
  else {
    _scene->_cachedVisibility = 1.f;
  }

  if (mesh) {
    onBindObservable().notifyObservers(mesh);
  }

  if (disableDepthWrite) {
    auto engine            = _scene->getEngine();
    _cachedDepthWriteState = engine->getDepthWrite();
    engine->setDepthWrite(false);
  }
}

void Material::unbind()
{
  onUnBindObservable().notifyObservers(this);

  if (disableDepthWrite) {
    auto engine = _scene->getEngine();
    engine->setDepthWrite(_cachedDepthWriteState);
  }
}

vector_t<BaseTexturePtr> Material::getActiveTextures() const
{
  return {};
}

bool Material::hasTexture(const BaseTexturePtr& /*texture*/) const
{
  return false;
}

MaterialPtr Material::clone(const string_t& /*name*/,
                            bool /*cloneChildren*/) const
{
  return nullptr;
}

vector_t<AbstractMesh*> Material::getBindedMeshes()
{
  vector_t<AbstractMesh*> result;

  for (auto& mesh : _scene->meshes) {
    if (mesh->material().get() == this) {
      result.emplace_back(mesh.get());
    }
  }

  return result;
}

void Material::forceCompilation(
  AbstractMesh* mesh,
  const ::std::function<void(Material* material)>& onCompiled,
  nullable_t<bool> clipPlane)
{
  auto subMesh = ::std::make_unique<BaseSubMesh>();
  auto scene   = getScene();

  const auto checkReady = [&]() {
    if (!_scene || !_scene->getEngine()) {
      return;
    }

    if (subMesh->_materialDefines) {
      subMesh->_materialDefines->_renderId = -1;
    }

    auto clipPlaneState = scene->clipPlane;

    if (clipPlane) {
      scene->clipPlane = Plane(0.f, 0.f, 0.f, 1.f);
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

    if (clipPlane.has_value() && (*clipPlane)) {
      scene->clipPlane = clipPlaneState;
    }
  };

  checkReady();
}

void Material::markAsDirty(unsigned int flag)
{
  if (flag & Material::TextureDirtyFlag()) {
    _markAllSubMeshesAsTexturesDirty();
  }

  if (flag & Material::LightDirtyFlag()) {
    _markAllSubMeshesAsLightsDirty();
  }

  if (flag & Material::FresnelDirtyFlag()) {
    _markAllSubMeshesAsFresnelDirty();
  }

  if (flag & Material::AttributesDirtyFlag()) {
    _markAllSubMeshesAsAttributesDirty();
  }

  if (flag & Material::MiscDirtyFlag()) {
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
      if (subMesh->getMaterial().get() != this) {
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
  _markAllSubMeshesAsDirty(
    [](MaterialDefines& defines) { defines.markAsImageProcessingDirty(); });
}

void Material::_markAllSubMeshesAsTexturesDirty()
{
  _markAllSubMeshesAsDirty(
    [](MaterialDefines& defines) { defines.markAsTexturesDirty(); });
}

void Material::_markAllSubMeshesAsFresnelDirty()
{
  _markAllSubMeshesAsDirty(
    [](MaterialDefines& defines) { defines.markAsFresnelDirty(); });
}

void Material::_markAllSubMeshesAsFresnelAndMiscDirty()
{
  _markAllSubMeshesAsDirty([](MaterialDefines& defines) {
    defines.markAsFresnelDirty();
    defines.markAsMiscDirty();
  });
}

void Material::_markAllSubMeshesAsLightsDirty()
{
  _markAllSubMeshesAsDirty(
    [](MaterialDefines& defines) { defines.markAsLightDirty(); });
}

void Material::_markAllSubMeshesAsAttributesDirty()
{
  _markAllSubMeshesAsDirty(
    [](MaterialDefines& defines) { defines.markAsAttributesDirty(); });
}

void Material::_markAllSubMeshesAsMiscDirty()
{
  _markAllSubMeshesAsDirty(
    [](MaterialDefines& defines) { defines.markAsMiscDirty(); });
}

void Material::_markAllSubMeshesAsTexturesAndMiscDirty()
{
  _markAllSubMeshesAsDirty([](MaterialDefines& defines) {
    defines.markAsTexturesDirty();
    defines.markAsMiscDirty();
  });
}

void Material::dispose(bool forceDisposeEffect, bool /*forceDisposeTextures*/)
{
  // Animations
  getScene()->stopAnimation(this);
  getScene()->freeProcessedMaterials();

  // Remove from scene
  _scene->materials.erase(::std::remove_if(_scene->materials.begin(),
                                           _scene->materials.end(),
                                           [this](const MaterialPtr& material) {
                                             return material.get() == this;
                                           }),
                          _scene->materials.end());

  // Remove from meshes
  Mesh* _mesh = nullptr;
  for (auto& mesh : _scene->meshes) {
    if (mesh->material().get() == this) {
      mesh->material = nullptr;

      _mesh = static_cast<Mesh*>(mesh.get());
      if (_mesh && _mesh->geometry()) {
        auto geometry = _mesh->geometry();
        if (storeEffectOnSubMeshes) {
          for (auto& subMesh : mesh->subMeshes) {
            geometry->_releaseVertexArrayObject(subMesh->_materialEffect);
            if (forceDisposeEffect && subMesh->_materialEffect) {
              _scene->getEngine()->_releaseEffect(subMesh->_materialEffect);
            }
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
  if (forceDisposeEffect && _effect) {
    if (!storeEffectOnSubMeshes) {
      _scene->getEngine()->_releaseEffect(_effect);
    }

    _effect = nullptr;
  }

  // Callback
  onDisposeObservable.notifyObservers(this);

  onDisposeObservable.clear();
  onBindObservable().clear();
  onUnBindObservable().clear();
}

void Material::copyTo(Material* other) const
{
  other->checkReadyOnlyOnce    = checkReadyOnlyOnce;
  other->checkReadyOnEveryCall = checkReadyOnEveryCall;
  other->alpha                 = _alpha;
  other->fillMode              = fillMode();
  other->backFaceCulling       = backFaceCulling();
  other->wireframe             = wireframe();
  other->fogEnabled            = fogEnabled();
  other->zOffset               = zOffset;
  other->alphaMode             = alphaMode();
  other->sideOrientation       = sideOrientation;
  other->disableDepthWrite     = disableDepthWrite;
  other->pointSize             = pointSize;
  other->pointsCloud           = pointsCloud();
}

Json::object Material::serialize() const
{
  return Json::object();
}

MultiMaterialPtr
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

MaterialPtr Material::Parse(const Json::value& parsedMaterial, Scene* scene,
                            const string_t& rootUrl)
{
  if (!parsedMaterial.contains("customType")
      || Json::GetString(parsedMaterial, "customType")
           == "BABYLON.StandardMaterial") {
    return StandardMaterial::Parse(parsedMaterial, scene, rootUrl);
  }

  return nullptr;
}

} // end of namespace BABYLON
