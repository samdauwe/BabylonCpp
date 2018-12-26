#include <babylon/extensions/dynamicterrain/dynamic_terrain.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/logging.h>
#include <babylon/engine/scene.h>
#include <babylon/extensions/dynamicterrain/dynamic_terrain_options.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/mesh_builder.h>
#include <babylon/mesh/vertex_buffer.h>
#include <babylon/mesh/vertex_data.h>
#include <babylon/mesh/vertex_data_options.h>
#include <babylon/tools/tools.h>

namespace BABYLON {
namespace Extensions {

Vector3 DynamicTerrain::_v1    = Vector3::Zero();
Vector3 DynamicTerrain::_v2    = Vector3::Zero();
Vector3 DynamicTerrain::_v3    = Vector3::Zero();
Vector3 DynamicTerrain::_v4    = Vector3::Zero();
Vector3 DynamicTerrain::_vAvB  = Vector3::Zero();
Vector3 DynamicTerrain::_vAvC  = Vector3::Zero();
Vector3 DynamicTerrain::_norm  = Vector3::Zero();
Vector3 DynamicTerrain::_bbMin = Vector3::Zero();
Vector3 DynamicTerrain::_bbMax = Vector3::Zero();

DynamicTerrain::DynamicTerrain(const std::string& iName,
                               DynamicTerrainOptions& options, Scene* scene)
    : name{iName}
    , _subToleranceX{1}
    , _subToleranceZ{1}
    , _initialLOD{1}
    , _LODValue{1}
    , _cameraLODCorrection{0}
    , _oldCorrection{0}
    , _deltaX{0.f}
    , _deltaZ{0.f}
    , _signX{0}
    , _signZ{0}
    , _deltaSubX{0}
    , _deltaSubZ{0}
    , _mapShiftX{0.f}
    , _mapShiftZ{0.f}
    , _mapFlgtNb{0}
    , _needsUpdate{false}
    , _updateLOD{false}
    , _updateForced{false}
    , _refreshEveryFrame{false}
    , _useCustomVertexFunction{false}
    , _computeNormals{true}
    , _datamap{false}
    , _uvmap{false}
    , _colormap{false}
    , _vertex{DynamicTerrainVertex{
        Vector3::Zero(),            // vertex position in the terrain space
        Vector2::Zero(),            // vertex uv
        Color4(1.0, 1.0, 1.0, 1.0), // vertex color
        1,                          // vertex LOD value on X axis
        1,                          // vertex LOD value on Z axis
        Vector3::Zero(),            // vertex World position
        0}}
    , _averageSubSizeX{0.f}
    , _averageSubSizeZ{0.f}
    , _terrainSizeX{0.f}
    , _terrainSizeZ{0.f}
    , _terrainHalfSizeX{0.f}
    , _terrainHalfSizeZ{0.f}
    , _centerWorld{Vector3::Zero()}
    , _centerLocal{Vector3::Zero()}
    , _mapSizeX{0.f}
    , _mapSizeZ{0.f}
    , _isAlwaysVisible{false}
    , _precomputeNormalsFromMap{false}
{
  _terrainSub
    = (options.terrainSub > 0) ? static_cast<unsigned>(options.terrainSub) : 60;
  _mapData    = options.mapData;
  _terrainIdx = _terrainSub + 1;
  _mapSubX    = (options.mapSubX > 0) ? static_cast<unsigned>(options.mapSubX) :
                                     _terrainIdx;
  _mapSubZ = (options.mapSubZ > 0) ? static_cast<unsigned>(options.mapSubZ) :
                                     _terrainIdx;
  // if not defined, it will be still populated by default values
  _mapUVs        = options.mapUVs;
  _mapColors     = options.mapColors;
  _scene         = scene;
  _terrainCamera = options.camera ? options.camera : scene->activeCamera;

  // initialize the map arrays if not passed as parameters
  _datamap  = !_mapData.empty() ? true : false;
  _uvmap    = !_mapUVs.empty() ? true : false;
  _colormap = !_mapColors.empty() ? true : false;
  _mapData  = _datamap ? _mapData : Float32Array(_terrainIdx * _terrainIdx * 3);
  _mapUVs   = _uvmap ? _mapUVs : Float32Array(_terrainIdx * _terrainIdx * 2);
  if (_datamap) {
    _mapNormals = !options.mapNormals.empty() ?
                    options.mapNormals :
                    Float32Array(_mapSubX * _mapSubZ * 3);
  }
  else {
    _mapNormals = Float32Array(_terrainIdx * _terrainIdx * 3);
  }

  // Ribbon creation
  std::size_t index    = 0; // current vertex index in the map array
  std::size_t posIndex = 0; // current position (coords) index in the map array
  std::size_t colIndex = 0; // current color index in the color array
  std::size_t uvIndex  = 0; // current uv index in the uv array
  Color4 color;             // current color
  Vector2 uv;               // current uv
  std::size_t terIndex = 0; // current index in the terrain array
  float y              = 0.0;       // current y coordinate
  std::vector<Vector3> terrainPath; // current path
  float u = 0.0;                    // current u of UV
  float v = 0.0;                    // current v of UV
  std::size_t lg
    = _terrainIdx + 1; // augmented length for the UV to finish before
  std::vector<std::vector<Vector3>> terrainData;
  std::vector<Color4> terrainColor;
  std::vector<Vector2> terrainUV;
  for (unsigned int j = 0; j <= _terrainSub; ++j) {
    terrainPath.clear();
    for (unsigned int i = 0; i <= _terrainSub; ++i) {
      index    = _mod(j * 3, _mapSubZ) * _mapSubX + _mod(i * 3, _mapSubX);
      posIndex = index * 3;
      colIndex = index * 3;
      uvIndex  = index * 2;
      terIndex = j * _terrainIdx + i;
      // geometry
      if (_datamap) {
        y = _mapData[posIndex + 1];
      }
      else {
        y                          = 0.f;
        _mapData[3 * terIndex]     = static_cast<float>(i);
        _mapData[3 * terIndex + 1] = y;
        _mapData[3 * terIndex + 2] = static_cast<float>(j);
      }
      terrainPath.emplace_back(
        Vector3(static_cast<float>(i), y, static_cast<float>(j)));
      // color
      if (_colormap) {
        color = Color4(_mapColors[colIndex], _mapColors[colIndex + 1],
                       _mapColors[colIndex + 2], 1.f);
      }
      else {
        color = Color4(1.f, 1.f, 1.f, 1.f);
      }
      terrainColor.emplace_back(color);
      // uvs
      if (_uvmap) {
        uv = Vector2(_mapUVs[uvIndex], _mapUVs[uvIndex + 1]);
      }
      else {
        u                         = 1.f - std::abs(1.f - 2.f * i / lg);
        v                         = 1.f - std::abs(1.f - 2.f * j / lg);
        _mapUVs[2 * terIndex]     = u;
        _mapUVs[2 * terIndex + 1] = v;
        uv                        = Vector2(u, v);
      }
      terrainUV.emplace_back(uv);
    }
    terrainData.emplace_back(terrainPath);
  }

  _mapSizeX = std::abs(_mapData[(_mapSubX - 1) * 3] - _mapData[0]);
  _mapSizeZ
    = std::abs(_mapData[(_mapSubZ - 1) * _mapSubX * 3 + 2] - _mapData[2]);
  _averageSubSizeX = _mapSizeX / _mapSubX;
  _averageSubSizeZ = _mapSizeZ / _mapSubZ;

  RibbonOptions ribbonOptions(terrainData);
  ribbonOptions.sideOrientation
    = (options.invertSide) ? Mesh::FRONTSIDE : Mesh::BACKSIDE;
  ribbonOptions.colors    = terrainColor;
  ribbonOptions.uvs       = terrainUV;
  ribbonOptions.updatable = true;

  _terrain   = MeshBuilder::CreateRibbon("terrain", ribbonOptions, _scene);
  _indices   = _terrain->getIndices();
  _positions = _terrain->getVerticesData(VertexBuffer::PositionKind);
  _normals   = _terrain->getVerticesData(VertexBuffer::NormalKind);
  _uvs       = _terrain->getVerticesData(VertexBuffer::UVKind);
  _colors    = _terrain->getVerticesData(VertexBuffer::ColorKind);
  computeNormalsFromMap();

  // update it immediatly and register the update callback function in the
  // render loop
  update(true);
  _terrain->position().x
    = _terrainCamera->globalPosition().x - _terrainHalfSizeX;
  _terrain->position().z
    = _terrainCamera->globalPosition().z - _terrainHalfSizeZ;
  // initialize deltaSub to make
  float deltaNbSubX = (_terrain->position().x - _mapData[0]) / _averageSubSizeX;
  float deltaNbSubZ = (_terrain->position().z - _mapData[2]) / _averageSubSizeZ;
  _deltaSubX        = (deltaNbSubX > 0.f) ?
                 static_cast<unsigned>(std::floor(deltaNbSubX)) :
                 static_cast<unsigned>(std::ceil(deltaNbSubX));
  _deltaSubZ = (deltaNbSubZ > 0.f) ?
                 static_cast<unsigned>(std::floor(deltaNbSubZ)) :
                 static_cast<unsigned>(std::ceil(deltaNbSubZ));
  _scene->registerBeforeRender([this](Scene*, EventState&) {
    beforeUpdate(_refreshEveryFrame);
    update(_refreshEveryFrame);
    afterUpdate(_refreshEveryFrame);
  });
  update(true); // recompute everything once the initial deltas are calculated
}

DynamicTerrain::~DynamicTerrain()
{
}

DynamicTerrain& DynamicTerrain::update(bool force)
{
  _needsUpdate  = false;
  _updateLOD    = false;
  _updateForced = (force) ? true : false;
  _deltaX       = _terrainHalfSizeX + _terrain->position().x
            - _terrainCamera->globalPosition().x;
  _deltaZ = _terrainHalfSizeZ + _terrain->position().z
            - _terrainCamera->globalPosition().z;
  _oldCorrection       = _cameraLODCorrection;
  _cameraLODCorrection = updateCameraLOD(_terrainCamera);
  _updateLOD           = (_oldCorrection != _cameraLODCorrection);

  _LODValue  = _initialLOD + _cameraLODCorrection;
  _LODValue  = (_LODValue > 0) ? _LODValue : 1;
  _mapShiftX = _averageSubSizeX * _subToleranceX * _LODValue;
  _mapShiftZ = _averageSubSizeZ * _subToleranceZ * _LODValue;

  if (std::abs(_deltaX) > _mapShiftX) {
    _signX     = (_deltaX > 0.f) ? -1 : 1;
    _mapFlgtNb = static_cast<unsigned>(std::abs(_deltaX / _mapShiftX));
    _terrain->position().x += _mapShiftX * _signX * _mapFlgtNb;
    if (_signX == 1) {
      _deltaSubX += (_subToleranceX * _LODValue * _mapFlgtNb);
    }
    else if (_signZ == -1) {
      _deltaSubX -= (_subToleranceX * _LODValue * _mapFlgtNb);
    }
    _needsUpdate = true;
  }
  if (std::abs(_deltaZ) > _mapShiftZ) {
    _signZ     = (_deltaZ > 0.f) ? -1 : 1;
    _mapFlgtNb = static_cast<unsigned>(std::abs(_deltaZ / _mapShiftZ));
    _terrain->position().z += _mapShiftZ * _signZ * _mapFlgtNb;
    if (_signZ == 1) {
      _deltaSubZ += (_subToleranceZ * _LODValue * _mapFlgtNb);
    }
    else if (_signZ == -1) {
      _deltaSubZ -= (_subToleranceZ * _LODValue * _mapFlgtNb);
    }
    _needsUpdate = true;
  }
  if (_needsUpdate || _updateLOD || _updateForced) {
    _deltaSubX = _mod(_deltaSubX, _mapSubX);
    _deltaSubZ = _mod(_deltaSubZ, _mapSubZ);
    _updateTerrain();
  }
  _updateForced  = false;
  _updateLOD     = false;
  _centerLocal.x = _terrainHalfSizeX;
  _centerLocal.y = _terrain->position().y;
  _centerLocal.z = _terrainHalfSizeZ;
  _centerWorld.x = _terrain->position().x + _terrainHalfSizeX;
  _centerWorld.y = _terrain->position().y;
  _centerWorld.z = _terrain->position().z + _terrainHalfSizeZ;
  return *this;
}

void DynamicTerrain::_updateTerrain()
{
  unsigned int stepJ        = 0;
  unsigned int stepI        = 0;
  unsigned int LODLimitDown = 0;
  unsigned int LODLimitUp   = 0;
  unsigned int LODValue     = _LODValue;
  unsigned int lodI         = LODValue;
  unsigned int lodJ         = LODValue;
  unsigned int l            = 0;
  unsigned int index        = 0; // current vertex index in the map data array
  unsigned int posIndex     = 0; // current position index in the map data array
  unsigned int colIndex     = 0; // current index in the map color array
  unsigned int uvIndex      = 0; // current index in the map uv array
  unsigned int terIndex = 0;  // current vertex index in the terrain map array
                              // when used as a data map
  unsigned int ribbonInd = 0; // current ribbon vertex index
  unsigned int ribbonPosInd
    = 0; // current ribbon position index (same than normal index)
  unsigned int ribbonUVInd   = 0; // current ribbon UV index
  unsigned int ribbonColInd  = 0; // current ribbon color index
  unsigned int ribbonPosInd1 = 0;
  unsigned int ribbonPosInd2 = 0;
  unsigned int ribbonPosInd3 = 0;
  // note : all the indexes are explicitly set as integers for the js optimizer
  // (store them all in the stack)

  if (_updateLOD || _updateForced) {
    updateTerrainSize();
  }
  Vector3::FromFloatsToRef(std::numeric_limits<float>::max(),
                           std::numeric_limits<float>::max(),
                           std::numeric_limits<float>::max(), _bbMin);
  Vector3::FromFloatsToRef(std::numeric_limits<float>::lowest(),
                           std::numeric_limits<float>::lowest(),
                           std::numeric_limits<float>::lowest(), _bbMax);

  for (unsigned int j = 0; j <= _terrainSub; ++j) {
    // LOD Z
    LODValue = _LODValue;
    for (l = 0; l < _LODLimits.size(); ++l) {
      LODLimitDown = _LODLimits[l];
      LODLimitUp   = _terrainSub - LODLimitDown - 1;
      if (j < LODLimitDown || j > LODLimitUp) {
        LODValue = l + 1 + _LODValue;
      }
      lodJ = LODValue;
    }

    for (unsigned int i = 0; i <= _terrainSub; ++i) {
      // LOD X
      LODValue = _LODValue;
      for (l = 0; l < _LODLimits.size(); ++l) {
        LODLimitDown = _LODLimits[l];
        LODLimitUp   = _terrainSub - LODLimitDown - 1;
        if (i < LODLimitDown || i > LODLimitUp) {
          LODValue = l + 1 + _LODValue;
        }
        lodI = LODValue;
      }

      // map current index
      index = _mod(_deltaSubZ + stepJ, _mapSubZ) * _mapSubX
              + _mod(_deltaSubX + stepI, _mapSubX);
      terIndex = _mod(_deltaSubZ + stepJ, _terrainIdx) * _terrainIdx
                 + _mod(_deltaSubX + stepI, _terrainIdx);

      // related index in the array of positions (data map)
      if (_datamap) {
        posIndex = 3 * index;
      }
      else {
        posIndex = 3 * terIndex;
      }
      // related index in the UV map
      if (_uvmap) {
        uvIndex = 2 * index;
      }
      else {
        uvIndex = 2 * terIndex;
      }
      // related index in the color map
      if (_colormap) {
        colIndex = 3 * index;
      }
      else {
        colIndex = 3 * terIndex;
      }
      // ribbon indexes
      ribbonPosInd  = 3 * ribbonInd;
      ribbonColInd  = 4 * ribbonInd;
      ribbonUVInd   = 2 * ribbonInd;
      ribbonPosInd1 = ribbonPosInd;
      ribbonPosInd2 = ribbonPosInd + 1;
      ribbonPosInd3 = ribbonPosInd + 2;
      ribbonInd += 1;

      // geometry
      _positions[ribbonPosInd1] = _averageSubSizeX * stepI;
      _positions[ribbonPosInd2] = _mapData[posIndex + 1];
      _positions[ribbonPosInd3] = _averageSubSizeZ * stepJ;

      if (!_computeNormals) {
        _normals[ribbonPosInd1] = _mapNormals[posIndex];
        _normals[ribbonPosInd2] = _mapNormals[posIndex + 1];
        _normals[ribbonPosInd3] = _mapNormals[posIndex + 2];
      }

      // bbox internal update
      if (_positions[ribbonPosInd1] < _bbMin.x) {
        _bbMin.x = _positions[ribbonPosInd1];
      }
      if (_positions[ribbonPosInd1] > _bbMax.x) {
        _bbMax.x = _positions[ribbonPosInd1];
      }
      if (_positions[ribbonPosInd2] < _bbMin.y) {
        _bbMin.y = _positions[ribbonPosInd2];
      }
      if (_positions[ribbonPosInd2] > _bbMax.y) {
        _bbMax.y = _positions[ribbonPosInd2];
      }
      if (_positions[ribbonPosInd3] < _bbMin.z) {
        _bbMin.z = _positions[ribbonPosInd3];
      }
      if (_positions[ribbonPosInd3] > _bbMax.z) {
        _bbMax.z = _positions[ribbonPosInd3];
      }
      // color
      if (_colormap) {
        _colors[ribbonColInd]     = _mapColors[colIndex];
        _colors[ribbonColInd + 1] = _mapColors[colIndex + 1];
        _colors[ribbonColInd + 2] = _mapColors[colIndex + 2];
      }
      // uv : the array _mapUVs is always populated
      _uvs[ribbonUVInd]     = _mapUVs[uvIndex];
      _uvs[ribbonUVInd + 1] = _mapUVs[uvIndex + 1];

      // call to user custom function with the current updated vertex object
      if (_useCustomVertexFunction) {
        _vertex.position.copyFromFloats(_positions[ribbonPosInd1],
                                        _positions[ribbonPosInd2],
                                        _positions[ribbonPosInd3]);
        _vertex.worldPosition.x = _mapData[posIndex];
        _vertex.worldPosition.y = _vertex.position.y;
        _vertex.worldPosition.z = _mapData[posIndex + 2];
        _vertex.lodX            = lodI;
        _vertex.lodZ            = lodJ;
        _vertex.color.r         = _colors[ribbonColInd];
        _vertex.color.g         = _colors[ribbonColInd + 1];
        _vertex.color.b         = _colors[ribbonColInd + 2];
        _vertex.color.a         = _colors[ribbonColInd + 3];
        _vertex.uvs.x           = _uvs[ribbonUVInd];
        _vertex.uvs.y           = _uvs[ribbonUVInd + 1];
        _vertex.mapIndex        = index;
        updateVertex(_vertex, i,
                     j); // the user can modify the array values here
        _colors[ribbonColInd]     = _vertex.color.r;
        _colors[ribbonColInd + 1] = _vertex.color.g;
        _colors[ribbonColInd + 2] = _vertex.color.b;
        _colors[ribbonColInd + 3] = _vertex.color.a;
        _uvs[ribbonUVInd]         = _vertex.uvs.x;
        _uvs[ribbonUVInd + 1]     = _vertex.uvs.y;
        _positions[ribbonPosInd1] = _vertex.position.x;
        _positions[ribbonPosInd2] = _vertex.position.y;
        _positions[ribbonPosInd3] = _vertex.position.z;
      }

      stepI += lodI;
    }
    stepI = 0;
    stepJ += lodJ;
  }

  // ribbon update
  _terrain->updateVerticesData(VertexBuffer::PositionKind, _positions, false,
                               false);
  if (_computeNormals) {
    VertexData::ComputeNormals(_positions, _indices, _normals);
  }
  _terrain->updateVerticesData(VertexBuffer::NormalKind, _normals, false,
                               false);
  _terrain->updateVerticesData(VertexBuffer::UVKind, _uvs, false, false);
  _terrain->updateVerticesData(VertexBuffer::ColorKind, _colors, false, false);
  _terrain->_boundingInfo = std::make_unique<BoundingInfo>(_bbMin, _bbMax);
  _terrain->_boundingInfo->update(_terrain->_worldMatrix);
}

DynamicTerrain& DynamicTerrain::updateTerrainSize()
{
  unsigned int remainder
    = _terrainSub;       // the remaining cells at the general current LOD value
  unsigned int nb   = 0; // nb of cells in the current LOD limit interval
  unsigned int next = 0; // next cell index, if it exists
  unsigned int lod
    = _LODValue + 1; // lod value in the current LOD limit interval
  float tsx = 0.0;   // current sum of cell sizes on x
  float tsz = 0.0;   // current sum of cell sizes on z
  for (unsigned int l = 0; l < _LODLimits.size(); ++l) {
    lod  = _LODValue + l + 1;
    next = (l >= _LODLimits.size() - 1) ? 0 : _LODLimits[l + 1];
    nb   = 2 * (_LODLimits[l] - next);
    tsx += _averageSubSizeX * lod * nb;
    tsz += _averageSubSizeZ * lod * nb;
    remainder -= nb;
  }
  tsx += remainder * _averageSubSizeX * _LODValue;
  tsz += remainder * _averageSubSizeZ * _LODValue;
  _terrainSizeX     = tsx;
  _terrainSizeZ     = tsz;
  _terrainHalfSizeX = tsx * 0.5f;
  _terrainHalfSizeZ = tsz * 0.5f;
  return *this;
}

float DynamicTerrain::getHeightFromMap(float x, float z,
                                       const Vector3& normal) const
{
  return DynamicTerrain::_GetHeightFromMap(x, z, _mapData, _mapSubX, _mapSubZ,
                                           _mapSizeX, _mapSizeZ, normal);
}

float DynamicTerrain::GetHeightFromMap(float x, float z,
                                       const Float32Array& mapData,
                                       unsigned int mapSubX,
                                       unsigned int mapSubZ,
                                       const Vector3& normal)
{
  const float mapSizeX = std::abs(mapData[(mapSubX - 1) * 3] - mapData[0]);
  const float mapSizeZ
    = std::abs(mapData[(mapSubZ - 1) * mapSubX * 3 + 2] - mapData[2]);
  return DynamicTerrain::_GetHeightFromMap(x, z, mapData, mapSubX, mapSubZ,
                                           mapSizeX, mapSizeZ, normal);
}

float DynamicTerrain::_GetHeightFromMap(float x, float z,
                                        const Float32Array& mapData,
                                        unsigned int mapSubX,
                                        unsigned int mapSubZ, float mapSizeX,
                                        float mapSizeZ, const Vector3& normal)
{
  const float x0 = mapData[0];
  const float z0 = mapData[2];

  // reset x and z in the map space so they are between 0 and the axis map size
  x = x - std::floor((x - x0) / mapSizeX) * mapSizeX;
  z = z - std::floor((z - z0) / mapSizeZ) * mapSizeZ;

  const unsigned int col1
    = static_cast<unsigned>(std::floor((x - x0) * mapSubX / mapSizeX));
  const unsigned int row1
    = static_cast<unsigned>(std::floor((z - z0) * mapSubZ / mapSizeZ));
  const unsigned int col2 = (col1 + 1) % mapSubX;
  const unsigned int row2 = (row1 + 1) % mapSubZ;
  // starting indexes of the positions of 4 vertices defining a quad on the map
  const unsigned int idx1 = 3 * (row1 * mapSubX + col1);
  const unsigned int idx2 = 3 * (row1 * mapSubX + col2);
  const unsigned int idx3 = 3 * ((row2)*mapSubX + col1);
  const unsigned int idx4 = 3 * ((row2)*mapSubX + col2);

  DynamicTerrain::_v1.copyFromFloats(mapData[idx1], mapData[idx1 + 1],
                                     mapData[idx1 + 2]);
  DynamicTerrain::_v2.copyFromFloats(mapData[idx2], mapData[idx2 + 1],
                                     mapData[idx2 + 2]);
  DynamicTerrain::_v3.copyFromFloats(mapData[idx3], mapData[idx3 + 1],
                                     mapData[idx3 + 2]);
  DynamicTerrain::_v4.copyFromFloats(mapData[idx4], mapData[idx4 + 1],
                                     mapData[idx4 + 2]);

  Vector3 vA = DynamicTerrain::_v1;
  Vector3 vB;
  Vector3 vC;
  Vector3 v;

  const float xv4v1 = DynamicTerrain::_v4.x - DynamicTerrain::_v1.x;
  const float zv4v1 = DynamicTerrain::_v4.z - DynamicTerrain::_v1.z;
  if (stl_util::almost_equal(xv4v1, 0.f)
      || stl_util::almost_equal(zv4v1, 0.f)) {
    return DynamicTerrain::_v1.y;
  }
  const float cd = zv4v1 / xv4v1;
  const float h  = DynamicTerrain::_v1.z - cd * DynamicTerrain::_v1.x;
  if (z < cd * x + h) {
    vB = DynamicTerrain::_v4;
    vC = DynamicTerrain::_v2;
    v  = vA;
  }
  else {
    vB = DynamicTerrain::_v3;
    vC = DynamicTerrain::_v4;
    v  = vB;
  }
  vB.subtractToRef(vA, DynamicTerrain::_vAvB);
  vC.subtractToRef(vA, DynamicTerrain::_vAvC);
  Vector3::CrossToRef(DynamicTerrain::_vAvB, DynamicTerrain::_vAvC,
                      DynamicTerrain::_norm);
  DynamicTerrain::_norm.normalize();
  if (normal != Vector3::Zero()) {
    auto tmpVector = normal;
    tmpVector.copyFrom(DynamicTerrain::_norm);
  }
  const float d
    = -(DynamicTerrain::_norm.x * v.x + DynamicTerrain::_norm.y * v.y
        + DynamicTerrain::_norm.z * v.z);
  float y = v.y;
  if (!stl_util::almost_equal(DynamicTerrain::_norm.y, 0.f)) {
    y = -(DynamicTerrain::_norm.x * x + DynamicTerrain::_norm.z * z + d)
        / DynamicTerrain::_norm.y;
  }

  return y;
}

void DynamicTerrain::ComputeNormalsFromMapToRef(const Float32Array& mapData,
                                                unsigned int mapSubX,
                                                unsigned int mapSubZ,
                                                Float32Array& normals)
{
  Uint32Array mapIndices;
  auto tmp1Normal = Vector3::Zero();
  auto tmp2Normal = Vector3::Zero();
  unsigned int l  = mapSubX * (mapSubZ - 1);
  for (unsigned int i = 0; i < l; i++) {
    stl_util::concat(mapIndices, {i + 1, i + mapSubX, i});
    stl_util::concat(mapIndices, {i + mapSubX, i + 1, i + mapSubX + 1});
  }
  VertexData::ComputeNormals(mapData, mapIndices, normals);
  // seam process

  unsigned int lastIdx  = (mapSubX - 1) * 3;
  unsigned int colStart = 0;
  unsigned int colEnd   = 0;
  for (unsigned int i = 0; i < mapSubZ; ++i) {
    colStart = i * mapSubX * 3;
    colEnd   = colStart + lastIdx;
    DynamicTerrain::GetHeightFromMap(mapData[colStart], mapData[colStart + 2],
                                     mapData, mapSubX, mapSubZ, tmp1Normal);
    DynamicTerrain::GetHeightFromMap(mapData[colEnd], mapData[colEnd + 2],
                                     mapData, mapSubX, mapSubZ, tmp2Normal);
    tmp1Normal.addInPlace(tmp2Normal).scaleInPlace(0.5f);
    normals[colStart]     = tmp1Normal.x;
    normals[colStart + 1] = tmp1Normal.y;
    normals[colStart + 2] = tmp1Normal.z;
    normals[colEnd]       = tmp1Normal.x;
    normals[colEnd + 1]   = tmp1Normal.y;
    normals[colEnd + 2]   = tmp1Normal.z;
  }
}

DynamicTerrain& DynamicTerrain::computeNormalsFromMap()
{
  DynamicTerrain::ComputeNormalsFromMapToRef(_mapData, _mapSubX, _mapSubZ,
                                             _mapNormals);
  return *this;
}

bool DynamicTerrain::contains(float x, float z)
{
  if (x < _positions[0] + mesh()->position().x
      || x > _positions[3 * _terrainIdx] + mesh()->position().x) {
    return false;
  }
  if (z < _positions[2] + mesh()->position().z
      || z > _positions[3 * _terrainIdx * _terrainIdx + 2]
               + mesh()->position().z) {
    return false;
  }
  return true;
}

Float32Array
DynamicTerrain::CreateMapFromHeightMap(const std::string& heightmapURL,
                                       const HeightMapOptions& options,
                                       Scene* scene)
{
  const auto subX = options.subX;
  const auto subZ = options.subZ;
  Float32Array data(subX * subZ * 3);
  DynamicTerrain::CreateMapFromHeightMapToRef(heightmapURL, options, data,
                                              scene);
  return data;
}

void DynamicTerrain::CreateMapFromHeightMapToRef(
  const std::string& heightmapURL, const HeightMapOptions& options,
  Float32Array& data, Scene* /*scene*/)
{
  const auto& width     = options.width;
  const auto& height    = options.height;
  const auto& subX      = options.subX;
  const auto& subZ      = options.subZ;
  const auto& minHeight = options.minHeight;
  const auto& maxHeight = options.maxHeight;
  const auto& offsetX   = options.offsetX;
  const auto& offsetZ   = options.offsetZ;
  const auto& filter    = options.colorFilter;
  const auto& onReady   = options.onReady;

  const auto onload = [&](const Image& img) {
    // Getting height map data
    // var canvas = document.createElement("canvas");
    // var context = canvas.getContext("2d");
    int bufferWidth  = img.width;
    int bufferHeight = img.height;
    // canvas.width = bufferWidth;
    // canvas.height = bufferHeight;
    // context.drawImage(img, 0, 0);
    // Cast is due to wrong definition in lib.d.ts from ts 1.3 -
    // https://github.com/Microsoft/TypeScript/issues/949
    // var buffer = <Uint8Array>(<any>context.getImageData(0, 0, bufferWidth,
    // bufferHeight).data);
    const Uint8Array& buffer = img.data;
    float x                  = 0.0;
    float y                  = 0.0;
    float z                  = 0.0;
    for (unsigned int row = 0; row < subZ; row++) {
      for (unsigned int col = 0; col < subX; col++) {
        x                = col * width / subX - width * 0.5f;
        z                = row * height / subZ - height * 0.5f;
        float heightmapX = ((x + width * 0.5f) / width * (bufferWidth - 1));
        float heightmapY
          = (bufferHeight - 1)
            - ((z + height * 0.5f) / height * (bufferHeight - 1));
        unsigned int pos
          = static_cast<unsigned>(heightmapX + heightmapY * bufferWidth) * 4;
        float gradient = (buffer[pos] * filter.r + buffer[pos + 1] * filter.g
                          + buffer[pos + 2] * filter.b)
                         / 255.f;
        y                = minHeight + (maxHeight - minHeight) * gradient;
        unsigned int idx = (row * subX + col) * 3;
        data[idx]        = x + offsetX;
        data[idx + 1]    = y;
        data[idx + 2]    = z + offsetZ;
      }
    }

    // callback function if any
    if (onReady) {
      onReady(data, subX, subZ);
    }
  };

  const auto onError
    = [](const std::string& msg, const std::string& /*exception*/) {
        BABYLON_LOG_ERROR("Tools", msg);
      };

  Tools::LoadImageFromUrl(heightmapURL, onload, onError);
}

void DynamicTerrain::CreateUVMapToRef(float subX, float subZ,
                                      Float32Array& mapUVs)
{
  for (float h = 0; h < subZ; h++) {
    for (float w = 0; w < subX; w++) {
      mapUVs[static_cast<std::size_t>(h * subX + w) * 2]     = w / subX;
      mapUVs[static_cast<std::size_t>(h * subX + w) * 2 + 1] = h / subZ;
    }
  }
}

Float32Array DynamicTerrain::CreateUVMap(float subX, float subZ)
{
  Float32Array mapUVs(static_cast<std::size_t>(subX * subZ * 2));
  DynamicTerrain::CreateUVMapToRef(subX, subZ, mapUVs);
  return mapUVs;
}

/**
 * Computes and sets the terrain UV map with values to fit the whole map.
 * Returns the terrain.
 */
DynamicTerrain& DynamicTerrain::createUVMap()
{
  setMapUVs(DynamicTerrain::CreateUVMap(static_cast<float>(_mapSubX),
                                        static_cast<float>(_mapSubZ)));
  return *this;
}

bool DynamicTerrain::refreshEveryFrame() const
{
  return _refreshEveryFrame;
}

void DynamicTerrain::setRefreshEveryFrame(bool val)
{
  _refreshEveryFrame = val;
}

MeshPtr& DynamicTerrain::mesh()
{
  return _terrain;
}

CameraPtr& DynamicTerrain::camera()
{
  return _terrainCamera;
}

void DynamicTerrain::setCamera(const CameraPtr& val)
{
  _terrainCamera = val;
}

unsigned int DynamicTerrain::subToleranceX() const
{
  return _subToleranceX;
}

void DynamicTerrain::setSubToleranceX(unsigned int val)
{
  _subToleranceX = (val > 0) ? val : 1;
}

unsigned int DynamicTerrain::subToleranceZ() const
{
  return _subToleranceZ;
}

void DynamicTerrain::setSubToleranceZ(unsigned int val)
{
  _subToleranceZ = (val > 0) ? val : 1;
}

unsigned int DynamicTerrain::initialLOD() const
{
  return _initialLOD;
}

void DynamicTerrain::setInitialLOD(unsigned int val)
{
  _initialLOD = (val > 0) ? val : 1;
}

unsigned int DynamicTerrain::LODValue() const
{
  return _LODValue;
}

unsigned int DynamicTerrain::cameraLODCorrection() const
{
  return _cameraLODCorrection;
}

void DynamicTerrain::setCameraLODCorrection(unsigned int val)
{
  _cameraLODCorrection = val;
}

float DynamicTerrain::averageSubSizeX() const
{
  return _averageSubSizeX;
}

float DynamicTerrain::averageSubSizeZ() const
{
  return _averageSubSizeZ;
}

float DynamicTerrain::terrainSizeX() const
{
  return _terrainSizeX;
}

float DynamicTerrain::terrainHalfSizeX() const
{
  return _terrainHalfSizeX;
}

float DynamicTerrain::terrainSizeZ() const
{
  return _terrainSizeZ;
}

float DynamicTerrain::terrainHalfSizeZ()
{
  return _terrainHalfSizeZ;
}

const Vector3& DynamicTerrain::centerLocal() const
{
  return _centerLocal;
}

const Vector3& DynamicTerrain::centerWorld() const
{
  return _centerWorld;
}

const Uint32Array& DynamicTerrain::LODLimits() const
{
  return _LODLimits;
}

void DynamicTerrain::LODLimits(Uint32Array ar)
{
  std::sort(ar.begin(), ar.end(), std::greater<std::uint32_t>());
  _LODLimits = std::move(ar);
}

const Float32Array& DynamicTerrain::mapData() const
{
  return _mapData;
}

void DynamicTerrain::setMapData(const Float32Array& val)
{
  _mapData  = val;
  _datamap  = true;
  _mapSizeX = std::abs(_mapData[(_mapSubX - 1) * 3] - _mapData[0]);
  _mapSizeZ
    = std::abs(_mapData[(_mapSubZ - 1) * _mapSubX * 3 + 2] - _mapData[2]);
  _averageSubSizeX = _mapSizeX / _mapSubX;
  _averageSubSizeZ = _mapSizeZ / _mapSubZ;
  if (_precomputeNormalsFromMap) {
    computeNormalsFromMap();
  }
  update(true);
}

unsigned int DynamicTerrain::mapSubX() const
{
  return _mapSubX;
}

void DynamicTerrain::setMapSubX(unsigned int val)
{
  _mapSubX = val;
}

unsigned int DynamicTerrain::mapSubZ() const
{
  return _mapSubZ;
}

void DynamicTerrain::setMapSubZ(unsigned int val)
{
  _mapSubZ = val;
}

const Float32Array& DynamicTerrain::mapColors() const
{
  return _mapColors;
}

void DynamicTerrain::setMapColors(const Float32Array& val)
{
  _colormap  = true;
  _mapColors = val;
}

const Float32Array& DynamicTerrain::mapUVs() const
{
  return _mapUVs;
}

void DynamicTerrain::setMapUVs(const Float32Array& val)
{
  _uvmap  = true;
  _mapUVs = val;
}

const Float32Array& DynamicTerrain::mapNormals() const
{
  return _mapNormals;
}

void DynamicTerrain::setMapNormals(const Float32Array& val)
{
  _mapNormals = val;
}

bool DynamicTerrain::computeNormals() const
{
  return _computeNormals;
}

void DynamicTerrain::setComputeNormals(bool val)
{
  _computeNormals = val;
}

bool DynamicTerrain::useCustomVertexFunction() const
{
  return _useCustomVertexFunction;
}

void DynamicTerrain::useCustomVertexFunction(bool val)
{
  _useCustomVertexFunction = val;
}

bool DynamicTerrain::isAlwaysVisible() const
{
  return _isAlwaysVisible;
}

void DynamicTerrain::setIsAlwaysVisible(bool val)
{
  mesh()->alwaysSelectAsActiveMesh = val;
  _isAlwaysVisible                 = val;
}

bool DynamicTerrain::precomputeNormalsFromMap() const
{
  return _precomputeNormalsFromMap;
}

void DynamicTerrain::setPrecomputeNormalsFromMap(bool val)
{
  _precomputeNormalsFromMap = val;
}

void DynamicTerrain::updateVertex(DynamicTerrainVertex& /*vertex*/,
                                  unsigned int /*i*/, unsigned /*j*/)
{
}

unsigned int DynamicTerrain::updateCameraLOD(const CameraPtr& /*terrainCamera*/)
{
  // LOD value increases with camera altitude
  unsigned int camLOD = 0;
  return camLOD;
}

void DynamicTerrain::beforeUpdate(bool /*refreshEveryFrame*/)
{
}

void DynamicTerrain::afterUpdate(bool /*refreshEveryFrame*/)
{
}

} // end of namespace Extensions
} // end of namespace BABYLON
