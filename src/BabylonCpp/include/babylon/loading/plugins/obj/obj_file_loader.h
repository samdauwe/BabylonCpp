#ifndef BABYLON_LOADING_PLUGINS_OBJ_OBJ_FILE_LOADER_H
#define BABYLON_LOADING_PLUGINS_OBJ_OBJ_FILE_LOADER_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/loading/plugins/obj/mtl_file_loader.h>
#include <babylon/maths/color4.h>
#include <babylon/maths/vector2.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT MeshObject {
  std::string name;
  IndicesArray indices;
  Float32Array positions;
  Float32Array normals;
  Float32Array colors;
  Float32Array uvs;
  std::string materialName;
}; // end of struct MeshObject

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
   * @brief This function set the data for each triangle.
   * Data are position, normals and uvs
   * If a tuple of (position, normal) is not set, add the data into the corresponding array
   * If the tuple already exist, add only their indice
   *
   * @param indicePositionFromObj Integer The index in positions array
   * @param indiceUvsFromObj Integer The index in uvs array
   * @param indiceNormalFromObj Integer The index in normals array
   * @param positionVectorFromOBJ Vector3 The value of position at index objIndice
   * @param textureVectorFromOBJ Vector3 The value of uvs
   * @param normalsVectorFromOBJ Vector3 The value of normals at index objNormale
   */
  void _setData(size_t indicePositionFromObj, size_t indiceUvsFromObj, size_t indiceNormalFromObj,
                const Vector3& positionVectorFromOBJ, const Vector2& textureVectorFromOBJ,
                const Vector3& normalsVectorFromOBJ,
                const std::optional<Color4>& positionColorsFromOBJ = std::nullopt);

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
   * @param triangles
   */
  void _getTriangles(const std::vector<std::string>& faces, size_t v,
                     std::vector<std::string>& triangles);

  /**
   * @brief Create triangles and push the data for each polygon for the pattern 1.
   * In this pattern we get vertice positions
   * @param face
   * @param v
   * @param triangles
   * @param positions
   * @param colors
   */
  void _setDataForCurrentFaceWithPattern1(const std::vector<std::string>& face, size_t v,
                                          std::vector<std::string>& triangles,
                                          const std::vector<Vector3>& positions,
                                          const std::vector<Color4>& colors);

  /**
   * @brief Create triangles and push the data for each polygon for the pattern 2.
   * In this pattern we get vertice positions and uvsu
   * @param face
   * @param v
   * @param triangles
   * @param positions
   * @param uvs
   * @param colors
   */
  void _setDataForCurrentFaceWithPattern2(const std::vector<std::string>& face, size_t v,
                                          std::vector<std::string>& triangles,
                                          const std::vector<Vector3>& positions,
                                          const std::vector<Vector2>& uvs,
                                          const std::vector<Color4>& colors);

  /**
   * @brief Create triangles and push the data for each polygon for the pattern 3.
   * In this pattern we get vertice positions, uvs and normals
   * @param face
   * @param v
   * @param triangles
   * @param positions
   * @param normals
   * @param colors
   */
  void _setDataForCurrentFaceWithPattern3(const std::vector<std::string>& face, size_t v,
                                          std::vector<std::string>& triangles,
                                          const std::vector<Vector3>& positions,
                                          const std::vector<Vector3>& normals,
                                          const std::vector<Vector2>& uvs);

  /**
   * @brief Create triangles and push the data for each polygon for the pattern 4.
   * In this pattern we get vertice positions and normals
   * @param face
   * @param v
   * @param triangles
   * @param positions
   * @param normals
   * @param colors
   */
  void _setDataForCurrentFaceWithPattern4(const std::vector<std::string>& face, size_t v,
                                          std::vector<std::string>& triangles,
                                          const std::vector<Vector3>& positions,
                                          const std::vector<Vector3>& normals,
                                          const std::vector<Color4>& colors);

  /**
   * @brief Create triangles and push the data for each polygon for the pattern 3
   * In this pattern we get vertice positions, uvs and normals
   * @param face
   * @param v
   * @param triangles
   * @param positions
   * @param normals
   * @param uvs
   * @param colors
   */
  void _setDataForCurrentFaceWithPattern5(const std::vector<std::string>& face, size_t v,
                                          std::vector<std::string>& triangles,
                                          const std::vector<Vector3>& positions,
                                          const std::vector<Vector3>& normals,
                                          const std::vector<Vector2>& uvs,
                                          const std::vector<Color4>& colors);

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
