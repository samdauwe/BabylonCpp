#ifndef BABYLON_LOADING_PLUGINS_OBJ_OBJ_FILE_LOADER_H
#define BABYLON_LOADING_PLUGINS_OBJ_OBJ_FILE_LOADER_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/loading/plugins/obj/mtl_file_loader.h>
#include <babylon/maths/color4.h>
#include <babylon/maths/vector2.h>

namespace BABYLON {

struct MeshObject {
  std::string name;
  IndicesArray indices;
  Float32Array positions;
  Float32Array normals;
  Float32Array colors;
  Float32Array uvs;
  std::string materialName;
}; // end of struct MeshObject

/**
 * @brief Tuple with indice of Position, Normal, UV  [pos, norm, uvs]
 */
struct TuplePosNormEntry {
  std::vector<size_t> normals = {};
  std::vector<size_t> idx     = {};
  std::vector<size_t> uv      = {};
}; // end of struct MeshObject

struct OBJParseSolidState {
  std::vector<Vector3> positions; // values for the positions of vertices
  std::vector<Vector3> normals;   // Values for the normals
  std::vector<Vector2> uvs;       // Values for the textures
  std::vector<Color4> colors;
  std::vector<MeshObject> meshesFromObj;          // [mesh] Contains all the obj meshes
  MeshObject handledMesh;                         // The current mesh of meshes array
  std::vector<uint32_t> indicesForBabylon;        // The list of indices for VertexData
  std::vector<Vector3> wrappedPositionForBabylon; // The list of position in vectors
  std::vector<Vector2> wrappedUvsForBabylon;      // Array with all value of uvs to match with the
                                                  // indices
  std::vector<Color4>
    wrappedColorsForBabylon; // Array with all color values to match with the indices
  std::vector<Vector3>
    wrappedNormalsForBabylon; // Array with all value of normals to match with the indices
  std::vector<TuplePosNormEntry>
    tuplePosNorm; // Create a tuple with indice of Position, Normal, UV  [pos, norm, uvs]
  size_t curPositionInIndices = 0;
  bool hasMeshes;                            // Meshes are defined in the file
  Float32Array unwrappedPositionsForBabylon; // Value of positionForBabylon w/o Vector3() [x,y,z]
  Float32Array unwrappedColorsForBabylon;    // Value of colorForBabylon w/o Color4() [r,g,b,a]
  Float32Array unwrappedNormalsForBabylon;   // Value of normalsForBabylon w/o Vector3()  [x,y,z]
  Float32Array unwrappedUVForBabylon;        // Value of uvsForBabylon w/o Vector3()      [x,y,z]
  std::vector<std::string> triangles;        // Indices from new triangles coming from polygons
  std::string materialNameFromObj;           // The name of the current material
  std::string fileToLoad;                    // The name of the mtlFile to load
  std::string objMeshName;                   // The name of the current obj mesh
  size_t increment;                          // Id for meshes created by the multimaterial
  bool isFirstMaterial;
  Color4 grayColor = Color4(0.5f, 0.5f, 0.5f, 1.f);
};

/**
 * @brief Options for loading OBJ/MTL files.
 */
struct BABYLON_SHARED_EXPORT MeshLoadOptions {
  /**
   * Defines if UVs are optimized by default during load.
   */
  bool OptimizeWithUV;
  /**
   * Defines custom scaling of UV coordinates of loaded meshes.
   */
  Vector2 UVScaling;
  /**
   * Invert model on y-axis (does a model scaling inversion)
   */
  bool InvertY;
  /**
   * Invert Y-Axis of referenced textures on load
   */
  bool InvertTextureY;
  /**
   * Include in meshes the vertex colors available in some OBJ files.  This is not part of OBJ
   * standard.
   */
  bool ImportVertexColors;
  /**
   * Compute the normals for the model, even if normals are present in the file.
   */
  bool ComputeNormals;
  /**
   * Skip loading the materials even if defined in the OBJ file (materials are ignored).
   */
  bool SkipMaterials;
  /**
   * When a material fails to load OBJ loader will silently fail and onSuccess() callback will be
   * triggered.
   */
  bool MaterialLoadingFailsSilently;
}; // end of struct MeshLoadOptions

class AbstractMesh;
using AbstractMeshPtr = std::shared_ptr<AbstractMesh>;

/**
 * @brief Class reading and parsing the MTL file bundled with the obj file.
 */
class BABYLON_SHARED_EXPORT OBJFileLoader {

public:
  /**
   * Defines if UVs are optimized by default during load.
   */
  static bool OPTIMIZE_WITH_UV;
  /**
   * Invert model on y-axis (does a model scaling inversion)
   */
  static bool INVERT_Y;
  /**
   * Invert Y-Axis of referenced textures on load
   */
  static bool INVERT_TEXTURE_Y()
  {
    return MTLFileLoader::INVERT_TEXTURE_Y;
  }

