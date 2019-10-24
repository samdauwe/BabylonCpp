#include <babylon/meshes/builders/mesh_builder_options.h>

namespace BABYLON {

//------------------------------------------------------------------------------
// Box mesh options
//------------------------------------------------------------------------------

BoxOptions::BoxOptions()
    : size{std::nullopt}
    , width{std::nullopt}
    , height{std::nullopt}
    , depth{std::nullopt}
    , faceUV{{Vector4(0.f, 0.f, 1.f, 1.f), Vector4(0.f, 0.f, 1.f, 1.f),
              Vector4(0.f, 0.f, 1.f, 1.f), Vector4(0.f, 0.f, 1.f, 1.f),
              Vector4(0.f, 0.f, 1.f, 1.f), Vector4(0.f, 0.f, 1.f, 1.f)}}
    , faceColors{{Color4(1.f, 1.f, 1.f, 1.f), Color4(1.f, 1.f, 1.f, 1.f),
                  Color4(1.f, 1.f, 1.f, 1.f), Color4(1.f, 1.f, 1.f, 1.f),
                  Color4(1.f, 1.f, 1.f, 1.f), Color4(1.f, 1.f, 1.f, 1.f)}}
    , sideOrientation{std::nullopt}
    , frontUVs{std::nullopt}
    , backUVs{std::nullopt}
    , wrap{std::nullopt}
    , topBaseAt{std::nullopt}
    , bottomBaseAt{std::nullopt}
    , updatable{std::nullopt}
{
}

BoxOptions::~BoxOptions()
{
}

//------------------------------------------------------------------------------
// Cylinder or cone mesh options
//------------------------------------------------------------------------------

CylinderOptions::CylinderOptions()
    : height{std::nullopt}
    , diameterTop{std::nullopt}
    , diameterBottom{std::nullopt}
    , diameter{std::nullopt}
    , tessellation{std::nullopt}
    , subdivisions{std::nullopt}
    , arc{std::nullopt}
    , faceColors{Color4(1.f, 1.f, 1.f, 1.f), Color4(1.f, 1.f, 1.f, 1.f),
                 Color4(1.f, 1.f, 1.f, 1.f)}
    , faceUV{Vector4(0.f, 0.f, 1.f, 1.f), Vector4(0.f, 0.f, 1.f, 1.f),
             Vector4(0.f, 0.f, 1.f, 1.f)}
    , updatable{std::nullopt}
    , hasRings{std::nullopt}
    , enclose{std::nullopt}
    , cap{std::nullopt}
    , sideOrientation{std::nullopt}
    , frontUVs{std::nullopt}
    , backUVs{std::nullopt}
{
}

CylinderOptions::~CylinderOptions()
{
}

//------------------------------------------------------------------------------
// Dashed lines mesh options
//------------------------------------------------------------------------------

DashedLinesOptions::DashedLinesOptions()
    : dashSize{std::nullopt}
    , gapSize{std::nullopt}
    , dashNb{std::nullopt}
    , updatable{std::nullopt}
    , instance{nullptr}
{
}

DashedLinesOptions::~DashedLinesOptions()
{
}

//------------------------------------------------------------------------------
// Decal mesh options
//------------------------------------------------------------------------------

DecalOptions::DecalOptions()
    : position{std::nullopt}
    , normal{std::nullopt}
    , size{std::nullopt}
    , angle{std::nullopt}
{
}

DecalOptions::~DecalOptions()
{
}

//------------------------------------------------------------------------------
// Disc mesh options
//------------------------------------------------------------------------------

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

DiscOptions::~DiscOptions()
{
}

//------------------------------------------------------------------------------
// Extrude shape mesh options
//------------------------------------------------------------------------------

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

ExtrudeShapeOptions::~ExtrudeShapeOptions()
{
}

//------------------------------------------------------------------------------
// Extrude shape custom mesh options
//------------------------------------------------------------------------------

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
    , instance{}
    , invertUV{std::nullopt}
{
}

ExtrudeShapeCustomOptions::~ExtrudeShapeCustomOptions()
{
}

//------------------------------------------------------------------------------
// Ground from height map mesh options
//------------------------------------------------------------------------------

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

GroundFromHeightMapOptions::~GroundFromHeightMapOptions()
{
}

//------------------------------------------------------------------------------
// Ground mesh options
//------------------------------------------------------------------------------

GroundOptions::GroundOptions()
    : width{std::nullopt}
    , height{std::nullopt}
    , subdivisions{std::nullopt}
    , subdivisionsX{std::nullopt}
    , subdivisionsY{std::nullopt}
    , updatable{std::nullopt}
{
}

GroundOptions::~GroundOptions()
{
}

//------------------------------------------------------------------------------
// Hemisphere mesh options
//------------------------------------------------------------------------------

HemisphereOptions::HemisphereOptions()
    : segments{std::nullopt}
    , diameter{std::nullopt}
    , sideOrientation{std::nullopt}
{
}

HemisphereOptions::~HemisphereOptions()
{
}

//------------------------------------------------------------------------------
// Icosphere mesh options
//------------------------------------------------------------------------------

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

IcoSphereOptions::~IcoSphereOptions()
{
}

//------------------------------------------------------------------------------
// Lathe mesh options
//------------------------------------------------------------------------------

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

LatheOptions::~LatheOptions()
{
}

//------------------------------------------------------------------------------
// Lines mesh options
//------------------------------------------------------------------------------

LinesOptions::LinesOptions()
    : updatable{std::nullopt}, useVertexAlpha{std::nullopt}, instance{nullptr}
{
}

LinesOptions::~LinesOptions()
{
}

//------------------------------------------------------------------------------
// Line system mesh options
//------------------------------------------------------------------------------

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

LineSystemOptions::~LineSystemOptions()
{
}

//------------------------------------------------------------------------------
// Plane mesh options
//------------------------------------------------------------------------------

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

PlaneOptions::~PlaneOptions()
{
}

//------------------------------------------------------------------------------
// Polyhedron mesh options
//------------------------------------------------------------------------------

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

PolyhedronOptions::~PolyhedronOptions()
{
}

//------------------------------------------------------------------------------
// Polygon mesh options
//------------------------------------------------------------------------------

PolygonOptions::PolygonOptions()
    : depth{std::nullopt}
    , updatable{std::nullopt}
    , sideOrientation{std::nullopt}
    , frontUVs{Vector4(0.f, 0.f, 1.f, 1.f)}
    , backUVs{Vector4(0.f, 0.f, 1.f, 1.f)}
{
}

PolygonOptions::~PolygonOptions()
{
}

//------------------------------------------------------------------------------
// Ribbon mesh options
//------------------------------------------------------------------------------

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

RibbonOptions::~RibbonOptions()
{
}

//------------------------------------------------------------------------------
// Sphere mesh options
//------------------------------------------------------------------------------

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
    , updatable{std::nullopt}
{
}

SphereOptions::SphereOptions(const SphereOptions& other)
    : segments{other.segments}
    , diameter{other.diameter}
    , diameterX{other.diameterX}
    , diameterY{other.diameterY}
    , diameterZ{other.diameterZ}
    , arc{other.arc}
    , slice{other.slice}
    , sideOrientation{other.sideOrientation}
    , frontUVs{other.frontUVs}
    , backUVs{other.backUVs}
    , updatable{other.updatable}
{
}

SphereOptions::SphereOptions(SphereOptions&& other)
    : segments{std::move(other.segments)}
    , diameter{std::move(other.diameter)}
    , diameterX{std::move(other.diameterX)}
    , diameterY{std::move(other.diameterY)}
    , diameterZ{std::move(other.diameterZ)}
    , arc{std::move(other.arc)}
    , slice{std::move(other.slice)}
    , sideOrientation{std::move(other.sideOrientation)}
    , frontUVs{std::move(other.frontUVs)}
    , backUVs{std::move(other.backUVs)}
    , updatable{std::move(other.updatable)}
{
}

SphereOptions& SphereOptions::operator=(const SphereOptions& other)
{
  if (&other != this) {
    segments        = other.segments;
    diameter        = other.diameter;
    diameterX       = other.diameterX;
    diameterY       = other.diameterY;
    diameterZ       = other.diameterZ;
    arc             = other.arc;
    slice           = other.slice;
    sideOrientation = other.sideOrientation;
    frontUVs        = other.frontUVs;
    backUVs         = other.backUVs;
    updatable       = other.updatable;
  }

  return *this;
}

SphereOptions& SphereOptions::operator=(SphereOptions&& other)
{
  if (&other != this) {
    segments        = std::move(other.segments);
    diameter        = std::move(other.diameter);
    diameterX       = std::move(other.diameterX);
    diameterY       = std::move(other.diameterY);
    diameterZ       = std::move(other.diameterZ);
    arc             = std::move(other.arc);
    slice           = std::move(other.slice);
    sideOrientation = std::move(other.sideOrientation);
    frontUVs        = std::move(other.frontUVs);
    backUVs         = std::move(other.backUVs);
    updatable       = std::move(other.updatable);
  }

  return *this;
}

SphereOptions::~SphereOptions()
{
}

//------------------------------------------------------------------------------
// Tiled ground mesh options
//------------------------------------------------------------------------------

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

TiledGroundOptions::~TiledGroundOptions()
{
}

//------------------------------------------------------------------------------
// Torus mesh options
//------------------------------------------------------------------------------

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

TorusOptions::~TorusOptions()
{
}

//------------------------------------------------------------------------------
// TorusKnot mesh options
//------------------------------------------------------------------------------

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

TorusKnotOptions::~TorusKnotOptions()
{
}

//------------------------------------------------------------------------------
// Tube mesh options
//------------------------------------------------------------------------------

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

TubeOptions::~TubeOptions()
{
}

} // end of namespace BABYLON
