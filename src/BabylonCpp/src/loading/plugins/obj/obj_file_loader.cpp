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

int OBJFileLoader::_isInArray(std::vector<TuplePosNormEntry>& arr, const std::vector<size_t>& obj)
{
  if (arr.size() < obj[0]) {
    arr.resize(obj[0] + 1);
    arr[obj[0]] = {};
  }
  auto idx = stl_util::index_of(arr[obj[0]].normals, obj[1]);

  return idx == -1 ? -1 : static_cast<int>(arr[obj[0]].idx[static_cast<size_t>(idx)]);
}

int OBJFileLoader::_isInArrayUV(std::vector<TuplePosNormEntry>& arr, const std::vector<size_t>& obj)
{
  if (arr.size() < obj[0]) {
    arr.resize(obj[0] + 1);
    arr[obj[0]] = {};
  }
  auto idx = stl_util::index_of(arr[obj[0]].normals, obj[1]);

  if (idx != 1 && (obj[2] == arr[obj[0]].uv[static_cast<size_t>(idx)])) {
    return static_cast<int>(arr[obj[0]].idx[static_cast<size_t>(idx)]);
  }
  return -1;
}

void OBJFileLoader::_setData(size_t indicePositionFromObj, size_t indiceUvsFromObj,
                             size_t indiceNormalFromObj, const Vector3& positionVectorFromOBJ,
                             const Vector2& textureVectorFromOBJ,
                             const Vector3& normalsVectorFromOBJ, OBJParseSolidState& state,
                             const std::optional<Color4>& positionColorsFromOBJ)
{
  // Check if this tuple already exists in the list of tuples
  int _index;
  if (_meshLoadOptions.OptimizeWithUV) {
    _index = _isInArrayUV(state.tuplePosNorm,
                          {indicePositionFromObj, indiceNormalFromObj, indiceUvsFromObj});
  }
  else {
    _index = _isInArray(state.tuplePosNorm, {indicePositionFromObj, indiceNormalFromObj});
  }

  // If it not exists
  if (_index == -1) {
    // Add an new indice.
    // The array of indices is only an array with his length equal to the number of triangles - 1.
    // We add vertices data in this order
    state.indicesForBabylon.emplace_back(state.wrappedPositionForBabylon.size());
    // Push the position of vertice for Babylon
    // Each element is a Vector3(x,y,z)
    state.wrappedPositionForBabylon.emplace_back(positionVectorFromOBJ);
    // Push the uvs for Babylon
    // Each element is a Vector3(u,v)
    state.wrappedUvsForBabylon.emplace_back(textureVectorFromOBJ);
    // Push the normals for Babylon
    // Each element is a Vector3(x,y,z)
    state.wrappedNormalsForBabylon.emplace_back(normalsVectorFromOBJ);

    if (positionColorsFromOBJ.has_value()) {
      // Push the colors for Babylon
      // Each element is a BABYLON.Color4(r,g,b,a)
      state.wrappedColorsForBabylon.emplace_back(*positionColorsFromOBJ);
    }

    // Add the tuple in the comparison list
    state.tuplePosNorm[indicePositionFromObj].normals.emplace_back(indiceNormalFromObj);
    state.tuplePosNorm[indicePositionFromObj].idx.emplace_back(state.curPositionInIndices++);
    if (_meshLoadOptions.OptimizeWithUV) {
      state.tuplePosNorm[indicePositionFromObj].uv.emplace_back(indiceUvsFromObj);
    }
  }
  else {
    // The tuple already exists
    // Add the index of the already existing tuple
    // At this index we can get the value of position, normal, color and uvs of vertex
    state.indicesForBabylon.emplace_back(_index);
  }
}

