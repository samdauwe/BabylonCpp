#ifndef BABYLON_EXTENSIONS_TREE_GENERATORS_SPS_TREE_GENERATOR_H
#define BABYLON_EXTENSIONS_TREE_GENERATORS_SPS_TREE_GENERATOR_H

#include <babylon/babylon_global.h>
#include <babylon/math/vector3.h>

namespace BABYLON {
namespace Extensions {

struct CoordSystem {
  Vector3 x;
  Vector3 y;
  Vector3 z;
}; // end of CoordSystem

struct SPSTreeBranch {
  Mesh* branch;
  std::vector<Vector3> core;
  Float32Array _radii;
}; // end of SPSTreeBranch

struct SPSTreeBase {
  Mesh* tree;
  std::vector<std::vector<Vector3>> paths;
  std::vector<Float32Array> radii;
  std::vector<CoordSystem> directions;
}; // end of SPSTreeBase

/**
 * @brief SPSTreeGenerator uses the solid particle system to generate a tree. A
 * base tree is created consisting of a trunk which forks into branches, which
 * then themselves may fork or not. This base tree is used in two different
 * ways.
 *   1. as the trunk and parent branches for the whole tree.
 *   2. with leaves added as a mini-tree that is added a number of times to the
 *      base trunk and parent branches to form the whole tree.
 */
struct BABYLON_SHARED_EXPORT SPSTreeGenerator {

  /**
   * @brief SPS Tree Generator function.
   *
   * @param trunkHeight - height of trunk of tree. The initial trunk radius is 1
   * unit.
   * @param trunkTaper - fraction of starting radius for the end radius of a
   * branch between 0 and 1.
   * @param trunkSlices - the number of points on the paths used for the ribbon
   * mesh that forms the branch.
   * @param trunk material - the material used on all branches.
   * @param boughs - the number of times the tree will split into forked
   * branches, 1 trunk splits into branches, 2 these branches also spilt into
   * branches.
   * @param forks - the number of branches a branch can split into, 5 or more
   * really slows the generation.
   * @param forkAngle - the angle a forked branch makes with its parent branch
   * measured from the direction of the branch.
   * @param forkRatio - the ratio of the length of a branch to its parent's
   * length, between 0 and 1.
   * @param branches - the number of mini-trees that are randomally added to the
   * tree.
   * @param branchAngle - the angle the mini-tree makes with its parent branch
   * from the direction of the branch.
   * @param bowFreq - the number of bows (bends) in a branch. A trunk is set to
   * have only one bow.
   * @param bowHeight - the height of a bow from the line of direction of the
   * branch.
   * @param leavesOnBranch - the number of leaves to be added to one side of a
   * branch.
   * @param leafWHRatio - width to height ratio for a leaf between 0 and 1,
   * closer to 0 longer leaf, closer to 1 more circular.
   * @param leafMaterial - material used for all leaves.
   * @param scene - BABYLON scene.
   */
  static Mesh* CreateTree(float trunkHeight, float trunkTaper,
                          size_t trunkSlices, Material* trunkMaterial,
                          unsigned int boughs, unsigned int forks,
                          float forkAngle, float forkRatio,
                          unsigned int branches, float branchAngle,
                          unsigned int bowFreq, float bowHeight,
                          unsigned int leavesOnBranch, float leafWHRatio,
                          Material* leafMaterial, Scene* scene);

  /**
   * @brief Creates an x, y, z coordinate system using the parameter vec3 for
   * the y axis.
   *
   * @param vec3 non-zero Vector3
   * @return x, y, z coordinate system
   */
  static CoordSystem coordSystem(Vector3& vec3);

  /**
   * @brief Randomizes a value v +/- p*100% of v.
   *
   * @param v
   * @param p
   * @return
   */
  static float randPct(float v, float p);

  /**
   * @brief Creates a single branch of the tree starting from branchAt (Vector3)
   * using the coordinate system branchSys. The branch is in the direction of
   * branchSys.y with the cross section of the  *branch in the branchSys.x and
   * branchSys.z plane. The branch starts with radius branchRadius and tapers to
   * branchRadius*branchTaper over its length branchLength. The parammeter
   * branchSlices gives the number of cross sectional slices that the branch is
   * divided into the number of bows (bends) in the branch is given by bowFreq
   * and the height of the bow by bowHeight.
   *
   * @param branchAt
   * @param branchSys
   * @param branchLength
   * @param branchTaper
   * @param branchSlices
   * @param bowFreq
   * @param bowHeight
   * @param branchRadius
   * @param scene
   * @return
   */
  static SPSTreeBranch createBranch(const Vector3& branchAt,
                                    const CoordSystem& branchSys,
                                    float branchLength, float branchTaper,
                                    size_t branchSlices, size_t bowFreq,
                                    float bowHeight, float branchRadius,
                                    Scene* scene);
  /**
   * @brief Creates a trunk and some branches. This is used as the base of the
   * tree and as the base mesh for an SPS mesh to give additional branches. When
   * boughs = 1 the trunk is created then branches are created at the top of the
   * trunk, the number of branches being given by forks. When boughs = 2 the
   * trunk and branches are created as for boughs = 1, then further branches are
   * created at the end of each of these branches. The parameter forkRatio gives
   * the fraction of branch length as branches are added to branches. The angle
   * of a new branch to the one it is growing from is given by forkAngle.
   *
   * @param trunkHeight
   * @param trunkTaper
   * @param trunkSlices
   * @param boughs
   * @param forks
   * @param forkAngle
   * @param forkRatio
   * @param bowFreq
   * @param bowHeight
   * @param scene
   * @return
   */
  static SPSTreeBase createTreeBase(float trunkHeight, float trunkTaper,
                                    size_t trunkSlices, unsigned int boughs,
                                    unsigned int forks, float forkAngle,
                                    float forkRatio, size_t bowFreq,
                                    float bowHeight, Scene* scene);

}; // end of struct SimplePineGenerator

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_TREE_GENERATORS_SPS_TREE_GENERATOR_H
