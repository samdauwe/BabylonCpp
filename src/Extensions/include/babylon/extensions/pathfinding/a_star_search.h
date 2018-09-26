#ifndef BABYLON_EXTENSIONS_PATH_FINDING_A_STAR_SEARCH_H
#define BABYLON_EXTENSIONS_PATH_FINDING_A_STAR_SEARCH_H

#include <algorithm>
#include <queue>
#include <unordered_map>
#include <utility>

#include <babylon/babylon_api.h>

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

template <typename NodeId>
struct AStarNode {
  // Id of the node it can most efficiently be reached from
  NodeId cameFrom;
  // The cost of getting from the start node to that node
  double gScore;
  // The total cost of getting from the start node to the goal by passing by
  // that node. That value is partly known, partly heuristic
  double fScore;
  // Whether or not the node has already been visited
  bool visited;
}; // end of struct

template <typename Graph>
std::vector<typename Graph::NodeId> AStarSearch(Graph& graph,
                                                typename Graph::Node& start,
                                                typename Graph::Node& goal)
{
  typedef typename Graph::NodeId NodeId;
  std::vector<NodeId> path;
  PriorityQueue<NodeId, double> frontier;
  frontier.put(start.id, 0);

  std::unordered_map<NodeId, AStarNode<NodeId>> aStarNodes(graph.size());
  aStarNodes[start.id] = AStarNode<NodeId>{
    start.id,                                 // cameFrom
    0.0,                                      // gScore
    graph.heuristicCostEstimate(start, goal), // fScore
    true                                      // visited
  };

  while (!frontier.empty()) {
    // The node in frontier having the lowest fScore
    auto current = frontier.get();

    if (current == goal.id) {
      while (aStarNodes[current].cameFrom != start.id) {
        path.emplace_back(current);
        current = aStarNodes[current].cameFrom;
      }
      path.emplace_back(current);
      path.emplace_back(start.id);
      std::reverse(path.begin(), path.end());
      break;
    }

    for (auto&& next : graph.neighbors(current)) {
      // The distance from start to a neighbor
      const auto tentative_gScore
        = aStarNodes[current].gScore + graph.cost(current, next);
      if (!aStarNodes.count(next.id)) {
        aStarNodes[next.id] = AStarNode<NodeId>{0, 0.0, 0.0, false};
      }
      auto& neighbor = aStarNodes[next.id];
      if (!neighbor.visited || tentative_gScore < neighbor.gScore) {
        neighbor.visited  = true;
        neighbor.cameFrom = current;
        neighbor.gScore   = tentative_gScore;
        neighbor.fScore
          = tentative_gScore + graph.heuristicCostEstimate(next, goal);
        frontier.put(next.id, neighbor.fScore);
      }
    }
  }
  return path;
}

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_PATH_FINDING_A_STAR_SEARCH_H
