#include <babylon/loading/plugins/obj/obj_file_loader.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/maths/vector3.h>
#include <babylon/misc/string_tools.h>

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

bool OBJFileLoader::canDirectLoad(const std::string& /*data*/)
{
  return false;
}

void OBJFileLoader::_setData(size_t indicePositionFromObj, size_t indiceUvsFromObj,
                             size_t indiceNormalFromObj, const Vector3& positionVectorFromOBJ,
                             const Vector2& textureVectorFromOBJ,
                             const Vector3& normalsVectorFromOBJ,
                             const std::optional<Color4>& positionColorsFromOBJ)
{
}

void OBJFileLoader::_getTriangles(const std::vector<std::string>& faces, size_t v,
                                  std::vector<std::string>& triangles)
{
  // Work for each element of the array
  for (size_t faceIndex = v; faceIndex < faces.size() - 1; ++faceIndex) {
    // Add on the triangle variable the indexes to obtain triangles
    stl_util::concat(triangles, {faces[0], faces[faceIndex], faces[faceIndex + 1]});
  }

  // Result obtained after 2 iterations:
  // Pattern1 => triangle = ["1","2","3","1","3","4"];
  // Pattern2 => triangle = ["1/1","2/2","3/3","1/1","3/3","4/4"];
  // Pattern3 => triangle = ["1/1/1","2/2/2","3/3/3","1/1/1","3/3/3","4/4/4"];
  // Pattern4 => triangle = ["1//1","2//2","3//3","1//1","3//3","4//4"];
  // Pattern5 => triangle = ["-1/-1/-1","-2/-2/-2","-3/-3/-3","-1/-1/-1","-3/-3/-3","-4/-4/-4"];
};

void OBJFileLoader::_setDataForCurrentFaceWithPattern1(const std::vector<std::string>& face,
                                                       size_t v,
                                                       std::vector<std::string>& triangles,
                                                       const std::vector<Vector3>& positions,
                                                       const std::vector<Color4>& colors)
{
  // Get the indices of triangles for each polygon
  _getTriangles(face, v, triangles);
  // For each element in the triangles array.
  // This var could contains 1 to an infinity of triangles
  for (const auto& triangle : triangles) {
    // Set position indice
    const auto indicePositionFromObj = StringTools::toNumber<size_t>(triangle) - 1;

    std::optional<Color4> positionColorsFromOBJ = std::nullopt;
    if (_meshLoadOptions.ImportVertexColors == true) {
      colors[indicePositionFromObj];
    }
    _setData(indicePositionFromObj, 0, 0,      // In the pattern 1, normals and uvs are not defined
             positions[indicePositionFromObj], // Get the vectors data
             Vector2::Zero(), Vector3::Up(),   // Create default vectors
             positionColorsFromOBJ);
  }
  // Reset variable for the next line
  triangles = {};
}

void OBJFileLoader::_setDataForCurrentFaceWithPattern2(const std::vector<std::string>& face,
                                                       size_t v,
                                                       std::vector<std::string>& triangles,
                                                       const std::vector<Vector3>& positions,
                                                       const std::vector<Vector2>& uvs,
                                                       const std::vector<Color4>& colors)
{
  // Get the indices of triangles for each polygon
  _getTriangles(face, v, triangles);
  for (const auto& triangle : triangles) {
    // triangle[k] = "1/1"
    // Split the data for getting position and uv
    const auto point = StringTools::split(triangle, '/'); // ["1", "1"]
    // Set position indice
    const auto indicePositionFromObj = StringTools::toNumber<size_t>(point[0]) - 1;
    // Set uv indice
    const auto indiceUvsFromObj = StringTools::toNumber<size_t>(point[1]) - 1;

    std::optional<Color4> positionColorsFromOBJ = std::nullopt;
    if (_meshLoadOptions.ImportVertexColors == true) {
      colors[indicePositionFromObj];
    }
    _setData(indicePositionFromObj, indiceUvsFromObj,
             0,                                // Default value for normals
             positions[indicePositionFromObj], // Get the values for each element
             uvs[indiceUvsFromObj],
             Vector3::Up(), // Default value for normals
             positionColorsFromOBJ);
  }

  // Reset variable for the next line
  triangles = {};
}

