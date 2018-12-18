#include <babylon/postprocess/post_process_manager.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/material.h>
#include <babylon/mesh/vertex_buffer.h>
#include <babylon/postprocess/post_process.h>

namespace BABYLON {

PostProcessManager::PostProcessManager(Scene* scene)
    : _scene{scene}, _indexBuffer{nullptr}
{
}

PostProcessManager::~PostProcessManager()
{
}

void PostProcessManager::_prepareBuffers()
{
  if (stl_util::contains(_vertexBuffers, VertexBuffer::PositionKindChars)
      && _vertexBuffers[VertexBuffer::PositionKindChars]) {
    return;
  }

  // VBO
  Float32Array vertices{
    1.f,  1.f,  //
    -1.f, 1.f,  //
    -1.f, -1.f, //
    1.f,  -1.f  //
  };
  _vertexBuffers[VertexBuffer::PositionKindChars]
    = std::make_unique<VertexBuffer>(_scene->getEngine(), vertices,
                                     VertexBuffer::PositionKind, false, false,
                                     2);
  _vertexBufferPtrs[VertexBuffer::PositionKindChars]
    = _vertexBuffers[VertexBuffer::PositionKindChars].get();

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
  if (!stl_util::contains(_vertexBuffers, VertexBuffer::PositionKindChars)
      || !_vertexBuffers[VertexBuffer::PositionKindChars]) {
    return;
  }
  _vertexBuffers[VertexBuffer::PositionKindChars]->_rebuild();
  _buildIndexBuffer();
}

bool PostProcessManager::_prepareFrame(
  const InternalTexturePtr& sourceTexture,
  const std::vector<PostProcessPtr>& iPostProcesses)
{
  auto camera = _scene->activeCamera;
  if (!camera) {
    return false;
  }

  auto postProcesses
    = !iPostProcesses.empty() ? iPostProcesses : camera->_postProcesses;

  // Filter out all null elements
  postProcesses.erase(
    std::remove_if(postProcesses.begin(), postProcesses.end(),
                   [](const PostProcessPtr& pp) { return pp == nullptr; }),
    postProcesses.end());

  if (postProcesses.empty() || !_scene->postProcessesEnabled) {
    return false;
  }

  postProcesses[0]->activate(camera, sourceTexture, !postProcesses.empty());
  return true;
}

void PostProcessManager::directRender(
  const std::vector<PostProcessPtr>& postProcesses,
  const InternalTexturePtr& targetTexture, bool forceFullscreenViewport,
  unsigned int faceIndex, int lodLevel)
{
  auto engine = _scene->getEngine();

  for (unsigned int index = 0; index < postProcesses.size(); ++index) {
    if (index < postProcesses.size() - 1) {
      postProcesses[index + 1]->activate(_scene->activeCamera, targetTexture);
    }
    else {
      if (targetTexture) {
        engine->bindFramebuffer(targetTexture, faceIndex, std::nullopt,
                                std::nullopt, forceFullscreenViewport, nullptr,
                                lodLevel);
      }
      else {
        engine->restoreDefaultFramebuffer();
      }
    }

    auto pp     = postProcesses[index];
    auto effect = pp->apply();

    if (effect) {
      pp->onBeforeRenderObservable.notifyObservers(effect.get());

      // VBOs
      _prepareBuffers();
      engine->bindBuffers(_vertexBufferPtrs, _indexBuffer.get(), effect);

      // Draw order
      engine->drawElementsType(Material::TriangleFillMode(), 0, 6);

      pp->onAfterRenderObservable.notifyObservers(effect.get());
    }
  }

  // Restore depth buffer
  engine->setDepthBuffer(true);
  engine->setDepthWrite(true);
}

void PostProcessManager::_finalizeFrame(
  bool doNotPresent, const InternalTexturePtr& targetTexture,
  unsigned int faceIndex, const std::vector<PostProcessPtr>& _postProcesses,
  bool forceFullscreenViewport)
{
  auto camera = _scene->activeCamera;
  if (!camera) {
    return;
  }

  auto postProcesses
    = _postProcesses.empty() ? camera->_postProcesses : _postProcesses;

  // Filter out all null elements
  postProcesses.erase(
    std::remove_if(postProcesses.begin(), postProcesses.end(),
                   [](const PostProcessPtr& pp) { return pp == nullptr; }),
    postProcesses.end());

  if (postProcesses.empty() || !_scene->postProcessesEnabled) {
    return;
  }
  auto engine = _scene->getEngine();

  for (size_t index = 0, len = postProcesses.size(); index < len; ++index) {
    auto& pp = postProcesses[index];
    if (index < len - 1) {
      pp->_outputTexture
        = postProcesses[index + 1]->activate(camera, targetTexture);
    }
    else {
      if (targetTexture) {
        engine->bindFramebuffer(targetTexture, faceIndex, 0, 0,
                                forceFullscreenViewport);
        pp->_outputTexture = targetTexture;
      }
      else {
        engine->restoreDefaultFramebuffer();
        pp->_outputTexture = nullptr;
      }
    }

    if (doNotPresent) {
      break;
    }

    auto effect = pp->apply();

    if (effect) {
      pp->onBeforeRenderObservable.notifyObservers(effect.get());

      // VBOs
      _prepareBuffers();
      engine->bindBuffers(_vertexBufferPtrs, _indexBuffer.get(), effect);

      // Draw order
      engine->drawElementsType(Material::TriangleFillMode(), 0, 6);

      pp->onAfterRenderObservable.notifyObservers(effect.get());
    }
  }

  // Restore states
  engine->setDepthBuffer(true);
  engine->setDepthWrite(true);
  engine->setAlphaMode(EngineConstants::ALPHA_DISABLE);
}

void PostProcessManager::dispose()
{
  if (stl_util::contains(_vertexBuffers, VertexBuffer::PositionKindChars)) {
    auto& buffer = _vertexBuffers[VertexBuffer::PositionKindChars];
    if (buffer) {
      buffer->dispose();
      _vertexBuffers[VertexBuffer::PositionKindChars] = nullptr;
      _vertexBuffers.erase(VertexBuffer::PositionKindChars);
      _vertexBufferPtrs.erase(VertexBuffer::PositionKindChars);
    }
  }

  if (_indexBuffer) {
    _scene->getEngine()->_releaseBuffer(_indexBuffer.get());
    _indexBuffer = nullptr;
  }
}

} // end of namespace BABYLON
