#include <babylon/meshes/builders/mesh_builder_options.h>

namespace BABYLON {

//--------------------------------------------------------------------------------------------------
// Box mesh options
//--------------------------------------------------------------------------------------------------

BoxOptions::BoxOptions()
    : size{std::nullopt}
    , width{std::nullopt}
    , height{std::nullopt}
    , depth{std::nullopt}
    , faceUV{{Vector4(0.f, 0.f, 1.f, 1.f), Vector4(0.f, 0.f, 1.f, 1.f), Vector4(0.f, 0.f, 1.f, 1.f),
              Vector4(0.f, 0.f, 1.f, 1.f), Vector4(0.f, 0.f, 1.f, 1.f),
              Vector4(0.f, 0.f, 1.f, 1.f)}}
    , faceColors{std::nullopt}
    , sideOrientation{std::nullopt}
    , frontUVs{std::nullopt}
    , backUVs{std::nullopt}
    , wrap{std::nullopt}
    , topBaseAt{std::nullopt}
    , bottomBaseAt{std::nullopt}
    , updatable{std::nullopt}
{
}

void BoxOptions::setFaceColor(size_t faceIndex, const Color4& color)
{
  if (!faceColors) {
    faceColors
      = {Color4(1.f, 1.f, 1.f, 1.f), Color4(1.f, 1.f, 1.f, 1.f), Color4(1.f, 1.f, 1.f, 1.f),
         Color4(1.f, 1.f, 1.f, 1.f), Color4(1.f, 1.f, 1.f, 1.f), Color4(1.f, 1.f, 1.f, 1.f)};
  }
  faceColors.value()[faceIndex] = color;
}

BoxOptions::~BoxOptions() = default;

//--------------------------------------------------------------------------------------------------
// Cylinder or cone mesh options
//--------------------------------------------------------------------------------------------------

CylinderOptions::CylinderOptions()
    : height{std::nullopt}
    , diameterTop{std::nullopt}
    , diameterBottom{std::nullopt}
    , diameter{std::nullopt}
    , tessellation{std::nullopt}
    , subdivisions{std::nullopt}
    , arc{std::nullopt}
    , updatable{std::nullopt}
    , hasRings{std::nullopt}
    , enclose{std::nullopt}
    , cap{std::nullopt}
    , sideOrientation{std::nullopt}
    , frontUVs{std::nullopt}
    , backUVs{std::nullopt}
{
}

CylinderOptions::~CylinderOptions() = default;

//--------------------------------------------------------------------------------------------------
// Dashed lines mesh options
//--------------------------------------------------------------------------------------------------

DashedLinesOptions::DashedLinesOptions()
    : dashSize{std::nullopt}
    , gapSize{std::nullopt}
    , dashNb{std::nullopt}
    , updatable{std::nullopt}
    , useVertexAlpha{std::nullopt}
    , instance{nullptr}
{
}

DashedLinesOptions::~DashedLinesOptions() = default;

//--------------------------------------------------------------------------------------------------
// Decal mesh options
//--------------------------------------------------------------------------------------------------

DecalOptions::DecalOptions()
    : position{std::nullopt}
    , normal{std::nullopt}
    , size{std::nullopt}
    , angle{std::nullopt}
    , captureUVS{std::nullopt}
{
}

DecalOptions::~DecalOptions() = default;

//--------------------------------------------------------------------------------------------------
// Disc mesh options
//--------------------------------------------------------------------------------------------------

DiscOptions::DiscOptions()
    : radius{std::nullopt}
    , tessellation{std::nullopt}
    , arc{std::nullopt}
    , updatable{std::nullopt}
    , sideOrientation{std::nullopt}
    , frontUVs{std::nullopt}
    , backUVs{std::nullopt}
{
}

DiscOptions::~DiscOptions() = default;

//--------------------------------------------------------------------------------------------------
// Extrude shape mesh options
//--------------------------------------------------------------------------------------------------

ExtrudeShapeOptions::ExtrudeShapeOptions()
    : scale{std::nullopt}
    , rotation{std::nullopt}
    , cap{std::nullopt}
    , updatable{std::nullopt}
    , sideOrientation{std::nullopt}
    , frontUVs{std::nullopt}
    , backUVs{std::nullopt}
    , instance{nullptr}
    , invertUV{std::nullopt}
{
}

ExtrudeShapeOptions::~ExtrudeShapeOptions() = default;

//--------------------------------------------------------------------------------------------------
// Extrude shape custom mesh options
//--------------------------------------------------------------------------------------------------

ExtrudeShapeCustomOptions::ExtrudeShapeCustomOptions()
    : scaleFunction{nullptr}
    , rotationFunction{nullptr}
    , ribbonCloseArray{std::nullopt}
    , ribbonClosePath{std::nullopt}
    , cap{std::nullopt}
    , updatable{std::nullopt}
    , sideOrientation{std::nullopt}
    , frontUVs{std::nullopt}
    , backUVs{std::nullopt}
    , invertUV{std::nullopt}
{
}

ExtrudeShapeCustomOptions::~ExtrudeShapeCustomOptions() = default;

//--------------------------------------------------------------------------------------------------
// Ground from height map mesh options
//--------------------------------------------------------------------------------------------------

GroundFromHeightMapOptions::GroundFromHeightMapOptions()
    : width{std::nullopt}
    , height{std::nullopt}
    , subdivisions{std::nullopt}
    , minHeight{std::nullopt}
    , maxHeight{std::nullopt}
    , colorFilter{std::nullopt}
    , alphaFilter{std::nullopt}
    , updatable{std::nullopt}
    , bufferWidth{std::nullopt}
    , bufferHeight{std::nullopt}
    , onReady{nullptr}
{
}

GroundFromHeightMapOptions::~GroundFromHeightMapOptions() = default;

//--------------------------------------------------------------------------------------------------
// Ground mesh options
//--------------------------------------------------------------------------------------------------

GroundOptions::GroundOptions()
    : width{std::nullopt}
    , height{std::nullopt}
    , subdivisions{std::nullopt}
    , subdivisionsX{std::nullopt}
    , subdivisionsY{std::nullopt}
    , updatable{std::nullopt}
{
}

GroundOptions::~GroundOptions() = default;

//--------------------------------------------------------------------------------------------------
// Hemisphere mesh options
//--------------------------------------------------------------------------------------------------

HemisphereOptions::HemisphereOptions()
    : segments{std::nullopt}, diameter{std::nullopt}, sideOrientation{std::nullopt}
{
}

HemisphereOptions::~HemisphereOptions() = default;

//--------------------------------------------------------------------------------------------------
// Icosphere mesh options
//--------------------------------------------------------------------------------------------------

IcoSphereOptions::IcoSphereOptions()
    : radius{std::nullopt}
    , radiusX{std::nullopt}
    , radiusY{std::nullopt}
    , radiusZ{std::nullopt}
    , flat{std::nullopt}
    , subdivisions{std::nullopt}
    , sideOrientation{std::nullopt}
    , frontUVs{std::nullopt}
    , backUVs{std::nullopt}
    , updatable{std::nullopt}
{
}

IcoSphereOptions::~IcoSphereOptions() = default;

//--------------------------------------------------------------------------------------------------
// Capsule mesh options
//--------------------------------------------------------------------------------------------------

ICreateCapsuleOptions::ICreateCapsuleOptions()
    : orientation{std::nullopt}
    , subdivisions{std::nullopt}
    , tessellation{std::nullopt}
    , height{std::nullopt}
    , radius{std::nullopt}
    , capSubdivisions{std::nullopt}
    , radiusTop{std::nullopt}
    , radiusBottom{std::nullopt}
    , topCapSubdivisions{std::nullopt}
    , bottomCapSubdivisions{std::nullopt}
    , updatable{std::nullopt}
{
}

ICreateCapsuleOptions::~ICreateCapsuleOptions() = default;

//--------------------------------------------------------------------------------------------------
// Lathe mesh options
//--------------------------------------------------------------------------------------------------

LatheOptions::LatheOptions()
    : radius{std::nullopt}
    , tessellation{std::nullopt}
    , clip{std::nullopt}
    , arc{std::nullopt}
    , closed{std::nullopt}
    , updatable{std::nullopt}
    , sideOrientation{std::nullopt}
    , frontUVs{std::nullopt}
    , backUVs{std::nullopt}
    , cap{std::nullopt}
    , invertUV{std::nullopt}
{
}

LatheOptions::~LatheOptions() = default;

//--------------------------------------------------------------------------------------------------
// Lines mesh options
//--------------------------------------------------------------------------------------------------

LinesOptions::LinesOptions()
    : updatable{std::nullopt}, useVertexAlpha{std::nullopt}, instance{nullptr}
{
}

LinesOptions::~LinesOptions() = default;

//--------------------------------------------------------------------------------------------------
// Line system mesh options
//--------------------------------------------------------------------------------------------------

LineSystemOptions::LineSystemOptions()
    : updatable{std::nullopt}, useVertexAlpha{std::nullopt}, instance{nullptr}
{
}

LineSystemOptions::LineSystemOptions(LinesOptions& linesOptions)
    : updatable{linesOptions.updatable}
    , useVertexAlpha{linesOptions.useVertexAlpha}
    , instance{linesOptions.instance}
{
  if (!linesOptions.points.empty()) {
    lines.emplace_back(linesOptions.points);
  }
  if (!linesOptions.colors.empty()) {
    colors.emplace_back(linesOptions.colors);
  }
}

LineSystemOptions::~LineSystemOptions() = default;

//--------------------------------------------------------------------------------------------------
// Plane mesh options
//--------------------------------------------------------------------------------------------------

PlaneOptions::PlaneOptions()
    : size{std::nullopt}
    , width{std::nullopt}
    , height{std::nullopt}
    , sideOrientation{std::nullopt}
    , frontUVs{std::nullopt}
    , backUVs{std::nullopt}
    , updatable{std::nullopt}
    , sourcePlane{nullptr}
{
}

PlaneOptions::~PlaneOptions() = default;

//--------------------------------------------------------------------------------------------------
// Polyhedron mesh options
//--------------------------------------------------------------------------------------------------

PolyhedronOptions::PolyhedronOptions()
    : type{std::nullopt}
    , size{std::nullopt}
    , sizeX{std::nullopt}
    , sizeY{std::nullopt}
    , sizeZ{std::nullopt}
    , custom{std::nullopt}
    , flat{std::nullopt}
    , updatable{std::nullopt}
    , sideOrientation{std::nullopt}
    , frontUVs{std::nullopt}
    , backUVs{std::nullopt}
{
}

PolyhedronOptions::~PolyhedronOptions() = default;

//--------------------------------------------------------------------------------------------------
// Polygon mesh options
//--------------------------------------------------------------------------------------------------

PolygonOptions::PolygonOptions()
    : depth{std::nullopt}
    , smoothingThreshold{std::nullopt}
    , faceUV{{
        Vector4(0.f, 0.f, 1.f, 1.f),
        Vector4(0.f, 0.f, 1.f, 1.f),
        Vector4(0.f, 0.f, 1.f, 1.f),
      }}
    , faceColors{std::nullopt}
    , updatable{std::nullopt}
    , sideOrientation{std::nullopt}
    , frontUVs{Vector4(0.f, 0.f, 1.f, 1.f)}
    , backUVs{Vector4(0.f, 0.f, 1.f, 1.f)}
    , wrap{std::nullopt}
{
}

PolygonOptions::~PolygonOptions() = default;

//--------------------------------------------------------------------------------------------------
// Ribbon mesh options
//--------------------------------------------------------------------------------------------------

RibbonOptions::RibbonOptions()
    : closeArray{std::nullopt}
    , closePath{std::nullopt}
    , offset{std::nullopt}
    , updatable{std::nullopt}
    , sideOrientation{std::nullopt}
    , frontUVs{std::nullopt}
    , backUVs{std::nullopt}
    , instance{nullptr}
    , invertUV{std::nullopt}
{
}

RibbonOptions::~RibbonOptions() = default;

//--------------------------------------------------------------------------------------------------
// Sphere mesh options
//--------------------------------------------------------------------------------------------------

SphereOptions::SphereOptions()
    : segments{std::nullopt}
    , diameter{std::nullopt}
    , diameterX{std::nullopt}
    , diameterY{std::nullopt}
    , diameterZ{std::nullopt}
    , arc{std::nullopt}
    , slice{std::nullopt}
    , sideOrientation{std::nullopt}
    , frontUVs{std::nullopt}
    , backUVs{std::nullopt}
    , dedupTopBottomIndices{std::nullopt}
    , updatable{std::nullopt}
{
}

SphereOptions::SphereOptions(const SphereOptions& other) = default;

SphereOptions::SphereOptions(SphereOptions&& other) = default;

SphereOptions& SphereOptions::operator=(const SphereOptions& other) = default;

SphereOptions& SphereOptions::operator=(SphereOptions&& other) = default;

SphereOptions::~SphereOptions() = default;

//--------------------------------------------------------------------------------------------------
// Tiled box mesh options
//--------------------------------------------------------------------------------------------------

TiledBoxOptions::TiledBoxOptions()
    : pattern{std::nullopt}
    , size{std::nullopt}
    , width{std::nullopt}
    , height{std::nullopt}
    , depth{std::nullopt}
    , tileSize{std::nullopt}
    , tileWidth{std::nullopt}
    , tileHeight{std::nullopt}
    , alignHorizontal{std::nullopt}
    , alignVertical{std::nullopt}
    , faceUV{{Vector4(0.f, 0.f, 1.f, 1.f), Vector4(0.f, 0.f, 1.f, 1.f), Vector4(0.f, 0.f, 1.f, 1.f),
              Vector4(0.f, 0.f, 1.f, 1.f), Vector4(0.f, 0.f, 1.f, 1.f),
              Vector4(0.f, 0.f, 1.f, 1.f)}}
    , faceColors{{Color4(1.f, 1.f, 1.f, 1.f), Color4(1.f, 1.f, 1.f, 1.f),
                  Color4(1.f, 1.f, 1.f, 1.f), Color4(1.f, 1.f, 1.f, 1.f),
                  Color4(1.f, 1.f, 1.f, 1.f), Color4(1.f, 1.f, 1.f, 1.f)}}
    , sideOrientation{std::nullopt}
    , updatable{std::nullopt}
{
}

TiledBoxOptions::~TiledBoxOptions() = default;

//--------------------------------------------------------------------------------------------------
// Tiled ground mesh options
//--------------------------------------------------------------------------------------------------

TiledGroundOptions::TiledGroundOptions()
    : xmin{std::nullopt}
    , zmin{std::nullopt}
    , xmax{std::nullopt}
    , zmax{std::nullopt}
    , subdivisions{std::nullopt}
    , precision{std::nullopt}
    , updatable{std::nullopt}
{
}

TiledGroundOptions::~TiledGroundOptions() = default;

//--------------------------------------------------------------------------------------------------
// Tiled plane mesh options
//--------------------------------------------------------------------------------------------------

TiledPlaneOptions::TiledPlaneOptions()
    : pattern{std::nullopt}
    , tileSize{std::nullopt}
    , tileWidth{std::nullopt}
    , tileHeight{std::nullopt}
    , size{std::nullopt}
    , width{std::nullopt}
    , height{std::nullopt}
    , alignHorizontal{std::nullopt}
    , alignVertical{std::nullopt}
    , updatable{std::nullopt}
    , sideOrientation{std::nullopt}
    , frontUVs{std::nullopt}
    , backUVs{std::nullopt}
{
}

TiledPlaneOptions::~TiledPlaneOptions() = default;

//--------------------------------------------------------------------------------------------------
// Torus mesh options
//--------------------------------------------------------------------------------------------------

TorusOptions::TorusOptions()
    : diameter{std::nullopt}
    , thickness{std::nullopt}
    , tessellation{std::nullopt}
    , updatable{std::nullopt}
    , sideOrientation{std::nullopt}
    , frontUVs{std::nullopt}
    , backUVs{std::nullopt}
{
}

TorusOptions::~TorusOptions() = default;

//--------------------------------------------------------------------------------------------------
// TorusKnot mesh options
//--------------------------------------------------------------------------------------------------

TorusKnotOptions::TorusKnotOptions()
    : radius{std::nullopt}
    , tube{std::nullopt}
    , radialSegments{std::nullopt}
    , tubularSegments{std::nullopt}
    , p{std::nullopt}
    , q{std::nullopt}
    , updatable{std::nullopt}
    , sideOrientation{std::nullopt}
    , frontUVs{std::nullopt}
    , backUVs{std::nullopt}
{
}

TorusKnotOptions::~TorusKnotOptions() = default;

//--------------------------------------------------------------------------------------------------
// Tube mesh options
//--------------------------------------------------------------------------------------------------

TubeOptions::TubeOptions()
    : radius{std::nullopt}
    , tessellation{std::nullopt}
    , radiusFunction{nullptr}
    , cap{std::nullopt}
    , arc{std::nullopt}
    , updatable{std::nullopt}
    , sideOrientation{std::nullopt}
    , frontUVs{std::nullopt}
    , backUVs{std::nullopt}
    , instance{nullptr}
    , invertUV{std::nullopt}
{
}

TubeOptions::~TubeOptions() = default;

} // end of namespace BABYLON