void OBJFileLoader::_unwrapData(OBJParseSolidState& state)
{
  // Every array has the same length
  for (size_t l = 0; l < state.wrappedPositionForBabylon.size(); ++l) {
    // Push the x, y, z values of each element in the unwrapped array
    stl_util::concat(state.unwrappedPositionsForBabylon,
                     {state.wrappedPositionForBabylon[l].x, state.wrappedPositionForBabylon[l].y,
                      state.wrappedPositionForBabylon[l].z});
    stl_util::concat(state.unwrappedNormalsForBabylon,
                     {state.wrappedNormalsForBabylon[l].x, state.wrappedNormalsForBabylon[l].y,
                      state.wrappedNormalsForBabylon[l].z});
    stl_util::concat(
      state.unwrappedUVForBabylon,
      {state.wrappedUvsForBabylon[l].x,
       state.wrappedUvsForBabylon[l].y}); // z is an optional value not supported by BABYLON
    if (_meshLoadOptions.ImportVertexColors == true) {
      // Push the r, g, b, a values of each element in the unwrapped array
      stl_util::concat(state.unwrappedColorsForBabylon,
                       {state.wrappedColorsForBabylon[l].r, state.wrappedColorsForBabylon[l].g,
                        state.wrappedColorsForBabylon[l].b, state.wrappedColorsForBabylon[l].a});
    }
  }
  // Reset arrays for the next new meshes
  state.wrappedPositionForBabylon = {};
  state.wrappedNormalsForBabylon  = {};
  state.wrappedUvsForBabylon      = {};
  state.wrappedColorsForBabylon   = {};
  state.tuplePosNorm              = {};
  state.curPositionInIndices      = 0;
}

void OBJFileLoader::_getTriangles(const std::vector<std::string>& faces, size_t v,
                                  OBJParseSolidState& state)
{
  // Work for each element of the array
  for (size_t faceIndex = v; faceIndex < faces.size() - 1; ++faceIndex) {
    // Add on the triangle variable the indexes to obtain triangles
    stl_util::concat(state.triangles, {faces[0], faces[faceIndex], faces[faceIndex + 1]});
  }

  // Result obtained after 2 iterations:
  // Pattern1 => triangle = ["1","2","3","1","3","4"];
  // Pattern2 => triangle = ["1/1","2/2","3/3","1/1","3/3","4/4"];
  // Pattern3 => triangle = ["1/1/1","2/2/2","3/3/3","1/1/1","3/3/3","4/4/4"];
  // Pattern4 => triangle = ["1//1","2//2","3//3","1//1","3//3","4//4"];
  // Pattern5 => triangle = ["-1/-1/-1","-2/-2/-2","-3/-3/-3","-1/-1/-1","-3/-3/-3","-4/-4/-4"];
}

void OBJFileLoader::_setDataForCurrentFaceWithPattern1(const std::vector<std::string>& face,
                                                       size_t v, OBJParseSolidState& state)
{
  // Get the indices of triangles for each polygon
  _getTriangles(face, v, state);
  // For each element in the triangles array.
  // This var could contains 1 to an infinity of triangles
  for (const auto& triangle : state.triangles) {
    // Set position indice
    const auto indicePositionFromObj = StringTools::toNumber<size_t>(triangle) - 1;

    std::optional<Color4> positionColorsFromOBJ = std::nullopt;
    if (_meshLoadOptions.ImportVertexColors == true) {
      state.colors[indicePositionFromObj];
    }
    _setData(indicePositionFromObj, 0, 0, // In the pattern 1, normals and uvs are not defined
             state.positions[indicePositionFromObj], // Get the vectors data
             Vector2::Zero(), Vector3::Up(),         // Create default vectors
             state, positionColorsFromOBJ);
  }
  // Reset variable for the next line
  state.triangles = {};
}

void OBJFileLoader::_setDataForCurrentFaceWithPattern2(const std::vector<std::string>& face,
                                                       size_t v, OBJParseSolidState& state)
{
  // Get the indices of triangles for each polygon
  _getTriangles(face, v, state);
  for (const auto& triangle : state.triangles) {
    // triangle[k] = "1/1"
    // Split the data for getting position and uv
    const auto point = StringTools::split(triangle, '/'); // ["1", "1"]
    // Set position indice
    const auto indicePositionFromObj = StringTools::toNumber<size_t>(point[0]) - 1;
    // Set uv indice
    const auto indiceUvsFromObj = StringTools::toNumber<size_t>(point[1]) - 1;

    std::optional<Color4> positionColorsFromOBJ = std::nullopt;
    if (_meshLoadOptions.ImportVertexColors == true) {
      state.colors[indicePositionFromObj];
    }
    _setData(indicePositionFromObj, indiceUvsFromObj,
             0,                                      // Default value for normals
             state.positions[indicePositionFromObj], // Get the values for each element
             state.uvs[indiceUvsFromObj],
             Vector3::Up(), // Default value for normals
             state, positionColorsFromOBJ);
  }

  // Reset variable for the next line
  state.triangles = {};
}

