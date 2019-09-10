#ifndef BABYLON_MATERIALS_NODE_OPTIMIZERS_NODE_MATERIAL_OPTIMIZER_H
#define BABYLON_MATERIALS_NODE_OPTIMIZERS_NODE_MATERIAL_OPTIMIZER_H

#include <memory>
#include <vector>

#include <babylon/babylon_api.h>

namespace BABYLON {

class NodeMaterialBlock;
using NodeMaterialBlockPtr = std::shared_ptr<NodeMaterialBlock>;

/**
 * @brief Root class for all node material optimizers.
 */
class BABYLON_SHARED_EXPORT NodeMaterialOptimizer {

  /**
   * @brief Function used to optimize a NodeMaterial graph.
   * @param vertexOutputNodes defines the list of output nodes for the vertex
   * shader
   * @param fragmentOutputNodes defines the list of output nodes for the
   * fragment shader
   */
  virtual void
  optimize(const std::vector<NodeMaterialBlockPtr>& vertexOutputNodes,
           const std::vector<NodeMaterialBlockPtr>& fragmentOutputNodes);

}; // end of class NodeMaterialOptimizer

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_OPTIMIZERS_NODE_MATERIAL_OPTIMIZER_H
