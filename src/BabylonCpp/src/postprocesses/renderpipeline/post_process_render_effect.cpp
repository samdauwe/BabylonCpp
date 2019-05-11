#include <babylon/postprocesses/renderpipeline/post_process_render_effect.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/postprocesses/post_process.h>

namespace BABYLON {

PostProcessRenderEffect::PostProcessRenderEffect(
  Engine* engine, const std::string& name,
  const std::function<std::vector<PostProcessPtr>()>& getPostProcesses,
  bool singleInstance)
    : _name{name}
    , _engine{engine}
    , isSupported{this, &PostProcessRenderEffect::get_isSupported}
    , _getPostProcesses{getPostProcesses}
    , _singleInstance{singleInstance}
{
}

PostProcessRenderEffect::~PostProcessRenderEffect()
{
}

bool PostProcessRenderEffect::get_isSupported() const
{
  for (auto& item : _postProcesses) {
    for (auto& pps : item.second) {
      if (!pps->isSupported()) {
        return false;
      }
    }
  }
  return true;
}

void PostProcessRenderEffect::_update()
{
}

void PostProcessRenderEffect::_attachCameras(
  const std::vector<CameraPtr>& cameras)
{
  std::string cameraKey;

  auto cams = cameras.empty() ? stl_util::extract_values(_cameras) : cameras;

  if (cams.empty()) {
    return;
  }

  for (auto& camera : cams) {
    const auto& cameraName = camera->name;

    if (_singleInstance) {
      cameraKey = "0";
    }
    else {
      cameraKey = cameraName;
    }

    if (!stl_util::contains(_postProcesses, cameraKey)
        || _postProcesses[cameraKey].empty()) {
      auto postProcess = _getPostProcesses();
      if (!postProcess.empty()) {
        _postProcesses[cameraKey] = postProcess;
      }
    }

    if (!stl_util::contains(_indicesForCamera, cameraName)) {
      _indicesForCamera[cameraName].clear();
    }

    for (auto& postProcess : _postProcesses[cameraKey]) {
      auto index = camera->attachPostProcess(postProcess);

      _indicesForCamera[cameraName].emplace_back(index);
    }

    if (!stl_util::contains(_cameras, cameraName)) {
      _cameras[cameraName] = camera;
    }
  }
}

void PostProcessRenderEffect::_detachCameras(
  const std::vector<CameraPtr>& cameras)
{
  auto cams = cameras.empty() ? stl_util::extract_values(_cameras) : cameras;

  if (cams.empty()) {
    return;
  }

  for (auto& camera : cams) {
    auto cameraName = camera->name;

    const auto cameraKey = _singleInstance ? "0" : cameraName;
    for (auto& postProcess : _postProcesses[cameraKey]) {
      camera->detachPostProcess(postProcess);
    }

    if (stl_util::contains(_cameras, cameraName)) {
      // _indicesForCamera.erase(cameraName);
      _cameras.erase(cameraName);
    }
  }
}

void PostProcessRenderEffect::_enable(const std::vector<CameraPtr>& cameras)
{
  auto cams = cameras.empty() ? stl_util::extract_values(_cameras) : cameras;

  if (cams.empty()) {
    return;
  }

  for (auto& camera : cams) {
    auto cameraName = camera->name;

    for (auto& j : _indicesForCamera[cameraName]) {
      auto index = _indicesForCamera[cameraName][j];
      if (index >= camera->_postProcesses.size()
          || camera->_postProcesses[index] == nullptr) {
        const auto cameraKey = _singleInstance ? "0" : cameraName;
        for (auto& postProcess : _postProcesses[cameraKey]) {
          camera->attachPostProcess(
            postProcess, static_cast<int>(_indicesForCamera[cameraName][j]));
        }
      }
    }
  }
}

void PostProcessRenderEffect::_disable(const std::vector<CameraPtr>& cameras)
{
  auto cams = cameras.empty() ? stl_util::extract_values(_cameras) : cameras;

  if (cams.empty()) {
    return;
  }

  for (auto& camera : cams) {
    auto cameraName      = camera->name;
    const auto cameraKey = _singleInstance ? "0" : cameraName;
    for (auto& postProcess : _postProcesses[cameraKey]) {
      camera->detachPostProcess(postProcess);
    }
  }
}

std::vector<PostProcessPtr>
PostProcessRenderEffect::getPostProcesses(Camera* camera)
{
  if (_singleInstance) {
    return _postProcesses["0"];
  }
  else {
    if (!camera) {
      return {};
    }
    return _postProcesses[camera->name];
  }
}

} // end of namespace BABYLON
