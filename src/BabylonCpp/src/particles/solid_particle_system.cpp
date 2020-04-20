#include <babylon/particles/solid_particle_system.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/cameras/target_camera.h>
#include <babylon/core/random.h>
#include <babylon/culling/bounding_info.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/multi_material.h>
#include <babylon/materials/standard_material.h>
#include <babylon/maths/axis.h>
#include <babylon/maths/color4.h>
#include <babylon/maths/tmp_vectors.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/meshes/vertex_data.h>
#include <babylon/particles/depth_sorted_particle.h>
#include <babylon/particles/model_shape.h>
#include <babylon/particles/solid_particle.h>

namespace BABYLON {

SolidParticleSystem::SolidParticleSystem(const std::string& iName, Scene* scene,
                                         const std::optional<SolidParticleSystemOptions>& options)
    : nbParticles{0}
    , billboard{false}
    , recomputeNormals{false}
    , counter{0}
    , mesh{nullptr}
    , computeParticleRotation{this, &SolidParticleSystem::get_computeParticleRotation,
                              &SolidParticleSystem::set_computeParticleRotation}
    , computeParticleColor{this, &SolidParticleSystem::get_computeParticleColor,
                           &SolidParticleSystem::set_computeParticleColor}
    , computeParticleTexture{this, &SolidParticleSystem::get_computeParticleTexture,
                             &SolidParticleSystem::set_computeParticleTexture}
    , computeParticleVertex{this, &SolidParticleSystem::get_computeParticleVertex,
                            &SolidParticleSystem::set_computeParticleVertex}
    , computeBoundingBox{this, &SolidParticleSystem::get_computeBoundingBox,
                         &SolidParticleSystem::set_computeBoundingBox}
    , depthSortParticles{this, &SolidParticleSystem::get_depthSortParticles,
                         &SolidParticleSystem::set_depthSortParticles}
    , expandable{this, &SolidParticleSystem::get_expandable}
    , multimaterialEnabled{this, &SolidParticleSystem::get_multimaterialEnabled}
    , useModelMaterial{this, &SolidParticleSystem::get_useModelMaterial}
    , materials{this, &SolidParticleSystem::get_materials}
    , multimaterial{this, &SolidParticleSystem::get_multimaterial,
                    &SolidParticleSystem::set_multimaterial}
    , autoUpdateSubMeshes{this, &SolidParticleSystem::get_autoUpdateSubMeshes,
                          &SolidParticleSystem::set_autoUpdateSubMeshes}
    , _index{0}
    , _updatable{true}
    , _pickable{false}
    , _isVisibilityBoxLocked{false}
    , _alwaysVisible{false}
    , _depthSort{false}
    , _expandable{false}
    , _shapeCounter{0}
    , _copy{std::make_unique<SolidParticle>(0, 0, 0, 0, nullptr, 0, 0, this,
                                            BoundingInfo{Vector3::Zero(), Vector3::Zero()})}
    , _color{std::make_unique<Color4>(0.f, 0.f, 0.f, 0.f)}
    , _computeParticleColor{true}
    , _computeParticleTexture{true}
    , _computeParticleRotation{true}
    , _computeParticleVertex{false}
    , _computeBoundingBox{false}
    , _depthSortParticles{true}
    , _mustUnrotateFixedNormals{false}
    , _scale{TmpVectors::Vector3Array[2]}
    , _translation{TmpVectors::Vector3Array[3]}
    , _needs32Bits{false}
    , _isNotBuilt{true}
    , _lastParticleId{0}
    , _multimaterialEnabled{false}
    , _useModelMaterial{false}
    , _multimaterial{nullptr}
    , _defaultMaterial{nullptr}
    , _autoUpdateSubMeshes{false}
{
  name                  = iName;
  _scene                = scene ? scene : Engine::LastCreatedScene();
  _camera               = std::static_pointer_cast<TargetCamera>(_scene->activeCamera());
  _pickable             = options ? options->isPickable : false;
  _depthSort            = options ? options->enableDepthSort : false;
  _multimaterialEnabled = options ? options->enableMultiMaterial : false;
  _useModelMaterial     = options ? options->useModelMaterial : false;
  _multimaterialEnabled = _useModelMaterial ? true : _multimaterialEnabled;
  _expandable           = options ? options->expandable : false;
  _particlesIntersect   = options ? options->particleIntersection : false;
  _bSphereOnly          = options ? options->boundingSphereOnly : false;
  _bSphereRadiusFactor
    = (options && options->bSphereRadiusFactor) ? options->bSphereRadiusFactor.value() : 1.f;
  if (options && options->updatable.has_value()) {
    _updatable = options->updatable.value();
  }
  else {
    _updatable = true;
  }
  if (_pickable) {
    pickedParticles = {};
  }

  if (_depthSort || _multimaterialEnabled) {
    depthSortedParticles = {};
  }
  if (_multimaterialEnabled) {
    _multimaterial       = MultiMaterial::New(name + "MultiMaterial", _scene);
    _materials           = {};
    _materialIndexesById = {};
  }

  _depthSortFunction = [](const DepthSortedParticle& p1, const DepthSortedParticle& p2) -> bool {
    return p1.sqDistance < p2.sqDistance;
  };

  _materialSortFunction = [](const DepthSortedParticle& p1, const DepthSortedParticle& p2) -> bool {
    return p2.materialIndex < p1.materialIndex;
  };
}

SolidParticleSystem::~SolidParticleSystem() = default;

MeshPtr SolidParticleSystem::buildMesh()
{
  if (!_isNotBuilt && mesh) {
    return mesh;
  }
  if (nbParticles == 0 && !mesh) {
    DiscOptions options;
    options.radius       = 1.f;
    options.tessellation = 3;
    auto triangle        = MeshBuilder::CreateDisc("", options, _scene);
    std::optional<SolidParticleSystemMeshBuilderOptions> shapeOptions = std::nullopt;
    addShape(triangle, 1, shapeOptions);
    triangle->dispose();
  }

  _indices32   = IndicesArray(_indices);
  _positions32 = Float32Array(_positions);
  _uvs32       = Float32Array(_uvs);
  _colors32    = Float32Array(_colors);

  if (!mesh) { // in case it's already expanded
    mesh = Mesh::New(name, _scene);
  }
  if (!_updatable && _multimaterialEnabled) {
    _sortParticlesByMaterial(); // this may reorder the indices32
  }
  if (recomputeNormals) {
    VertexData::ComputeNormals(_positions32, _indices32, _normals);
  }
  _normals32     = Float32Array(_normals);
  _fixedNormal32 = Float32Array(_normals);
  // The particles could be created already rotated in the mesh with a positionFunction
  if (_mustUnrotateFixedNormals) {
    _unrotateFixedNormals();
  }

  auto vertexData     = std::make_unique<VertexData>();
  vertexData->indices = (_depthSort) ? _indices : _indices32;
  vertexData->set(_positions32, VertexBuffer::PositionKind);
  vertexData->set(_normals32, VertexBuffer::NormalKind);

  if (!_uvs32.empty()) {
    vertexData->set(_uvs32, VertexBuffer::UVKind);
  }
  if (!_colors32.empty()) {
    vertexData->set(_colors32, VertexBuffer::ColorKind);
  }

  vertexData->applyToMesh(*mesh, _updatable);
  mesh->isPickable = _pickable;

  if (_multimaterialEnabled) {
    setMultiMaterial(_materials);
  }

  if (!_expandable) {
    // free memory
    if (!_depthSort && !_multimaterialEnabled) {
      _indices.clear();
    }
    _positions.clear();
    _normals.clear();
    _uvs.clear();
    _colors.clear();

    if (!_updatable) {
      particles.clear();
    }
  }
  _isNotBuilt      = false;
  recomputeNormals = false;

  return mesh;
}

SolidParticleSystem&
SolidParticleSystem::digest(Mesh* _mesh, std::optional<SolidParticleSystemDigestOptions>& options)
{
  auto size    = (options && options->facetNb) ? options->facetNb : 1;
  auto number  = (options && options->number) ? options->number : 0;
  auto delta   = (options && options->delta) ? options->delta : 0;
  auto meshPos = _mesh->getVerticesData(VertexBuffer::PositionKind);
  auto meshInd = _mesh->getIndices();
  auto meshUV  = _mesh->getVerticesData(VertexBuffer::UVKind);
  auto meshCol = _mesh->getVerticesData(VertexBuffer::ColorKind);
  auto meshNor = mesh->getVerticesData(VertexBuffer::NormalKind);

  auto f = 0ull; // facet counter
  // a facet is a triangle, so 3 indices
  size_t totalFacets = meshInd.size() / 3;
  // compute size from number
  if (number) {
    number = (number > totalFacets) ? totalFacets : number;
    size   = static_cast<size_t>(
      std::round(static_cast<float>(totalFacets) / static_cast<float>(number)));
    delta = 0;
  }
  else {
    size = (size > totalFacets) ? totalFacets : size;
  }

  Float32Array facetPos; // submesh positions
  Float32Array facetNor;
  Uint32Array facetInd;  // submesh indices
  Float32Array facetUV;  // submesh UV
  Float32Array facetCol; // submesh colors
  auto barycenter = Vector3::Zero();
  auto sizeO      = size;

  while (f < totalFacets) {
    size
      = sizeO + static_cast<size_t>(std::floor((1.f + static_cast<float>(delta)) * Math::random()));
    if (f > totalFacets - size) {
      size = totalFacets - f;
    }
    // reset temp arrays
    facetPos.clear();
    facetNor.clear();
    facetInd.clear();
    facetUV.clear();
    facetCol.clear();

    // iterate over "size" facets
    int fi = 0;
    for (size_t j = f * 3; j < (f + size) * 3; ++j) {
      facetInd.emplace_back(fi);
      auto i  = static_cast<uint32_t>(meshInd[j]);
      auto i3 = i * 3;
      stl_util::concat(facetPos, {meshPos[i3], meshPos[i3 + 1], meshPos[i3 + 2]});
      stl_util::concat(meshNor, {meshNor[i3], meshNor[i3 + 1], meshNor[i3 + 2]});
      if (!meshUV.empty()) {
        auto i2 = i * 2;
        stl_util::concat(facetUV, {meshUV[i2], meshUV[i2 + 1]});
      }
      if (!meshCol.empty()) {
        auto i4 = i * 4;
        stl_util::concat(facetCol,
                         {meshCol[i4 + 0], meshCol[i4 + 1], meshCol[i4 + 2], meshCol[i4 + 3]});
      }
      ++fi;
    }

    // create a model shape for each single particle
    auto idx             = nbParticles;
    auto shape           = _posToShape(facetPos);
    auto shapeUV         = _uvsToShapeUV(facetUV);
    const auto& shapeInd = facetInd;
    const auto& shapeCol = facetCol;
    const auto& shapeNor = facetNor;

    // compute the barycenter of the shape
    barycenter.copyFromFloats(0.f, 0.f, 0.f);
    for (const auto& iShape : shape) {
      barycenter.addInPlace(iShape);
    }
    barycenter.scaleInPlace(1.f / static_cast<float>(shape.size()));

    // shift the shape from its barycenter to the origin and compute the BBox required for
    // intersection.
    Vector3 minimum{std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
                    std::numeric_limits<float>::max()};
    Vector3 maximum{std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(),
                    std::numeric_limits<float>::lowest()};
    for (auto& iShape : shape) {
      iShape.subtractInPlace(barycenter);
      minimum.minimizeInPlaceFromFloats(iShape.x, iShape.y, iShape.z);
      maximum.maximizeInPlaceFromFloats(iShape.x, iShape.y, iShape.z);
    }
    BoundingInfo bInfo{Vector3::Zero(), Vector3::Zero()};
    if (_particlesIntersect) {
      bInfo = BoundingInfo(minimum, maximum);
    }
    MaterialPtr material = nullptr;
    if (_useModelMaterial) {
      material = (mesh->material()) ? mesh->material() : _setDefaultMaterial();
    }
    auto modelShape = std::make_shared<ModelShape>(_shapeCounter, shape, shapeInd, shapeNor,
                                                   shapeCol, shapeUV, nullptr, nullptr, material);

    // add the particle in the SPS
    auto currentPos = static_cast<uint32_t>(_positions.size());
    auto currentInd = static_cast<uint32_t>(_indices.size());
    _meshBuilder(_index, currentInd, shape, _positions, shapeInd, _indices, facetUV, _uvs, shapeCol,
                 _colors, shapeNor, _normals, idx, 0, std::nullopt, modelShape);
    if (options.has_value() && options->storage.has_value()) {
      _addParticle(idx, _lastParticleId, currentPos, currentInd, modelShape, _shapeCounter, 0,
                   bInfo, *options->storage);
    }
    else {
      _addParticle(idx, _lastParticleId, currentPos, currentInd, modelShape, _shapeCounter, 0,
                   bInfo);
    }
    // initialize the particle position
    particles[nbParticles]->position.addInPlace(barycenter);

    if (!options.has_value() || !options->storage.has_value()) {
      _index += static_cast<uint32_t>(shape.size());
      ++idx;
      ++nbParticles;
      ++_lastParticleId;
    }
    ++_shapeCounter;
    f += size;
  }
  _isNotBuilt = true; // buildMesh() is now expected for setParticles() to work
  return *this;
}

void SolidParticleSystem::_unrotateFixedNormals()
{
  auto index              = 0u;
  auto idx                = 0u;
  auto& tmpNormal         = TmpVectors::Vector3Array[0];
  auto& quaternion        = TmpVectors::QuaternionArray[0];
  auto& invertedRotMatrix = TmpVectors::MatrixArray[0];
  for (const auto& particle : particles) {
    auto& shape = particle->_model->_shape;

    // computing the inverse of the rotation matrix from the quaternion is equivalent to computing
    // the matrix of the inverse quaternion, i.e of the conjugate quaternion
    if (particle->rotationQuaternion) {
      particle->rotationQuaternion->conjugateToRef(quaternion);
    }
    else {
      const auto& rotation = particle->rotation;
      Quaternion::RotationYawPitchRollToRef(rotation.y, rotation.x, rotation.z, quaternion);
      quaternion.conjugateInPlace();
    }
    quaternion.toRotationMatrix(invertedRotMatrix);

    for (uint32_t pt = 0; pt < shape.size(); pt++) {
      idx = index + pt * 3;
      Vector3::TransformNormalFromFloatsToRef(_normals32[idx], _normals32[idx + 1],
                                              _normals32[idx + 2], invertedRotMatrix, tmpNormal);
      tmpNormal.toArray(_fixedNormal32, idx);
    }
    index = idx + 3;
  }
}

void SolidParticleSystem::_resetCopy()
{
  const auto& copy = _copy;
  copy->position.setAll(0.f);
  copy->rotation.setAll(0.f);
  copy->rotationQuaternion = nullptr;
  copy->scaling.setAll(1.f);
  copy->uvs.copyFromFloats(0.f, 0.f, 1.f, 1.f);
  copy->color              = std::nullopt;
  copy->translateFromPivot = false;
  copy->materialIndex      = std::nullopt;
}

SolidParticle* SolidParticleSystem::_meshBuilder(
  size_t p, size_t ind, const std::vector<Vector3>& shape, Float32Array& positions,
  const Uint32Array& meshInd, Uint32Array& indices, const Float32Array& meshUV, Float32Array& uvs,
  const Float32Array& meshCol, Float32Array& colors, const Float32Array& meshNor,
  Float32Array& normals, size_t idx, size_t idxInShape,
  const std::optional<SolidParticleSystemMeshBuilderOptions>& options, const ModelShapePtr& model)
{
  size_t i = 0;
  size_t u = 0;
  size_t c = 0;
  size_t n = 0;

  _resetCopy();
  auto& copy      = *_copy;
  auto storeApart = (options && options->storage) ? true : false;
  copy.idx        = idx;
  copy.idxInShape = idxInShape;
  if (_useModelMaterial) {
    const auto materialId     = model->_material->uniqueId;
    auto& materialIndexesById = _materialIndexesById;
    if (!stl_util::contains(materialIndexesById, materialId)) {
      materialIndexesById[materialId] = _materials.size();
      _materials.emplace_back(model->_material);
    }
    auto matIdx        = materialIndexesById[materialId];
    copy.materialIndex = matIdx;
  }

  if (options && options->positionFunction) { // call to custom
    options->positionFunction(_copy.get(), idx, idxInShape);
    _mustUnrotateFixedNormals = true;
  }

  // in case the particle geometry must NOT be inserted in the SPS mesh geometry
  if (storeApart) {
    return &copy;
  }

  auto& rotMatrix            = TmpVectors::MatrixArray[0];
  auto& tmpVertex            = TmpVectors::Vector3Array[0];
  auto& tmpRotated           = TmpVectors::Vector3Array[1];
  auto& pivotBackTranslation = TmpVectors::Vector3Array[2];
  auto& scaledPivot          = TmpVectors::Vector3Array[3];
  Matrix::IdentityToRef(rotMatrix);
  copy.getRotationMatrix(rotMatrix);

  copy.pivot.multiplyToRef(copy.scaling, scaledPivot);

  if (copy.translateFromPivot) {
    pivotBackTranslation.setAll(0.0);
  }
  else {
    pivotBackTranslation.copyFrom(scaledPivot);
  }

  auto someVertexFunction = (options && options->vertexFunction);
  for (i = 0; i < shape.size(); i++) {
    tmpVertex.copyFrom(shape[i]);
    if (someVertexFunction) {
      options->vertexFunction(&copy, tmpVertex, i);
    }

    tmpVertex.multiplyInPlace(copy.scaling).subtractInPlace(scaledPivot);
    Vector3::TransformCoordinatesToRef(tmpVertex, rotMatrix, tmpRotated);
    tmpRotated.addInPlace(pivotBackTranslation).addInPlace(copy.position);
    stl_util::concat(positions, {tmpRotated.x, tmpRotated.y, tmpRotated.z});
    if (!meshUV.empty()) {
      auto copyUvs = copy.uvs;
      stl_util::concat(uvs, {(copyUvs.z - copyUvs.x) * meshUV[u] + copyUvs.x,
                             (copyUvs.w - copyUvs.y) * meshUV[u + 1] + copyUvs.y});
      u += 2;
    }

    if (copy.color) {
      _color = std::make_unique<Color4>(copy.color.value());
    }
    else {
      auto& color = *_color;
      if (!meshCol.empty() && (c + 3) < meshCol.size()) {
        color.r = meshCol[c];
        color.g = meshCol[c + 1];
        color.b = meshCol[c + 2];
        color.a = meshCol[c + 3];
      }
      else {
        color.r = 1.f;
        color.g = 1.f;
        color.b = 1.f;
        color.a = 1.f;
      }
    }
    stl_util::concat(colors, {_color->r, _color->g, _color->b, _color->a});
    c += 4;

    if (!recomputeNormals && (n + 2) < meshNor.size()) {
      Vector3::TransformNormalFromFloatsToRef(meshNor[n], meshNor[n + 1], meshNor[n + 2], rotMatrix,
                                              tmpVertex);
      stl_util::concat(normals, {tmpVertex.x, tmpVertex.y, tmpVertex.z});
      n += 3;
    }
  }

  const auto _p = static_cast<uint32_t>(p);
  for (i = 0; i < meshInd.size(); ++i) {
    auto current_ind = _p + meshInd[i];
    indices.emplace_back(current_ind);
    if (current_ind > 65535) {
      _needs32Bits = true;
    }
  }

  if (_pickable) {
    auto nbfaces = meshInd.size() / 3;
    for (i = 0; i < nbfaces; ++i) {
      pickedParticles.emplace_back(PickedParticle{
        idx, // idx
        i    // faceId
      });
    }
  }

  if (_depthSort || _multimaterialEnabled) {
    auto matIndex = copy.materialIndex.has_value() ? copy.materialIndex : 0;
    depthSortedParticles.emplace_back(
      DepthSortedParticle(idx, ind, meshInd.size(), matIndex.value()));
  }

  return &copy;
}

std::vector<Vector3> SolidParticleSystem::_posToShape(const Float32Array& positions)
{
  std::vector<Vector3> shape;
  for (uint32_t i = 0; i < positions.size(); i += 3) {
    shape.emplace_back(Vector3::FromArray(positions, i));
  }
  return shape;
}

Float32Array SolidParticleSystem::_uvsToShapeUV(const Float32Array& uvs)
{
  Float32Array shapeUV;
  if (!uvs.empty()) {
    for (const auto& uv : uvs) {
      shapeUV.emplace_back(uv);
    }
  }
  return shapeUV;
}

SolidParticlePtr SolidParticleSystem::_addParticle(size_t idx, size_t id, size_t idxpos,
                                                   size_t idxind, const ModelShapePtr& model,
                                                   int shapeId, size_t idxInShape,
                                                   const BoundingInfo& bInfo)
{
  auto sp = std::make_shared<SolidParticle>(idx, static_cast<int>(id), idxpos, idxind, model,
                                            shapeId, idxInShape, this, bInfo);

  particles.emplace_back(sp);
  return sp;
}

SolidParticlePtr SolidParticleSystem::_addParticle(size_t idx, size_t id, size_t idxpos,
                                                   size_t idxind, const ModelShapePtr& model,
                                                   int shapeId, size_t idxInShape,
                                                   const BoundingInfo& bInfo,
                                                   std::vector<SolidParticlePtr>& storage)
{
  auto sp = std::make_shared<SolidParticle>(idx, static_cast<int>(id), idxpos, idxind, model,
                                            shapeId, idxInShape, this, bInfo);

  storage.emplace_back(sp);
  return sp;
}

int SolidParticleSystem::addShape(const MeshPtr& iMesh, size_t nb,
                                  SolidParticleSystemMeshBuilderOptions& options)
{
  std::optional<SolidParticleSystemMeshBuilderOptions> shapOptions = options;
  return addShape(iMesh, nb, shapOptions);
}

int SolidParticleSystem::addShape(const MeshPtr& iMesh, size_t nb,
                                  std::optional<SolidParticleSystemMeshBuilderOptions>& options)
{
  const auto meshPos       = iMesh->getVerticesData(VertexBuffer::PositionKind);
  const auto meshInd       = iMesh->getIndices();
  const auto meshUV        = iMesh->getVerticesData(VertexBuffer::UVKind);
  const auto meshCol       = iMesh->getVerticesData(VertexBuffer::ColorKind);
  const auto meshNor       = iMesh->getVerticesData(VertexBuffer::NormalKind);
  recomputeNormals         = !meshNor.empty() ? false : true;
  const auto& indices      = meshInd;
  const auto& shapeNormals = meshNor;
  const auto& shapeColors  = meshCol;
  BoundingInfoPtr bbInfo   = nullptr;
  if (_particlesIntersect) {
    bbInfo = mesh->getBoundingInfo();
  }

  auto shape   = _posToShape(meshPos);
  auto shapeUV = _uvsToShapeUV(meshUV);

  auto posfunc         = options ? options->positionFunction : nullptr;
  auto vtxfunc         = options ? options->vertexFunction : nullptr;
  MaterialPtr material = nullptr;
  if (_useModelMaterial) {
    material = mesh->material() ? mesh->material : _setDefaultMaterial();
  }
  auto modelShape = std::make_shared<ModelShape>(_shapeCounter, shape, indices, shapeNormals,
                                                 shapeColors, shapeUV, posfunc, vtxfunc, material);

  // particles
  if (options && options->storage) {
    for (size_t i = 0; i < nb; i++) {
      _insertNewParticle(nbParticles, i, modelShape, shape, meshInd, meshUV, meshCol, meshNor,
                         bbInfo, options->storage, options);
    }
  }
  else {
    std::optional<std::vector<SolidParticlePtr>> storage = std::nullopt;
    for (size_t i = 0; i < nb; i++) {
      _insertNewParticle(nbParticles, i, modelShape, shape, meshInd, meshUV, meshCol, meshNor,
                         bbInfo, storage, options);
    }
  }
  ++_shapeCounter;
  _isNotBuilt = true; // buildMesh() call is now expected for setParticles() to work
  return _shapeCounter - 1;
}

void SolidParticleSystem::_rebuildParticle(SolidParticle* particle, bool reset)
{
  _resetCopy();
  auto& copy = *_copy;
  if (particle->_model->_positionFunction) { // recall to stored custom positionFunction
    particle->_model->_positionFunction(&copy, particle->idx, particle->idxInShape);
  }

  auto& rotMatrix            = TmpVectors::MatrixArray[0];
  auto& tmpVertex            = TmpVectors::Vector3Array[0];
  auto& tmpRotated           = TmpVectors::Vector3Array[1];
  auto& pivotBackTranslation = TmpVectors::Vector3Array[2];
  auto& scaledPivot          = TmpVectors::Vector3Array[3];

  copy.getRotationMatrix(rotMatrix);

  particle->pivot.multiplyToRef(particle->scaling, scaledPivot);

  if (copy.translateFromPivot) {
    pivotBackTranslation.copyFromFloats(0.f, 0.f, 0.f);
  }
  else {
    pivotBackTranslation.copyFrom(scaledPivot);
  }

  auto& shape = particle->_model->_shape;

  for (uint32_t pt = 0; pt < shape.size(); ++pt) {
    tmpVertex.copyFrom(shape[pt]);
    if (particle->_model->_vertexFunction) {
      particle->_model->_vertexFunction(&copy, tmpVertex,
                                        pt); // recall to stored vertexFunction
    }

    tmpVertex.multiplyInPlace(copy.scaling).subtractInPlace(scaledPivot);
    Vector3::TransformCoordinatesToRef(tmpVertex, rotMatrix, tmpRotated);
    tmpRotated.addInPlace(pivotBackTranslation)
      .addInPlace(copy.position)
      .toArray(_positions32, static_cast<uint32_t>(particle->_pos + pt * 3));
  }
  if (reset) {
    particle->position.setAll(0.f);
    particle->rotation.setAll(0.f);
    particle->rotationQuaternion = nullptr;
    particle->scaling.setAll(1.f);
    particle->uvs.setAll(0.f);
    particle->pivot.setAll(0.f);
    particle->translateFromPivot = false;
    particle->parentId           = std::nullopt;
  }
}

SolidParticleSystem& SolidParticleSystem::rebuildMesh(bool reset)
{
  for (const auto& particle : particles) {
    _rebuildParticle(particle.get(), reset);
  }
  mesh->updateVerticesData(VertexBuffer::PositionKind, _positions32, false, false);
  return *this;
}

std::vector<SolidParticlePtr> SolidParticleSystem::removeParticles(size_t start, size_t end)
{
  auto nb = end - start + 1;
  if (!_expandable || (start >= end + 1) || nb >= nbParticles || !_updatable) {
    return {};
  }
  const auto& currentNb = nbParticles;
  if (end < currentNb - 1) { // update the particle indexes in the positions array in case they're
                             // remaining particles after the last removed
    auto firstRemaining = end + 1;
    auto shiftPos       = particles[firstRemaining]->_pos - particles[start]->_pos;
    auto shifInd        = particles[firstRemaining]->_ind - particles[start]->_ind;
    for (size_t i = firstRemaining; i < currentNb; i++) {
      auto part = particles[i];
      part->_pos -= shiftPos;
      part->_ind -= shifInd;
    }
  }
  auto removed = stl_util::splice(particles, static_cast<int>(start), static_cast<int>(nb));
  _positions.clear();
  _indices.clear();
  _colors.clear();
  _uvs.clear();
  _normals.clear();
  _index = 0;
  _idxOfId.clear();
  if (_depthSort || _multimaterialEnabled) {
    depthSortedParticles = {};
  }
  auto ind                   = 0ull;
  const auto particlesLength = particles.size();
  for (uint32_t p = 0; p < particlesLength; ++p) {
    const auto& particle                        = particles[p];
    const auto& model                           = particle->_model;
    const auto& shape                           = model->_shape;
    const auto& modelIndices                    = model->_indices;
    const auto& modelNormals                    = model->_normals;
    const auto& modelColors                     = model->_shapeColors;
    const auto& modelUVs                        = model->_shapeUV;
    particle->idx                               = p;
    _idxOfId[static_cast<size_t>(particle->id)] = p;
    _meshBuilder(_index, ind, shape, _positions, modelIndices, _indices, modelUVs, _uvs,
                 modelColors, _colors, modelNormals, _normals, particle->idx, particle->idxInShape,
                 std::nullopt, model);
    _index += shape.size();
    ind += modelIndices.size();
  }
  nbParticles -= nb;
  _isNotBuilt = true; // buildMesh() call is now expected for setParticles() to work
  return removed;
}

SolidParticleSystem& SolidParticleSystem::insertParticlesFromArray(
  const std::vector<SolidParticlePtr>& solidParticleArray)
{
  if (!_expandable) {
    return *this;
  }
  auto idxInShape     = 0ull;
  auto currentShapeId = solidParticleArray[0]->shapeId;
  const auto nb       = solidParticleArray.size();
  for (size_t i = 0; i < nb; i++) {
    const auto& sp      = solidParticleArray[i];
    const auto& model   = sp->_model;
    const auto& shape   = model->_shape;
    const auto& meshInd = model->_indices;
    const auto& meshUV  = model->_shapeUV;
    const auto& meshCol = model->_shapeColors;
    const auto& meshNor = model->_normals;
    const auto noNor    = (!meshNor.empty()) ? false : true;
    recomputeNormals    = (noNor || recomputeNormals);
    const auto& bbInfo  = sp->_boundingInfo;

    std::optional<std::vector<SolidParticlePtr>> storage = std::nullopt;
    auto newPart = _insertNewParticle(nbParticles, idxInShape, model, shape, meshInd, meshUV,
                                      meshCol, meshNor, bbInfo, storage, std::nullopt);
    sp->copyToRef(*newPart);
    ++idxInShape;
    if (currentShapeId != sp->shapeId) {
      currentShapeId = sp->shapeId;
      idxInShape     = 0;
    }
  }
  _isNotBuilt = true; // buildMesh() call is now expected for setParticles() to work
  return *this;
}

SolidParticlePtr SolidParticleSystem::_insertNewParticle(
  size_t idx, size_t i, const ModelShapePtr& modelShape, const std::vector<Vector3>& shape,
  const IndicesArray& meshInd, const Float32Array& meshUV, const Float32Array& meshCol,
  const Float32Array& meshNor, const BoundingInfoPtr& bbInfo,
  std::optional<std::vector<SolidParticlePtr>>& storage,
  const std::optional<SolidParticleSystemMeshBuilderOptions>& options)
{
  const auto currentPos = _positions.size();
  const auto currentInd = _indices.size();
  const auto currentCopy
    = _meshBuilder(_index, currentInd, shape, _positions, meshInd, _indices, meshUV, _uvs, meshCol,
                   _colors, meshNor, _normals, idx, i, options, modelShape);
  SolidParticlePtr sp = nullptr;
  if (_updatable) {
    auto defaultBbInfo = BoundingInfo{Vector3::Zero(), Vector3::Zero()};
    if (storage) {
      sp = _addParticle(nbParticles, _lastParticleId, currentPos, currentInd, modelShape,
                        _shapeCounter, i, bbInfo ? *bbInfo : defaultBbInfo, *storage);
    }
    else {
      _addParticle(nbParticles, _lastParticleId, currentPos, currentInd, modelShape, _shapeCounter,
                   i, bbInfo ? *bbInfo : defaultBbInfo);
    }
    sp->position.copyFrom(currentCopy->position);
    sp->rotation.copyFrom(currentCopy->rotation);
    if (currentCopy->rotationQuaternion) {
      if (sp->rotationQuaternion) {
        sp->rotationQuaternion->copyFrom(*currentCopy->rotationQuaternion);
      }
      else {
        sp->rotationQuaternion = currentCopy->rotationQuaternion->clone();
      }
    }
    if (currentCopy->color) {
      if (sp->color) {
        sp->color->copyFrom(*currentCopy->color);
      }
      else {
        sp->color = currentCopy->color;
      }
    }
    sp->scaling.copyFrom(currentCopy->scaling);
    sp->uvs.copyFrom(currentCopy->uvs);
    if (currentCopy->materialIndex.has_value()) {
      sp->materialIndex = currentCopy->materialIndex;
    }
    if (expandable) {
      _idxOfId[static_cast<size_t>(sp->id)] = sp->idx;
    }
  }
  if (!storage) {
    _index += shape.size();
    ++nbParticles;
    ++_lastParticleId;
  }
  return sp;
}

SolidParticleSystem& SolidParticleSystem::setParticles(size_t start, size_t end, bool update)
{
  if (!_updatable || _isNotBuilt) {
    return *this;
  }

  // custom beforeUpdate
  beforeUpdateParticles(start, end, update);

  auto& rotMatrix      = TmpVectors::MatrixArray[0];
  auto& invertedMatrix = TmpVectors::MatrixArray[1];
  auto& colors32       = _colors32;
  auto& positions32    = _positions32;
  auto& normals32      = _normals32;
  auto& uvs32          = _uvs32;
  auto& indices32      = _indices32;
  auto& indices        = _indices;
  auto& fixedNormal32  = _fixedNormal32;

  auto& tempVectors         = TmpVectors::Vector3Array;
  auto& camAxisX            = tempVectors[5].copyFromFloats(1.f, 0.f, 0.f);
  auto& camAxisY            = tempVectors[6].copyFromFloats(0.f, 1.f, 0.f);
  auto& camAxisZ            = tempVectors[7].copyFromFloats(0.f, 0.f, 1.f);
  auto& minimum             = tempVectors[8].setAll(std::numeric_limits<float>::max());
  auto& maximum             = tempVectors[9].setAll(std::numeric_limits<float>::lowest());
  auto& camInvertedPosition = tempVectors[10].setAll(0.f);

  // cases when the World Matrix is to be computed first
  if (billboard || _depthSort) {
    mesh->computeWorldMatrix(true);
    mesh->_worldMatrix.invertToRef(invertedMatrix);
  }
  // if the particles will always face the camera
  if (billboard) {
    // compute the camera position and un-rotate it by the current mesh rotation
    auto& tmpVertex = tempVectors[0];
    _camera->getDirectionToRef(Axis::Z(), tmpVertex);
    Vector3::TransformNormalToRef(tmpVertex, invertedMatrix, camAxisZ);
    camAxisZ.normalize();
    // same for camera up vector extracted from the cam view matrix
    auto& view        = _camera->getViewMatrix(true);
    const auto& viewM = view.m();
    Vector3::TransformNormalFromFloatsToRef(viewM[1], viewM[5], viewM[9], invertedMatrix, camAxisY);
    Vector3::CrossToRef(camAxisY, camAxisZ, camAxisX);
    camAxisY.normalize();
    camAxisX.normalize();
  }

  // if depthSort, compute the camera global position in the mesh local system
  if (_depthSort) {
    Vector3::TransformCoordinatesToRef(_camera->globalPosition(), invertedMatrix,
                                       camInvertedPosition); // then un-rotate the camera
  }

  Matrix::IdentityToRef(rotMatrix);
  auto idx   = 0ull;      // current position index in the global array positions32
  auto index = 0ull;      // position start index in the global array positions32 of
                          // the current particle
  auto colidx     = 0ull; // current color index in the global array colors32
  auto colorIndex = 0ull; // color start index in the global array colors32 of
                          // the current particle
  auto uvidx   = 0ull;    // current uv index in the global array uvs32
  auto uvIndex = 0ull;    // uv start index in the global array uvs32 of the current particle
  auto pt      = 0ull;    // current index in the particle model shape

  if (mesh->isFacetDataEnabled()) {
    _computeBoundingBox = true;
  }

  end = (end >= nbParticles) ? nbParticles - 1 : end;
  if (_computeBoundingBox) {
    if (start != 0 || end != nbParticles - 1) { // only some particles are updated, then
                                                // use the current existing BBox basis.
                                                // Note : it can only increase.
      auto& boundingInfo = mesh->_boundingInfo;
      if (boundingInfo) {
        minimum.copyFrom(boundingInfo->minimum);
        maximum.copyFrom(boundingInfo->maximum);
      }
    }
  }

  // particle loop
  index      = particles[start]->_pos;
  auto vpos  = (index / 3);
  colorIndex = vpos * 4;
  uvIndex    = vpos * 2;

  for (size_t p = start; p <= end; p++) {
    auto particle = particles[p].get();

    // call to custom user function to update the particle properties
    updateParticle(particle);

    auto& shape                  = particle->_model->_shape;
    auto& shapeUV                = particle->_model->_shapeUV;
    auto& particleRotationMatrix = particle->_rotationMatrix;
    auto& particlePosition       = particle->position;
    auto& particleRotation       = particle->rotation;
    auto& particleScaling        = particle->scaling;
    auto& particleGlobalPosition = particle->_globalPosition;

    // camera-particle distance for depth sorting
    if (_depthSort && _depthSortParticles) {
      auto& dsp         = depthSortedParticles[p];
      dsp.ind           = particle->_ind;
      dsp.indicesLength = particle->_model->_indicesLength;
      dsp.sqDistance    = Vector3::DistanceSquared(particle->position, camInvertedPosition);
    }

    // skip the computations for inactive or already invisible particles
    if (!particle->alive || (particle->_stillInvisible && !particle->isVisible)) {
      // increment indexes for the next particle
      pt = shape.size();
      index += pt * 3;
      colorIndex += pt * 4;
      uvIndex += pt * 2;
      continue;
    }

    if (particle->isVisible) {
      particle->_stillInvisible = false; // un-mark permanent invisibility

      auto& scaledPivot = tempVectors[12];
      particle->pivot.multiplyToRef(particleScaling, scaledPivot);

      // particle rotation matrix
      if (billboard) {
        particleRotation.x = 0.f;
        particleRotation.y = 0.f;
      }
      if (_computeParticleRotation || billboard) {
        particle->getRotationMatrix(rotMatrix);
      }

      auto particleHasParent = particle->parentId.has_value();
      if (particleHasParent) {
        auto parent = getParticleById(particle->parentId.value_or(0));
        if (parent) {
          auto& parentRotationMatrix = parent->_rotationMatrix;
          auto& parentGlobalPosition = parent->_globalPosition;

          auto rotatedY = particlePosition.x * parentRotationMatrix[1]
                          + particlePosition.y * parentRotationMatrix[4]
                          + particlePosition.z * parentRotationMatrix[7];
          auto rotatedX = particlePosition.x * parentRotationMatrix[0]
                          + particlePosition.y * parentRotationMatrix[3]
                          + particlePosition.z * parentRotationMatrix[6];
          auto rotatedZ = particlePosition.x * parentRotationMatrix[2]
                          + particlePosition.y * parentRotationMatrix[5]
                          + particlePosition.z * parentRotationMatrix[8];

          particleGlobalPosition.x = parentGlobalPosition.x + rotatedX;
          particleGlobalPosition.y = parentGlobalPosition.y + rotatedY;
          particleGlobalPosition.z = parentGlobalPosition.z + rotatedZ;

          if (_computeParticleRotation || billboard) {
            const auto& rotMatrixValues = rotMatrix.m();
            particleRotationMatrix[0]   = rotMatrixValues[0] * parentRotationMatrix[0]
                                        + rotMatrixValues[1] * parentRotationMatrix[3]
                                        + rotMatrixValues[2] * parentRotationMatrix[6];
            particleRotationMatrix[1] = rotMatrixValues[0] * parentRotationMatrix[1]
                                        + rotMatrixValues[1] * parentRotationMatrix[4]
                                        + rotMatrixValues[2] * parentRotationMatrix[7];
            particleRotationMatrix[2] = rotMatrixValues[0] * parentRotationMatrix[2]
                                        + rotMatrixValues[1] * parentRotationMatrix[5]
                                        + rotMatrixValues[2] * parentRotationMatrix[8];
            particleRotationMatrix[3] = rotMatrixValues[4] * parentRotationMatrix[0]
                                        + rotMatrixValues[5] * parentRotationMatrix[3]
                                        + rotMatrixValues[6] * parentRotationMatrix[6];
            particleRotationMatrix[4] = rotMatrixValues[4] * parentRotationMatrix[1]
                                        + rotMatrixValues[5] * parentRotationMatrix[4]
                                        + rotMatrixValues[6] * parentRotationMatrix[7];
            particleRotationMatrix[5] = rotMatrixValues[4] * parentRotationMatrix[2]
                                        + rotMatrixValues[5] * parentRotationMatrix[5]
                                        + rotMatrixValues[6] * parentRotationMatrix[8];
            particleRotationMatrix[6] = rotMatrixValues[8] * parentRotationMatrix[0]
                                        + rotMatrixValues[9] * parentRotationMatrix[3]
                                        + rotMatrixValues[10] * parentRotationMatrix[6];
            particleRotationMatrix[7] = rotMatrixValues[8] * parentRotationMatrix[1]
                                        + rotMatrixValues[9] * parentRotationMatrix[4]
                                        + rotMatrixValues[10] * parentRotationMatrix[7];
            particleRotationMatrix[8] = rotMatrixValues[8] * parentRotationMatrix[2]
                                        + rotMatrixValues[9] * parentRotationMatrix[5]
                                        + rotMatrixValues[10] * parentRotationMatrix[8];
          }
        }
        else { // in case the parent were removed at some moment
          particle->parentId = std::nullopt;
        }
      }
      else {
        particleGlobalPosition.x = particlePosition.x;
        particleGlobalPosition.y = particlePosition.y;
        particleGlobalPosition.z = particlePosition.z;

        if (_computeParticleRotation || billboard) {
          const auto& rotMatrixValues = rotMatrix.m();
          particleRotationMatrix[0]   = rotMatrixValues[0];
          particleRotationMatrix[1]   = rotMatrixValues[1];
          particleRotationMatrix[2]   = rotMatrixValues[2];
          particleRotationMatrix[3]   = rotMatrixValues[4];
          particleRotationMatrix[4]   = rotMatrixValues[5];
          particleRotationMatrix[5]   = rotMatrixValues[6];
          particleRotationMatrix[6]   = rotMatrixValues[8];
          particleRotationMatrix[7]   = rotMatrixValues[9];
          particleRotationMatrix[8]   = rotMatrixValues[10];
        }
      }

      auto& pivotBackTranslation = tempVectors[11];
      if (particle->translateFromPivot) {
        pivotBackTranslation.setAll(0.f);
      }
      else {
        pivotBackTranslation.copyFrom(scaledPivot);
      }

      // particle vertex loop
      for (pt = 0; pt < shape.size(); ++pt) {
        idx    = index + pt * 3;
        colidx = colorIndex + pt * 4;
        uvidx  = uvIndex + pt * 2;

        auto& tmpVertex = tempVectors[0];
        tmpVertex.copyFrom(shape[pt]);
        if (_computeParticleVertex) {
          updateParticleVertex(particle, tmpVertex, pt);
        }

        // positions
        auto vertexX = tmpVertex.x * particleScaling.x - scaledPivot.x;
        auto vertexY = tmpVertex.y * particleScaling.y - scaledPivot.y;
        auto vertexZ = tmpVertex.z * particleScaling.z - scaledPivot.z;

        auto rotatedX = vertexX * particleRotationMatrix[0] + vertexY * particleRotationMatrix[3]
                        + vertexZ * particleRotationMatrix[6];
        auto rotatedY = vertexX * particleRotationMatrix[1] + vertexY * particleRotationMatrix[4]
                        + vertexZ * particleRotationMatrix[7];
        auto rotatedZ = vertexX * particleRotationMatrix[2] + vertexY * particleRotationMatrix[5]
                        + vertexZ * particleRotationMatrix[8];

        rotatedX += pivotBackTranslation.x;
        rotatedY += pivotBackTranslation.y;
        rotatedZ += pivotBackTranslation.z;

        auto px = positions32[idx] = particleGlobalPosition.x + camAxisX.x * rotatedX
                                     + camAxisY.x * rotatedY + camAxisZ.x * rotatedZ;
        auto py = positions32[idx + 1] = particleGlobalPosition.y + camAxisX.y * rotatedX
                                         + camAxisY.y * rotatedY + camAxisZ.y * rotatedZ;
        auto pz = positions32[idx + 2] = particleGlobalPosition.z + camAxisX.z * rotatedX
                                         + camAxisY.z * rotatedY + camAxisZ.z * rotatedZ;

        if (_computeBoundingBox) {
          minimum.minimizeInPlaceFromFloats(px, py, pz);
          maximum.maximizeInPlaceFromFloats(px, py, pz);
        }

        // normals : if the particles can't be morphed then just rotate the normals, what is much
        // more faster than ComputeNormals()
        if (!_computeParticleVertex) {
          const auto& normalx = fixedNormal32[idx];
          const auto& normaly = fixedNormal32[idx + 1];
          const auto& normalz = fixedNormal32[idx + 2];

          const auto rotatedx = normalx * particleRotationMatrix[0]
                                + normaly * particleRotationMatrix[3]
                                + normalz * particleRotationMatrix[6];
          const auto rotatedy = normalx * particleRotationMatrix[1]
                                + normaly * particleRotationMatrix[4]
                                + normalz * particleRotationMatrix[7];
          const auto rotatedz = normalx * particleRotationMatrix[2]
                                + normaly * particleRotationMatrix[5]
                                + normalz * particleRotationMatrix[8];

          normals32[idx] = camAxisX.x * rotatedx + camAxisY.x * rotatedy + camAxisZ.x * rotatedz;
          normals32[idx + 1]
            = camAxisX.y * rotatedx + camAxisY.y * rotatedy + camAxisZ.y * rotatedz;
          normals32[idx + 2]
            = camAxisX.z * rotatedx + camAxisY.z * rotatedy + camAxisZ.z * rotatedz;
        }

        if (_computeParticleColor && particle->color.has_value()) {
          const auto& color     = particle->color.value();
          _colors32[colidx]     = color.r;
          _colors32[colidx + 1] = color.g;
          _colors32[colidx + 2] = color.b;
          _colors32[colidx + 3] = color.a;
        }

        if (_computeParticleTexture) {
          const auto& uvs  = particle->uvs;
          uvs32[uvidx]     = shapeUV[pt * 2] * (uvs.z - uvs.x) + uvs.x;
          uvs32[uvidx + 1] = shapeUV[pt * 2 + 1] * (uvs.w - uvs.y) + uvs.y;
        }
      }
    }
    // particle just set invisible : scaled to zero and positioned at the origin
    else {
      particle->_stillInvisible = true; // mark the particle as invisible
      for (pt = 0; pt < shape.size(); ++pt) {
        idx    = index + pt * 3;
        colidx = colorIndex + pt * 4;
        uvidx  = uvIndex + pt * 2;

        positions32[idx] = positions32[idx + 1] = positions32[idx + 2] = 0;
        normals32[idx] = normals32[idx + 1] = normals32[idx + 2] = 0;
        if (_computeParticleColor && particle->color.has_value()) {
          const auto& color    = particle->color.value();
          colors32[colidx]     = color.r;
          colors32[colidx + 1] = color.g;
          colors32[colidx + 2] = color.b;
          colors32[colidx + 3] = color.a;
        }
        if (_computeParticleTexture) {
          const auto& uvs  = particle->uvs;
          uvs32[uvidx]     = shapeUV[pt * 2] * (uvs.z - uvs.x) + uvs.x;
          uvs32[uvidx + 1] = shapeUV[pt * 2 + 1] * (uvs.w - uvs.y) + uvs.y;
        }
      }
    }

    // if the particle intersections must be computed : update the bbInfo
    if (_particlesIntersect) {
      auto& bInfo             = particle->_boundingInfo;
      auto& bBox              = bInfo->boundingBox;
      auto& bSphere           = bInfo->boundingSphere;
      auto& modelBoundingInfo = particle->_modelBoundingInfo;
      if (!_bSphereOnly) {
        // place, scale and rotate the particle bbox within the SPS local system, then update it
        auto& modelBoundingInfoVectors = modelBoundingInfo->boundingBox.vectors;

        auto& tempMin = tempVectors[1];
        auto& tempMax = tempVectors[2];
        tempMin.setAll(std::numeric_limits<float>::max());
        tempMax.setAll(std::numeric_limits<float>::min());
        for (uint32_t b = 0; b < 8; ++b) {
          const auto scaledX  = modelBoundingInfoVectors[b].x * particleScaling.x;
          const auto scaledY  = modelBoundingInfoVectors[b].y * particleScaling.y;
          const auto scaledZ  = modelBoundingInfoVectors[b].z * particleScaling.z;
          const auto rotatedX = scaledX * particleRotationMatrix[0]
                                + scaledY * particleRotationMatrix[3]
                                + scaledZ * particleRotationMatrix[6];
          const auto rotatedY = scaledX * particleRotationMatrix[1]
                                + scaledY * particleRotationMatrix[4]
                                + scaledZ * particleRotationMatrix[7];
          const auto rotatedZ = scaledX * particleRotationMatrix[2]
                                + scaledY * particleRotationMatrix[5]
                                + scaledZ * particleRotationMatrix[8];
          const auto x = particlePosition.x + camAxisX.x * rotatedX + camAxisY.x * rotatedY
                         + camAxisZ.x * rotatedZ;
          const auto y = particlePosition.y + camAxisX.y * rotatedX + camAxisY.y * rotatedY
                         + camAxisZ.y * rotatedZ;
          const auto z = particlePosition.z + camAxisX.z * rotatedX + camAxisY.z * rotatedY
                         + camAxisZ.z * rotatedZ;
          tempMin.minimizeInPlaceFromFloats(x, y, z);
          tempMax.maximizeInPlaceFromFloats(x, y, z);
        }

        bBox.reConstruct(tempMin, tempMax, mesh->_worldMatrix);
      }

      // place and scale the particle bouding sphere in the SPS local system, then update it
      auto minBbox = modelBoundingInfo->minimum().multiplyToRef(particleScaling, tempVectors[1]);
      auto maxBbox = modelBoundingInfo->maximum().multiplyToRef(particleScaling, tempVectors[2]);

      const auto bSphereCenter = maxBbox.addToRef(minBbox, tempVectors[3])
                                   .scaleInPlace(0.5f)
                                   .addInPlace(particleGlobalPosition);
      const auto halfDiag
        = maxBbox.subtractToRef(minBbox, tempVectors[4]).scaleInPlace(0.5f * _bSphereRadiusFactor);
      const auto bSphereMinBbox = bSphereCenter.subtractToRef(halfDiag, tempVectors[1]);
      const auto bSphereMaxBbox = bSphereCenter.addToRef(halfDiag, tempVectors[2]);
      bSphere.reConstruct(bSphereMinBbox, bSphereMaxBbox, mesh->_worldMatrix);
    }

    // increment indexes for the next particle
    index      = idx + 3;
    colorIndex = colidx + 4;
    uvIndex    = uvidx + 2;
  }

  // if the VBO must be updated
  if (update) {
    if (_computeParticleColor) {
      mesh->updateVerticesData(VertexBuffer::ColorKind, colors32, false, false);
    }
    if (_computeParticleTexture) {
      mesh->updateVerticesData(VertexBuffer::UVKind, uvs32, false, false);
    }
    mesh->updateVerticesData(VertexBuffer::PositionKind, positions32, false, false);
    if (!mesh->areNormalsFrozen || mesh->isFacetDataEnabled) {
      if (_computeParticleVertex || mesh->isFacetDataEnabled) {
        // recompute the normals only if the particles can be morphed, update then also the normal
        // reference array _fixedNormal32[]
        if (mesh->isFacetDataEnabled) {
          VertexData::ComputeNormals(positions32, indices32, normals32,
                                     mesh->getFacetDataParameters());
        }
        else {
          VertexData::ComputeNormals(positions32, indices32, normals32, std::nullopt);
        }
        for (size_t i = 0; i < normals32.size(); ++i) {
          fixedNormal32[i] = normals32[i];
        }
      }
      if (!mesh->areNormalsFrozen) {
        mesh->updateVerticesData(VertexBuffer::NormalKind, normals32, false, false);
      }
    }
    if (_depthSort && _depthSortParticles) {
      std::sort(depthSortedParticles.begin(), depthSortedParticles.end(), _depthSortFunction);
      const auto dspl = depthSortedParticles.size();
      auto sid        = 0ull;
      for (size_t sorted = 0; sorted < dspl; ++sorted) {
        const auto lind = depthSortedParticles[sorted].indicesLength;
        const auto sind = depthSortedParticles[sorted].ind;
        for (size_t i = 0; i < lind; i++) {
          indices32[sid] = indices[sind + i];
          ++sid;
        }
      }
      mesh->updateIndices(indices32);
    }
  }
  if (_computeBoundingBox) {
    if (mesh->_boundingInfo) {
      mesh->_boundingInfo->reConstruct(minimum, maximum, mesh->_worldMatrix);
    }
    else {
      mesh->_boundingInfo = std::make_unique<BoundingInfo>(minimum, maximum, mesh->_worldMatrix);
    }
  }
  if (_autoUpdateSubMeshes) {
    computeSubMeshes();
  }
  afterUpdateParticles(start, end, update);
  return *this;
}

void SolidParticleSystem::dispose(bool /*doNotRecurse*/, bool /*disposeMaterialAndTextures*/)
{
  mesh->dispose();
  // drop references to internal big arrays for the GC
  _positions.clear();
  _indices.clear();
  _normals.clear();
  _uvs.clear();
  _colors.clear();
  _indices32.clear();
  _positions32.clear();
  _normals32.clear();
  _fixedNormal32.clear();
  _uvs32.clear();
  _colors32.clear();
  pickedParticles.clear();
}

SolidParticlePtr SolidParticleSystem::getParticleById(size_t id)
{
  if (id < particles.size() && particles[id] && particles[id]->id == static_cast<int>(id)) {
    const auto& p = particles[id];
    return p;
  }
  if (id < _idxOfId.size()) {
    const auto idx = _idxOfId[id];
    return particles[idx];
  }
  auto i        = 0ull;
  const auto nb = nbParticles;
  while (i < nb) {
    const auto& particle = particles[i];
    if (particle->id == static_cast<int>(id)) {
      return particle;
    }
    ++i;
  }
  return nullptr;
}

std::vector<SolidParticlePtr> SolidParticleSystem::getParticlesByShapeId(int shapeId)
{
  std::vector<SolidParticlePtr> ref;
  getParticlesByShapeIdToRef(shapeId, ref);
  return ref;
}

SolidParticleSystem&
SolidParticleSystem::getParticlesByShapeIdToRef(int shapeId, std::vector<SolidParticlePtr>& ref)
{
  ref.clear();
  for (size_t i = 0; i < nbParticles; ++i) {
    const auto& p = particles[i];
    if (p->shapeId == shapeId) {
      ref.emplace_back(p);
    }
  }
  return *this;
}

SolidParticleSystem& SolidParticleSystem::computeSubMeshes()
{
  if (!mesh || !_multimaterialEnabled) {
    return *this;
  }
  if (!particles.empty()) {
    for (size_t p = 0; p < particles.size(); ++p) {
      const auto& part = particles[p];
      if (!part->materialIndex.has_value()) {
        part->materialIndex = 0;
      }
      auto& sortedPart         = depthSortedParticles[p];
      sortedPart.materialIndex = part->materialIndex.value_or(0);
      sortedPart.ind           = part->_ind;
      sortedPart.indicesLength = part->_model->_indicesLength;
    }
  }
  _sortParticlesByMaterial();
  const auto& indicesByMaterial = _indicesByMaterial;
  const auto& materialIndexes   = _materialIndexes;
  mesh->subMeshes               = {};
  const auto vcount             = mesh->getTotalVertices();
  for (size_t m = 0; m < materialIndexes.size(); ++m) {
    const auto start    = static_cast<uint32_t>(indicesByMaterial[m]);
    const auto count    = static_cast<uint32_t>(indicesByMaterial[m + 1] - start);
    const auto matIndex = static_cast<uint32_t>(materialIndexes[m]);
    SubMesh::New(matIndex, 0, vcount, start, count, mesh);
  }
  return *this;
}

SolidParticleSystem& SolidParticleSystem::_sortParticlesByMaterial()
{
  std::vector<size_t> indicesByMaterial = {0};
  _indicesByMaterial                    = indicesByMaterial;
  std::vector<size_t> materialIndexes;
  _materialIndexes = materialIndexes;
  std::sort(depthSortedParticles.begin(), depthSortedParticles.end(), _materialSortFunction);
  auto length       = depthSortedParticles.size();
  auto& indices32   = _indices32;
  auto& indices     = _indices;
  auto sid          = 0ull;
  auto lastMatIndex = depthSortedParticles[0].materialIndex;
  materialIndexes.emplace_back(lastMatIndex);
  for (size_t sorted = 0; sorted < length; ++sorted) {
    auto& sortedPart = depthSortedParticles[sorted];
    auto lind        = sortedPart.indicesLength;
    auto sind        = sortedPart.ind;
    if (sortedPart.materialIndex != lastMatIndex) {
      lastMatIndex = sortedPart.materialIndex;
      indicesByMaterial.emplace_back(sid);
      materialIndexes.emplace_back(lastMatIndex);
    }
    for (size_t i = 0; i < lind; i++) {
      indices32[sid] = indices[sind + i];
      sid++;
    }
  }
  indicesByMaterial.emplace_back(
    indices32
      .size()); // add the last number to ease the indices start/count values for subMeshes creation
  if (_updatable) {
    mesh->updateIndices(indices32);
  }
  return *this;
}

void SolidParticleSystem::_setMaterialIndexesById()
{
  _materialIndexesById = {};
  for (size_t i = 0; i < _materials.size(); ++i) {
    auto id                  = _materials[i]->uniqueId;
    _materialIndexesById[id] = i;
  }
}

std::vector<MaterialPtr>
SolidParticleSystem::_filterUniqueMaterialId(const std::vector<MaterialPtr>& array)
{
  std::vector<MaterialPtr> filtered;
  for (const auto& value : array) {
    stl_util::push_unique(filtered, value);
  }
  return filtered;
}

MaterialPtr SolidParticleSystem::_setDefaultMaterial()
{
  if (!_defaultMaterial) {
    _defaultMaterial = StandardMaterial::New(name + "DefaultMaterial", _scene);
  }
  return _defaultMaterial;
}

SolidParticleSystem& SolidParticleSystem::refreshVisibleSize()
{
  if (!_isVisibilityBoxLocked) {
    mesh->refreshBoundingInfo();
  }
  return *this;
}

void SolidParticleSystem::setVisibilityBox(float size)
{
  auto vis = size / 2.f;
  mesh->_boundingInfo
    = std::make_shared<BoundingInfo>(Vector3(-vis, -vis, -vis), Vector3(vis, vis, vis));
}

bool SolidParticleSystem::isAlwaysVisible() const
{
  return _alwaysVisible;
}

void SolidParticleSystem::setIsAlwaysVisible(bool val)
{
  _alwaysVisible                 = val;
  mesh->alwaysSelectAsActiveMesh = val;
}

void SolidParticleSystem::setIsVisibilityBoxLocked(bool val)
{
  _isVisibilityBoxLocked = val;

  auto& boundingInfo = *mesh->getBoundingInfo();

  boundingInfo.isLocked = val;
}

bool SolidParticleSystem::SolidParticleSystem::isVisibilityBoxLocked() const
{
  return _isVisibilityBoxLocked;
}

void SolidParticleSystem::set_computeParticleRotation(bool val)
{
  _computeParticleRotation = val;
}

void SolidParticleSystem::set_computeParticleColor(bool val)
{
  _computeParticleColor = val;
}

void SolidParticleSystem::set_computeParticleTexture(bool val)
{
  _computeParticleTexture = val;
}

void SolidParticleSystem::set_computeParticleVertex(bool val)
{
  _computeParticleVertex = val;
}

void SolidParticleSystem::set_computeBoundingBox(bool val)
{
  _computeBoundingBox = val;
}

void SolidParticleSystem::set_depthSortParticles(bool val)
{
  _depthSortParticles = val;
}

bool SolidParticleSystem::get_computeParticleRotation() const
{
  return _computeParticleRotation;
}

bool SolidParticleSystem::get_computeParticleColor() const
{
  return _computeParticleColor;
}

bool SolidParticleSystem::get_computeParticleTexture() const
{
  return _computeParticleTexture;
}

bool SolidParticleSystem::get_computeParticleVertex() const
{
  return _computeParticleVertex;
}

bool SolidParticleSystem::get_computeBoundingBox() const
{
  return _computeBoundingBox;
}

bool SolidParticleSystem::get_depthSortParticles() const
{
  return _depthSortParticles;
}

bool SolidParticleSystem::get_expandable() const
{
  return _expandable;
}

bool SolidParticleSystem::get_multimaterialEnabled() const
{
  return _multimaterialEnabled;
}

bool SolidParticleSystem::get_useModelMaterial() const
{
  return _useModelMaterial;
}

std::vector<MaterialPtr>& SolidParticleSystem::get_materials()
{
  return _materials;
}

void SolidParticleSystem::setMultiMaterial(const std::vector<MaterialPtr>& iMaterials)
{
  _materials = _filterUniqueMaterialId(iMaterials);
  _setMaterialIndexesById();
  if (_multimaterial) {
    _multimaterial->dispose();
  }
  _multimaterial = MultiMaterial::New(name + "MultiMaterial", _scene);
  for (const auto& material : _materials) {
    _multimaterial->subMaterials().emplace_back(material);
  }
  computeSubMeshes();
  mesh->material = _multimaterial;
}

void SolidParticleSystem::initParticles()
{
}

MultiMaterialPtr& SolidParticleSystem::get_multimaterial()
{
  return _multimaterial;
}

void SolidParticleSystem::set_multimaterial(const MultiMaterialPtr& mm)
{
  _multimaterial = mm;
}

bool SolidParticleSystem::get_autoUpdateSubMeshes() const
{
  return _autoUpdateSubMeshes;
}

void SolidParticleSystem::set_autoUpdateSubMeshes(bool val)
{
  _autoUpdateSubMeshes = val;
}

SolidParticle* SolidParticleSystem::recycleParticle(SolidParticle* particle)
{
  return particle;
}

SolidParticle* SolidParticleSystem::updateParticle(SolidParticle* particle)
{
  return particle;
}

Vector3 SolidParticleSystem::updateParticleVertex(SolidParticle* /*particle*/,
                                                  const Vector3& vertex, size_t /*pt*/)
{
  return vertex;
}

void SolidParticleSystem::beforeUpdateParticles(size_t /*start*/, size_t /*stop*/, bool /*update*/)
{
}

void SolidParticleSystem::afterUpdateParticles(size_t /*start*/, size_t /*stop*/, bool /*update*/)
{
}

} // end of namespace BABYLON
