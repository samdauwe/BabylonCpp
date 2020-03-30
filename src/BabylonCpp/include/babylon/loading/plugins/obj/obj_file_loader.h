#ifndef BABYLON_LOADING_PLUGINS_OBJ_OBJ_FILE_LOADER_H
#define BABYLON_LOADING_PLUGINS_OBJ_OBJ_FILE_LOADER_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/loading/plugins/obj/mtl_file_loader.h>
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

private:
  static MeshLoadOptions currentMeshLoadOptions();

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
