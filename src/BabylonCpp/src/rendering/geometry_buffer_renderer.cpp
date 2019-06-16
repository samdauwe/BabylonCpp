#include <babylon/rendering/geometry_buffer_renderer.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/bones/skeleton.h>
#include <babylon/core/string.h>
#include <babylon/engines/constants.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/material.h>
#include <babylon/materials/textures/multi_render_target.h>
#include <babylon/meshes/_instances_batch.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/rendering/geometry_buffer_renderer_scene_component.h>

namespace BABYLON {

GeometryBufferRenderer::GeometryBufferRenderer(Scene* scene, float ratio)
    : renderList{this, &GeometryBufferRenderer::set_renderList}
    , isSupported{this, &GeometryBufferRenderer::get_isSupported}
    , enablePosition{this, &GeometryBufferRenderer::get_enablePosition,
                     &GeometryBufferRenderer::set_enablePosition}
    , enableVelocity{this, &GeometryBufferRenderer::get_enableVelocity,
                     &GeometryBufferRenderer::set_enableVelocity}
    , scene{this, &GeometryBufferRenderer::get_scene}
    , ratio{this, &GeometryBufferRenderer::get_ratio}
    , samples{this, &GeometryBufferRenderer::get_samples,
              &GeometryBufferRenderer::set_samples}
    , _effect{nullptr}
    , _cachedDefines{""}
    , _multiRenderTarget{nullptr}
    , _enablePosition{false}
    , _enableVelocity{false}
    , _positionIndex{-1}
    , _velocityIndex{-1}
{
  _scene = scene;
  _ratio = ratio;

  // Register the G Buffer component to the scene.
  auto component
    = std::static_pointer_cast<GeometryBufferRendererSceneComponent>(
      scene->_getComponent(
        SceneComponentConstants::NAME_GEOMETRYBUFFERRENDERER));
  if (!component) {
    component = GeometryBufferRendererSceneComponent::New(scene);
    scene->_addComponent(component);
  }

  // Render target
  _createRenderTargets();
}

GeometryBufferRenderer::~GeometryBufferRenderer()
{
}

int GeometryBufferRenderer::getTextureIndex(unsigned int textureType)
{
  switch (textureType) {
    case GeometryBufferRenderer::POSITION_TEXTURE_TYPE:
      return _positionIndex;
    case GeometryBufferRenderer::VELOCITY_TEXTURE_TYPE:
      return _velocityIndex;
    default:
      return -1;
  }
}

void GeometryBufferRenderer::set_renderList(const std::vector<MeshPtr>& meshes)
{
  _multiRenderTarget->renderList().clear();
  _multiRenderTarget->renderList().reserve(meshes.size());
  for (auto& mesh : meshes) {
    _multiRenderTarget->renderList().emplace_back(mesh.get());
  }
}

bool GeometryBufferRenderer::get_isSupported() const
{
  return _multiRenderTarget->isSupported();
}

bool GeometryBufferRenderer::get_enablePosition() const
{
  return _enablePosition;
}

void GeometryBufferRenderer::set_enablePosition(bool enable)
{
  _enablePosition = enable;
  dispose();
  _createRenderTargets();
}

bool GeometryBufferRenderer::get_enableVelocity() const
{
  return _enableVelocity;
}

void GeometryBufferRenderer::set_enableVelocity(bool enable)
{
  _enableVelocity = enable;

  if (!enable) {
    _previousTransformationMatrices = {};
  }

  dispose();
  _createRenderTargets();
}

Scene*& GeometryBufferRenderer::get_scene()
{
  return _scene;
}

float GeometryBufferRenderer::get_ratio() const
{
  return _ratio;
}

bool GeometryBufferRenderer::isReady(SubMesh* subMesh, bool useInstances)
{
  auto material = subMesh->getMaterial();

  if (material && material->disableDepthWrite) {
    return false;
  }

  std::vector<std::string> defines;

  std::vector<std::string> attribs{VertexBuffer::PositionKind,
                                   VertexBuffer::NormalKind};

  auto mesh = subMesh->getMesh();

  // Alpha test
  if (material && material->needAlphaTesting()) {
    defines.emplace_back("#define ALPHATEST");
    if (mesh->isVerticesDataPresent(VertexBuffer::UVKind)) {
      attribs.emplace_back(VertexBuffer::UVKind);
      defines.emplace_back("#define UV1");
    }
    if (mesh->isVerticesDataPresent(VertexBuffer::UV2Kind)) {
      attribs.emplace_back(VertexBuffer::UV2Kind);
      defines.emplace_back("#define UV2");
    }
  }

  // Buffers
  if (_enablePosition) {
    defines.emplace_back("#define POSITION");
    defines.emplace_back("#define POSITION_INDEX "
                         + std::to_string(_positionIndex));
  }

  if (_enableVelocity) {
    defines.emplace_back("#define VELOCITY");
    defines.emplace_back("#define VELOCITY_INDEX "
                         + std::to_string(_velocityIndex));
    if (stl_util::contains(excludedSkinnedMeshesFromVelocity, mesh)) {
      defines.emplace_back("#define BONES_VELOCITY_ENABLED");
    }
  }

  // Bones
  if (mesh->useBones() && mesh->computeBonesUsingShaders()) {
    attribs.emplace_back(VertexBuffer::MatricesIndicesKind);
    attribs.emplace_back(VertexBuffer::MatricesWeightsKind);
    if (mesh->numBoneInfluencers() > 4) {
      attribs.emplace_back(VertexBuffer::MatricesIndicesExtraKind);
      attribs.emplace_back(VertexBuffer::MatricesWeightsExtraKind);
    }
    defines.emplace_back("#define NUM_BONE_INFLUENCERS "
                         + std::to_string(mesh->numBoneInfluencers()));
    defines.emplace_back("#define BonesPerMesh "
                         + std::to_string(mesh->skeleton() ?
                                            mesh->skeleton()->bones.size() + 1 :
                                            0));
  }
  else {
    defines.emplace_back("#define NUM_BONE_INFLUENCERS 0");
  }

  // Instances
  if (useInstances) {
    defines.emplace_back("#define INSTANCES");
    attribs.emplace_back(VertexBuffer::World0Kind);
    attribs.emplace_back(VertexBuffer::World1Kind);
    attribs.emplace_back(VertexBuffer::World2Kind);
    attribs.emplace_back(VertexBuffer::World3Kind);
  }

  // Setup textures count
  defines.emplace_back("#define RENDER_TARGET_COUNT "
                       + std::to_string(_multiRenderTarget->textures().size()));

  // Get correct effect
  auto join = String::join(defines, '\n');
  if (_cachedDefines != join) {
    _cachedDefines = join;

    const std::unordered_map<std::string, unsigned int> indexParameters{
      {"buffersCount", _enablePosition ? 3u : 2u}};

    EffectCreationOptions options;
    options.attributes = std::move(attribs);
    options.uniformsNames
      = {"world", "mBones",        "viewProjection",         "diffuseMatrix",
         "view",  "previousWorld", "previousViewProjection", "mPreviousBones"};
    options.samplers        = {"diffuseSampler"};
    options.defines         = std::move(join);
    options.indexParameters = std::move(indexParameters);

    _effect = _scene->getEngine()->createEffect("geometry", options,
                                                _scene->getEngine());
  }

  return _effect->isReady();
}

MultiRenderTargetPtr GeometryBufferRenderer::getGBuffer() const
{
  return _multiRenderTarget;
}

unsigned int GeometryBufferRenderer::get_samples() const
{
  return _multiRenderTarget->samples();
}

void GeometryBufferRenderer::set_samples(unsigned int value)
{
  _multiRenderTarget->samples = value;
}

void GeometryBufferRenderer::dispose()
{
  getGBuffer()->dispose();
}

void GeometryBufferRenderer::_createRenderTargets()
{
  auto engine = _scene->getEngine();

  auto count = 2;

  if (_enablePosition) {
    _positionIndex = count;
    ++count;
  }

  if (_enableVelocity) {
    _velocityIndex = count;
    ++count;
  }

  // Render target
  IMultiRenderTargetOptions options;
  options.generateMipMaps      = false;
  options.generateDepthTexture = true;
  options.defaultType          = Constants::TEXTURETYPE_FLOAT;
  _multiRenderTarget           = std::make_shared<MultiRenderTarget>(
    "gBuffer",
    Size{static_cast<int>(engine->getRenderWidth() * _ratio),
         static_cast<int>(engine->getRenderHeight() * _ratio)},
    count, _scene, options);
  if (!isSupported()) {
    return;
  }

  _multiRenderTarget->wrapU           = TextureConstants::CLAMP_ADDRESSMODE;
  _multiRenderTarget->wrapV           = TextureConstants::CLAMP_ADDRESSMODE;
  _multiRenderTarget->refreshRate     = 1;
  _multiRenderTarget->renderParticles = false;
  _multiRenderTarget->renderList      = {};

  // set default depth value to 1.0 (far away)
  _multiRenderTarget->onClearObservable.add([](Engine* engine, EventState&) {
    engine->clear(Color4(0.f, 0.f, 0.f, 1.f), true, true, true);
  });

  // Custom render function
  _multiRenderTarget->customRenderFunction
    = [this, engine](const std::vector<SubMesh*>& opaqueSubMeshes,
                     const std::vector<SubMesh*>& alphaTestSubMeshes,
                     const std::vector<SubMesh*>& /*transparentSubMeshes*/,
                     const std::vector<SubMesh*>& depthOnlySubMeshes,
                     const std::function<void()>& /*beforeTransparents*/) {
        if (!depthOnlySubMeshes.empty()) {
          engine->setColorWrite(false);
          for (auto& depthOnlySubMesh : depthOnlySubMeshes) {
            renderSubMesh(depthOnlySubMesh);
          }
          engine->setColorWrite(true);
        }

        for (auto& opaqueSubMesh : opaqueSubMeshes) {
          renderSubMesh(opaqueSubMesh);
        }

        for (auto& alphaTestSubMesh : alphaTestSubMeshes) {
          renderSubMesh(alphaTestSubMesh);
        }
      };
}

void GeometryBufferRenderer::renderSubMesh(SubMesh* subMesh)
{
  auto mesh     = subMesh->getRenderingMesh();
  auto engine   = _scene->getEngine();
  auto material = subMesh->getMaterial();

  if (!material) {
    return;
  }

  // Velocity
  if (_enableVelocity
      && !stl_util::contains(_previousTransformationMatrices, mesh->uniqueId)) {
    _previousTransformationMatrices[mesh->uniqueId]
      = ISavedTransformationMatrix{
        Matrix::Identity(),          // world
        _scene->getTransformMatrix() // viewProjection
      };

    if (mesh->skeleton()) {
      const auto bonesTransformations
        = mesh->skeleton()->getTransformMatrices(mesh.get());
      Float32Array dest(bonesTransformations.size());
      _previousBonesTransformationMatrices[mesh->uniqueId]
        = _copyBonesTransformationMatrices(bonesTransformations, dest);
    }
  }

  // Culling
  engine->setState(material->backFaceCulling(), 0, false,
                   _scene->useRightHandedSystem());

  // Managing instances
  auto batch = mesh->_getInstancesRenderList(subMesh->_id);

  if (batch->mustReturn) {
    return;
  }

  auto hardwareInstancedRendering
    = (engine->getCaps().instancedArrays != 0)
      && (stl_util::contains(batch->visibleInstances, subMesh->_id))
      && (!batch->visibleInstances[subMesh->_id].empty());

  if (isReady(subMesh, hardwareInstancedRendering)) {
    engine->enableEffect(_effect);
    mesh->_bind(subMesh, _effect, Material::TriangleFillMode());

    _effect->setMatrix("viewProjection", _scene->getTransformMatrix());
    _effect->setMatrix("view", _scene->getViewMatrix());

    // Alpha test
    if (material && material->needAlphaTesting()) {
      auto alphaTexture = material->getAlphaTestTexture();
      if (alphaTexture) {
        _effect->setTexture("diffuseSampler", alphaTexture);
        _effect->setMatrix("diffuseMatrix", *alphaTexture->getTextureMatrix());
      }
    }

    // Bones
    if (mesh->useBones() && mesh->computeBonesUsingShaders()
        && mesh->skeleton()) {
      _effect->setMatrices("mBones",
                           mesh->skeleton()->getTransformMatrices(mesh.get()));
      if (_enableVelocity) {
        _effect->setMatrices(
          "mPreviousBones",
          _previousBonesTransformationMatrices[mesh->uniqueId]);
      }
    }

    // Velocity
    if (_enableVelocity) {
      _effect->setMatrix("previousWorld",
                         _previousTransformationMatrices[mesh->uniqueId].world);
      _effect->setMatrix(
        "previousViewProjection",
        _previousTransformationMatrices[mesh->uniqueId].viewProjection);
    }

    // Draw
    mesh->_processRendering(subMesh, _effect, Material::TriangleFillMode(),
                            batch, hardwareInstancedRendering,
                            [this](bool /*isInstance*/, Matrix world,
                                   Material* /*effectiveMaterial*/) {
                              _effect->setMatrix("world", world);
                            });
  }

  // Velocity
  if (_enableVelocity) {
    _previousTransformationMatrices[mesh->uniqueId].world
      = mesh->getWorldMatrix();
    _previousTransformationMatrices[mesh->uniqueId].viewProjection
      = _scene->getTransformMatrix();
    if (mesh->skeleton()) {
      _copyBonesTransformationMatrices(
        mesh->skeleton()->getTransformMatrices(mesh.get()),
        _previousBonesTransformationMatrices[mesh->uniqueId]);
    }
  }
}

Float32Array& GeometryBufferRenderer::_copyBonesTransformationMatrices(
  const Float32Array& source, Float32Array& target)
{
  for (size_t i = 0; i < source.size(); ++i) {
    target[i] = source[i];
  }

  return target;
}

} // end of namespace BABYLON
