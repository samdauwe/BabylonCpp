#include <babylon/rendering/outline_renderer.h>

#include <babylon/bones/skeleton.h>
#include <babylon/core/string.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/material.h>
#include <babylon/mesh/_instances_batch.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {

OutlineRenderer::OutlineRenderer(Scene* scene) : _scene{scene}
{
}

OutlineRenderer::~OutlineRenderer()
{
}

void OutlineRenderer::render(SubMesh* subMesh, _InstancesBatch* batch,
                             bool useOverlay)
{
  auto engine = _scene->getEngine();

  bool hardwareInstancedRendering
    = (engine->getCaps().instancedArrays != false)
      && (batch->visibleInstances.find(subMesh->_id)
          != batch->visibleInstances.end())
      && (!batch->visibleInstances[subMesh->_id].empty());

  if (!isReady(subMesh, hardwareInstancedRendering)) {
    return;
  }

  auto mesh     = subMesh->getRenderingMesh();
  auto material = subMesh->getMaterial();

  engine->enableEffect(_effect);
  _effect->setFloat("offset", useOverlay ? 0 : mesh->outlineWidth);
  _effect->setColor4("color",
                     useOverlay ? mesh->overlayColor : mesh->outlineColor,
                     useOverlay ? mesh->overlayAlpha : 1.f);
  _effect->setMatrix("viewProjection", _scene->getTransformMatrix());

  // Bones
  if (mesh->useBones() && mesh->computeBonesUsingShaders()) {
    _effect->setMatrices("mBones",
                         mesh->skeleton()->getTransformMatrices(mesh));
  }

  mesh->_bind(subMesh, _effect, Material::TriangleFillMode);

  // Alpha test
  if (material && material->needAlphaTesting()) {
    auto alphaTexture = material->getAlphaTestTexture();
    _effect->setTexture("diffuseSampler", alphaTexture);
    _effect->setMatrix("diffuseMatrix", *alphaTexture->getTextureMatrix());
  }

  mesh->_processRendering(subMesh, _effect, Material::TriangleFillMode, batch,
                          hardwareInstancedRendering,
                          [this](bool, Matrix world, Material*) {
                            _effect->setMatrix("world", world);
                          });
}

bool OutlineRenderer::isReady(SubMesh* subMesh, bool useInstances)
{
  std::vector<std::string> defines;
  std::vector<std::string> attribs{VertexBuffer::PositionKindChars,
                                   VertexBuffer::NormalKindChars};

  auto mesh     = subMesh->getMesh();
  auto material = subMesh->getMaterial();

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
      + std::to_string((mesh->skeleton()->bones.size() + 1)));
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
  std::string join = String::join(defines, '\n');
  if (_cachedDefines != join) {
    _cachedDefines = join;
    _effect        = _scene->getEngine()->createEffect(
      "outline", attribs,
      {"world", "mBones", "viewProjection", "diffuseMatrix", "offset", "color"},
      {"diffuseSampler"}, join);
  }

  return _effect->isReady();
}

} // end of namespace BABYLON
