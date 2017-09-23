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

  void add(shared_ptr_t<ICameraInput<TCamera>>&& input);
  void remove(ICameraInput<TCamera>* inputToRemove);
  void removeByType(const string_t& inputType);
  ::std::function<void()> _addCheckInputs(const ::std::function<void()>& fn);
  void attachInput(ICameraInput<TCamera>* input);
  void attachElement(ICanvas* canvas, bool _noPreventDefault);
  void detachElement(ICanvas* canvas, bool disconnect = false);
  void rebuildInputCheck();
  void clear();
  Json::object serialize(Json::object& serializedCamera) const;
  void parse(const Json::value& parsedCamera);

public:
  ICanvas* attachedElement;
  bool noPreventDefault;
  TCamera* camera;
  ::std::function<void()> checkInputs;

private:
  unordered_map_t<string_t, shared_ptr_t<ICameraInput<TCamera>>> attached;

}; // end of class CameraInputsManager<TCamera>

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_CAMERA_INPUTS_MANAGER_H
