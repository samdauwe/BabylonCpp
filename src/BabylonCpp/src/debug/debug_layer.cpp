#include <babylon/debug/debug_layer.h>

namespace BABYLON {

DebugLayer::DebugLayer(Scene* scene) : _scene{scene}
{
}

DebugLayer::~DebugLayer() = default;

void DebugLayer::hide()
{
}

} // end of namespace BABYLON