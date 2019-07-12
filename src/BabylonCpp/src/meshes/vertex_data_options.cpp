#include <babylon/meshes/vertex_data_options.h>

#include <babylon/meshes/mesh.h>

namespace BABYLON {

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
    , alphaFilter{0.f}
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

} // end of namespace BABYLON
