#ifndef BABYLON_EXTENSIONS_MANIPULATION_HELPERS_SIMPLE_INTERACTION_HELPER_H
#define BABYLON_EXTENSIONS_MANIPULATION_HELPERS_SIMPLE_INTERACTION_HELPER_H

#include <babylon/babylon_api.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

class Node;
class PointerInfo;
class Scene;
using NodePtr = std::shared_ptr<Node>;

namespace Extensions {

class ManipulatorInteractionHelper;

/**
 * @brief The purpose of this class is to allow the camera manipulation, single
 * node selection and manipulation. You can use it as an example to create your
 * more complexe/different interaction helper.
 */
class BABYLON_SHARED_EXPORT SimpleInteractionHelper {

public:
  enum class SIHCurrentAction : int {
    None      = 0,
    Selector  = 1,
    Camerator = 2,
  }; // end of enum class SIHCurrentAction

public:
  SimpleInteractionHelper(Scene* scene);
  ~SimpleInteractionHelper();

  SIHCurrentAction currentAction() const;
  ManipulatorInteractionHelper* manipulator();

private:
  void pointerCallback(const PointerInfo& p, EventState& s);
  void doSelectorInteraction(const PointerInfo& p, EventState& s);
  void detectActionChanged(const PointerInfo& p, EventState& s);

private:
  static float CameratorSwitchThreshold;

private:
  Node* _pickedNode;
  std::vector<SIHCurrentAction> _actionStack;
  Scene* _scene;
  Observer<PointerInfo>::Ptr _pointerObserver;
  std::unique_ptr<ManipulatorInteractionHelper> _manipulator;

}; // end of class SimpleInteractionHelper

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of
       // BABYLON_EXTENSIONS_MANIPULATION_HELPERS_SIMPLE_INTERACTION_HELPER_H
