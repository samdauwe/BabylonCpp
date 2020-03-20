#include <babylon/meshes/builders/shape_builder.h>

#include <babylon/maths/path3d.h>
#include <babylon/maths/tmp_vectors.h>
#include <babylon/meshes/_creation_data_storage.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/builders/ribbon_builder.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {

MeshPtr ShapeBuilder::ExtrudeShape(const std::string& name, ExtrudeShapeOptions& options,
                                   Scene* scene)
{
  const auto& path           = options.path;
  const auto& shape          = options.shape;
  const auto scale           = options.scale.value_or(1.f);
  const auto rotation        = options.rotation.value_or(0.f);
  const auto cap             = options.cap.value_or(Mesh::NO_CAP);
  const auto& updatable      = options.updatable;
  const auto sideOrientation = Mesh::_GetDefaultSideOrientation(options.sideOrientation);
  const auto& instance       = options.instance;
  const auto invertUV        = options.invertUV.value_or(false);

  return ShapeBuilder::_ExtrudeShapeGeneric(
    name, shape, path, scale, rotation, nullptr, nullptr, false, false, cap, false, scene,
    updatable.has_value(), sideOrientation, instance, invertUV, options.frontUVs, options.backUVs);
}

MeshPtr ShapeBuilder::ExtrudeShapeCustom(const std::string& name,
                                         ExtrudeShapeCustomOptions& options, Scene* scene)
{
  const auto& path         = options.path;
  const auto& shape        = options.shape;
  const auto scaleFunction = options.scaleFunction ?
                               options.scaleFunction :
                               ([](float /*i*/, float /*distance*/) -> float { return 1.f; });
  const auto rotationFunction = options.rotationFunction ?
                                  options.rotationFunction :
                                  ([](float /*i*/, float /*distance*/) -> float { return 0.f; });
  const auto ribbonCloseArray = options.ribbonCloseArray.value_or(false);
  const auto ribbonClosePath  = options.ribbonClosePath.value_or(false);
  const auto cap              = options.cap.value_or(Mesh::NO_CAP);
  const auto& updatable       = options.updatable;
  const auto sideOrientation  = Mesh::_GetDefaultSideOrientation(options.sideOrientation);
  const auto& instance        = options.instance;
  const auto invertUV         = options.invertUV.value_or(false);
  return ShapeBuilder::_ExtrudeShapeGeneric(
    name, shape, path, std::nullopt, std::nullopt, scaleFunction, rotationFunction,
    ribbonCloseArray, ribbonClosePath, cap, true, scene, updatable.has_value(), sideOrientation,
    instance, invertUV, options.frontUVs, options.backUVs);
}

