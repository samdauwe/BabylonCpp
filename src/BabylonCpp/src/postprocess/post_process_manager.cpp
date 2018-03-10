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
    = ::std::make_unique<VertexBuffer>(_scene->getEngine(), vertices,
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
  if (stl_util::contains(_vertexBuffers, VertexBuffer::PositionKindChars)
      && _vertexBuffers[VertexBuffer::PositionKindChars]) {
    return;
  }
  _vertexBuffers[VertexBuffer::PositionKindChars]->_rebuild();
  _buildIndexBuffer();
}

bool PostProcessManager::_prepareFrame(
  InternalTexture* sourceTexture, const vector_t<PostProcess*>& iPostProcesses)
{
  auto camera = _scene->activeCamera;
  if (!camera) {
    return false;
  }

  const auto& postProcesses
    = !iPostProcesses.empty() ? iPostProcesses : camera->_postProcesses;

  if (postProcesses.empty() || !_scene->postProcessesEnabled) {
    return false;
  }

  postProcesses[0]->activate(camera, sourceTexture, !postProcesses.empty());
  return true;
}

void PostProcessManager::directRender(
  const vector_t<PostProcess*>& postProcesses, InternalTexture* targetTexture,
  bool forceFullscreenViewport)
{
  auto engine = _scene->getEngine();

  for (unsigned int index = 0; index < postProcesses.size(); ++index) {
    if (index < postProcesses.size() - 1) {
      postProcesses[index + 1]->activate(_scene->activeCamera, targetTexture);
    }
    else {
      if (targetTexture) {
        engine->bindFramebuffer(targetTexture, 0, 0, 0,
                                forceFullscreenViewport);
      }
      else {
        engine->restoreDefaultFramebuffer();
      }
    }

    auto pp     = postProcesses[index];
    auto effect = pp->apply();

    if (effect) {
      pp->onBeforeRenderObservable.notifyObservers(effect);

      // VBOs
      _prepareBuffers();
      engine->bindBuffers(_vertexBufferPtrs, _indexBuffer.get(), effect);

      // Draw order
      engine->drawElementsType(Material::TriangleFillMode(), 0, 6);

      pp->onAfterRenderObservable.notifyObservers(effect);
    }
  }

  // Restore depth buffer
  engine->setDepthBuffer(true);
  engine->setDepthWrite(true);
}

void PostProcessManager::_finalizeFrame(
  bool doNotPresent, InternalTexture* targetTexture, unsigned int faceIndex,
  const vector_t<PostProcess*>& _postProcesses, bool forceFullscreenViewport)
{
  auto camera = _scene->activeCamera;
  if (!camera) {
    return;
  }

  const auto& postProcesses
    = _postProcesses.empty() ? camera->_postProcesses : _postProcesses;
  if (postProcesses.empty() || !_scene->postProcessesEnabled) {
    return;
  }
  auto engine = _scene->getEngine();

  for (unsigned int index = 0; index < postProcesses.size(); ++index) {
    if (index < postProcesses.size() - 1) {
      postProcesses[index + 1]->activate(camera, targetTexture);
    }
    else {
      if (targetTexture) {
        engine->bindFramebuffer(targetTexture, faceIndex, 0, 0,
                                forceFullscreenViewport);
      }
      else {
        engine->restoreDefaultFramebuffer();
      }
    }

    if (doNotPresent) {
      break;
    }

    auto pp     = postProcesses[index];
    auto effect = pp->apply();

    if (effect) {
      pp->onBeforeRenderObservable.notifyObservers(effect);

      // VBOs
      _prepareBuffers();
      engine->bindBuffers(_vertexBufferPtrs, _indexBuffer.get(), effect);

      // Draw order
      engine->drawElementsType(Material::TriangleFillMode(), 0, 6);

      pp->onAfterRenderObservable.notifyObservers(effect);
    }
  }

  // Restore states
  engine->setDepthBuffer(true);
  engine->setDepthWrite(true);
  engine->setAlphaMode(EngineConstants::ALPHA_DISABLE);
}

void PostProcessManager::dispose(bool /*doNotRecurse*/)
{
  if (stl_util::contains(_vertexBuffers, VertexBuffer::PositionKindChars)) {
    auto& buffer = _vertexBuffers[VertexBuffer::PositionKindChars];
    if (buffer) {
      buffer->dispose();
      _vertexBuffers[VertexBuffer::PositionKindChars].reset(nullptr);
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
