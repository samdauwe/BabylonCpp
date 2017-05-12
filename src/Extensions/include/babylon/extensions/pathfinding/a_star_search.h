#ifndef BABYLON_EXTENSIONS_PATH_FINDING_A_STAR_SEARCH_H
#define BABYLON_EXTENSIONS_PATH_FINDING_A_STAR_SEARCH_H

#include <babylon/babylon_global.h>

namespace BABYLON {
namespace Extensions {

template <typename T, typename priority_t>
struct PriorityQueue {
  typedef std::pair<priority_t, T> PQElement;
  std::priority_queue<PQElement, std::vector<PQElement>,
                      std::greater<PQElement>>
    elements;

  inline bool empty() const
  {
    return elements.empty();
  }

  inline void put(T item, priority_t priority)
  {
    elements.emplace(priority, item);
  }

  inline T get()
  {
    T best_item = elements.top().second;
    elements.pop();
    return best_item;
  }
}; // end of struct PriorityQueue

template <typename Graph>
std::vector<typename Graph::Node> AStarSearch(Graph& graph,
                                              typename Graph::Node& start,
                                              typename Graph::Node& goal)
{
  typedef typename Graph::Node Node;
  typedef typename Graph::NodeId NodeId;
  std::vector<Node> path;
  PriorityQueue<NodeId, double> frontier;
  frontier.put(start.id, 0);

  start.parentId  = start.id;
  start.costSoFar = 0.0;

  while (!frontier.empty()) {
    auto& current = graph(frontier.get());

    if (current == goal) {
      auto curr = current;
      while (curr.parentId != start.id) {
        path.emplace_back(curr);
        curr = graph(curr.parentId);
      }
      path.emplace_back(curr);
      path.emplace_back(start);
      std::reverse(path.begin(), path.end());
      break;
    }

    for (auto&& next : graph.neighbors(current)) {
      const auto newCost = current.costSoFar + graph.cost(current, next);
      if (!next.visited || newCost < next.costSoFar) {
        graph(next.id).visited   = true;
        graph(next.id).costSoFar = newCost;
        const auto priority      = newCost + graph.heuristic(next, goal);
        frontier.put(next.id, priority);
        graph(next.id).parentId = current.id;
      }
    }
  }
  return path;
}

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_PATH_FINDING_A_STAR_SEARCH_H
