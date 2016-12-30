#ifndef BABYLON_ACTIONS_DIRECTACTIONS_SET_STATE_ACTION_H
#define BABYLON_ACTIONS_DIRECTACTIONS_SET_STATE_ACTION_H

#include <babylon/actions/action.h>
#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT SetStateAction : public Action {

public:
  SetStateAction(unsigned int triggerOptions, IAnimatable* target,
                 const std::string& value, Condition* condition = nullptr);
  ~SetStateAction();

  void execute(const ActionEvent& evt) override;
  Json::object serialize(Json::object& parent) const override;

private:
  IAnimatable* _target;
  std::string _value;

}; // end of class SetStateAction

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_DIRECTACTIONS_SET_STATE_ACTION_H
