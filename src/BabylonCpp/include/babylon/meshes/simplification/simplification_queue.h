#ifndef BABYLON_MESHES_SIMPLIFICATION_SIMPLIFICATION_QUEUE_H
#define BABYLON_MESHES_SIMPLIFICATION_SIMPLIFICATION_QUEUE_H

#include <queue>

#include <babylon/babylon_api.h>
#include <babylon/meshes/simplification/isimplification_task.h>

namespace BABYLON {

class ISimplifier;

/**
 * @brief Queue used to order the simplification tasks.
 * @see http://doc.babylonjs.com/how_to/in-browser_mesh_simplification
 */
class BABYLON_SHARED_EXPORT SimplificationQueue {

public:
  /**
   * @brief Creates a new queue.
   */
  SimplificationQueue();
  ~SimplificationQueue(); // = default

  /**
   * @brief Adds a new simplification task.
   * @param task defines a task to add
   */
  void addTask(const ISimplificationTask& task);

  /**
   * @brief Execute next task.
   */
  void executeNext();

  /**
   * @brief Execute a simplification task.
   * @param task defines the task to run
   */
  void runSimplification(const ISimplificationTask& task);

private:
  ISimplifier* getSimplifier(const ISimplificationTask& task);

public:
  /**
   * Gets a boolean indicating that the process is still running
   */
  bool running;

private:
  std::queue<ISimplificationTask> _simplificationQueue;

}; // end of class SimplificationQueue

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_SIMPLIFICATION_SIMPLIFICATION_QUEUE_H
