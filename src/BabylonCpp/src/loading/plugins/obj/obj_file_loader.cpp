#include <babylon/loading/plugins/obj/obj_file_loader.h>

namespace BABYLON {

bool OBJFileLoader::OPTIMIZE_WITH_UV = true;

bool OBJFileLoader::INVERT_Y = false;

bool OBJFileLoader::IMPORT_VERTEX_COLORS = false;

bool OBJFileLoader::COMPUTE_NORMALS = false;

Vector2 OBJFileLoader::UV_SCALING = Vector2(1.f, 1.f);

bool OBJFileLoader::SKIP_MATERIALS = false;

bool OBJFileLoader::MATERIAL_LOADING_FAILS_SILENTLY = true;

OBJFileLoader::OBJFileLoader(const std::optional<MeshLoadOptions>& meshLoadOptions)
{
  _meshLoadOptions = meshLoadOptions.value_or(OBJFileLoader::currentMeshLoadOptions());
}

MeshLoadOptions OBJFileLoader::currentMeshLoadOptions()
{
  MeshLoadOptions options{};
  options.ComputeNormals               = OBJFileLoader::COMPUTE_NORMALS;
  options.ImportVertexColors           = OBJFileLoader::IMPORT_VERTEX_COLORS;
  options.InvertY                      = OBJFileLoader::INVERT_Y;
  options.InvertTextureY               = OBJFileLoader::INVERT_TEXTURE_Y();
  options.UVScaling                    = OBJFileLoader::UV_SCALING;
  options.MaterialLoadingFailsSilently = OBJFileLoader::MATERIAL_LOADING_FAILS_SILENTLY;
  options.OptimizeWithUV               = OBJFileLoader::OPTIMIZE_WITH_UV;
  options.SkipMaterials                = OBJFileLoader::SKIP_MATERIALS;
  return options;
}

} // end of namespace BABYLON