  static void setINVERT_TEXTURE_Y(bool value)
  {
    MTLFileLoader::INVERT_TEXTURE_Y = value;
  }

  /**
   * Include in meshes the vertex colors available in some OBJ files.  This is not part of OBJ
   * standard.
   */
  static bool IMPORT_VERTEX_COLORS;
  /**
   * Compute the normals for the model, even if normals are present in the file.
   */
  static bool COMPUTE_NORMALS;
  /**
   * Defines custom scaling of UV coordinates of loaded meshes.
   */
  static Vector2 UV_SCALING;
  /**
   * Skip loading the materials even if defined in the OBJ file (materials are ignored).
   */
  static bool SKIP_MATERIALS;

  /**
   * When a material fails to load OBJ loader will silently fail and onSuccess() callback will be
   * triggered.
   *
   * Defaults to true for backwards compatibility.
   */
  static bool MATERIAL_LOADING_FAILS_SILENTLY;

public:
  /**
   * @brief Creates loader for .OBJ files.
   *
   * @param meshLoadOptions options for loading and parsing OBJ/MTL files.
   */
  OBJFileLoader(const std::optional<MeshLoadOptions>& meshLoadOptions = std::nullopt);
  ~OBJFileLoader() = default;

  /**
   * @brief If the data string can be loaded directly.
   *
   * @param data string containing the file data
   * @returns if the data can be loaded directly
   */
  bool canDirectLoad(const std::string& data);

private:
  static MeshLoadOptions currentMeshLoadOptions();

  /**
   * @brief Read the OBJ file and create an Array of meshes.
   * Each mesh contains all information given by the OBJ and the MTL file.
   * i.e. vertices positions and indices, optional normals values, optional UV values, optional
   * material
   *
   * @param meshesNames
   * @param scene Scene The scene where are displayed the data
   * @param data String The content of the obj file
   * @param rootUrl String The path to the folder
   * @returns Array<AbstractMesh>
   * @private
   */
  std::vector<AbstractMeshPtr> _parseSolid(const std::vector<std::string>& meshesNames,
                                           Scene* scene, const std::string& data,
                                           const std::string& rootUrl);

  /**
   * @brief Search for obj in the given array.
   * This function is called to check if a couple of data already exists in an array.
   *
   * If found, returns the index of the founded tuple index. Returns -1 if not found
   * @param arr Array<{ normals: Array<number>, idx: Array<number> }>
   * @param obj Array<number>
   * @returns {boolean}
   */
  int _isInArray(std::vector<TuplePosNormEntry>& arr, const std::vector<size_t>& obj);

  /**
   * @brief Search for obj in the given array.
   * This function is called to check if a couple of data already exists in an array.
   *
   * If found, returns the index of the founded tuple index. Returns -1 if not found
   * @param arr Array<{ normals: Array<number>, idx: Array<number> }>
   * @param obj Array<number>
   * @returns {boolean}
   */
  int _isInArrayUV(std::vector<TuplePosNormEntry>& arr, const std::vector<size_t>& obj);

  /**
   * @brief This function set the data for each triangle.
   * Data are position, normals and uvs
   * If a tuple of (position, normal) is not set, add the data into the corresponding array
   * If the tuple already exist, add only their indice
   *
   * @param tuplePosNorm Tuples with indice of Position, Normal, UV  [pos, norm, uvs]
   * @param indicePositionFromObj Integer The index in positions array
   * @param indiceUvsFromObj Integer The index in uvs array
   * @param indiceNormalFromObj Integer The index in normals array
   * @param positionVectorFromOBJ Vector3 The value of position at index objIndice
   * @param textureVectorFromOBJ Vector3 The value of uvs
   * @param normalsVectorFromOBJ Vector3 The value of normals at index objNormale
   */
  void _setData(size_t indicePositionFromObj, size_t indiceUvsFromObj, size_t indiceNormalFromObj,
                const Vector3& positionVectorFromOBJ, const Vector2& textureVectorFromOBJ,
                const Vector3& normalsVectorFromOBJ, OBJParseSolidState& state,
                const std::optional<Color4>& positionColorsFromOBJ = std::nullopt);

  /**
   * @brief Transform Vector() and BABYLON.Color() objects into numbers in an array.
   * @param state
   */
  void _unwrapData(OBJParseSolidState& state);

