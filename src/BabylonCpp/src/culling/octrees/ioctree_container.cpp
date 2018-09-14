#include <babylon/culling/octrees/ioctree_container.h>

#include <babylon/culling/octrees/octree_block.h>

namespace BABYLON {

class AbstractMesh;
class SubMesh;

template struct IOctreeContainer<AbstractMesh*>;
template struct IOctreeContainer<SubMesh*>;

} // end of namespace BABYLON