void OBJFileLoader::_setDataForCurrentFaceWithPattern3(const std::vector<std::string>& face,
                                                       size_t v,
                                                       std::vector<std::string>& triangles,
                                                       const std::vector<Vector3>& positions,
                                                       const std::vector<Vector3>& normals,
                                                       const std::vector<Vector2>& uvs)
{
  // Get the indices of triangles for each polygon
  _getTriangles(face, v, triangles);

  for (const auto& triangle : triangles) {
    // triangle[k] = "1/1/1"
    // Split the data for getting position, uv, and normals
    const auto point = StringTools::split(triangle, '/'); // ["1", "1", "1"]
    // Set position indice
    const auto indicePositionFromObj = StringTools::toNumber<size_t>(point[0]) - 1;
    // Set uv indice
    const auto indiceUvsFromObj = StringTools::toNumber<size_t>(point[1]) - 1;
    // Set normal indice
    const auto indiceNormalFromObj = StringTools::toNumber<size_t>(point[2]) - 1;

    _setData(indicePositionFromObj, indiceUvsFromObj, indiceNormalFromObj,
             positions[indicePositionFromObj], uvs[indiceUvsFromObj],
             normals[indiceNormalFromObj] // Set the vector for each component
    );
  }
  // Reset variable for the next line
  triangles = {};
}

void OBJFileLoader::_setDataForCurrentFaceWithPattern4(const std::vector<std::string>& face,
                                                       size_t v,
                                                       std::vector<std::string>& triangles,
                                                       const std::vector<Vector3>& positions,
                                                       const std::vector<Vector3>& normals,
                                                       const std::vector<Color4>& colors)
{
  _getTriangles(face, v, triangles);

  for (const auto& triangle : triangles) {
    // triangle[k] = "1//1"
    // Split the data for getting position and normals
    const auto point = StringTools::split(triangle, "//"); // ["1", "1"] // TODO FIXME
    // We check indices, and normals
    const auto indicePositionFromObj = StringTools::toNumber<size_t>(point[0]) - 1;
    const auto indiceNormalFromObj   = StringTools::toNumber<size_t>(point[1]) - 1;

    std::optional<Color4> positionColorsFromOBJ = std::nullopt;
    if (_meshLoadOptions.ImportVertexColors == true) {
      colors[indicePositionFromObj];
    }
    _setData(indicePositionFromObj,
             1, // Default value for uv
             indiceNormalFromObj,
             positions[indicePositionFromObj], // Get each vector of data
             Vector2::Zero(), normals[indiceNormalFromObj], positionColorsFromOBJ);
  }
  // Reset variable for the next line
  triangles = {};
}

void OBJFileLoader::_setDataForCurrentFaceWithPattern5(
  const std::vector<std::string>& face, size_t v, std::vector<std::string>& triangles,
  const std::vector<Vector3>& positions, const std::vector<Vector3>& normals,
  const std::vector<Vector2>& uvs, const std::vector<Color4>& colors)
{
  // Get the indices of triangles for each polygon
  _getTriangles(face, v, triangles);

  for (const auto& triangle : triangles) {
    // triangle[k] = "-1/-1/-1"
    // Split the data for getting position, uv, and normals
    const auto point = StringTools::split(triangle, '/'); // ["-1", "-1", "-1"]
    // Set position indice
    const auto indicePositionFromObj = positions.size() + StringTools::toNumber<size_t>(point[0]);
    // Set uv indice
    const auto indiceUvsFromObj = uvs.size() + StringTools::toNumber<size_t>(point[1]);
    // Set normal indice
    const auto indiceNormalFromObj = normals.size() + StringTools::toNumber<size_t>(point[2]);

    std::optional<Color4> positionColorsFromOBJ = std::nullopt;
    if (_meshLoadOptions.ImportVertexColors == true) {
      colors[indicePositionFromObj];
    }
    _setData(indicePositionFromObj, indiceUvsFromObj, indiceNormalFromObj,
             positions[indicePositionFromObj], uvs[indiceUvsFromObj],
             normals[indiceNormalFromObj], // Set the vector for each component
             positionColorsFromOBJ);
  }
  // Reset variable for the next line
  triangles = {};
}

} // end of namespace BABYLON
