#include <babylon/postprocess/post_process_manager.h>

#include <babylon/cameras/camera.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/postprocess/post_process.h>

namespace BABYLON {

PostProcessManager::PostProcessManager(Scene* scene) : _scene{scene}
{
}

PostProcessManager::~PostProcessManager()
{
}

void PostProcessManager::_prepareBuffers()
{
  if (_vertexBuffer) {
    return;
  }

  // VBO
  Float32Array vertices = {1.f, 1.f, -1.f, 1.f, -1.f, -1.f, 1.f, -1.f};
  _vertexBuffer         = _scene->getEngine()->createVertexBuffer(vertices);

  // Indices
  Uint32Array indices = {0, 1, 2, 0, 2, 3};
  _indexBuffer        = _scene->getEngine()->createIndexBuffer(indices);
}

bool PostProcessManager::_prepareFrame(GL::IGLTexture* sourceTexture)
{
  const std::vector<PostProcess*>& postProcesses
    = _scene->activeCamera->_postProcesses;
  const Uint32Array& postProcessesTakenIndices
    = _scene->activeCamera->_postProcessesTakenIndices;

  if (postProcessesTakenIndices.empty() || !_scene->postProcessesEnabled) {
    return false;
  }

  postProcesses[_scene->activeCamera->_postProcessesTakenIndices[0]]->activate(
    _scene->activeCamera, sourceTexture);

  return true;
}

void PostProcessManager::directRender(
  const std::vector<PostProcess*>& postProcesses, GL::IGLTexture* targetTexture)
{
  Engine* engine = _scene->getEngine();

  for (unsigned int index = 0; index < postProcesses.size(); ++index) {
    if (index < postProcesses.size() - 1) {
      postProcesses[index + 1]->activate(_scene->activeCamera, targetTexture);
    }
    else {
      if (targetTexture) {
        engine->bindFramebuffer(targetTexture);
      }
      else {
        engine->restoreDefaultFramebuffer();
      }
    }

    PostProcess* pp = postProcesses[index];
    Effect* effect  = pp->apply();

    if (effect) {
      // if (pp->onBeforeRender) {
      //  pp->onBeforeRender(effect);
      //}

      // VBOs
      _prepareBuffers();
      engine->bindBuffersDirectly(_vertexBuffer.get(), _indexBuffer.get(),
                                  _vertexDeclaration, _vertexStrideSize,
                                  effect);

      // Draw order
      engine->draw(true, 0, 6);

      // if (pp->onAfterRender) {
      //  pp->onAfterRender(effect);
      //}
    }
  }

  // Restore depth buffer
  engine->setDepthBuffer(true);
  engine->setDepthWrite(true);
}

void PostProcessManager::_finalizeFrame(
  bool doNotPresent, GL::IGLTexture* targetTexture, int faceIndex,
  const std::vector<PostProcess*>& _postProcesses)
{
  const std::vector<PostProcess*>& postProcesses
    = (!_postProcesses.empty()) ? _postProcesses :
                                  _scene->activeCamera->_postProcesses;
  const Uint32Array& postProcessesTakenIndices
    = _scene->activeCamera->_postProcessesTakenIndices;
  if (postProcessesTakenIndices.empty() || !_scene->postProcessesEnabled) {
    return;
  }
  Engine* engine = _scene->getEngine();

  for (unsigned int index = 0; index < postProcessesTakenIndices.size();
       ++index) {
    if (index < postProcessesTakenIndices.size() - 1) {
      postProcesses[postProcessesTakenIndices[index + 1]]->activate(
        _scene->activeCamera);
    }
    else {
      if (targetTexture) {
        engine->bindFramebuffer(targetTexture, faceIndex);
      }
      else {
        engine->restoreDefaultFramebuffer();
      }
    }

    if (doNotPresent) {
      break;
    }

    PostProcess* pp = postProcesses[postProcessesTakenIndices[index]];
    Effect* effect  = pp->apply();

    if (effect) {
      // if (pp->onBeforeRender) {
      //  pp->onBeforeRender(effect);
      //}

      // VBOs
      _prepareBuffers();
      engine->bindBuffersDirectly(_vertexBuffer.get(), _indexBuffer.get(),
                                  _vertexDeclaration, _vertexStrideSize,
                                  effect);

      // Draw order
      engine->draw(true, 0, 6);

      // if (pp->onAfterRender) {
      //  pp->onAfterRender(effect);
      //}
    }
  }

  // Restore depth buffer
  engine->setDepthBuffer(true);
  engine->setDepthWrite(true);
}

void PostProcessManager::dispose(bool /*doNotRecurse*/)
{
  if (_vertexBuffer) {
    _scene->getEngine()->_releaseBuffer(_vertexBuffer.get());
    _vertexBuffer.reset(nullptr);
  }

  if (_indexBuffer) {
    _scene->getEngine()->_releaseBuffer(_indexBuffer.get());
    _indexBuffer.reset(nullptr);
  }
}

} // end of namespace BABYLON
