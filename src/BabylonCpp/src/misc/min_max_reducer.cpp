#include <babylon/misc/min_max_reducer.h>

#include <babylon/cameras/camera.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/postprocesses/post_process.h>
#include <babylon/postprocesses/post_process_manager.h>

namespace BABYLON {

MinMaxReducer::MinMaxReducer(const CameraPtr& camera)
    : refreshRate{this, &MinMaxReducer::get_refreshRate, &MinMaxReducer::set_refreshRate}
    , activated{this, &MinMaxReducer::get_activated}
    , _sourceTexture{nullptr}
    , _postProcessManager{nullptr}
    , _onAfterUnbindObserver{nullptr}
    , _forceFullscreenViewport{true}
    , _activated{false}
{
  _camera             = camera;
  _postProcessManager = std::make_unique<PostProcessManager>(camera->getScene());
}

MinMaxReducer::~MinMaxReducer() = default;

RenderTargetTexturePtr& MinMaxReducer::get_sourceTexture()
{
  return _sourceTexture;
}

void MinMaxReducer::setSourceTexture(const RenderTargetTexturePtr& sourceTexture, bool depthRedux,
                                     unsigned int type, bool forceFullscreenViewport)
{
  if (sourceTexture == _sourceTexture) {
    return;
  }

  dispose(false);

  _sourceTexture           = sourceTexture;
  _reductionSteps          = {};
  _forceFullscreenViewport = forceFullscreenViewport;

  auto scene = _camera->getScene();

  // create the first step
  auto reductionInitial = PostProcess::New(
    "Initial reduction phase",                                                  // name
    "minmaxRedux",                                                              // shader
    {"texSize"},                                                                // parameters
    {"sourceTexture"},                                                          // textures
    1.f,                                                                        // options
    nullptr,                                                                    // camera
    Constants::TEXTURE_NEAREST_NEAREST,                                         // sampling
    scene->getEngine(),                                                         // engine
    false,                                                                      // reusable
    "#define INITIAL" + std::string(depthRedux ? "\n#define DEPTH_REDUX" : ""), // defines
    type,                                                                       // textureType
    "",                                                                         // vertexUrl
    {},                                                                         // indexParameters
    false,                                                                      // blockCompilation
    Constants::TEXTUREFORMAT_RG                                                 // textureFormat
  );

  reductionInitial->autoClear               = false;
  reductionInitial->forceFullscreenViewport = forceFullscreenViewport;

  auto w = static_cast<float>(_sourceTexture->getRenderWidth()),
       h = static_cast<float>(_sourceTexture->getRenderHeight());

  reductionInitial->onApply = [this, w, h](Effect* effect, EventState& /*es*/) -> void {
    effect->setTexture("sourceTexture", _sourceTexture);
    effect->setFloat2("texSize", w, h);
  };

  _reductionSteps.emplace_back(reductionInitial);

  auto index = 1u;

  // create the additional steps
  while (w > 1 || h > 1) {
    w = std::max(std::round(w / 2.f), 1.f);
    h = std::max(std::round(h / 2.f), 1.f);

    const auto _w = static_cast<int>(w);
    const auto _h = static_cast<int>(h);

    auto reduction = PostProcess::New("Reduction phase " + std::to_string(index), // name
                                      "minmaxRedux",                              // shader
                                      {"texSize"},                                // parameters
                                      {},                                         // textures
                                      PostProcessOptions{_w, _h},                 // options
                                      nullptr,                                    // camera
                                      Constants::TEXTURE_NEAREST_NEAREST,         // sampling
                                      scene->getEngine(),                         // engine
                                      false,                                      // reusable
                                      "#define "
                                        + std::string((_w == 1 && _h == 1) ? "LAST" :
                                                      (_w == 1 || _h == 1) ? "ONEBEFORELAST" :
                                                                             "MAIN"), // defines
                                      type,                                           // textureType
                                      "",                                             // vertexUrl
                                      {},                         // indexParameters
                                      false,                      // blockCompilation
                                      Constants::TEXTUREFORMAT_RG // textureFormat
    );

    reduction->autoClear               = false;
    reduction->forceFullscreenViewport = forceFullscreenViewport;

    reduction->onApply = [_w, _h](Effect* effect, EventState& /*es*/) -> void {
      if (_w == 1 || _h == 1) {
        effect->setInt2("texSize", _w, _h);
      }
      else {
        effect->setFloat2("texSize", static_cast<float>(_w), static_cast<float>(_h));
      }
    };

    _reductionSteps.emplace_back(reduction);

    ++index;

    if (_w == 1 && _h == 1) {
      const auto func = [this, scene](int w, int h, const PostProcessPtr& reduction) {
        const auto func
          = [this, scene, reduction, w, h](Effect* /*effect*/, EventState /*es*/) -> void {
          Float32Array buffer(4 * static_cast<size_t>(w * h));
          MinMaxFloats minmax{0.f, 0.f};
          scene->getEngine()->_readTexturePixels(reduction->inputTexture, w, h, -1, 0, buffer,
                                                 false);
          minmax.min = buffer[0];
          minmax.max = buffer[1];
          onAfterReductionPerformed.notifyObservers(&minmax);
        };
        return func;
      };
      reduction->onAfterRenderObservable.add(func(_w, _h, reduction));
    }
  }
}

int MinMaxReducer::get_refreshRate() const
{
  return _sourceTexture ? _sourceTexture->refreshRate() : -1;
}

void MinMaxReducer::set_refreshRate(int value)
{
  if (_sourceTexture) {
    _sourceTexture->refreshRate = value;
  }
}

bool MinMaxReducer::get_activated() const
{
  return _activated;
}

void MinMaxReducer::activate()
{
  if (_onAfterUnbindObserver || !_sourceTexture) {
    return;
  }

  _onAfterUnbindObserver = _sourceTexture->onAfterUnbindObservable.add(
    [this](RenderTargetTexture* /*texture*/, EventState& /*es*/) -> void {
      if (_reductionSteps.empty() || !_reductionSteps[0] || !_postProcessManager || !_camera) {
        return;
      }

      const auto engine = _camera->getScene()->getEngine();
      engine->_debugPushGroup("min max reduction", 1);
      _reductionSteps[0]->activate(_camera);
      _postProcessManager->directRender(_reductionSteps, _reductionSteps[0]->inputTexture(),
                                        _forceFullscreenViewport);
      _camera->getScene()->getEngine()->unBindFramebuffer(_reductionSteps[0]->inputTexture(),
                                                          false);
      engine->unBindFramebuffer(_reductionSteps[0]->inputTexture(), false);
      engine->_debugPopGroup(1);
    });

  _activated = true;
}

void MinMaxReducer::deactivate()
{
  if (!_onAfterUnbindObserver || !_sourceTexture) {
    return;
  }

  _sourceTexture->onAfterUnbindObservable.remove(_onAfterUnbindObserver);
  _onAfterUnbindObserver = nullptr;
  _activated             = false;
}

void MinMaxReducer::dispose(bool disposeAll)
{
  if (disposeAll) {
    onAfterReductionPerformed.clear();
  }

  deactivate();

  if (!_reductionSteps.empty()) {
    for (const auto& reductionStep : _reductionSteps) {
      reductionStep->dispose();
    }
    _reductionSteps.clear();
  }

  if (_postProcessManager && disposeAll) {
    _postProcessManager->dispose();
  }

  _sourceTexture = nullptr;
}

} // end of namespace BABYLON
