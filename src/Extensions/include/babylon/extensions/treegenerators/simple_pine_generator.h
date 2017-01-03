#ifndef BABYLON_EXTENSIONS_TREE_GENERATORS_SIMPLE_PINE_TREE_GENERATOR_H
#define BABYLON_EXTENSIONS_TREE_GENERATORS_SIMPLE_PINE_TREE_GENERATOR_H

#include <babylon/babylon_global.h>

namespace BABYLON {
namespace Extensions {

/**
 * @brief SimplePineGenerator uses a tube to generate a tree.
 */
struct BABYLON_SHARED_EXPORT SimplePineGenerator {

  /**
   * Simple Pine Generator function.
   *
   * @param canopies - number of leaf sections.
   * @param height - height of tree.
   * @param trunkMaterial - material used for trunk.
   * @param leafMaterial - material for canopies.
   * @param scene - BABYLON scene.
   */
  static Mesh* CreateTree(unsigned int canopies, unsigned int height,
                          Material* trunkMaterial, Material* leafMaterial,
                          Scene* scene);

}; // end of class SimplePineGenerator

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_TREE_GENERATORS_SIMPLE_PINE_TREE_GENERATOR_H
