#include <babylon/postprocesses/post_process_manager.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/engines/constants.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/material.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/misc/string_tools.h>
#include <babylon/postprocesses/post_process.h>

namespace BABYLON {

PostProcessManager::PostProcessManager(Scene* scene) : _scene{scene}, _indexBuffer{nullptr}
{
}

PostProcessManager::~PostProcessManager() = default;

void PostProcessManager::_prepareBuffers()
{
  if (stl_util::contains(_vertexBuffers, VertexBuffer::PositionKind)
      && _vertexBuffers[VertexBuffer::PositionKind]) {
    return;
  }

  // VBO
  Float32Array vertices{
    1.f,  1.f,  //
    -1.f, 1.f,  //
    -1.f, -1.f, //
    1.f,  -1.f  //
  };
  _vertexBuffers[VertexBuffer::PositionKind] = std::make_shared<VertexBuffer>(
    _scene->getEngine(), vertices, VertexBuffer::PositionKind, false, false, 2);

  _buildIndexBuffer();
}

void PostProcessManager::_buildIndexBuffer()
{
  // Indices
  IndicesArray indices{
    0, //
    1, //
    2, //
    0, //
    2, //
    3  //
  };

  _indexBuffer = _scene->getEngine()->createIndexBuffer(indices);
}

void PostProcessManager::_rebuild()
{
  if (!stl_util::contains(_vertexBuffers, VertexBuffer::PositionKind)
      || !_vertexBuffers[VertexBuffer::PositionKind]) {
    return;
  }
  _vertexBuffers[VertexBuffer::PositionKind]->_rebuild();
  _buildIndexBuffer();
}

bool PostProcessManager::_prepareFrame(const InternalTexturePtr& sourceTexture,
                                       const std::vector<PostProcessPtr>& iPostProcesses)
{
  const auto& camera = _scene->activeCamera();
  if (!camera) {
    return false;
  }

  auto postProcesses = !iPostProcesses.empty() ? iPostProcesses : camera->_postProcesses;

  // Filter out all null elements
  stl_util::erase_remove_if(postProcesses, [](const PostProcessPtr& pp) { return pp == nullptr; });

  if (postProcesses.empty() || !_scene->postProcessesEnabled) {
    return false;
  }

  postProcesses[0]->activate(camera, sourceTexture, !postProcesses.empty());
  return true;
}

void PostProcessManager::directRender(const std::vector<PostProcessPtr>& postProcesses,
                                      const InternalTexturePtr& targetTexture,
                                      bool forceFullscreenViewport, unsigned int faceIndex,
                                      int lodLevel, bool doNotBindFrambuffer)
{
  auto engine = _scene->getEngine();

  for (unsigned int index = 0; index < postProcesses.size(); ++index) {
    if (index < postProcesses.size() - 1) {
      postProcesses[index + 1]->activate(_scene->activeCamera(), targetTexture);
    }
    else {
      if (targetTexture) {
        engine->bindFramebuffer(targetTexture, faceIndex, std::nullopt, std::nullopt,
                                forceFullscreenViewport, lodLevel);
      }
      else if (!doNotBindFrambuffer) {
        engine->restoreDefaultFramebuffer();
      }
      engine->_debugInsertMarker(
        StringTools::printf("post process %s output", postProcesses[index]->name.c_str()));
    }

    auto pp     = postProcesses[index];
    auto effect = pp->apply();

    if (effect) {
      pp->onBeforeRenderObservable.notifyObservers(effect.get());

      // VBOs
      _prepareBuffers();
      engine->bindBuffers(_vertexBuffers, _indexBuffer, effect);

      // Draw order
      engine->drawElementsType(Constants::MATERIAL_TriangleFillMode, 0, 6);

      pp->onAfterRenderObservable.notifyObservers(effect.get());
    }
  }

  // Restore depth buffer
  engine->setDepthBuffer(true);
  engine->setDepthWrite(true);
}

void PostProcessManager::_finalizeFrame(bool doNotPresent, const InternalTexturePtr& targetTexture,
                                        unsigned int faceIndex,
                                        const std::vector<PostProcessPtr>& _postProcesses,
                                        bool forceFullscreenViewport)
{
  const auto& camera = _scene->activeCamera();
  if (!camera) {
    return;
  }

  auto postProcesses = _postProcesses.empty() ? camera->_postProcesses : _postProcesses;

  // Filter out all null elements
  stl_util::erase_remove_if(postProcesses, [](const PostProcessPtr& pp) { return pp == nullptr; });

  if (postProcesses.empty() || !_scene->postProcessesEnabled) {
    return;
  }
  auto engine = _scene->getEngine();

  for (size_t index = 0, len = postProcesses.size(); index < len; ++index) {
    auto& pp = postProcesses[index];
    if (index < len - 1) {
      pp->_outputTexture = postProcesses[index + 1]->activate(camera, targetTexture);
    }
    else {
      if (targetTexture) {
        engine->bindFramebuffer(targetTexture, faceIndex, 0, 0, forceFullscreenViewport);
        pp->_outputTexture = targetTexture;
      }
      else {
        engine->restoreDefaultFramebuffer();
        pp->_outputTexture = nullptr;
      }
      engine->_debugInsertMarker(
        StringTools::printf("post process %s output", postProcesses[index]->name.c_str()));
    }

    if (doNotPresent) {
      break;
    }

    auto effect = pp->apply();

    if (effect) {
      pp->onBeforeRenderObservable.notifyObservers(effect.get());

      // VBOs
      _prepareBuffers();
      engine->bindBuffers(_vertexBuffers, _indexBuffer, effect);

      // Draw order
      engine->drawElementsType(Constants::MATERIAL_TriangleFillMode, 0, 6);

      pp->onAfterRenderObservable.notifyObservers(effect.get());
    }
  }

  // Restore states
  engine->setDepthBuffer(true);
  engine->setDepthWrite(true);
  engine->setAlphaMode(Constants::ALPHA_DISABLE);
}

void PostProcessManager::dispose()
{
  if (stl_util::contains(_vertexBuffers, VertexBuffer::PositionKind)) {
    auto& buffer = _vertexBuffers[VertexBuffer::PositionKind];
    if (buffer) {
      buffer->dispose();
      _vertexBuffers[VertexBuffer::PositionKind] = nullptr;
      _vertexBuffers.erase(VertexBuffer::PositionKind);
    }
  }

  if (_indexBuffer) {
    _scene->getEngine()->_releaseBuffer(_indexBuffer);
    _indexBuffer = nullptr;
  }
}

} // end of namespace BABYLON
