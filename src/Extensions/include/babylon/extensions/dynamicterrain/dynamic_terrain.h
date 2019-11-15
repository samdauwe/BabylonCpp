#ifndef BABYLON_EXTENSIONS_DYNAMIC_TERRAIN_DYNAMIC_TERRAIN_H
#define BABYLON_EXTENSIONS_DYNAMIC_TERRAIN_DYNAMIC_TERRAIN_H

#include <functional>

#include <babylon/babylon_api.h>
#include <babylon/math/color3.h>
#include <babylon/math/color4.h>
#include <babylon/math/vector2.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class Camera;
class Mesh;
class Scene;
using CameraPtr = std::shared_ptr<Camera>;
using MeshPtr   = std::shared_ptr<Mesh>;

namespace Extensions {

struct DynamicTerrainOptions;

struct DynamicTerrainVertex {
  // vertex position in the terrain space
  Vector3 position = Vector3::Zero();
  // vertex uv
  Vector2 uvs = Vector2::Zero();
  // vertex color
  Color4 color = Color4(1.f, 1.f, 1.f, 1.f);
  // vertex LOD value on X axis
  unsigned int lodX;
  // vertex LOD value on Z axis
  unsigned int lodZ;
  // vertex World position
  Vector3 worldPosition = Vector3::Zero();
  // current map index
  unsigned int mapIndex;
}; // end of struct DynamicTerrainVertex

struct HeightMapOptions {
  // Map width
  float width = 300.f;
  // Map height
  float height = 300.f;
  // The wanted number of points along the map width (default 100)
  unsigned int subX = 100;
  // The wanted number of points along the map height (default 100)
  unsigned int subZ = 100;
  // Minimum altitude of the map (default 0)
  float minHeight = 0.f;
  // Maximum altitude of the map (default 10)
  float maxHeight = 10.f;
  // The filter to apply to the image pixel colors to compute the height
  // (default Color3(0.3, 0.59, 0.11))
  Color3 colorFilter = Color3(0.3f, 0.59f, 0.11f);
  // Offset in the X direction
  float offsetX = 0.f;
  // Offset in the Z direction
  float offsetZ = 0.f;
  // An optional callback function, called once the map is computed. It's passed
  // the computed map.
  std::function<void(const Float32Array& map, unsigned int subX,
                     unsigned int subZ)>
    onReady = nullptr;
}; // end of struct HeightMapOptions

/**
 * @brief The DynamicTerrain class provides a way to display a terrain
 * dynamically morphed from a map of 3D data.
 *
 * The dynamic terrain is a mesh that morphs on a logical data map.
 * This map is a simple flat array of successive 3D coordinates (x, y, z) as
 * floats. It can be as huge as you need, as long as you've enough memory. This
 * won't be rendered anyway. The map must be passed to the dynamic terrain
 * constructor as well as the number of subdivisions on the map width and
 * height.
 */
class DynamicTerrain {

public:
  /**
   * @brief Constructor.
   * @param name
   * @param options
   * @param scene
   */
  DynamicTerrain(const std::string& name, DynamicTerrainOptions& options,
                 Scene* scene);
  virtual ~DynamicTerrain(); // = default

  /**
   * @brief Updates the terrain position and shape according to the camera
   * position.
   * @param force Forces the terrain update even if no camera position change.
   * @returns The terrain.
   */
  DynamicTerrain& update(bool force);

  /**
   * @brief Updates the mesh terrain size according to the LOD limits and the
   * camera position.
   * Returns The terrain.
   */
  DynamicTerrain& updateTerrainSize();

  /**
   * @brief Returns the altitude (float) at the coordinates (x, z) of the map.
   * @param x
   * @param z
   * @param {normal: Vector3} (optional)
   * If the optional object {normal: Vector3} is passed, then its property
   * "normal" is updated with the normal vector value at the coordinates (x, z).
   * @returns The altitude (float) at the coordinates (x, z) of the passed map.
   */
  float getHeightFromMap(float x, float z, const Vector3& normal) const;

  /**
   * @brief Returns the altitude (float) at the coordinates (x, z) of the passed
   * map.
   * @param x
   * @param z
   * @param mapSubX the number of points along the map width
   * @param mapSubX the number of points along the map height
   * @param {normal: Vector3} (optional)
   * If the optional object {normal: Vector3} is passed, then its property
   * "normal" is updated with the normal vector value at the coordinates (x, z).
   * @returns The altitude (float) at the coordinates (x, z) of the passed map.
   */
  static float GetHeightFromMap(float x, float z, const Float32Array& mapData,
                                unsigned int mapSubX, unsigned int mapSubZ,
                                const Vector3& normal = Vector3::Zero());