  /**
   * @brief Create triangles from polygons
   * It is important to notice that a triangle is a polygon
   * We get 5 patterns of face defined in OBJ File :
   * facePattern1 = ["1","2","3","4","5","6"]
   * facePattern2 = ["1/1","2/2","3/3","4/4","5/5","6/6"]
   * facePattern3 = ["1/1/1","2/2/2","3/3/3","4/4/4","5/5/5","6/6/6"]
   * facePattern4 = ["1//1","2//2","3//3","4//4","5//5","6//6"]
   * facePattern5 = ["-1/-1/-1","-2/-2/-2","-3/-3/-3","-4/-4/-4","-5/-5/-5","-6/-6/-6"]
   * Each pattern is divided by the same method
   * @param face Array[String] The indices of elements
   * @param v Integer The variable to increment
   * @param state
   */
  void _getTriangles(const std::vector<std::string>& face, size_t v, OBJParseSolidState& state);

  /**
   * @brief Create triangles and push the data for each polygon for the pattern 1.
   * In this pattern we get vertice positions
   * @param face
   * @param v
   * @param state
   */
  void _setDataForCurrentFaceWithPattern1(const std::vector<std::string>& face, size_t v,
                                          OBJParseSolidState& state);

  /**
   * @brief Create triangles and push the data for each polygon for the pattern 2.
   * In this pattern we get vertice positions and uvsu
   * @param face
   * @param v
   * @param state
   */
  void _setDataForCurrentFaceWithPattern2(const std::vector<std::string>& face, size_t v,
                                          OBJParseSolidState& state);

  /**
   * @brief Create triangles and push the data for each polygon for the pattern 3.
   * In this pattern we get vertice positions, uvs and normals
   * @param face
   * @param v
   * @param state
   */
  void _setDataForCurrentFaceWithPattern3(const std::vector<std::string>& face, size_t v,
                                          OBJParseSolidState& state);

  /**
   * @brief Create triangles and push the data for each polygon for the pattern 4.
   * In this pattern we get vertice positions and normals
   * @param face
   * @param v
   * @param state
   */
  void _setDataForCurrentFaceWithPattern4(const std::vector<std::string>& face, size_t v,
                                          OBJParseSolidState& state);

  /**
   * @brief Create triangles and push the data for each polygon for the pattern 3
   * In this pattern we get vertice positions, uvs and normals
   * @param face
   * @param v
   * @param state
   */
  void _setDataForCurrentFaceWithPattern5(const std::vector<std::string>& face, size_t v,
                                          OBJParseSolidState& state);

  /**
   * @brief Hidden
   */
  void _addPreviousObjMesh(OBJParseSolidState& state);

public:
  /**
   * Defines the name of the plugin.
   */
  std::string name = "obj";
  /**
   * Defines the extension the plugin is able to load.
   */
  std::string extensions = ".obj";
  /** @hidden */
  std::string obj = "^o";
  /** @hidden */
  std::string group = "^g";
  /** @hidden */
  std::string mtllib = "^mtllib ";
  /** @hidden */
  std::string usemtl = "^usemtl ";
  /** @hidden */
  std::string smooth = "^s ";
  /** @hidden */
  std::string vertexPattern = "v( +[\\d|\\.|\\+|\\-|e|E]+){3,7}";
  // vn float float float
  /** @hidden */
  std::string normalPattern
    = "vn( +[\\d|\\.|\\+|\\-|e|E]+)( +[\\d|\\.|\\+|\\-|e|E]+)( +[\\d|\\.|\\+|\\-|e|E]+)";
  // vt float float
  /** @hidden */
  std::string uvPattern = "vt( +[\\d|\\.|\\+|\\-|e|E]+)( +[\\d|\\.|\\+|\\-|e|E]+)";
  // f vertex vertex vertex ...
  /** @hidden */
  std::string facePattern1 = "f\\s+(([\\d]{1,}[\\s]?){3,})+";
  // f vertex/uvs vertex/uvs vertex/uvs ...
  /** @hidden */
  std::string facePattern2 = "f\\s+((([\\d]{1,}\\/[\\d]{1,}[\\s]?){3,})+)";
  // f vertex/uvs/normal vertex/uvs/normal vertex/uvs/normal ...
  /** @hidden */
  std::string facePattern3 = "f\\s+((([\\d]{1,}\\/[\\d]{1,}\\/[\\d]{1,}[\\s]?){3,})+)";
  // f vertex//normal vertex//normal vertex//normal ...
  /** @hidden */
  std::string facePattern4 = "f\\s+((([\\d]{1,}\\/\\/[\\d]{1,}[\\s]?){3,})+)";
  // f -vertex/-uvs/-normal -vertex/-uvs/-normal -vertex/-uvs/-normal ...
  /** @hidden */
  std::string facePattern5 = "f\\s+(((-[\\d]{1,}\\/-[\\d]{1,}\\/-[\\d]{1,}[\\s]?){3,})+)";

private:
  bool _forAssetContainer = false;

  MeshLoadOptions _meshLoadOptions = {};

}; // end of class OBJFileLoader

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_PLUGINS_OBJ_OBJ_FILE_LOADER_H
