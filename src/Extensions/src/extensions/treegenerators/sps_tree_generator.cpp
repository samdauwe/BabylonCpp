#include <babylon/extensions/treegenerators/sps_tree_generator.h>

#include <babylon/core/random.h>
#include <babylon/math/axis.h>
#include <babylon/mesh/mesh_builder.h>
#include <babylon/mesh/vertex_data_options.h>
#include <babylon/particles/solid_particle.h>
#include <babylon/particles/solid_particle_system.h>

namespace BABYLON {
namespace Extensions {

Mesh* SPSTreeGenerator::CreateTree(
  unsigned int trunkHeight, float trunkTaper, unsigned int trunkSlices,
  Material* trunkMaterial, unsigned int iBoughs, unsigned int forks,
  float forkAngle, float forkRatio, unsigned int branches, float branchAngle,
  unsigned int bowFreq, float bowHeight, unsigned int leavesOnBranch,
  float leafWHRatio, Material* leafMaterial, Scene* scene)
{
  auto boughs = iBoughs;
  if (!(boughs == 1 || boughs == 2)) {
    boughs = 1;
  }
  float _boughs = static_cast<float>(boughs);

  // Create base tree
  auto _base
    = createTreeBase(trunkHeight, trunkTaper, trunkSlices, boughs, forks,
                     forkAngle, forkRatio, bowFreq, bowHeight, scene);
  _base.tree->setMaterial(trunkMaterial);

  // Create one leaf
  float _branch_length = trunkHeight * std::pow(forkRatio, _boughs);
  float _leaf_gap      = _branch_length / (2.f * leavesOnBranch);
  float _leaf_width    = 1.5f * std::pow(trunkTaper, _boughs - 1.f);
  DiscOptions discOptions;
  discOptions.radius          = _leaf_width / 2.f;
  discOptions.tessellation    = 12;
  discOptions.sideOrientation = Mesh::DOUBLESIDE;
  auto _leaf = MeshBuilder::CreateDisc("leaf", discOptions, scene);

  // Create solid particle system for leaves
  SolidParticleSystemOptions spsOptions;
  spsOptions.updatable = false;
  auto _leaves_SPS     = new SolidParticleSystem("leaveSPS", scene, spsOptions);

  // Function to position leaves on base tree
  auto _set_leaves
    = [&](SolidParticle* particle, const Vector3& /*i*/, unsigned int s) {
        unsigned int _a
          = static_cast<unsigned>(std::floor(s / (2.f * leavesOnBranch)));
        if (boughs == 1) {
          ++_a;
        }
        else {
          _a = 2 + _a % forks
               + static_cast<unsigned>(std::floor(_a / forks) * (forks + 1));
        }
        unsigned int _j = s % (2 * leavesOnBranch);
        float _g = (_j * _leaf_gap + 3 * _leaf_gap / 2.f) / _branch_length;

        size_t _upper = static_cast<size_t>(std::ceil(trunkSlices * _g));
        if (_upper > _base.paths[_a].size() - 1) {
          _upper = _base.paths[_a].size() - 1;
        }
        size_t _lower = _upper - 1;
        size_t _gl    = _lower / (trunkSlices - 1);
        size_t _gu    = _upper / (trunkSlices - 1);
        float _px     = _base.paths[_a][_lower].x
                    + (_base.paths[_a][_upper].x - _base.paths[_a][_lower].x)
                        * (_g - _gl) / (_gu - _gl);
        float _py = _base.paths[_a][_lower].y
                    + (_base.paths[_a][_upper].y - _base.paths[_a][_lower].y)
                        * (_g - _gl) / (_gu - _gl);
        float _pz = _base.paths[_a][_lower].z
                    + (_base.paths[_a][_upper].z - _base.paths[_a][_lower].z)
                        * (_g - _gl) / (_gu - _gl);
        particle->position = Vector3(
          _px, _py
                 + (0.6f * _leaf_width / leafWHRatio + _base.radii[_a][_upper])
                     * (2 * (s % 2) - 1),
          _pz);
        particle->rotation.z = Math::random() * Math::PI_4;
        particle->rotation.y = Math::random() * Math::PI_2;
        particle->rotation.z = Math::random() * Math::PI_4;
        particle->scaling.y  = 1.f / leafWHRatio;
      };

  // add leaf mesh _leaf enough for all the final forked branches
  SolidParticleSystemMeshBuilderOptions spsBuilderOptions;
  spsBuilderOptions.positionFunction = _set_leaves;
  _leaves_SPS->addShape(
    _leaf, static_cast<size_t>(2.f * leavesOnBranch
                               * std::pow(static_cast<float>(forks), _boughs)),
    spsBuilderOptions);
  auto _leaves = _leaves_SPS->buildMesh(); // mesh of leaves
  // _leaves->billboard=true;
  _leaf->dispose();

  // Create SPS to use with base tree mesh base.tree
  auto _mini_trees_SPS = new SolidParticleSystem("miniSPS", scene, spsOptions);

  // create SPS to use with leaves mesh
  auto _mini_leaves_SPS
    = new SolidParticleSystem("minileavesSPS", scene, spsOptions);

  // The mini base trees and leaves added to both the SPS systems have to be
  // positioned at the same places and angles.
  // An array of random angles are formed to be used by both the mini base trees
  // and the leaves
  // when they are added as forks at the end of the final branches.
  Float32Array _turns;
  float _fork_turn = Math::PI2 / forks;
  for (double f = 0; f < std::pow(forks, boughs + 1); f++) {
    _turns.emplace_back(randPct(
      static_cast<float>(std::floor(f / std::pow(forks, boughs))) * _fork_turn,
      0.2f));
  }

  // The _set_mini_trees function positions mini base trees and leaves at the
  // end of base tree branches, one for each of the forks
  auto _set_mini_trees
    = [&](SolidParticle* particle, const Vector3& /*i*/, unsigned int s) {
        unsigned int _a = s % static_cast<unsigned>(std::pow(forks, boughs));
        if (boughs == 1) {
          _a++;
        }
        else {
          _a = 2 + _a % forks
               + static_cast<unsigned>(std::floor(static_cast<float>(_a)
                                                  / static_cast<float>(forks)))
                   * (forks + 1);
        }
        CoordSystem& _mini_sys = _base.directions[_a];
        Vector3 mini_top(_base.paths[_a][_base.paths[_a].size() - 1].x,
                         _base.paths[_a][_base.paths[_a].size() - 1].y,
                         _base.paths[_a][_base.paths[_a].size() - 1].z);
        float turn = _turns[s];
        Vector3 _mini_direction
          = _mini_sys.y.scale(std::cos(randPct(forkAngle, 0.f)))
              .add(_mini_sys.x.scale(std::sin(randPct(forkAngle, 0.f))
                                     * std::sin(turn)))
              .add(_mini_sys.z.scale(std::sin(randPct(forkAngle, 0.f))
                                     * std::cos(turn)));
        Vector3 axis = Vector3::Cross(Axis::Y, _mini_direction);
        float _theta = std::acos(Vector3::Dot(_mini_direction, Axis::Y)
                                 / _mini_direction.length());
        particle->scaling = Vector3(std::pow(trunkTaper, _boughs + 1.f),
                                    std::pow(trunkTaper, _boughs + 1.f),
                                    std::pow(trunkTaper, _boughs + 1.f));
        particle->rotationQuaternion = std_util::make_unique<Quaternion>(
          Quaternion::RotationAxis(axis, _theta));
        particle->position = mini_top;
      };

  // The mini base trees and leaves added to both the SPS systems have to be
  // positioned at the same places and angles.
  // An array of random positions and angles are formed to be used by both the
  // mini base trees and the leaves
  // when they are added as random mini leafed trees to the forked tree.
  // The random positions are chosen by selecting one of the random paths for a
  // branch and a random point along the branch.
  Float32Array _bturns;
  std::vector<Uint32Array> _places;
  size_t _bplen  = _base.paths.size();
  size_t _bp0len = _base.paths[0].size();
  for (unsigned int b = 0; b < branches; ++b) {
    _bturns.emplace_back(2.f * Math::PI * Math::random() - Math::PI);
    Uint32Array newPlace{
      static_cast<unsigned>(std::floor(Math::random() * _bplen)),
      static_cast<unsigned>(std::floor(Math::random() * (_bp0len - 1) + 1))};
    _places.emplace_back(newPlace);
  }

  // The _set_branches function positions mini base trees and leaves at random
  // positions along random branches
  auto _set_branches
    = [&](SolidParticle* particle, const Vector3& /*i*/, unsigned int s) {
        unsigned int _a        = _places[s][0];
        unsigned int _b        = _places[s][1];
        CoordSystem& _mini_sys = _base.directions[_a];
        Vector3 _mini_place(_base.paths[_a][_b].x, _base.paths[_a][_b].y,
                            _base.paths[_a][_b].z);
        _mini_place.addInPlace(_mini_sys.z.scale(_base.radii[_a][_b] / 2.f));
        float _turn = _bturns[s];
        Vector3 _mini_direction
          = _mini_sys.y.scale(std::cos(randPct(branchAngle, 0.f)))
              .add(_mini_sys.x.scale(std::sin(randPct(branchAngle, 0.f))
                                     * std::sin(_turn)))
              .add(_mini_sys.z.scale(std::sin(randPct(branchAngle, 0.f))
                                     * std::cos(_turn)));
        Vector3 _axis = Vector3::Cross(Axis::Y, _mini_direction);
        float _theta  = std::acos(Vector3::Dot(_mini_direction, Axis::Y)
                                 / _mini_direction.length());
        particle->scaling = Vector3(std::pow(trunkTaper, _boughs + 1.f),
                                    std::pow(trunkTaper, _boughs + 1.f),
                                    std::pow(trunkTaper, _boughs + 1.f));
        particle->rotationQuaternion = std_util::make_unique<Quaternion>(
          Quaternion::RotationAxis(_axis, _theta));
        particle->position = _mini_place;
      };

  // Add base tree mesh enough for all the final forked branches
  SolidParticleSystemMeshBuilderOptions spsBuilderOptionsMiniTrees;
  spsBuilderOptionsMiniTrees.positionFunction = _set_mini_trees;
  _mini_trees_SPS->addShape(_base.tree,
                            static_cast<size_t>(std::pow(forks, boughs + 1)),
                            spsBuilderOptionsMiniTrees);

  // Add base tree mesh given the number of branches in that parameter.
  SolidParticleSystemMeshBuilderOptions spsBuilderOptionsBranches;
  spsBuilderOptionsBranches.positionFunction = _set_branches;
  _mini_trees_SPS->addShape(_base.tree, branches, spsBuilderOptionsBranches);
  // Form mesh with all mini trees
  auto _tree_crown = _mini_trees_SPS->buildMesh();
  _tree_crown->setMaterial(trunkMaterial);

  // Add leaves mesh enough for all the final forked branches
  _mini_leaves_SPS->addShape(_leaves,
                             static_cast<size_t>(std::pow(forks, boughs + 1)),
                             spsBuilderOptionsMiniTrees);

  // Add leaves mesh given the number of branches in that parameter.
  _mini_leaves_SPS->addShape(_leaves, branches, spsBuilderOptionsBranches);
  // Form mesh of all leaves
  auto _leaves_crown = _mini_leaves_SPS->buildMesh();
  _leaves->dispose();
  _leaves_crown->setMaterial(leafMaterial);

  // Give the three mesh elements of full tree the same parent.
  BoxOptions boxOptions;
  auto _root       = MeshBuilder::CreateBox("", boxOptions, scene);
  _root->isVisible = false;
  _base.tree->setParent(_root);
  _tree_crown->setParent(_root);
  _leaves_crown->setParent(_root);

  return _root;
}

CoordSystem SPSTreeGenerator::coordSystem(Vector3& vec3)
{
  Vector3 _x;
  auto _y = vec3.normalize();
  if (std_util::almost_equal(std::abs(vec3.x), 0.f)
      && std_util::almost_equal(std::abs(vec3.y), 0.f)) {
    _x = Vector3(vec3.z, 0.f, 0.f).normalize();
  }
  else {
    _x = Vector3(vec3.y, -vec3.x, 0.f).normalize();
  }
  auto _z = Vector3::Cross(_x, _y);
  return {_x, _y, _z};
}

float SPSTreeGenerator::randPct(float v, float p)
{
  if (std_util::almost_equal(p, 0.f)) {
    return v;
  }

  return (1.f + (1.f - 2.f * Math::random()) * p) * v;
}

SPSTreeBranch SPSTreeGenerator::createBranch(
  const Vector3& branchAt, const CoordSystem& branchSys, float branchLength,
  float branchTaper, size_t branchSlices, size_t bowFreq, float bowHeight,
  float branchRadius, Scene* scene)
{
  Vector3 _core_point;
  // array of Vector3 points that give the path of the central core of the
  // branch
  std::vector<Vector3> _core_path(branchSlices);
  Vector3 _path;
  float _xsr = 0.f;
  // array of radii for each core point
  Float32Array _radii(branchSlices);
  float _d_slices_length = 0.f;
  float _theta           = 0.f;
  size_t _a_sides        = 12;
  std::vector<std::vector<Vector3>> _cross_section_paths(_a_sides);

  for (size_t d = 0; d < branchSlices; d++) {
    _d_slices_length = d / branchSlices;
    // central point along core path
    _core_point = branchSys.y.scale(_d_slices_length * branchLength);
    // add damped wave along branch to give bows
    _core_point.addInPlace(
      branchSys.x.scale(bowHeight * std::exp(-_d_slices_length)
                        * std::sin(bowFreq * _d_slices_length * Math::PI)));
    // set core point start at spur position.
    _core_point.addInPlace(branchAt);
    _core_path[d] = _core_point;
    // randomize radius by +/- 40% of radius and taper to branchTaper*100% of
    // radius  at top
    _xsr = branchRadius * (1.f + (0.4f * Math::random() - 0.2f))
           * (1.f - (1.f - branchTaper) * _d_slices_length);
    _radii[d] = _xsr;

    // determine the paths for each vertex of the cross sectional polygons
    for (size_t a = 0; a < _a_sides; ++a) {
      _theta = a * Math::PI / 6.f;
      // path followed by one point on cross section in branchSys.y direction
      _path = branchSys.x.scale(_xsr * std::cos(_theta))
                .add(branchSys.z.scale(_xsr * std::sin(_theta)));
      // align with core
      _path.addInPlace(_core_path[d]);
      _cross_section_paths[a].emplace_back(_path);
    }
  }

  // Add cap at end of branch.
  for (size_t a = 0; a < _a_sides; a++) {
    _cross_section_paths[a].emplace_back(_core_path.back());
  }

  // Create ribbon mesh to repreent the branch.
  RibbonOptions options(_cross_section_paths);
  options.closeArray = true;

  auto _branch = MeshBuilder::CreateRibbon("branch", options, scene);

  SPSTreeBranch spsTreeBranch;
  spsTreeBranch.branch = std::move(_branch);
  spsTreeBranch.core   = std::move(_core_path);
  spsTreeBranch._radii = std::move(_radii);

  return spsTreeBranch;
}

SPSTreeBase SPSTreeGenerator::createTreeBase(
  float trunkHeight, float trunkTaper, size_t trunkSlices, unsigned int boughs,
  unsigned int forks, float forkAngle, float forkRatio, size_t bowFreq,
  float bowHeight, Scene* scene)
{
  // Golden ratio for scale
  const float _PHI = 2.f / (1.f + std::sqrt(5.f));
  // Trunk starts in y direction
  Vector3 _trunk_direction(0, 1, 0);

  auto _trunk_sys = coordSystem(_trunk_direction);
  Vector3 _trunk_root_at(0.f, 0.f, 0.f);
  // Array holding the mesh of each branch
  std::vector<Mesh*> _tree_branches;
  // Array holding the central core points for each branch created
  std::vector<std::vector<Vector3>> _tree_paths;
  // Array holding the branch radius for each branch created
  std::vector<Float32Array> _tree_radii;
  // Array holding the branch direction for each branch created
  std::vector<CoordSystem> _tree_directions;

  // Create trunk
  auto _trunk = createBranch(_trunk_root_at, _trunk_sys, trunkHeight,
                             trunkTaper, trunkSlices, 1, bowHeight, 1, scene);
  _tree_branches.emplace_back(_trunk.branch);
  auto& _core_path = _trunk.core;
  _tree_paths.emplace_back(_core_path);
  _tree_radii.emplace_back(_trunk._radii);
  _tree_directions.emplace_back(_trunk_sys);
  auto& _top_point = _core_path.back();
  // Angle of spread of forks around a branch
  float _fork_turn = 2.f * Math::PI / forks;

  Vector3 _fork_branch_direction, _bough_top, _bough_direction;
  CoordSystem _fork_branchSys, _bough_sys;
  SPSTreeBranch _branch, _bough;
  std::vector<Vector3> _bough_core_path;
  float _turn, _bough_turn;

  // create new branch at top of trunk for number of forks
  for (unsigned int f = 0; f < forks; ++f) {
    // Randomise angle of spread for a fork
    _turn = randPct(f * _fork_turn, 0.25f);
    _fork_branch_direction
      = _trunk_sys.y.scale(std::cos(randPct(forkAngle, 0.15f)))
          .add(_trunk_sys.x.scale(std::sin(randPct(forkAngle, 0.15f))
                                  * std::sin(_turn)))
          .add(_trunk_sys.z.scale(std::sin(randPct(forkAngle, 0.15f))
                                  * std::cos(_turn)));
    _fork_branchSys = coordSystem(_fork_branch_direction);
    _branch = createBranch(_top_point, _fork_branchSys, trunkHeight * forkRatio,
                           trunkTaper, trunkSlices, bowFreq, bowHeight * _PHI,
                           trunkTaper, scene);
    _bough_core_path = _branch.core;
    _bough_top       = _bough_core_path.back();

    // store branch details
    _tree_branches.emplace_back(_branch.branch);
    _tree_paths.emplace_back(_branch.core);
    _tree_radii.emplace_back(_branch._radii);
    _tree_directions.emplace_back(_fork_branchSys);
    if (boughs > 1) {
      // When boughs = 2 create further branches at end of new branch
      for (unsigned int k = 0; k < forks; k++) {
        _bough_turn = randPct(k * _fork_turn, 0.25f);
        _bough_direction
          = _fork_branchSys.y.scale(std::cos(randPct(forkAngle, 0.15f)))
              .add(_fork_branchSys.x.scale(std::sin(randPct(forkAngle, 0.15f))
                                           * std::sin(_bough_turn)))
              .add(_fork_branchSys.z.scale(std::sin(randPct(forkAngle, 0.15f))
                                           * std::cos(_bough_turn)));
        _bough_sys = coordSystem(_bough_direction);
        _bough     = createBranch(_bough_top, _bough_sys,
                              trunkHeight * forkRatio * forkRatio, trunkTaper,
                              trunkSlices, bowFreq, bowHeight * _PHI * _PHI,
                              trunkTaper * trunkTaper, scene);

        // store branch details
        _tree_branches.emplace_back(_bough.branch);
        _tree_paths.emplace_back(_bough.core);
        _tree_radii.emplace_back(_branch._radii);
        _tree_directions.emplace_back(_bough_sys);
      }
    }
  }

  // Merge branch meshes into a single mesh
  auto _tree = Mesh::MergeMeshes(_tree_branches);

  SPSTreeBase spsTreeBase;
  spsTreeBase.tree       = std::move(_tree);
  spsTreeBase.paths      = std::move(_tree_paths);
  spsTreeBase.radii      = std::move(_tree_radii);
  spsTreeBase.directions = std::move(_tree_directions);

  return spsTreeBase;
}

} // end of namespace Extensions
} // end of namespace BABYLON