  /**
   * @brief Computes the height and optionnally the normal at the coordinates
   * (x,z) from the passed map.
   * @returns The altitude at the coordinates (x, z) of the map.
   */
  static float _GetHeightFromMap(float x, float z, const Float32Array& mapData,
                                 unsigned int mapSubX, unsigned int mapSubZ,
                                 float mapSizeX, float mapSizeZ,
                                 const Vector3& normal = Vector3::Zero());

  /**
   * @brief Computes all the normals from the terrain data map  and stores them
   * in the passed Float32Array reference.
   * This passed array must have the same size than the mapData array.
   */
  static void ComputeNormalsFromMapToRef(const Float32Array& mapData,
                                         unsigned int mapSubX,
                                         unsigned int mapSubZ,
                                         Float32Array& normals);

  /**
   * @brief Computes all the map normals from the current terrain data map and
   * sets them to the terrain.
   * @returns The terrain.
   */
  DynamicTerrain& computeNormalsFromMap();

  /**
   * @brief Returns true if the World coordinates (x, z) are in the current
   * terrain.
   * @param x
   * @param z
   * @return
   */
  bool contains(float x, float z);

  /**
   * @brief Returns a new data map from the passed heightmap image file.
   * @param width map width size (positive float, default 300)
   * @param height map height size (positive float, default 300)
   * @param subX is the wanted number of points along the map width (default
   * 100).
   * @param subZ is the wanted number of points along the map height (default
   * 100).
   * @param minHeight is the minimum altitude of the map (float, default 0).
   * @param maxHeight is the maximum altitude of the map (float, default 10).
   * @param colorFilter is the filter to apply to the image pixel colors to
   * compute the height (optional Color3, default (0.3, 0.59, 0.11) ).
   * @param onReady is an optional callback function, called once the map is
   * computed. It's passed the computed map.
   * @param scene is the Scene object whose database will store the downloaded
   * image.
   */
  static Float32Array CreateMapFromHeightMap(const std::string& heightmapURL,
                                             const HeightMapOptions& options,
                                             Scene* scene);

  /**
   * @brief Updates the passed array or Float32Array with a data map computed
   * from the passed heightmap image file.
   * @param width map width size (positive float, default 300)
   * @param height map height size (positive float, default 300)
   * @param subX is the wanted number of points along the map width (default
   * 100).
   * @param subZ is the wanted number of points along the map height (default
   * 100).
   * @param minHeight is the minimum altitude of the map (float, default 0).
   * @param maxHeight is the maximum altitude of the map (float, default 10).
   * @param colorFilter is the filter to apply to the image pixel colors to
   * compute the height (optional Color3, default (0.3, 0.59, 0.11) ).
   * @param onReady is an optional callback function, called once the map is
   * computed. It's passed the computed map.
   * @param data Float32Array must be the right size : 3 x subX x subZ.
   * @param scene is the Scene object whose database will store the downloaded
   * image.
   */
  static void CreateMapFromHeightMapToRef(const std::string& heightmapURL,
                                          const HeightMapOptions& options,
                                          Float32Array& data, Scene* scene);

  /**
   * @brief Updates the passed arrays with UVs values to fit the whole map
   * with
   * subX points along its width and subZ points along its height.
   * The passed array must be the right size : subX x subZ x 2.
   */
  static void CreateUVMapToRef(float subX, float subZ, Float32Array& mapUVs);

  /**
   * @brief Returns a new UV array with values to fit the whole map with subX
   * points along its width and subZ points along its height.
   */
  static Float32Array CreateUVMap(float subX, float subZ);

  /**
   * @brief Computes and sets the terrain UV map with values to fit the whole
   * map.
   * @returns The terrain.
   */
  DynamicTerrain& createUVMap();

  // Getters / Setters

  /**
   * If the terrain must be recomputed every frame.
   */
  bool refreshEveryFrame() const;
  void setRefreshEveryFrame(bool val);

  /**
   * The logical terrain underlying mesh.
   */
  MeshPtr& mesh();

  /**
   * The camera the terrain is linked to
   */
  CameraPtr& camera();
  void setCamera(const CameraPtr& val);

  /**
   * Number of cells flought over by the cam on the X axis before the terrain is
   * updated.
   * Integer greater or equal to 1.
   */
  unsigned int subToleranceX() const;
  void setSubToleranceX(unsigned int val);

  /**
   * Number of cells flought over by the cam on the Z axis before the terrain is
   * updated.
   * Integer greater or equal to 1. Default 1.
   */
  unsigned int subToleranceZ() const;
  void setSubToleranceZ(unsigned int val);

