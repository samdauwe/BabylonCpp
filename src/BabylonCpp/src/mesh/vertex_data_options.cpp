#include <babylon/mesh/vertex_data_options.h>

#include <babylon/mesh/mesh.h>

namespace BABYLON {

//------------------------------------------------------------------------------
// Box mesh options
//------------------------------------------------------------------------------

std::array<Vector4, 6> BoxOptions::DefaultBoxFaceUV{
  {Vector4(0.f, 0.f, 1.f, 1.f), Vector4(0.f, 0.f, 1.f, 1.f),
   Vector4(0.f, 0.f, 1.f, 1.f), Vector4(0.f, 0.f, 1.f, 1.f),
   Vector4(0.f, 0.f, 1.f, 1.f), Vector4(0.f, 0.f, 1.f, 1.f)}};
std::array<Color4, 6> BoxOptions::DefaultBoxFaceColors{
  {Color4(1.f, 1.f, 1.f, 1.f), Color4(1.f, 1.f, 1.f, 1.f),
   Color4(1.f, 1.f, 1.f, 1.f), Color4(1.f, 1.f, 1.f, 1.f),
   Color4(1.f, 1.f, 1.f, 1.f), Color4(1.f, 1.f, 1.f, 1.f)}};
Vector4 BoxOptions::DefaultFrontUVs{Vector4(0.f, 0.f, 1.f, 1.f)};
Vector4 BoxOptions::DefaultBackUVs{Vector4(0.f, 0.f, 1.f, 1.f)};

BoxOptions::BoxOptions()
    : width{1.f}
    , height{1.f}
    , depth{1.f}
    , faceUV{DefaultBoxFaceUV}
    , faceColors{DefaultBoxFaceColors}
    , sideOrientation{Mesh::DEFAULTSIDE}
    , frontUVs{DefaultFrontUVs}
    , backUVs{DefaultBackUVs}
    , updatable{false}
{
}

BoxOptions::BoxOptions(float size)
    : width{size}
    , height{size}
    , depth{size}
    , faceUV{DefaultBoxFaceUV}
    , faceColors{DefaultBoxFaceColors}
    , sideOrientation{Mesh::DEFAULTSIDE}
    , frontUVs{DefaultFrontUVs}
    , backUVs{DefaultBackUVs}
    , updatable{false}
{
}

BoxOptions::~BoxOptions()
{
}

//------------------------------------------------------------------------------
// Cylinder or cone mesh options
//------------------------------------------------------------------------------

std::vector<Vector4> CylinderOptions::DefaultCylinderFaceUV{
  {Vector4(0.f, 0.f, 1.f, 1.f), Vector4(0.f, 0.f, 1.f, 1.f),
   Vector4(0.f, 0.f, 1.f, 1.f)}};
std::vector<Color4> CylinderOptions::DefaultCylinderFaceColors{
  {Color4(1.f, 1.f, 1.f, 1.f), Color4(1.f, 1.f, 1.f, 1.f),
   Color4(1.f, 1.f, 1.f, 1.f)}};
Vector4 CylinderOptions::DefaultFrontUVs{Vector4(0.f, 0.f, 1.f, 1.f)};
Vector4 CylinderOptions::DefaultBackUVs{Vector4(0.f, 0.f, 1.f, 1.f)};

CylinderOptions::CylinderOptions()
    : height{2.f}
    , diameterTop{1.f}
    , diameterBottom{1.f}
    , tessellation{24}
    , subdivisions{1}
    , hasRings{false}
    , enclose{false}
    , faceUV{DefaultCylinderFaceUV}
    , sideOrientation{Mesh::DEFAULTSIDE}
    , frontUVs{DefaultFrontUVs}
    , backUVs{DefaultBackUVs}
    , updatable{false}
    , _arc{1.f}
{
}

CylinderOptions::CylinderOptions(float diameter)
    : height{2.f}
    , diameterTop{diameter}
    , diameterBottom{diameter}
    , tessellation{24}
    , subdivisions{1}
    , hasRings{false}
    , enclose{false}
    , faceUV{DefaultCylinderFaceUV}
    , sideOrientation{Mesh::DEFAULTSIDE}
    , frontUVs{DefaultFrontUVs}
    , backUVs{DefaultBackUVs}
    , updatable{false}
    , _arc{1.f}
{
}

CylinderOptions::~CylinderOptions()
{
}

float CylinderOptions::arc() const
{
  return _arc;
}

void CylinderOptions::setArc(float value)
{
  _arc = (value <= 0 || value > 1) ? 1.f : value;
}

//------------------------------------------------------------------------------
// Dashed lines mesh options
//------------------------------------------------------------------------------

DashedLinesOptions::DashedLinesOptions()
    : dashSize{3.f}
    , gapSize{1.f}
    , dashNb{200}
    , updatable{false}
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
    : position{Vector3::Zero()}
    , normal{Vector3::Up()}
    , size{Vector3::One()}
    , angle{0.f}
    , calculateNormal{false}
{
}

DecalOptions::~DecalOptions()
{
}

//------------------------------------------------------------------------------
// Disc mesh options
//------------------------------------------------------------------------------

Vector4 DiscOptions::DefaultFrontUVs{Vector4(0.f, 0.f, 1.f, 1.f)};
Vector4 DiscOptions::DefaultBackUVs{Vector4(0.f, 0.f, 1.f, 1.f)};

DiscOptions::DiscOptions()
    : radius{0.5f}
    , tessellation{64}
    , sideOrientation{Mesh::DEFAULTSIDE}
    , frontUVs{DefaultFrontUVs}
    , backUVs{DefaultBackUVs}
    , updatable{false}
    , _arc{1.f}
{
}

DiscOptions::~DiscOptions()
{
}

float DiscOptions::arc() const
{
  return _arc;
}

void DiscOptions::setArc(float value)
{
  _arc = (value <= 0 || value > 1) ? 1.f : value;
}

//------------------------------------------------------------------------------
// Extrude shape mesh options
//------------------------------------------------------------------------------

Vector4 ExtrudeShapeOptions::DefaultFrontUVs{Vector4(0.f, 0.f, 1.f, 1.f)};
Vector4 ExtrudeShapeOptions::DefaultBackUVs{Vector4(0.f, 0.f, 1.f, 1.f)};

ExtrudeShapeOptions::ExtrudeShapeOptions()
    : rotation{0.f}
    , scale{1.f}
    , cap{Mesh::NO_CAP}
    , instance{nullptr}
    , invertUV{false}
    , sideOrientation{Mesh::DEFAULTSIDE}
    , frontUVs{DefaultFrontUVs}
    , backUVs{DefaultBackUVs}
    , updatable{false}
{
}

ExtrudeShapeOptions::~ExtrudeShapeOptions()
{
}

//------------------------------------------------------------------------------
// Extrude shape custom mesh options
//------------------------------------------------------------------------------

Vector4 ExtrudeShapeCustomOptions::DefaultFrontUVs{Vector4(0.f, 0.f, 1.f, 1.f)};
Vector4 ExtrudeShapeCustomOptions::DefaultBackUVs{Vector4(0.f, 0.f, 1.f, 1.f)};

ExtrudeShapeCustomOptions::ExtrudeShapeCustomOptions()
    : ribbonCloseArray{false}
    , ribbonClosePath{false}
    , cap{Mesh::NO_CAP}
    , instance{nullptr}
    , invertUV{false}
    , sideOrientation{Mesh::DEFAULTSIDE}
    , frontUVs{DefaultFrontUVs}
    , backUVs{DefaultBackUVs}
    , updatable{false}
{
  scaleFunction    = [](float, float) { return 1.f; };
  rotationFunction = [](float, float) { return 0.f; };
}

ExtrudeShapeCustomOptions::~ExtrudeShapeCustomOptions()
{
}

//------------------------------------------------------------------------------
// Ground from height map mesh options
//------------------------------------------------------------------------------

GroundFromHeightMapOptions::GroundFromHeightMapOptions()
    : width{10.f}
    , height{10.f}
    , subdivisions{1}
    , minHeight{0.f}
    , maxHeight{1.f}
    , bufferWidth{1}
    , bufferHeight{1}
    , alphaFilter{std::nullopt}
    , colorFilter{Color3(0.3f, 0.59f, 0.11f)}
    , updatable{false}
    , onReady{nullptr}
{
}

GroundFromHeightMapOptions::~GroundFromHeightMapOptions()
{
}

//------------------------------------------------------------------------------
// Ground mesh options
//------------------------------------------------------------------------------

GroundOptions::GroundOptions(unsigned int subdivisions)
    : width{1}
    , height{1}
    , subdivisionsX{subdivisions}
    , subdivisionsY{subdivisions}
    , updatable{false}
{
}

GroundOptions::~GroundOptions()
{
}

//------------------------------------------------------------------------------
// Icosphere mesh options
//------------------------------------------------------------------------------

Vector4 IcoSphereOptions::DefaultFrontUVs{Vector4(0.f, 0.f, 1.f, 1.f)};
Vector4 IcoSphereOptions::DefaultBackUVs{Vector4(0.f, 0.f, 1.f, 1.f)};

IcoSphereOptions::IcoSphereOptions()
    : radius{1.f}
    , radiusX{1.f}
    , radiusY{1.f}
    , radiusZ{1.f}
    , flat{true}
    , subdivisions{4}
    , sideOrientation{Mesh::DEFAULTSIDE}
    , frontUVs{DefaultFrontUVs}
    , backUVs{DefaultBackUVs}
    , updatable{false}
{
}

IcoSphereOptions::IcoSphereOptions(float _radius)
    : radius{_radius}
    , radiusX{_radius}
    , radiusY{_radius}
    , radiusZ{_radius}
    , flat{true}
    , subdivisions{4}
    , sideOrientation{Mesh::DEFAULTSIDE}
    , frontUVs{DefaultFrontUVs}
    , backUVs{DefaultBackUVs}
    , updatable{false}
{
}

IcoSphereOptions::~IcoSphereOptions()
{
}

//------------------------------------------------------------------------------
// Lathe mesh options
//------------------------------------------------------------------------------

Vector4 LatheOptions::DefaultFrontUVs{Vector4(0.f, 0.f, 1.f, 1.f)};
Vector4 LatheOptions::DefaultBackUVs{Vector4(0.f, 0.f, 1.f, 1.f)};

LatheOptions::LatheOptions()
    : radius{1.f}
    , tessellation{64}
    , clip{0}
    , closed{true}
    , updatable{false}
    , sideOrientation{Mesh::DEFAULTSIDE}
    , frontUVs{DefaultFrontUVs}
    , backUVs{DefaultBackUVs}
    , cap{Mesh::NO_CAP}
    , invertUV{false}
    , _arc{1.f}
{
}

LatheOptions::~LatheOptions()
{
}

float LatheOptions::arc() const
{
  return _arc;
}

void LatheOptions::setArc(float value)
{
  _arc = (value <= 0 || value > 1) ? 1.f : value;
}

//------------------------------------------------------------------------------
// Lines mesh options
//------------------------------------------------------------------------------

LinesOptions::LinesOptions() : updatable{false}, instance{nullptr}
{
}

LinesOptions::~LinesOptions()
{
}

//------------------------------------------------------------------------------
// Line system mesh options
//------------------------------------------------------------------------------

LineSystemOptions::LineSystemOptions() : updatable{false}, instance{nullptr}
{
}

LineSystemOptions::LineSystemOptions(LinesOptions& options)
    : updatable{options.updatable}
    , useVertexAlpha{options.useVertexAlpha}
    , instance{options.instance}
{
  if (!options.points.empty()) {
    lines.emplace_back(options.points);
  }
  if (!options.colors.empty()) {
    colors.emplace_back(options.colors);
  }
}

LineSystemOptions::~LineSystemOptions()
{
}

//------------------------------------------------------------------------------
// Plane mesh options
//------------------------------------------------------------------------------

Vector4 PlaneOptions::DefaultFrontUVs{Vector4(0.f, 0.f, 1.f, 1.f)};
Vector4 PlaneOptions::DefaultBackUVs{Vector4(0.f, 0.f, 1.f, 1.f)};

PlaneOptions::PlaneOptions()
    : width{1.f}
    , height{1.f}
    , sideOrientation{Mesh::DEFAULTSIDE}
    , frontUVs{DefaultFrontUVs}
    , backUVs{DefaultBackUVs}
    , updatable{false}
    , sourcePlane{nullptr}
{
}

PlaneOptions::PlaneOptions(float size)
    : width{size}
    , height{size}
    , sideOrientation{Mesh::DEFAULTSIDE}
    , frontUVs{DefaultFrontUVs}
    , backUVs{DefaultBackUVs}
    , updatable{false}
    , sourcePlane{nullptr}
{
}

PlaneOptions::~PlaneOptions()
{
}

//------------------------------------------------------------------------------
// Polygon mesh options
//------------------------------------------------------------------------------

Vector4 PolygonOptions::DefaultFrontUVs{Vector4(0.f, 0.f, 1.f, 1.f)};
Vector4 PolygonOptions::DefaultBackUVs{Vector4(0.f, 0.f, 1.f, 1.f)};

PolygonOptions::PolygonOptions()
    : depth{0.f}
    , updatable{false}
    , sideOrientation{Mesh::DEFAULTSIDE}
    , frontUVs{DefaultFrontUVs}
    , backUVs{DefaultBackUVs}
{
}

PolygonOptions::~PolygonOptions()
{
}

//------------------------------------------------------------------------------
// Polyhedron mesh options
//------------------------------------------------------------------------------

Vector4 PolyhedronOptions::DefaultFrontUVs{Vector4(0.f, 0.f, 1.f, 1.f)};
Vector4 PolyhedronOptions::DefaultBackUVs{Vector4(0.f, 0.f, 1.f, 1.f)};

PolyhedronOptions::PolyhedronOptions()
    : type{0}
    , size{1.f}
    , sizeX{1.f}
    , sizeY{1.f}
    , sizeZ{1.f}
    , flat{true}
    , sideOrientation{Mesh::DEFAULTSIDE}
    , frontUVs{DefaultFrontUVs}
    , backUVs{DefaultBackUVs}
    , updatable{false}
{
}

PolyhedronOptions::PolyhedronOptions(float _size)
    : type{0}
    , size{_size}
    , sizeX{_size}
    , sizeY{_size}
    , sizeZ{_size}
    , flat{true}
    , sideOrientation{Mesh::DEFAULTSIDE}
    , frontUVs{DefaultFrontUVs}
    , backUVs{DefaultBackUVs}
    , updatable{false}
{
}

PolyhedronOptions::~PolyhedronOptions()
{
}

//------------------------------------------------------------------------------
// Ribbon mesh options
//------------------------------------------------------------------------------

RibbonOptions::RibbonOptions(const std::vector<std::vector<Vector3>>& pathArray,
                             int offset)
    : closeArray{false}
    , closePath{false}
    , invertUV{false}
    , sideOrientation{Mesh::DEFAULTSIDE}
    , frontUVs{std::nullopt}
    , backUVs{std::nullopt}
    , updatable{false}
    , instance{nullptr}
    , _pathArray{pathArray}
{
  size_t defaultOffset = 0;
  if (!_pathArray.empty()) {
    defaultOffset = _pathArray[0].size() / 2;
  }
  _offset = (offset == -1) ? defaultOffset : _offset;
  _offset = _offset > defaultOffset ? defaultOffset : _offset;
}

RibbonOptions::~RibbonOptions()
{
}

std::vector<std::vector<Vector3>>& RibbonOptions::pathArray()
{
  return _pathArray;
}

const std::vector<std::vector<Vector3>>& RibbonOptions::pathArray() const
{
  return _pathArray;
}

size_t RibbonOptions::offset() const
{
  return _offset;
}

//------------------------------------------------------------------------------
// Sphere mesh options
//------------------------------------------------------------------------------

Vector4 SphereOptions::DefaultFrontUVs{Vector4(0.f, 0.f, 1.f, 1.f)};
Vector4 SphereOptions::DefaultBackUVs{Vector4(0.f, 0.f, 1.f, 1.f)};

SphereOptions::SphereOptions()
    : segments{32}
    , diameterX{1.f}
    , diameterY{1.f}
    , diameterZ{1.f}
    , sideOrientation{Mesh::DEFAULTSIDE}
    , frontUVs{DefaultFrontUVs}
    , backUVs{DefaultBackUVs}
    , updatable{false}
    , _arc{1.f}
    , _slice{1.f}
{
}

SphereOptions::SphereOptions(float diameter)
    : segments{32}
    , diameterX{diameter}
    , diameterY{diameter}
    , diameterZ{diameter}
    , sideOrientation{Mesh::DEFAULTSIDE}
    , frontUVs{DefaultFrontUVs}
    , backUVs{DefaultBackUVs}
    , updatable{false}
    , _arc{1.f}
    , _slice{1.f}
{
}

SphereOptions::~SphereOptions()
{
}

float SphereOptions::arc() const
{
  return _arc;
}

void SphereOptions::setArc(float value)
{
  _arc = (value <= 0 || value > 1) ? 1.f : value;
}

float SphereOptions::slice() const
{
  return _slice;
}

void SphereOptions::setSlice(float value)
{
  _slice = (value <= 0) ? 1.f : value;
}

//------------------------------------------------------------------------------
// Tiled ground mesh options
//------------------------------------------------------------------------------

TiledGroundOptions::TiledGroundOptions()
    : xmin{-1.f}
    , zmin{-1.f}
    , xmax{1.f}
    , zmax{1.f}
    , subdivisions({1, 1})
    , precision({1, 1})
    , updatable{false}
{
}

TiledGroundOptions::~TiledGroundOptions()
{
}

//------------------------------------------------------------------------------
// TorusKnot mesh options
//------------------------------------------------------------------------------

Vector4 TorusKnotOptions::DefaultFrontUVs{Vector4(0.f, 0.f, 1.f, 1.f)};
Vector4 TorusKnotOptions::DefaultBackUVs{Vector4(0.f, 0.f, 1.f, 1.f)};

TorusKnotOptions::TorusKnotOptions()
    : radius{2.f}
    , tube{0.5f}
    , radialSegments{32}
    , tubularSegments{32}
    , p{2}
    , q{3}
    , sideOrientation{Mesh::DEFAULTSIDE}
    , frontUVs{DefaultFrontUVs}
    , backUVs{DefaultBackUVs}
    , updatable{false}
{
}

TorusKnotOptions::~TorusKnotOptions()
{
}

//------------------------------------------------------------------------------
// Torus mesh options
//------------------------------------------------------------------------------

Vector4 TorusOptions::DefaultFrontUVs{Vector4(0.f, 0.f, 1.f, 1.f)};
Vector4 TorusOptions::DefaultBackUVs{Vector4(0.f, 0.f, 1.f, 1.f)};

TorusOptions::TorusOptions()
    : diameter{1.f}
    , thickness{0.5f}
    , tessellation{16}
    , sideOrientation{Mesh::DEFAULTSIDE}
    , frontUVs{DefaultFrontUVs}
    , backUVs{DefaultBackUVs}
    , updatable{false}
{
}

TorusOptions::~TorusOptions()
{
}

//------------------------------------------------------------------------------
// Tube mesh options
//------------------------------------------------------------------------------

Vector4 TubeOptions::DefaultFrontUVs{Vector4(0.f, 0.f, 1.f, 1.f)};
Vector4 TubeOptions::DefaultBackUVs{Vector4(0.f, 0.f, 1.f, 1.f)};

TubeOptions::TubeOptions()
    : radius{1.f}
    , tessellation{64}
    , radiusFunction{nullptr}
    , cap{Mesh::NO_CAP}
    , sideOrientation{Mesh::DEFAULTSIDE}
    , frontUVs{DefaultFrontUVs}
    , backUVs{DefaultBackUVs}
    , instance{nullptr}
    , invertUV{false}
    , updatable{false}
    , _arc{1.f}
{
}

TubeOptions::~TubeOptions()
{
}

float TubeOptions::arc() const
{
  return _arc;
}

void TubeOptions::setArc(float value)
{
  _arc = (value <= 0 || value > 1) ? 1.f : value;
}

} // end of namespace BABYLON
