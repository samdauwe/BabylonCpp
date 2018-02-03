#include <babylon/mesh/simplification/simplification_queue.h>

#include <babylon/mesh/simplification/simplification_settings.h>

namespace BABYLON {

SimplificationQueue::SimplificationQueue() : running{false}
{
}

SimplificationQueue::~SimplificationQueue()
{
}

void SimplificationQueue::addTask(const ISimplificationTask& task)
{
  _simplificationQueue.emplace(task);
}

void SimplificationQueue::executeNext()
{
  if (!_simplificationQueue.empty()) {
    running                         = true;
    const ISimplificationTask& task = _simplificationQueue.front();
    _simplificationQueue.pop();
    runSimplification(task);
  }
  else {
    running = false;
  }
}

void SimplificationQueue::runSimplification(const ISimplificationTask& /*task*/)
{
}

ISimplifier* SimplificationQueue::getSimplifier(const ISimplificationTask& task)
{
  switch (task.simplificationType) {
    case SimplificationType::QUADRATIC:
    default:
      // TODO FIXME
      return nullptr;
      // return new QuadraticErrorSimplification(task.mesh);
  }
}

} // end of namespace BABYLON