  /**
   * Initial LOD factor value.
   * Integer greater or equal to 1. Default 1.
   */
  unsigned int initialLOD() const;
  void setInitialLOD(unsigned int val);

  /**
   * Current LOD factor value : the lower factor in the terrain.
   * The LOD value is the sum of the initialLOD and the current
   * cameraLODCorrection.
   * Integer greater or equal to 1. Default 1.
   */
  unsigned int LODValue() const;

  /**
   * Camera LOD correction : the factor to add to the initial LOD according to
   * the camera position, movement, etc.
   * Positive integer (default 0)
   */
  unsigned int cameraLODCorrection() const;
  void setCameraLODCorrection(unsigned int val);

  /**
   * Average map and terrain subdivision size on X axis.
   * Returns a float.
   */
  float averageSubSizeX() const;

  /**
   * Average map and terrain subdivision size on Z axis.
   * Returns a float.
   */
  float averageSubSizeZ() const;

  /**
   * Current terrain size on the X axis.
   * Returns a float.
   */
  float terrainSizeX() const;

  /**
   * Current terrain half size on the X axis.
   * Returns a float.
   */
  float terrainHalfSizeX() const;

  /**
   * Current terrain size on the Z axis.
   * Returns a float.
   */
  float terrainSizeZ() const;

  /**
   * Current terrain half size on the Z axis.
   * Returns a float.
   */
  float terrainHalfSizeZ();

  /**
   * Current position of terrain center in its local space.
   * Returns a Vector3.
   */
  const Vector3& centerLocal() const;

  /**
   * Current position of terrain center in the World space.
   * Returns a Vector3.
   */
  const Vector3& centerWorld() const;

  /**
   * The array of the limit values to change the LOD factor.
   * Returns an array of integers or an empty array.
   * This array is always sorted in the descending order once set.
   */
  const Uint32Array& LODLimits() const;
  void LODLimits(Uint32Array ar);

  /**
   * The data of the map.
   * A flat array of successive 3D float coordinates (x, y, z).
   * This property can be set only if a mapData array was passed at construction
   * time.
   */
  const Float32Array& mapData() const;
  void setMapData(const Float32Array& val);

  /**
   * The number of points on the map width.
   * Positive Integer.
   */
  unsigned int mapSubX() const;
  void setMapSubX(unsigned int val);

  /**
   * The number of points on the map height .
   * Positive Integer.
   */
  unsigned int mapSubZ() const;
  void setMapSubZ(unsigned int val);

  /**
   * The map of colors.
   * A flat array of successive floats between 0 and 1 as r,g,b values.
   * This property can be set only if a mapColors array was passed at
   * construction time.
   */
  const Float32Array& mapColors() const;
  void setMapColors(const Float32Array& val);

  /**
   * The map of UVs.
   * A flat array of successive floats between 0 and 1 as (u, v) values.
   * This property can be set only if a mapUVs array was passed at construction
   * time.
   */
  const Float32Array& mapUVs() const;
  void setMapUVs(const Float32Array& val);

  /**
   * The map of normals.
   * A flat array of successive floats as normal vector coordinates (x, y, z) on
   * each map point.
   */
  const Float32Array& mapNormals() const;
  void setMapNormals(const Float32Array& val);

  /**
   * @brief Must the normals be recomputed on each terrain update (default :
   * false).
   * By default, all the map normals are pre-computed on terrain creation.
   */
  bool computeNormals() const;
  void setComputeNormals(bool val);

  /**
   * @brief Will the custom function updateVertex() be called on each terrain
   * update ?
   * Default false.
   */
  bool useCustomVertexFunction() const;
  void useCustomVertexFunction(bool val);

  /**
   * @brief Is the terrain always directly selected for rendering ?
   */
  bool isAlwaysVisible() const;
  void setIsAlwaysVisible(bool val);

  /**
   * @brief When assigning a new data map to the existing, shall the normals be
   * automatically precomputed once ?
   * Default false.
   */
  bool precomputeNormalsFromMap() const;
  void setPrecomputeNormalsFromMap(bool val);

  // User custom functions.
  // These following can be overwritten bu the user to fit his needs.

  /**
   * @brief Custom function called for each terrain vertex and passed the:
   * - current vertex {position: Vector3, uvs: Vector2, color: Color4, lodX:
   * integer, lodZ: integer, worldPosition: Vector3, mapIndex: integer}
   * - i : the vertex index on the terrain x axis
   * - j : the vertex index on the terrain x axis
   * This function is called only if the property useCustomVertexFunction is set
   * to true.
   */
  virtual void updateVertex(DynamicTerrainVertex& vertex, unsigned int i,
                            unsigned j);

