#include <babylon/rendering/outline_renderer.h>

#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/string.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/material.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/mesh/_instances_batch.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {

OutlineRenderer::OutlineRenderer(Scene* iScene) : zOffset{1.f}
{
  ISceneComponent::name = OutlineRenderer::name;
  scene                 = iScene;
  _engine               = scene->getEngine();
}

OutlineRenderer::~OutlineRenderer()
{
}

void OutlineRenderer::addToScene(const OutlineRendererPtr& outlineRenderer)
{
  scene->_addComponent(outlineRenderer);
}

void OutlineRenderer::_register()
{
  scene->_beforeRenderingMeshStage.registerStep(
    SceneComponentConstants::STEP_BEFORERENDERINGMESH_OUTLINE, this,
    [this](AbstractMesh* mesh, SubMesh* subMesh,
           const _InstancesBatchPtr& batch) {
      _beforeRenderingMesh(mesh, subMesh, batch);
    });
  scene->_afterRenderingMeshStage.registerStep(
    SceneComponentConstants::STEP_AFTERRENDERINGMESH_OUTLINE, this,
    [this](AbstractMesh* mesh, SubMesh* subMesh,
           const _InstancesBatchPtr& batch) {
      _afterRenderingMesh(mesh, subMesh, batch);
    });
}

void OutlineRenderer::rebuild()
{
  // Nothing to do here.
}

void OutlineRenderer::dispose()
{
  // Nothing to do here.
}

void OutlineRenderer::render(SubMesh* subMesh, const _InstancesBatchPtr& batch,
                             bool useOverlay)
{
  auto engine = scene->getEngine();

  bool hardwareInstancedRendering
    = engine->getCaps().instancedArrays
      && (batch->visibleInstances.find(subMesh->_id)
          != batch->visibleInstances.end())
      && (!batch->visibleInstances[subMesh->_id].empty());

  if (!isReady(subMesh, hardwareInstancedRendering)) {
    return;
  }

  auto mesh     = subMesh->getRenderingMesh();
  auto material = subMesh->getMaterial();

  if (!material || !scene->activeCamera) {
    return;
  }

  engine->enableEffect(_effect);

  // Logarithmic depth
  if (material->useLogarithmicDepth()) {
    _effect->setFloat(
      "logarithmicDepthConstant",
      2.f / (std::log(scene->activeCamera->maxZ + 1.f) / Math::LN2));
  }

  _effect->setFloat("offset", useOverlay ? 0 : mesh->outlineWidth);
  _effect->setColor4("color",
                     useOverlay ? mesh->overlayColor : mesh->outlineColor,
                     useOverlay ? mesh->overlayAlpha : material->alpha());
  _effect->setMatrix("viewProjection", scene->getTransformMatrix());

  // Bones
  if (mesh->useBones() && mesh->computeBonesUsingShaders()
      && mesh->skeleton()) {
    _effect->setMatrices("mBones",
                         mesh->skeleton()->getTransformMatrices(mesh.get()));
  }

  mesh->_bind(subMesh, _effect, Material::TriangleFillMode());

  // Alpha test
  if (material && material->needAlphaTesting()) {
    auto alphaTexture = material->getAlphaTestTexture();
    if (alphaTexture) {
      _effect->setTexture("diffuseSampler", alphaTexture);
      _effect->setMatrix("diffuseMatrix", *alphaTexture->getTextureMatrix());
    }
  }

  engine->setZOffset(-zOffset);

  mesh->_processRendering(subMesh, _effect, Material::TriangleFillMode(), batch,
                          hardwareInstancedRendering,
                          [this](bool, const Matrix& world, Material*) {
                            _effect->setMatrix("world", world);
                          });

  engine->setZOffset(0.f);
}

bool OutlineRenderer::isReady(SubMesh* subMesh, bool useInstances)
{
  std::vector<std::string> defines;
  std::vector<std::string> attribs{VertexBuffer::PositionKindChars,
                                   VertexBuffer::NormalKindChars};

  auto mesh     = subMesh->getMesh();
  auto material = subMesh->getMaterial();

  if (material) {
    // Alpha test
    if (material->needAlphaTesting()) {
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

    // Logarithmic depth
    if (material->useLogarithmicDepth()) {
      defines.emplace_back("#define LOGARITHMICDEPTH");
    }
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
                         + std::to_string(mesh->numBoneInfluencers()));
    defines.emplace_back(
      "#define BonesPerMesh "
      + std::to_string(
          (mesh->skeleton() ? mesh->skeleton()->bones.size() + 1 : 0)));
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

    EffectCreationOptions options;
    options.attributes = std::move(attribs);
    options.uniformsNames
      = {"world",  "mBones", "viewProjection",          "diffuseMatrix",
         "offset", "color",  "logarithmicDepthConstant"};
    options.samplers = {"diffuseSampler"};
    options.defines  = std::move(join);

    _effect = scene->getEngine()->createEffect("outline", options,
                                               scene->getEngine());
  }

  return _effect->isReady();
}

void OutlineRenderer::_beforeRenderingMesh(AbstractMesh* mesh, SubMesh* subMesh,
                                           const _InstancesBatchPtr& batch)
{
  // Outline - step 1
  _savedDepthWrite = _engine->getDepthWrite();
  if (mesh->renderOutline) {
    _engine->setDepthWrite(false);
    render(subMesh, batch);
    _engine->setDepthWrite(_savedDepthWrite);
  }
}

void OutlineRenderer::_afterRenderingMesh(AbstractMesh* mesh, SubMesh* subMesh,
                                          const _InstancesBatchPtr& batch)
{
  // Outline - step 2
  if (mesh->renderOutline && _savedDepthWrite) {
    _engine->setDepthWrite(true);
    _engine->setColorWrite(false);
    render(subMesh, batch);
    _engine->setColorWrite(true);
  }

  // Overlay
  if (mesh->renderOverlay) {
    auto currentMode = _engine->getAlphaMode();
    _engine->setAlphaMode(EngineConstants::ALPHA_COMBINE);
    render(subMesh, batch, true);
    _engine->setAlphaMode(currentMode);
  }
}

} // end of namespace BABYLON
