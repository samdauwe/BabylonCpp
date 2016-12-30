#ifndef BABYLON_CAMERAS_CAMERA_INPUTS_MANAGER_H
#define BABYLON_CAMERAS_CAMERA_INPUTS_MANAGER_H

#include <babylon/babylon_global.h>
#include <babylon/cameras/icamera_input.h>

namespace BABYLON {

template <class TCamera>
class BABYLON_SHARED_EXPORT CameraInputsManager {

public:
  CameraInputsManager(TCamera* camera);
  ~CameraInputsManager();

  void add(std::unique_ptr<ICameraInput<TCamera>>&& input);
  void remove(ICameraInput<TCamera>* inputToRemove);
  void removeByType(const std::string& inputType);
  std::function<void()> _addCheckInputs(const std::function<void()>& fn);
  void attachInput(ICameraInput<TCamera>* input);
  void attachElement(ICanvas* canvas, bool _noPreventDefault);
  void detachElement(ICanvas* canvas);
  void rebuildInputCheck();
  void clear();
  Json::object serialize(Json::object& serializedCamera) const;
  void parse(const Json::value& parsedCamera);

public:
  ICanvas* attachedElement;
  bool noPreventDefault;
  TCamera* camera;
  std::function<void()> checkInputs;

private:
  std::unordered_map<std::string, std::unique_ptr<ICameraInput<TCamera>>>
    attached;

}; // end of class CameraInputsManager<TCamera>

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_CAMERA_INPUTS_MANAGER_H
