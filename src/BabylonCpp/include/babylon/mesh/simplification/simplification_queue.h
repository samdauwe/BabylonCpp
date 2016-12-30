#ifndef BABYLON_MESH_SIMPLIFICATION_SIMPLIFICATION_QUEUE_H
#define BABYLON_MESH_SIMPLIFICATION_SIMPLIFICATION_QUEUE_H

#include <babylon/babylon_global.h>
#include <babylon/mesh/simplification/isimplification_task.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT SimplificationQueue {

public:
  SimplificationQueue();
  ~SimplificationQueue();

  void addTask(const ISimplificationTask& task);
  void executeNext();
  void runSimplification(const ISimplificationTask& task);

private:
  ISimplifier* getSimplifier(const ISimplificationTask& task);

public:
  bool running;

private:
  std::queue<ISimplificationTask> _simplificationQueue;

}; // end of class SimplificationQueue

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_SIMPLIFICATION_SIMPLIFICATION_QUEUE_H