MeshPtr ShapeBuilder::_ExtrudeShapeGeneric(
  const std::string& name, const std::vector<Vector3>& shape, const std::vector<Vector3>& curve,
  const std::optional<float>& scale, const std::optional<float>& rotation,
  const std::function<float(float i, float distance)>& scaleFunction,
  const std::function<float(float i, float distance)>& rotateFunction, bool rbCA, bool rbCP,
  unsigned int cap, bool custom, Scene* scene, bool updtbl, unsigned int side, MeshPtr instance,
  bool invertUV, const std::optional<Vector4>& frontUVs, const std::optional<Vector4>& backUVs)
{
  // extrusion geometry
  const auto extrusionPathArray
    = [](const std::vector<Vector3>& _shape, const std::vector<Vector3>& _curve, Path3D& path3D,
         std::vector<std::vector<Vector3>> shapePaths, const std::optional<float>& _scale,
         const std::optional<float>& _rotation,
         const std::function<float(float i, float distance)>& _scaleFunction,
         const std::function<float(float i, float distance)>& _rotateFunction, unsigned int _cap,
         bool _custom) {
        auto& tangents        = path3D.getTangents();
        const auto& normals   = path3D.getNormals();
        const auto& binormals = path3D.getBinormals();
        const auto& distances = path3D.getDistances();

        auto angle       = 0.f;
        auto returnScale = [_scale](float /*i*/, float /*distance*/) {
          return _scale.has_value() ? *_scale : 1.f;
        };
        auto returnRotation = [_rotation](float /*i*/, float /*distance*/) {
          return _rotation.has_value() ? *_rotation : 0.f;
        };
        auto rotate          = _custom ? _rotateFunction : returnRotation;
        auto scl             = _custom ? _scaleFunction : returnScale;
        auto index           = (_cap == Mesh::NO_CAP || _cap == Mesh::CAP_END) ? 0u : 2u;
        auto& rotationMatrix = TmpVectors::MatrixArray[0];
        shapePaths.resize(_curve.size());

        for (size_t i = 0ull; i < _curve.size(); ++i) {
          std::vector<Vector3> shapePath;
          auto angleStep  = rotate(static_cast<float>(i), distances[i]);
          auto scaleRatio = scl(static_cast<float>(i), distances[i]);
          for (const auto& p : _shape) {
            Matrix::RotationAxisToRef(tangents[i], angle, rotationMatrix);
            auto planed
              = ((tangents[i].scale(p.z)).add(normals[i].scale(p.x)).add(binormals[i].scale(p.y)));
            auto rotated = Vector3::Zero();
            Vector3::TransformCoordinatesToRef(planed, rotationMatrix, rotated);
            rotated.scaleInPlace(scaleRatio).addInPlace(_curve[i]);
            shapePath.emplace_back(rotated);
          }
          shapePaths[index] = shapePath;
          angle += angleStep;
          ++index;
        }
        // cap
        const auto capPath = [&](const std::vector<Vector3>& shapePath) {
          std::vector<Vector3> pointCap;
          auto barycenter = Vector3::Zero();
          for (const auto& pt3d : shapePath) {
            barycenter.addInPlace(pt3d);
          }
          barycenter.scaleInPlace(1.f / static_cast<float>(shapePath.size()));
          for (size_t i = 0ull; i < shapePath.size(); ++i) {
            pointCap.emplace_back(barycenter);
          }
          return pointCap;
        };
        switch (_cap) {
          case Mesh::NO_CAP:
            break;
          case Mesh::CAP_START:
            shapePaths[0] = capPath(shapePaths[2]);
            shapePaths[1] = shapePaths[2];
            break;
          case Mesh::CAP_END:
            shapePaths[index + 0] = shapePaths[index - 1];
            shapePaths[index + 1] = capPath(shapePaths[index - 1]);
            break;
          case Mesh::CAP_ALL:
            shapePaths[0]         = capPath(shapePaths[2]);
            shapePaths[1]         = shapePaths[2];
            shapePaths[index + 0] = shapePaths[index - 1];
            shapePaths[index + 1] = capPath(shapePaths[index - 1]);
            break;
          default:
            break;
        }
        return shapePaths;
      };

  Path3D path3D;
  std::vector<std::vector<Vector3>> pathArray;
  if (instance) { // instance update
    auto& storage = instance->_creationDataStorage;
    path3D        = storage->path3D.update(curve);
    pathArray     = extrusionPathArray(shape, curve, storage->path3D, storage->pathArray, scale,
                                   rotation, scaleFunction, rotateFunction, storage->cap, custom);
    instance = Mesh::CreateRibbon("", pathArray, false, false, 0, scene, false, Mesh::DEFAULTSIDE,
                                  instance);

    return instance;
  }
  // extruded shape creation
  path3D = Path3D(curve);
  std::vector<std::vector<Vector3>> newShapePaths;
  uint32_t _cap = (cap > 3) ? 0 : cap;
  pathArray     = extrusionPathArray(shape, curve, path3D, newShapePaths, scale, rotation,
                                 scaleFunction, rotateFunction, _cap, custom);
  RibbonOptions ribbonOptions;
  ribbonOptions.pathArray       = pathArray;
  ribbonOptions.closeArray      = rbCA;
  ribbonOptions.closePath       = rbCP;
  ribbonOptions.updatable       = updtbl;
  ribbonOptions.sideOrientation = side;
  ribbonOptions.invertUV        = invertUV;
  ribbonOptions.frontUVs        = frontUVs;
  ribbonOptions.backUVs         = backUVs;
  auto extrudedGeneric          = RibbonBuilder::CreateRibbon(name, ribbonOptions, scene);
  extrudedGeneric->_creationDataStorage->pathArray = pathArray;
  extrudedGeneric->_creationDataStorage->path3D    = path3D;
  extrudedGeneric->_creationDataStorage->cap       = _cap;

  return extrudedGeneric;
}

} // end of namespace BABYLON
