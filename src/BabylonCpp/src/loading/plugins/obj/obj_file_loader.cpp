#include <babylon/loading/plugins/obj/obj_file_loader.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/logging.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/standard_material.h>
#include <babylon/maths/vector3.h>
#include <babylon/meshes/geometry.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/vertex_data.h>
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

int OBJFileLoader::_isInArray(std::vector<TuplePosNormEntry>& arr, const std::vector<size_t>& iObj)
{
  if (arr.size() < iObj[0]) {
    arr.resize(iObj[0] + 1);
    arr[iObj[0]] = {};
  }
  auto idx = stl_util::index_of(arr[iObj[0]].normals, iObj[1]);

  return idx == -1 ? -1 : static_cast<int>(arr[iObj[0]].idx[static_cast<size_t>(idx)]);
}

int OBJFileLoader::_isInArrayUV(std::vector<TuplePosNormEntry>& arr,
                                const std::vector<size_t>& iObj)
{
  if (arr.size() < iObj[0]) {
    arr.resize(iObj[0] + 1);
    arr[iObj[0]] = {};
  }
  auto idx = stl_util::index_of(arr[iObj[0]].normals, iObj[1]);

  if (idx != 1 && (iObj[2] == arr[iObj[0]].uv[static_cast<size_t>(idx)])) {
    return static_cast<int>(arr[iObj[0]].idx[static_cast<size_t>(idx)]);
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
    state.indicesForBabylon.emplace_back(
      static_cast<uint32_t>(state.wrappedPositionForBabylon.size()));
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

std::vector<AbstractMeshPtr> OBJFileLoader::_parseSolid(const std::vector<std::string>& meshesNames,
                                                        Scene* scene, const std::string& data,
                                                        const std::string& /*rootUrl*/)
{
  OBJParseSolidState state = {};

  // Split the file into lines
  auto lines = StringTools::split(data, '\n');
  const std::string regex{"\\s\\s"};
  // Look at each line
  for (auto line : lines) {
    line = StringTools::regexReplace(StringTools::trim(line), regex, " ");
    std::vector<std::string> result;

    // Comment or newLine
    if (line.empty() || line[0] == '#') {
      continue;

      // Get information about one position possible for the vertices
    }
    else if (!StringTools::regexMatch(line, std::regex(vertexPattern, std::regex::optimize))
                .empty()) {
      result = StringTools::regexMatch(line, std::regex(R"([^ ]+)", std::regex::optimize));

      // Value of result with line: "v 1.0 2.0 3.0"
      // ["v", "1.0", "2.0", "3.0"]
      // Create a Vector3 with the position x, y, z
      state.positions.emplace_back(Vector3(StringTools::toNumber<float>(result[1]), //
                                           StringTools::toNumber<float>(result[2]), //
                                           StringTools::toNumber<float>(result[3])  //
                                           ));

      if (_meshLoadOptions.ImportVertexColors == true) {
        if (result.size() >= 7) {
          // TODO: if these numbers are > 1 we can use Color4.FromInts(r,g,b,a)
          state.colors.emplace_back(Color4(StringTools::toNumber<float>(result[4]), //
                                           StringTools::toNumber<float>(result[5]), //
                                           StringTools::toNumber<float>(result[6]), //
                                           (result.size() == 7 /* || result[7] == undefined */) ?
                                             1.f :
                                             StringTools::toNumber<float>(result[7])));
        }
        else {
          // TODO: maybe push NULL and if all are NULL to skip (and remove grayColor var).
          state.colors.emplace_back(state.grayColor);
        }
      }
    }
    else if (!(result
               = StringTools::regexMatch(line, std::regex(normalPattern, std::regex::optimize)))
                .empty()) {
      // Create a Vector3 with the normals x, y, z
      // Value of result
      //  ["vn 1.0 2.0 3.0", "1.0", "2.0", "3.0"]
      // Add the Vector in the list of normals
      state.normals.emplace_back(Vector3(StringTools::toNumber<float>(result[1]), //
                                         StringTools::toNumber<float>(result[2]), //
                                         StringTools::toNumber<float>(result[3])  //
                                         ));
    }
    else if (!(result = StringTools::regexMatch(line, std::regex(uvPattern, std::regex::optimize)))
                .empty()) {
      // Create a Vector2 with the normals u, v
      // Value of result
      //  ["vt 0.1 0.2 0.3", "0.1", "0.2"]
      // Add the Vector in the list of uvs
      state.uvs.emplace_back(
        Vector2(StringTools::toNumber<float>(result[1]) * OBJFileLoader::UV_SCALING.x,
                StringTools::toNumber<float>(result[2]) * OBJFileLoader::UV_SCALING.y));

      // Identify patterns of faces
      // Face could be defined in different type of pattern
    }
    else if (!(result
               = StringTools::regexMatch(line, std::regex(facePattern3, std::regex::optimize)))
                .empty()) {
      // Value of result:
      // ["f 1/1/1 2/2/2 3/3/3", "1/1/1 2/2/2 3/3/3"...]

      // Set the data for this face
      _setDataForCurrentFaceWithPattern3(
        StringTools::split(StringTools::trim(result[1]), " "), // ["1/1/1", "2/2/2", "3/3/3"]
        1,                                                     // v
        state                                                  // state
      );
    }
    else if (!(result
               = StringTools::regexMatch(line, std::regex(facePattern4, std::regex::optimize)))
                .empty()) {
      // Value of result:
      // ["f 1//1 2//2 3//3", "1//1 2//2 3//3"...]

      // Set the data for this face
      _setDataForCurrentFaceWithPattern4(
        StringTools::split(StringTools::trim(result[1]), " "), // ["1//1", "2//2", "3//3"]
        1,                                                     // v
        state                                                  // state
      );
    }
    else if (!(result
               = StringTools::regexMatch(line, std::regex(facePattern5, std::regex::optimize)))
                .empty()) {
      // Value of result:
      // ["f -1/-1/-1 -2/-2/-2 -3/-3/-3", "-1/-1/-1 -2/-2/-2 -3/-3/-3"...]

      // Set the data for this face
      _setDataForCurrentFaceWithPattern5(
        StringTools::split(StringTools::trim(result[1]),
                           " "), // ["-1/-1/-1", "-2/-2/-2", "-3/-3/-3"]
        1,                       // v
        state                    // state
      );
    }
    else if (!(result
               = StringTools::regexMatch(line, std::regex(facePattern2, std::regex::optimize)))
                .empty()) {
      // Value of result:
      //["f 1/1 2/2 3/3", "1/1 2/2 3/3"...]

      // Set the data for this face
      _setDataForCurrentFaceWithPattern2(
        StringTools::split(StringTools::trim(result[1]), " "), // ["1/1", "2/2", "3/3"]
        1,                                                     // v
        state                                                  // state
      );
    }
    else if (!(result
               = StringTools::regexMatch(line, std::regex(facePattern2, std::regex::optimize)))
                .empty()) {
      // Value of result
      //["f 1 2 3", "1 2 3"...]

      // Set the data for this face
      _setDataForCurrentFaceWithPattern1(
        StringTools::split(StringTools::trim(result[1]), " "), // ["1", "2", "3"]
        1,                                                     // v
        state                                                  // state
      );

      // Define a mesh or an object
      // Each time this keyword is analysed, create a new Object with all data for creating a
      // babylonMesh
    }
    else if ((!StringTools::regexMatch(line, std::regex(group, std::regex::optimize)).empty())
             || (!StringTools::regexMatch(obj, std::regex(group, std::regex::optimize)).empty())) {
      // Create a new mesh corresponding to the name of the group.
      // Definition of the mesh
      auto substr = line.substr(2);
      MeshObject objMesh{
        StringTools::trim(substr), // Set the name of the current obj mesh
        {},                        // indices
        {},                        // positions
        {},                        // normals
        {},                        // uvs
        {},                        // colors
        ""                         // materialName
      };
      _addPreviousObjMesh(state);

      // Push the last mesh created with only the name
      state.meshesFromObj.emplace_back(objMesh);

      // Set this variable to indicate that now meshesFromObj has objects defined inside
      state.hasMeshes       = true;
      state.isFirstMaterial = true;
      state.increment       = 1;
      // Keyword for applying a material
    }
    else if (!StringTools::regexMatch(line, std::regex(usemtl, std::regex::optimize)).empty()) {
      // Get the name of the material
      auto substr               = line.substr(7);
      state.materialNameFromObj = StringTools::trim(substr);

      // If this new material is in the same mesh

      if (!state.isFirstMaterial || !state.hasMeshes) {
        // Set the data for the previous mesh
        _addPreviousObjMesh(state);
        // Create a new mesh
        MeshObject objMesh
          // Set the name of the current obj mesh
          {
            StringTools::printf("%s_mm%zu",
                                (!state.objMeshName.empty() ? state.objMeshName.c_str() : "mesh"),
                                state.increment), // Set the name of the current obj mesh
            {},                                   // indices
            {},                                   // positions
            {},                                   // normals
            {},                                   // uvs
            {},                                   // colors
            state.materialNameFromObj             // materialName
          };
        ++state.increment;
        // If meshes are already defined
        state.meshesFromObj.emplace_back(objMesh);
        state.hasMeshes = true;
      }
      // Set the material name if the previous line define a mesh

      if (state.hasMeshes && state.isFirstMaterial) {
        // Set the material name to the previous mesh (1 material per mesh)
        state.meshesFromObj.back().materialName = state.materialNameFromObj;
        state.isFirstMaterial                   = false;
      }
      // Keyword for loading the mtl file
    }
    else if (!StringTools::regexMatch(line, std::regex(mtllib, std::regex::optimize)).empty()) {
      // Get the name of mtl file
      auto substr      = line.substr(7);
      state.fileToLoad = StringTools::trim(substr);

      // Apply smoothing
    }
    else if (!StringTools::regexMatch(line, std::regex(smooth, std::regex::optimize)).empty()) {
      // smooth shading => apply smoothing
      // Today I don't know it work with babylon and with obj.
      // With the obj file  an integer is set
    }
    else {
      // If there is another possibility
      BABYLON_LOG_ERROR("OBJFileLoader", "Unhandled expression at line : %s", line.c_str())
    }
  }

  // At the end of the file, add the last mesh into the meshesFromObj array
  if (state.hasMeshes) {
    // Set the data for the last mesh
    state.handledMesh = state.meshesFromObj.back();

    // Reverse indices for displaying faces in the good sense
    std::reverse(state.indicesForBabylon.begin(), state.indicesForBabylon.end());
    // Get the good array
    _unwrapData(state);
    // Set array
    state.handledMesh.indices   = state.indicesForBabylon;
    state.handledMesh.positions = state.unwrappedPositionsForBabylon;
    state.handledMesh.normals   = state.unwrappedNormalsForBabylon;
    state.handledMesh.uvs       = state.unwrappedUVForBabylon;

    if (_meshLoadOptions.ImportVertexColors == true) {
      state.handledMesh.colors = state.unwrappedColorsForBabylon;
    }
  }

  // If any o or g keyword found, create a mesh with a random id
  if (!state.hasMeshes) {
    // reverse tab of indices
    std::reverse(state.indicesForBabylon.begin(), state.indicesForBabylon.end());
    // Get positions normals uvs
    _unwrapData(state);
    // Set data for one mesh
    state.meshesFromObj.emplace_back(MeshObject{
      Geometry::RandomId(),               // name
      state.indicesForBabylon,            // indices
      state.unwrappedPositionsForBabylon, // positions
      state.unwrappedColorsForBabylon,    // colors
      state.unwrappedNormalsForBabylon,   // normals
      state.unwrappedUVForBabylon,        // uvs
      state.materialNameFromObj           // materialName
    });
  }

  // Create a Mesh list
  std::vector<AbstractMeshPtr> babylonMeshesArray; // The mesh for babylon
  std::vector<std::string> materialToUse;

  // Set data for each mesh
  for (const auto& meshFromObj : state.meshesFromObj) {

    // check meshesNames (stlFileLoader)
    if (!meshesNames.empty() && !meshFromObj.name.empty()) {
      if (!stl_util::contains(meshesNames, meshFromObj.name)) {
        continue;
      }
    }

    // Get the current mesh
    // Set the data with VertexBuffer for each mesh
    state.handledMesh = meshFromObj;
    // Create a Mesh with the name of the obj mesh

    scene->_blockEntityCollection = _forAssetContainer;
    auto babylonMesh              = Mesh::New(meshFromObj.name, scene);
    scene->_blockEntityCollection = false;

    // Push the name of the material to an array
    // This is indispensable for the importMesh function
    materialToUse.emplace_back(meshFromObj.materialName);

    auto vertexData = std::make_unique<VertexData>(); // The container for the values
    // Set the data for the babylonMesh
    vertexData->uvs       = state.handledMesh.uvs;
    vertexData->indices   = state.handledMesh.indices;
    vertexData->positions = state.handledMesh.positions;
    if (_meshLoadOptions.ComputeNormals == true) {
      Float32Array normals;
      VertexData::ComputeNormals(state.handledMesh.positions, state.handledMesh.indices, normals);
      vertexData->normals = normals;
    }
    else {
      vertexData->normals = state.handledMesh.normals;
    }
    if (_meshLoadOptions.ImportVertexColors == true) {
      vertexData->colors = state.handledMesh.colors;
    }
    // Set the data from the VertexBuffer to the current Mesh
    vertexData->applyToMesh(*babylonMesh);
    if (_meshLoadOptions.InvertY) {
      babylonMesh->scaling().y *= -1.f;
    }

    // Push the mesh into an array
    babylonMeshesArray.emplace_back(babylonMesh);
  }

  // Return an array with all Mesh
  return babylonMeshesArray;
}

} // end of namespace BABYLON
