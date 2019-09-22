#ifndef BABYLON_MATERIALS_NODE_BLOCKS_INPUT_ANIMATED_INPUT_BLOCK_TYPES_H
#define BABYLON_MATERIALS_NODE_BLOCKS_INPUT_ANIMATED_INPUT_BLOCK_TYPES_H

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Enum defining the type of animations supported by InputBlock.
 */
enum class BABYLON_SHARED_EXPORT AnimatedInputBlockTypes : unsigned int {
  /** No animation */
  None,
  /** Time based animation. Will only work for floats */
  Time
}; // end of enum class AnimatedInputBlockTypes

} // end of namespace BABYLON

#endif // end of
       // BABYLON_MATERIALS_NODE_BLOCKS_INPUT_ANIMATED_INPUT_BLOCK_TYPES_H
