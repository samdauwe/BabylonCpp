#include <babylon/rendering/geometry_buffer_renderer.h>

#include <babylon/bones/skeleton.h>
#include <babylon/core/string.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/material.h>
#include <babylon/materials/textures/multi_render_target.h>
#include <babylon/mesh/_instances_batch.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {

GeometryBufferRenderer::GeometryBufferRenderer(Scene* scene, float ratio)
    : renderList{this, &GeometryBufferRenderer::set_renderList}
    , isSupported{this, &GeometryBufferRenderer::get_isSupported}
    , enablePosition{this, &GeometryBufferRenderer::get_enablePosition,
                     &GeometryBufferRenderer::set_enablePosition}
    , scene{this, &GeometryBufferRenderer::get_scene}
    , ratio{this, &GeometryBufferRenderer::get_ratio}
    , samples{this, &GeometryBufferRenderer::get_samples,
              &GeometryBufferRenderer::set_samples}
    , _effect{nullptr}
    , _cachedDefines{""}
    , _scene{scene}
    , _multiRenderTarget{nullptr}
    , _ratio{ratio}
    , _enablePosition{false}
{
  // Render target
  _createRenderTargets();
}

GeometryBufferRenderer::~GeometryBufferRenderer()
{
}

void GeometryBufferRenderer::set_renderList(const vector_t<MeshPtr>& meshes)
{
  _multiRenderTarget->renderList().clear();
  _multiRenderTarget->renderList().reserve(meshes.size());
  for (auto& mesh : meshes) {
    _multiRenderTarget->renderList().emplace_back(mesh);
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

  vector_t<string_t> defines;

  vector_t<string_t> attribs{VertexBuffer::PositionKindChars,
                             VertexBuffer::NormalKindChars};

  auto mesh = subMesh->getMesh();

  // Alpha test
  if (material && material->needAlphaTesting()) {
    defines.emplace_back("#define ALPHATEST");
    if (mesh->isVerticesDataPresent(VertexBuffer::UVKind)) {
      attribs.emplace_back(VertexBuffer::UVKindChars);
      defines.emplace_back("#define UV1");
    }
    if (mesh->isVerticesDataPresent(VertexBuffer::UV2Kind)) {
      attribs.emplace_back(VertexBuffer::UV2KindChars);
      defines.emplace_back("#define UV2");
    }
  }

  // Buffers
  if (_enablePosition) {
    defines.emplace_back("#define POSITION");
  }

  // Bones
  if (mesh->useBones() && mesh->computeBonesUsingShaders()) {
    attribs.emplace_back(VertexBuffer::MatricesIndicesKindChars);
    attribs.emplace_back(VertexBuffer::MatricesWeightsKindChars);
    if (mesh->numBoneInfluencers() > 4) {
      attribs.emplace_back(VertexBuffer::MatricesIndicesExtraKindChars);
      attribs.emplace_back(VertexBuffer::MatricesWeightsExtraKindChars);
    }
    defines.emplace_back("#define NUM_BONE_INFLUENCERS "
                         + ::std::to_string(mesh->numBoneInfluencers()));
    defines.emplace_back(
      "#define BonesPerMesh "
      + ::std::to_string(mesh->skeleton() ? mesh->skeleton()->bones.size() + 1 :
                                            0));
  }
  else {
    defines.emplace_back("#define NUM_BONE_INFLUENCERS 0");
  }

  // Instances
  if (useInstances) {
    defines.emplace_back("#define INSTANCES");
    attribs.emplace_back(VertexBuffer::World0KindChars);
    attribs.emplace_back(VertexBuffer::World1KindChars);
    attribs.emplace_back(VertexBuffer::World2KindChars);
    attribs.emplace_back(VertexBuffer::World3KindChars);
  }

  // Get correct effect
  auto join = String::join(defines, '\n');
  if (_cachedDefines != join) {
    _cachedDefines = join;

    const unordered_map_t<string_t, unsigned int> indexParameters{
      {"buffersCount", _enablePosition ? 3u : 2u}};

    EffectCreationOptions options;
    options.attributes = ::std::move(attribs);
    options.uniformsNames
      = {"world", "mBones", "viewProjection", "diffuseMatrix", "view"};
    options.samplers        = {"diffuseSampler"};
    options.defines         = ::std::move(join);
    options.indexParameters = ::std::move(indexParameters);

    _effect = _scene->getEngine()->createEffect("geometry", options,
                                                _scene->getEngine());
  }

  return _effect->isReady();
}

MultiRenderTarget* GeometryBufferRenderer::getGBuffer() const
{
  return _multiRenderTarget.get();
}

unsigned int GeometryBufferRenderer::get_samples() const
{
  return _multiRenderTarget->samples();
}

void GeometryBufferRenderer::set_samples(unsigned int value)
{
  _multiRenderTarget->setSamples(value);
}

void GeometryBufferRenderer::dispose()
{
  getGBuffer()->dispose();
}

void GeometryBufferRenderer::_createRenderTargets()
{
  auto engine = _scene->getEngine();
  auto count  = _enablePosition ? 3 : 2;

  // Render target
  IMultiRenderTargetOptions options;
  options.generateMipMaps      = false;
  options.generateDepthTexture = true;
  options.defaultType          = EngineConstants::TEXTURETYPE_FLOAT;
  _multiRenderTarget           = ::std::make_unique<MultiRenderTarget>(
    "gBuffer",
    Size{static_cast<int>(engine->getRenderWidth() * _ratio),
         static_cast<int>(engine->getRenderHeight() * _ratio)},
    count, _scene, options);
  if (!isSupported()) {
    return;
  }

  _multiRenderTarget->setWrapU(TextureConstants::CLAMP_ADDRESSMODE);
  _multiRenderTarget->setWrapV(TextureConstants::CLAMP_ADDRESSMODE);
  _multiRenderTarget->refreshRate     = 1;
  _multiRenderTarget->renderParticles = false;
  _multiRenderTarget->renderList      = {};

  // set default depth value to 1.0 (far away)
  _multiRenderTarget->onClearObservable.add([](Engine* engine, EventState&) {
    engine->clear(Color4(0.f, 0.f, 0.f, 1.f), true, true, true);
  });

  // Custom render function
  _multiRenderTarget->customRenderFunction
    = [this, engine](const vector_t<SubMeshPtr>& opaqueSubMeshes,
                     const vector_t<SubMeshPtr>& alphaTestSubMeshes,
                     const vector_t<SubMeshPtr>& /*transparentSubMeshes*/,
                     const vector_t<SubMeshPtr>& depthOnlySubMeshes,
                     const ::std::function<void()>& /*beforeTransparents*/) {
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

void GeometryBufferRenderer::renderSubMesh(const SubMeshPtr& subMesh)
{
  auto mesh     = subMesh->getRenderingMesh();
  auto scene    = _scene;
  auto engine   = scene->getEngine();
  auto material = subMesh->getMaterial();

  if (!material) {
    return;
  }

  // Culling
  engine->setState(material->backFaceCulling(), 0, false,
                   scene->useRightHandedSystem());

  // Managing instances
  auto batch = mesh->_getInstancesRenderList(subMesh->_id);

  if (batch->mustReturn) {
    return;
  }

  auto hardwareInstancedRendering
    = (engine->getCaps().instancedArrays != 0)
      && (stl_util::contains(batch->visibleInstances, subMesh->_id))
      && (!batch->visibleInstances[subMesh->_id].empty());

  if (isReady(subMesh.get(), hardwareInstancedRendering)) {
    engine->enableEffect(_effect);
    mesh->_bind(subMesh.get(), _effect, Material::TriangleFillMode());

    _effect->setMatrix("viewProjection", scene->getTransformMatrix());
    _effect->setMatrix("view", scene->getViewMatrix());

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
    }

    // Draw
    mesh->_processRendering(subMesh.get(), _effect,
                            Material::TriangleFillMode(), batch,
                            hardwareInstancedRendering,
                            [this](bool /*isInstance*/, Matrix world,
                                   Material* /*effectiveMaterial*/) {
                              _effect->setMatrix("world", world);
                            });
  }
}

} // end of namespace BABYLON
