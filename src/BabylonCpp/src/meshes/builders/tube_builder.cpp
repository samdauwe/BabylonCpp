#include <babylon/meshes/builders/tube_builder.h>

#include <babylon/math/tmp.h>
#include <babylon/meshes/_creation_data_storage.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/builders/ribbon_builder.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {

MeshPtr TubeBuilder::CreateTube(const std::string& name, TubeOptions& options,
                                Scene* scene)
{
  const auto& path = options.path;
  auto& instance   = options.instance;
  auto radius      = 1.f;

  if (options.radius.has_value()) {
    radius = *options.radius;
  }
  else if (options.instance) {
    radius = options.instance->_creationDataStorage->radius;
  }

  const auto& tessellation   = options.tessellation.value_or(64);
  const auto& radiusFunction = options.radiusFunction;
  auto cap                   = options.cap.value_or(Mesh::NO_CAP);
  auto invertUV              = options.invertUV.value_or(false);
  const auto& updatable      = options.updatable;
  const auto sideOrientation
    = Mesh::_GetDefaultSideOrientation(options.sideOrientation);
  auto arc
    = options.arc.has_value() ?
        ((*options.arc <= 0.f || options.arc > 1.f) ? 1.f : *options.arc) :
        1.f;

  // tube geometry
  const auto tubePathArray
    = [](const std::vector<Vector3>& _path, Path3D& path3D,
         std::vector<std::vector<Vector3>>& circlePaths, float _radius,
         unsigned int _tessellation,
         const std::function<float(unsigned int i, float distance)>&
           _radiusFunction,
         unsigned int _cap, float _arc) {
        auto& tangents        = path3D.getTangents();
        const auto& normals   = path3D.getNormals();
        const auto& distances = path3D.getDistances();
        const auto& pi2       = Math::PI2;
        const auto step       = pi2 / static_cast<float>(_tessellation) * _arc;

        auto rad = 0.f;
        Vector3 normal;
        Vector3 rotated;
        auto& rotationMatrix = Tmp::MatrixArray[0];
        // TODO FIXME
        unsigned int index
          = (_cap == Mesh::NO_CAP || _cap == Mesh::CAP_END) ? 0 : 0;
        circlePaths.resize(_path.size() + index);
        for (unsigned int i = 0; i < _path.size(); ++i) {
          rad = (_radiusFunction == nullptr) ?
                  _radius :
                  _radiusFunction(i, distances[i]); // current radius
          std::vector<Vector3> circlePath;          // current circle array
          normal = normals[i];                      // current normal
          for (std::size_t t = 0; t < _tessellation; ++t) {
            Matrix::RotationAxisToRef(tangents[i], step * static_cast<float>(t),
                                      rotationMatrix);
            rotated
              = (t + 1 <= circlePath.size()) ? circlePath[t] : Vector3::Zero();
            Vector3::TransformCoordinatesToRef(normal, rotationMatrix, rotated);
            rotated.scaleInPlace(rad).addInPlace(_path[i]);
            circlePath.emplace_back(rotated);
          }
          circlePaths[index] = circlePath;
          ++index;
        }
        // cap
        const auto capPath
          = [_path](unsigned int nbPoints, unsigned int pathIndex) {
              std::vector<Vector3> pointCap;
              for (std::size_t i = 0; i < nbPoints; ++i) {
                pointCap.emplace_back(_path[pathIndex]);
              }
              return pointCap;
            };
        switch (_cap) {
          case Mesh::NO_CAP:
            break;
          case Mesh::CAP_START:
            circlePaths[0] = capPath(_tessellation, 0);
            circlePaths[1] = circlePaths[2];
            break;
          case Mesh::CAP_END:
            circlePaths[index]     = circlePaths[index - 1];
            circlePaths[index + 1] = capPath(
              _tessellation, static_cast<unsigned int>(_path.size() - 1));
            break;
          case Mesh::CAP_ALL:
            circlePaths[0]         = capPath(_tessellation, 0);
            circlePaths[1]         = circlePaths[2];
            circlePaths[index]     = circlePaths[index - 1];
            circlePaths[index + 1] = capPath(
              _tessellation, static_cast<unsigned int>(_path.size() - 1));
            break;
          default:
            break;
        }
        return circlePaths;
      };
  Path3D path3D;
  std::vector<std::vector<Vector3>> pathArray;
  if (instance) {
    // tube update
    auto& storage = instance->_creationDataStorage;
    auto iArc     = options.arc.value_or(storage->arc);
    path3D        = storage->path3D.update(path);
    pathArray     = tubePathArray(path, path3D, storage->pathArray, radius,
                              storage->tessellation, radiusFunction,
                              storage->cap, iArc);
    RibbonOptions ribbonOptions;
    ribbonOptions.pathArray = pathArray;
    instance = RibbonBuilder::CreateRibbon("", ribbonOptions, scene);
    // Update mode, no need to recreate the storage.
    storage->path3D    = path3D;
    storage->pathArray = pathArray;
    storage->arc       = iArc;
    storage->radius    = radius;

    return instance;
  }
  // tube creation
  path3D = Path3D(path);
  std::vector<std::vector<Vector3>> newPathArray;
  cap       = (cap > 3) ? 0 : cap;
  pathArray = tubePathArray(path, path3D, newPathArray, radius, tessellation,
                            radiusFunction, cap, arc);
  RibbonOptions ribbonOptions;
  ribbonOptions.pathArray       = pathArray;
  ribbonOptions.closePath       = true;
  ribbonOptions.closeArray      = false;
  ribbonOptions.updatable       = updatable;
  ribbonOptions.sideOrientation = sideOrientation;
  ribbonOptions.invertUV        = invertUV;
  auto tube = RibbonBuilder::CreateRibbon(name, ribbonOptions, scene);
  tube->_creationDataStorage->pathArray    = pathArray;
  tube->_creationDataStorage->path3D       = path3D;
  tube->_creationDataStorage->tessellation = tessellation;
  tube->_creationDataStorage->cap          = cap;
  tube->_creationDataStorage->arc          = arc;
  tube->_creationDataStorage->radius       = radius;

  return tube;
}

} // end of namespace BABYLON
