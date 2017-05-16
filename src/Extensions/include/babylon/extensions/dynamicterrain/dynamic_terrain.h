#ifndef BABYLON_EXTENSIONS_DYNAMIC_TERRAIN_DYNAMIC_TERRAIN_H
#define BABYLON_EXTENSIONS_DYNAMIC_TERRAIN_DYNAMIC_TERRAIN_H

#include <babylon/babylon_global.h>
#include <babylon/math/color4.h>
#include <babylon/math/vector2.h>
#include <babylon/math/vector3.h>

namespace BABYLON {
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
}; // end of struct

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
  virtual ~DynamicTerrain();

  /**
   * @brief Updates the terrain position and shape according to the camera
   * position.
   * @param force Forces the terrain update even if no camera position change.
   */
  void update(bool force);

  /**
   * @brief Updates the mesh terrain size according to the LOD limits and the
   * camera position.
   */
  void updateTerrainSize();

  /**
   * @brief Returns the altitude at the coordinates (x, z) of the map.
   * @param x
   * @param z
   * @param normal
   * @return The altitude at the coordinates (x, z) of the map.
   */
  float getHeightFromMap(float x, float z,
                         const Vector3& normal = Vector3::Zero());

  /**
   * @brief Returns true if the World coordinates (x, z) are in the current
   * terrain.
   * @param x
   * @param z
   * @return
   */
  bool contains(float x, float z) const;

  // Getters / Setters

  /**
   * If the terrain must be recomputed every frame.
   */
  bool refreshEveryFrame() const;
  void setRefreshEveryFrame(bool val);

  /**
   * The logical terrain underlying mesh.
   */
  Mesh* mesh() const;

  /**
   * The camera the terrain is linked to
   */
  Camera* camera() const;
  void setCamera(Camera* val);

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
   * Must the normals be recomputed on each terrain update (default : true)
   */
  bool computeNormals() const;
  void setComputeNormals(bool val);

  /**
   * Will the custom function updateVertex() be called on each terrain update ?
   * Default false.
   */
  bool useCustomVertexFunction() const;
  void useCustomVertexFunction(bool val);

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
  virtual unsigned int updateCameraLOD(Camera* terrainCamera);

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
  Camera* _terrainCamera;
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
  Mesh* _terrain;
  // tmp vectors
  Vector3 _v1;
  Vector3 _v2;
  Vector3 _v3;
  Vector3 _v4;
  Vector3 _vAvB;
  Vector3 _vAvC;
  Vector3 _norm;
  Vector3 _bbMin;
  Vector3 _bbMax;

}; // end of class DynamicTerrain

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_DYNAMIC_TERRAIN_DYNAMIC_TERRAIN_H
