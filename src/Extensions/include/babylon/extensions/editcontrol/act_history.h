#ifndef BABYLON_EXTENSIONS_EDIT_CONTROL_ACT_HISTORY_H
#define BABYLON_EXTENSIONS_EDIT_CONTROL_ACT_HISTORY_H

#include <babylon/babylon_api.h>
#include <babylon/extensions/editcontrol/act.h>

namespace BABYLON {
namespace Extensions {

class ActHist {

public:
  ActHist(const AbstractMeshPtr& mesh, int capacity);
  ~ActHist();

  void setCapacity(int c);
  void add();
  void undo();
  void redo();

private:
  AbstractMeshPtr mesh;
  int lastMax;
  std::vector<Act> acts;
  int last;
  int current;

}; // end of class Act

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_EDIT_CONTROL_ACT_HISTORY_H
