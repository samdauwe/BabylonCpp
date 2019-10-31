#include <babylon/meshes/mesh_builder.h>

#include <babylon/meshes/builders/box_builder.h>
#include <babylon/meshes/builders/cylinder_builder.h>
#include <babylon/meshes/builders/decal_builder.h>
#include <babylon/meshes/builders/disc_builder.h>
#include <babylon/meshes/builders/ground_builder.h>
#include <babylon/meshes/builders/ico_sphere_builder.h>
#include <babylon/meshes/builders/lathe_builder.h>
#include <babylon/meshes/builders/lines_builder.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/builders/plane_builder.h>
#include <babylon/meshes/builders/polygon_builder.h>
#include <babylon/meshes/builders/polyhedron_builder.h>
#include <babylon/meshes/builders/ribbon_builder.h>
#include <babylon/meshes/builders/shape_builder.h>
#include <babylon/meshes/builders/sphere_builder.h>
#include <babylon/meshes/builders/tiled_box_builder.h>
#include <babylon/meshes/builders/tiled_plane_builder.h>
#include <babylon/meshes/builders/torus_builder.h>
#include <babylon/meshes/builders/torus_knot_builder.h>
#include <babylon/meshes/builders/tube_builder.h>

namespace BABYLON {

MeshPtr MeshBuilder::CreateBox(const std::string& name, BoxOptions& options,
                               Scene* scene)
{
  return BoxBuilder::CreateBox(name, options, scene);
}

MeshPtr MeshBuilder::CreateTiledBox(const std::string& name,
                                    TiledBoxOptions& options, Scene* scene)
{
  return TiledBoxBuilder::CreateTiledBox(name, options, scene);
}

MeshPtr MeshBuilder::CreateSphere(const std::string& name,
                                  SphereOptions& options, Scene* scene)
{
  return SphereBuilder::CreateSphere(name, options, scene);
}

MeshPtr MeshBuilder::CreateDisc(const std::string& name, DiscOptions& options,
                                Scene* scene)
{
  return DiscBuilder::CreateDisc(name, options, scene);
}

MeshPtr MeshBuilder::CreateIcoSphere(const std::string& name,
                                     IcoSphereOptions& options, Scene* scene)
{
  return IcoSphereBuilder::CreateIcoSphere(name, options, scene);
}

MeshPtr MeshBuilder::CreateRibbon(const std::string& name,
                                  RibbonOptions& options, Scene* scene)

{
  return RibbonBuilder::CreateRibbon(name, options, scene);
}

MeshPtr MeshBuilder::CreateCylinder(const std::string& name,
                                    CylinderOptions& options, Scene* scene)
{
  return CylinderBuilder::CreateCylinder(name, options, scene);
}

MeshPtr MeshBuilder::CreateTorus(const std::string& name, TorusOptions& options,
                                 Scene* scene)
{
  return TorusBuilder::CreateTorus(name, options, scene);
}

MeshPtr MeshBuilder::CreateTorusKnot(const std::string& name,
                                     TorusKnotOptions& options, Scene* scene)
{
  return TorusKnotBuilder::CreateTorusKnot(name, options, scene);
}

LinesMeshPtr MeshBuilder::CreateLineSystem(const std::string& name,
                                           LineSystemOptions& options,
                                           Scene* scene)
{
  return LinesBuilder::CreateLineSystem(name, options, scene);
}

LinesMeshPtr MeshBuilder::CreateLines(const std::string& name,
                                      LinesOptions& options, Scene* scene)
{
  return LinesBuilder::CreateLines(name, options, scene);
}

LinesMeshPtr MeshBuilder::CreateDashedLines(const std::string& name,
                                            DashedLinesOptions& options,
                                            Scene* scene)
{
  return LinesBuilder::CreateDashedLines(name, options, scene);
}

MeshPtr MeshBuilder::ExtrudeShape(const std::string& name,
                                  ExtrudeShapeOptions& options, Scene* scene)
{
  return ShapeBuilder::ExtrudeShape(name, options, scene);
}

MeshPtr MeshBuilder::ExtrudeShapeCustom(const std::string& name,
                                        ExtrudeShapeCustomOptions& options,
                                        Scene* scene)
{
  return ShapeBuilder::ExtrudeShapeCustom(name, options, scene);
}

MeshPtr MeshBuilder::CreateLathe(const std::string& name, LatheOptions& options,
                                 Scene* scene)
{
  return LatheBuilder::CreateLathe(name, options, scene);
}

MeshPtr MeshBuilder::CreateTiledPlane(const std::string& name,
                                      TiledPlaneOptions& options, Scene* scene)
{
  return TiledPlaneBuilder::CreateTiledPlane(name, options, scene);
}

MeshPtr MeshBuilder::CreatePlane(const std::string& name, PlaneOptions& options,
                                 Scene* scene)
{
  return PlaneBuilder::CreatePlane(name, options, scene);
}

GroundMeshPtr MeshBuilder::CreateGround(const std::string& name,
                                        GroundOptions& options, Scene* scene)
{
  return GroundBuilder::CreateGround(name, options, scene);
}

MeshPtr MeshBuilder::CreateTiledGround(const std::string& name,
                                       TiledGroundOptions& options,
                                       Scene* scene)
{
  return GroundBuilder::CreateTiledGround(name, options, scene);
}

GroundMeshPtr MeshBuilder::CreateGroundFromHeightMap(
  const std::string& name, const std::string& url,
  GroundFromHeightMapOptions& options, Scene* scene)
{
  return GroundBuilder::CreateGroundFromHeightMap(name, url, options, scene);
}

MeshPtr MeshBuilder::CreatePolygon(const std::string& name,
                                   PolygonOptions& options, Scene* scene)
{
  return PolygonBuilder::CreatePolygon(name, options, scene);
}

MeshPtr MeshBuilder::ExtrudePolygon(const std::string& name,
                                    PolygonOptions& options, Scene* scene)
{
  return PolygonBuilder::ExtrudePolygon(name, options, scene);
}

MeshPtr MeshBuilder::CreateTube(const std::string& name, TubeOptions& options,
                                Scene* scene)
{
  return TubeBuilder::CreateTube(name, options, scene);
}

MeshPtr MeshBuilder::CreatePolyhedron(const std::string& name,
                                      PolyhedronOptions& options, Scene* scene)
{
  return PolyhedronBuilder::CreatePolyhedron(name, options, scene);
}

MeshPtr MeshBuilder::CreateDecal(const std::string& name,
                                 const AbstractMeshPtr& sourceMesh,
                                 DecalOptions& options)
{
  return DecalBuilder::CreateDecal(name, sourceMesh, options);
}

} // end of namespace BABYLON