  /**
   * @brief Custom function called each frame and passed the terrain camera
   * reference.
   * This should return a positive integer or zero.
   * Returns zero by default.
   */
  virtual unsigned int updateCameraLOD(const CameraPtr& terrainCamera);

  /**
   * @brief Custom function called before each terrain update.
   * The value of reference is passed.
   * Does nothing by default.
   */
  virtual void beforeUpdate(bool refreshEveryFrame);

  /**
   * @brief Custom function called after each terrain update.
   * The value of refreshEveryFrame is passed.
   * Does nothing by default.
   */
  virtual void afterUpdate(bool refreshEveryFrame);

private:
  /**
   * @brief Updates the underlying ribbon.
   */
  void _updateTerrain();

  template <typename T>
  T _mod(T a, T b)
  {
    return ((a % b) + b) % b;
  }

public:
  std::string name;

private:
  // terrain number of subdivisions per axis
  unsigned int _terrainSub;
  // data of the map
  Float32Array _mapData;
  // actual terrain vertex number per axis
  unsigned int _terrainIdx;
  // map number of subdivisions on X axis
  unsigned int _mapSubX;
  // map number of subdivisions on Z axis
  unsigned int _mapSubZ;
  // UV data of the map
  Float32Array _mapUVs;
  // Color data of the map
  Float32Array _mapColors;
  // Normal data of the map
  Float32Array _mapNormals;
  // current scene
  Scene* _scene;
  // how many cells flought over thy the camera on the terrain x axis before
  // update
  unsigned int _subToleranceX;
  // how many cells flought over thy the camera on the terrain z axis before
  // update
  unsigned int _subToleranceZ;
  // array of LOD limits
  Uint32Array _LODLimits;
  // initial LOD value (integer > 0)
  unsigned int _initialLOD;
  // current LOD value : initial + camera correction
  unsigned int _LODValue;
  // LOD correction (integer) according to the camera altitude
  unsigned int _cameraLODCorrection;
  // former correction
  unsigned int _oldCorrection;
  // camera linked to the terrain
  CameraPtr _terrainCamera;
  IndicesArray _indices;
  Float32Array _positions;
  Float32Array _normals;
  Float32Array _colors;
  Float32Array _uvs;
  // camera / terrain x position delta
  float _deltaX;
  // camera-/ terrain z position delta
  float _deltaZ;
  // x delta sign
  int _signX;
  // z delta sign
  int _signZ;
  // map x subdivision delta
  unsigned int _deltaSubX;
  // map z subdivision delta
  unsigned int _deltaSubZ;
  // x shift in world space
  float _mapShiftX;
  // z shift in world space
  float _mapShiftZ;
  // tmp number of map cells flought over by the camera in the delta shift
  unsigned int _mapFlgtNb;
  // the ribbon needs to be recomputed
  bool _needsUpdate;
  // ribbon recomputation + LOD change
  bool _updateLOD;
  // forced ribbon recomputation
  bool _updateForced;
  // to force the terrain computation every frame
  bool _refreshEveryFrame;
  // to allow the call to updateVertex()
  bool _useCustomVertexFunction;
  // boolean : to skip or not the normal computation
  bool _computeNormals;
  // true if an data map is passed as parameter
  bool _datamap;
  // true if an UV map is passed as parameter
  bool _uvmap;
  // true if an color map is passed as parameter
  bool _colormap;
  // current vertex object passed to the user custom function
  DynamicTerrainVertex _vertex;
  // map cell average x size
  float _averageSubSizeX;
  // map cell average z size
  float _averageSubSizeZ;
  // terrain x size
  float _terrainSizeX;
  // terrain y size
  float _terrainSizeZ;
  float _terrainHalfSizeX;
  float _terrainHalfSizeZ;
  // terrain world center position
  Vector3 _centerWorld;
  // terrain local center position
  Vector3 _centerLocal;
  // map x size
  float _mapSizeX;
  // map z size
  float _mapSizeZ;
  // reference to the ribbon
  MeshPtr _terrain;
  bool _isAlwaysVisible;
  bool _precomputeNormalsFromMap;
  // tmp vectors
  static Vector3 _v1;
  static Vector3 _v2;
  static Vector3 _v3;
  static Vector3 _v4;
  static Vector3 _vAvB;
  static Vector3 _vAvC;
  static Vector3 _norm;
  static Vector3 _bbMin;
  static Vector3 _bbMax;

}; // end of class DynamicTerrain

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_DYNAMIC_TERRAIN_DYNAMIC_TERRAIN_H
