#ifndef BABYLON_EXTENSIONS_MANIPULATION_HELPERS_SYMBOLIC_VISUAL_HELPER_H
#define BABYLON_EXTENSIONS_MANIPULATION_HELPERS_SYMBOLIC_VISUAL_HELPER_H

#include <babylon/babylon_global.h>

namespace BABYLON {
namespace Extensions {

struct BABYLON_SHARED_EXPORT SymbolicVisualHelper {

    SymbolicVisualHelper();
    virtual ~SymbolicVisualHelper();

    virtual void render();

    bool renderLight;
    bool renderManipulator;

}; // end of class SymbolicVisualHelper

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of
       // BABYLON_EXTENSIONS_MANIPULATION_HELPERS_SYMBOLIC_VISUAL_HELPER_H
