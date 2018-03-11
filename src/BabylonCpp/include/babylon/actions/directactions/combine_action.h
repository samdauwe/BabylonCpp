#ifndef BABYLON_ACTIONS_DIRECTACTIONS_COMBINE_ACTION_H
#define BABYLON_ACTIONS_DIRECTACTIONS_COMBINE_ACTION_H

#include <babylon/actions/action.h>
#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT CombineAction : public Action {

public:
  CombineAction(unsigned int triggerOptions, const vector_t<Action*>& children,
                Condition* condition = nullptr);
  ~CombineAction() override;

  void _prepare() override;
  void execute(const ActionEvent& evt) override;
  Json::object serialize(Json::object& parent) const override;

public:
  vector_t<Action*> children;

}; // end of class CombineAction

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_DIRECTACTIONS_COMBINE_ACTION_H
