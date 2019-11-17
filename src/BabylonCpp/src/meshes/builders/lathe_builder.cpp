#include <babylon/meshes/builders/lathe_builder.h>

#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/builders/ribbon_builder.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/vertex_data.h>

namespace BABYLON {

MeshPtr LatheBuilder::CreateLathe(const std::string& name,
                                  LatheOptions& options, Scene* scene)
{
  auto arc
    = options.arc.has_value() ?
        ((*options.arc <= 0.f || options.arc > 1.f) ? 1.f : *options.arc) :
        1.f;
  auto closed       = options.closed.value_or(true);
  auto& shape       = options.shape;
  auto radius       = options.radius.value_or(1.f);
  auto tessellation = static_cast<float>(options.tessellation.value_or(64));
  auto clip         = static_cast<float>(options.clip.value_or(0));
  auto updatable    = options.updatable;
  auto sideOrientation
    = Mesh::_GetDefaultSideOrientation(options.sideOrientation);
  auto cap = options.cap.value_or(Mesh::NO_CAP);
  auto pi2 = Math::PI2;
  std::vector<std::vector<Vector3>> paths;
  auto invertUV = options.invertUV.value_or(false);

  const auto step = pi2 / tessellation * arc;
  Vector3 rotated;
  for (int i = 0; i <= tessellation - clip; ++i) {
    std::vector<Vector3> path;
    if (cap == Mesh::CAP_START || cap == Mesh::CAP_ALL) {
      path.emplace_back(Vector3(0.f, shape[0].y, 0.f));
      path.emplace_back(Vector3(std::cos((float)i * step) * shape[0].x * radius,
                                shape[0].y,
                                std::sin((float)i * step) * shape[0].x * radius));
    }
    for (auto& p : shape) {
      rotated = Vector3(std::cos(i * step) * p.x * radius, p.y, std::sin(i * step) * p.x * radius);
      path.emplace_back(rotated);
    }
    if (cap == Mesh::CAP_END || cap == Mesh::CAP_ALL) {
      path.emplace_back(
        Vector3(std::cos(i * step) * shape[shape.size() - 1].x * radius,
                shape[shape.size() - 1].y,
                std::sin(i * step) * shape[shape.size() - 1].x * radius));
      path.emplace_back(Vector3(0.f, shape[shape.size() - 1].y, 0.f));
    }
    paths.emplace_back(path);
  }

  // lathe ribbon
  RibbonOptions ribbonOptions;
  ribbonOptions.pathArray       = paths;
  ribbonOptions.closeArray      = closed;
  ribbonOptions.sideOrientation = sideOrientation;
  ribbonOptions.updatable       = updatable;
  ribbonOptions.invertUV        = invertUV;
  ribbonOptions.frontUVs        = options.frontUVs;
  ribbonOptions.backUVs         = options.backUVs;
  auto lathe = RibbonBuilder::CreateRibbon(name, ribbonOptions, scene);
  return lathe;
}

} // end of namespace BABYLON
