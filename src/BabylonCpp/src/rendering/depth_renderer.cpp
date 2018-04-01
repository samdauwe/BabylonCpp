#include <babylon/rendering/depth_renderer.h>

#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/string.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/material.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/math/color4.h>
#include <babylon/mesh/_instances_batch.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/mesh/vertex_buffer.h>
#include <babylon/rendering/depth_renderer.h>

namespace BABYLON {

DepthRenderer::DepthRenderer(Scene* scene, unsigned int type, Camera* camera)
    : _scene{scene}, _depthMap{nullptr}, _effect{nullptr}, _camera{camera}
{
  auto engine = scene->getEngine();

  // Render target
  _depthMap = ::std::make_unique<RenderTargetTexture>(
    "depthMap", ISize{engine->getRenderWidth(), engine->getRenderHeight()},
    _scene, false, true, type);
  _depthMap->wrapU           = TextureConstants::CLAMP_ADDRESSMODE;
  _depthMap->wrapV           = TextureConstants::CLAMP_ADDRESSMODE;
  _depthMap->refreshRate     = 1;
  _depthMap->renderParticles = false;
  _depthMap->renderList.clear();

  // Camera to get depth map from to support multiple concurrent cameras
  _depthMap->activeCamera           = _camera;
  _depthMap->ignoreCameraViewport   = true;
  _depthMap->useCameraPostProcesses = false;

  // set default depth value to 1.0 (far away)
  _depthMap->onClearObservable.add([](Engine* _engine, EventState&) {
    _engine->clear(Color4(1.f, 1.f, 1.f, 1.f), true, true);
  });

  // Custom render function
  auto renderSubMesh = [this](SubMesh* subMesh) {
    auto mesh     = subMesh->getRenderingMesh();
    auto scene    = _scene;
    auto engine   = scene->getEngine();
    auto material = subMesh->getMaterial();

    if (!material) {
      return;
    }

    // Culling and reverse (right handed system)
    engine->setState(material->backFaceCulling(), 0, false,
                     scene->useRightHandedSystem());

    // Managing instances
    auto batch = mesh->_getInstancesRenderList(subMesh->_id);

    if (batch->mustReturn) {
      return;
    }

    bool hardwareInstancedRendering
      = engine->getCaps().instancedArrays
        && (batch->visibleInstances.find(subMesh->_id)
            != batch->visibleInstances.end());

    auto camera = (!_camera) ? _camera : scene->activeCamera;
    if (isReady(subMesh, hardwareInstancedRendering) && camera) {
      engine->enableEffect(_effect);
      mesh->_bind(subMesh, _effect, Material::TriangleFillMode());

      _effect->setMatrix("viewProjection", _scene->getTransformMatrix());

      _effect->setFloat2("depthValues", camera->minZ,
                         camera->minZ + camera->maxZ);

      // Alpha test
      if (material && material->needAlphaTesting()) {
        auto alphaTexture = material->getAlphaTestTexture();
        if (alphaTexture) {
          _effect->setTexture("diffuseSampler", alphaTexture);
          _effect->setMatrix("diffuseMatrix",
                             *alphaTexture->getTextureMatrix());
        }
      }

      // Bones
      if (mesh->useBones() && mesh->computeBonesUsingShaders()
          && mesh->skeleton()) {
        _effect->setMatrices("mBones",
                             mesh->skeleton()->getTransformMatrices(mesh));
      }

      // Draw
      mesh->_processRendering(subMesh, _effect, Material::TriangleFillMode(),
                              batch, hardwareInstancedRendering,
                              [this](bool /*isInstance*/, Matrix world,
                                     Material* /*effectiveMaterial*/) {
                                _effect->setMatrix("world", world);
                              });
    }
  };

  _depthMap->customRenderFunction
    = [engine,
       renderSubMesh](const vector_t<SubMesh*>& opaqueSubMeshes,
                      const vector_t<SubMesh*>& alphaTestSubMeshes,
                      const vector_t<SubMesh*>& /*transparentSubMeshes*/,
                      const vector_t<SubMesh*>& depthOnlySubMeshes,
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

DepthRenderer::~DepthRenderer()
{
}

bool DepthRenderer::isReady(SubMesh* subMesh, bool useInstances)
{
  auto material = subMesh->getMaterial();
  if (material && material->disableDepthWrite) {
    return false;
  }

  vector_t<string_t> defines;

  vector_t<string_t> attribs{VertexBuffer::PositionKindChars};

  auto mesh = subMesh->getMesh();

  // Alpha test
  if (material && material->needAlphaTesting()
      && material->getAlphaTestTexture()) {
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

    EffectCreationOptions options;
    options.attributes = ::std::move(attribs);
    options.uniformsNames
      = {"world", "mBones", "viewProjection", "diffuseMatrix", "depthValues"};
    options.samplers = {"diffuseSampler"};
    options.defines  = ::std::move(join);

    _effect = _scene->getEngine()->createEffect("depth", options,
                                                _scene->getEngine());
  }

  return _effect->isReady();
}

RenderTargetTexture* DepthRenderer::getDepthMap()
{
  return _depthMap.get();
}

void DepthRenderer::dispose()
{
  _depthMap->dispose();
}

} // end of namespace BABYLON