void OBJFileLoader::_setDataForCurrentFaceWithPattern3(const std::vector<std::string>& face,
                                                       size_t v, OBJParseSolidState& state)
{
  // Get the indices of triangles for each polygon
  _getTriangles(face, v, state);

  for (const auto& triangle : state.triangles) {
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
             state.positions[indicePositionFromObj], state.uvs[indiceUvsFromObj],
             state.normals[indiceNormalFromObj], // Set the vector for each component
             state);
  }
  // Reset variable for the next line
  state.triangles = {};
}

void OBJFileLoader::_setDataForCurrentFaceWithPattern4(const std::vector<std::string>& face,
                                                       size_t v, OBJParseSolidState& state)
{
  _getTriangles(face, v, state);

  for (const auto& triangle : state.triangles) {
    // triangle[k] = "1//1"
    // Split the data for getting position and normals
    const auto point = StringTools::split(triangle, "//"); // ["1", "1"] // TODO FIXME
    // We check indices, and normals
    const auto indicePositionFromObj = StringTools::toNumber<size_t>(point[0]) - 1;
    const auto indiceNormalFromObj   = StringTools::toNumber<size_t>(point[1]) - 1;

    std::optional<Color4> positionColorsFromOBJ = std::nullopt;
    if (_meshLoadOptions.ImportVertexColors == true) {
      state.colors[indicePositionFromObj];
    }
    _setData(indicePositionFromObj,
             1, // Default value for uv
             indiceNormalFromObj,
             state.positions[indicePositionFromObj], // Get each vector of data
             Vector2::Zero(), state.normals[indiceNormalFromObj], state, positionColorsFromOBJ);
  }
  // Reset variable for the next line
  state.triangles = {};
}

void OBJFileLoader::_setDataForCurrentFaceWithPattern5(const std::vector<std::string>& face,
                                                       size_t v, OBJParseSolidState& state)
{
  // Get the indices of triangles for each polygon
  _getTriangles(face, v, state);

  for (const auto& triangle : state.triangles) {
    // triangle[k] = "-1/-1/-1"
    // Split the data for getting position, uv, and normals
    const auto point = StringTools::split(triangle, '/'); // ["-1", "-1", "-1"]
    // Set position indice
    const auto indicePositionFromObj
      = state.positions.size() + StringTools::toNumber<size_t>(point[0]);
    // Set uv indice
    const auto indiceUvsFromObj = state.uvs.size() + StringTools::toNumber<size_t>(point[1]);
    // Set normal indice
    const auto indiceNormalFromObj = state.normals.size() + StringTools::toNumber<size_t>(point[2]);

    std::optional<Color4> positionColorsFromOBJ = std::nullopt;
    if (_meshLoadOptions.ImportVertexColors == true) {
      state.colors[indicePositionFromObj];
    }
    _setData(indicePositionFromObj, indiceUvsFromObj, indiceNormalFromObj,
             state.positions[indicePositionFromObj], state.uvs[indiceUvsFromObj],
             state.normals[indiceNormalFromObj], // Set the vector for each component
             state, positionColorsFromOBJ);
  }
  // Reset variable for the next line
  state.triangles = {};
}

void OBJFileLoader::_addPreviousObjMesh(OBJParseSolidState& state)
{
  // Check if it is not the first mesh. Otherwise we don't have data.
  if (state.meshesFromObj.size() > 0) {
    // Get the previous mesh for applying the data about the faces
    // => in obj file, faces definition append after the name of the mesh
    state.handledMesh = state.meshesFromObj.back();

    // Set the data into Array for the mesh
    _unwrapData(state);

    // Reverse tab. Otherwise face are displayed in the wrong sense
    std::reverse(state.indicesForBabylon.begin(), state.indicesForBabylon.end());
    // Set the information for the mesh
    // Slice the array to avoid rewriting because of the fact this is the same var which be rewrited
    state.handledMesh.indices   = state.indicesForBabylon;
    state.handledMesh.positions = state.unwrappedPositionsForBabylon;
    state.handledMesh.normals   = state.unwrappedNormalsForBabylon;
    state.handledMesh.uvs       = state.unwrappedUVForBabylon;

    if (_meshLoadOptions.ImportVertexColors == true) {
      state.handledMesh.colors = state.unwrappedColorsForBabylon;
    }

    // Reset the array for the next mesh
    state.indicesForBabylon            = {};
    state.unwrappedPositionsForBabylon = {};
    state.unwrappedColorsForBabylon    = {};
    state.unwrappedNormalsForBabylon   = {};
    state.unwrappedUVForBabylon        = {};
  }
}

} // end of namespace BABYLON
