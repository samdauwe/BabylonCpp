#ifndef BABYLON_MESHES_BUILDERS_MESH_BUILDER_OPTIONS_H
#define BABYLON_MESHES_BUILDERS_MESH_BUILDER_OPTIONS_H

#include <array>
#include <functional>

#include <babylon/babylon_api.h>
#include <babylon/core/structs.h>
#include <babylon/math/color3.h>
#include <babylon/math/color4.h>
#include <babylon/math/isize.h>
#include <babylon/math/vector2.h>
#include <babylon/math/vector4.h>

namespace BABYLON {

class GroundMesh;
class LinesMesh;
class Mesh;
class plane;
using GroundMeshPtr = std::shared_ptr<GroundMesh>;
using LinesMeshPtr  = std::shared_ptr<LinesMesh>;
using MeshPtr       = std::shared_ptr<Mesh>;
using PlanePtr      = std::shared_ptr<Plane>;

//------------------------------------------------------------------------------
// Box mesh options
//------------------------------------------------------------------------------

/**
 * @brief Options used to create a box mesh.
 */
class BABYLON_SHARED_EXPORT BoxOptions {

public:
  BoxOptions();
  ~BoxOptions();

public:
  std::optional<float> size;
  std::optional<float> width;
  std::optional<float> height;
  std::optional<float> depth;
  std::array<Vector4, 6> faceUV;
  std::array<Color4, 6> faceColors;
  std::optional<unsigned int> sideOrientation;
  std::optional<Vector4> frontUVs;
  std::optional<Vector4> backUVs;
  std::optional<bool> wrap;
  std::optional<unsigned int> topBaseAt;
  std::optional<unsigned int> bottomBaseAt;
  std::optional<bool> updatable;

}; // end of class BoxOptions

//------------------------------------------------------------------------------
// Cylinder or cone mesh options
//------------------------------------------------------------------------------

/**
 * @brief Options used to create a cylinder or cone mesh.
 */
class BABYLON_SHARED_EXPORT CylinderOptions {

public:
  CylinderOptions();
  ~CylinderOptions();

public:
  std::optional<float> height;
  std::optional<float> diameterTop;
  std::optional<float> diameterBottom;
  std::optional<float> diameter;
  std::optional<unsigned int> tessellation;
  std::optional<unsigned int> subdivisions;
  std::optional<float> arc;
  std::vector<Color4> faceColors;
  std::vector<Vector4> faceUV;
  std::optional<bool> updatable;
  std::optional<bool> hasRings;
  std::optional<bool> enclose;
  std::optional<unsigned int> sideOrientation;
  std::optional<Vector4> frontUVs;
  std::optional<Vector4> backUVs;

}; // end of class CylinderOptions

//------------------------------------------------------------------------------
// Dashed lines mesh options
//------------------------------------------------------------------------------

/**
 * @brief Options used to create a dashed lines mesh.
 */
class BABYLON_SHARED_EXPORT DashedLinesOptions {

public:
  DashedLinesOptions();
  ~DashedLinesOptions();

public:
  std::vector<Vector3> points;
  std::optional<float> dashSize;
  std::optional<float> gapSize;
  std::optional<unsigned int> dashNb;
  std::optional<bool> updatable;
  LinesMeshPtr instance;

}; // end of class DashedLinesOptions

//------------------------------------------------------------------------------
// Decal mesh options
//------------------------------------------------------------------------------

/**
 * @brief Options used to create a decal mesh.
 */
class BABYLON_SHARED_EXPORT DecalOptions {

public:
  DecalOptions();
  ~DecalOptions();

public:
  std::optional<Vector3> position;
  std::optional<Vector3> normal;
  std::optional<Vector3> size;
  std::optional<float> angle;

}; // end of class DecalOptions

//------------------------------------------------------------------------------
// Disc mesh options
//------------------------------------------------------------------------------

/**
 * @brief Options used to create a disc mesh.
 */
class BABYLON_SHARED_EXPORT DiscOptions {

public:
  DiscOptions();
  ~DiscOptions();

public:
  std::optional<float> radius;
  std::optional<unsigned int> tessellation;
  std::optional<float> arc;
  std::optional<bool> updatable;
  std::optional<unsigned int> sideOrientation;
  std::optional<Vector4> frontUVs;
  std::optional<Vector4> backUVs;

}; // end of class DiscOptions

//------------------------------------------------------------------------------
// Extrude shape mesh options
//------------------------------------------------------------------------------

/**
 * @brief Options used to create an extruded shape mesh.
 */
class BABYLON_SHARED_EXPORT ExtrudeShapeOptions {

public:
  ExtrudeShapeOptions();
  ~ExtrudeShapeOptions();

public:
  std::vector<Vector3> shape;
  std::vector<Vector3> path;
  std::optional<float> scale;
  std::optional<float> rotation;
  std::optional<unsigned int> cap;
  std::optional<bool> updatable;
  std::optional<unsigned int> sideOrientation;
  std::optional<Vector4> frontUVs;
  std::optional<Vector4> backUVs;
  MeshPtr instance;
  std::optional<bool> invertUV;

}; // end of class ExtrudeShapeOptions

//------------------------------------------------------------------------------
// Extrude shape custom mesh options
//------------------------------------------------------------------------------

/**
 * @brief Options used to create an custom extruded shape mesh.
 */
class BABYLON_SHARED_EXPORT ExtrudeShapeCustomOptions {

public:
  ExtrudeShapeCustomOptions();
  ~ExtrudeShapeCustomOptions();

public:
  std::vector<Vector3> shape;
  std::vector<Vector3> path;
  std::function<float(float i, float distance)> scaleFunction;
  std::function<float(float i, float distance)> rotationFunction;
  std::optional<bool> ribbonCloseArray;
  std::optional<bool> ribbonClosePath;
  std::optional<unsigned int> cap;
  std::optional<bool> updatable;
  std::optional<unsigned int> sideOrientation;
  std::optional<Vector4> frontUVs;
  std::optional<Vector4> backUVs;
  MeshPtr instance;
  std::optional<bool> invertUV;

}; // end of class ExtrudeShapeCustomOptions

//------------------------------------------------------------------------------
// Ground from height map mesh options
//------------------------------------------------------------------------------

/**
 * @brief Options used to create a ground from height map mesh.
 */
class BABYLON_SHARED_EXPORT GroundFromHeightMapOptions {

public:
  GroundFromHeightMapOptions();
  ~GroundFromHeightMapOptions();

public:
  std::optional<float> width;
  std::optional<float> height;
  std::optional<unsigned int> subdivisions;
  std::optional<float> minHeight;
  std::optional<float> maxHeight;
  std::optional<Color3> colorFilter;
  std::optional<float> alphaFilter;
  std::optional<bool> updatable;
  std::optional<unsigned int> bufferWidth;
  std::optional<unsigned int> bufferHeight;
  Uint8Array buffer;
  std::function<void(const GroundMeshPtr& mesh)> onReady;

}; // end of class GroundFromHeightMapOptions

//------------------------------------------------------------------------------
// Ground mesh options
//------------------------------------------------------------------------------

/**
 * @brief Options used to create a ground mesh.
 */
class BABYLON_SHARED_EXPORT GroundOptions {

public:
  GroundOptions();
  ~GroundOptions();

public:
  std::optional<unsigned int> width;
  std::optional<unsigned int> height;
  std::optional<unsigned int> subdivisions;
  std::optional<unsigned int> subdivisionsX;
  std::optional<unsigned int> subdivisionsY;
  std::optional<bool> updatable;

}; // end of class GroundOptions

//------------------------------------------------------------------------------
// Hemisphere mesh options
//------------------------------------------------------------------------------

/**
 * @brief Options used to create a hemisphere mesh.
 */
class BABYLON_SHARED_EXPORT HemisphereOptions {

public:
  HemisphereOptions();
  ~HemisphereOptions();

public:
  std::optional<unsigned int> segments;
  std::optional<float> diameter;
  std::optional<unsigned int> sideOrientation;

}; // end of class HemisphereOptions

//------------------------------------------------------------------------------
// Icosphere mesh options
//------------------------------------------------------------------------------

/**
 * @brief Options used to create a icosphere mesh.
 */
class BABYLON_SHARED_EXPORT IcoSphereOptions {

public:
  IcoSphereOptions();
  ~IcoSphereOptions();

public:
  std::optional<float> radius;
  std::optional<float> radiusX;
  std::optional<float> radiusY;
  std::optional<float> radiusZ;
  std::optional<bool> flat;
  std::optional<unsigned int> subdivisions;
  std::optional<unsigned int> sideOrientation;
  std::optional<Vector4> frontUVs;
  std::optional<Vector4> backUVs;
  std::optional<bool> updatable;

}; // end of class IcoSphereOptions

//------------------------------------------------------------------------------
// Lathe mesh options
//------------------------------------------------------------------------------

/**
 * @brief Options used to a lathe mesh.
 */
class BABYLON_SHARED_EXPORT LatheOptions {

public:
  LatheOptions();
  ~LatheOptions();

public:
  std::vector<Vector3> shape;
  std::optional<float> radius;
  std::optional<unsigned int> tessellation;
  std::optional<unsigned int> clip;
  std::optional<float> arc;
  std::optional<bool> closed;
  std::optional<bool> updatable;
  std::optional<unsigned int> sideOrientation;
  std::optional<Vector4> frontUVs;
  std::optional<Vector4> backUVs;
  std::optional<unsigned int> cap;
  std::optional<bool> invertUV;

}; // end of class LatheOptions

//------------------------------------------------------------------------------
// Lines mesh options
//------------------------------------------------------------------------------

/**
 * @brief Options used to create a lines mesh.
 */
class BABYLON_SHARED_EXPORT LinesOptions {

public:
  LinesOptions();
  ~LinesOptions();

public:
  std::vector<Vector3> points;
  std::vector<Color4> colors;
  std::optional<bool> updatable;
  std::optional<bool> useVertexAlpha;
  LinesMeshPtr instance;

}; // end of class LinesOptions

//------------------------------------------------------------------------------
// Line system mesh options
//------------------------------------------------------------------------------

/**
 * @brief Options used to create a line system mesh.
 */
class BABYLON_SHARED_EXPORT LineSystemOptions {

public:
  LineSystemOptions();
  LineSystemOptions(LinesOptions& linesOptions);
  ~LineSystemOptions();

public:
  std::vector<std::vector<Vector3>> lines;
  std::vector<std::vector<Color4>> colors;
  std::optional<bool> updatable;
  std::optional<bool> useVertexAlpha;
  LinesMeshPtr instance;

}; // end of class LineSystemOptions

//------------------------------------------------------------------------------
// Plane mesh options
//------------------------------------------------------------------------------

/**
 * @brief Options used to create a plane mesh.
 */
class BABYLON_SHARED_EXPORT PlaneOptions {

public:
  PlaneOptions();
  ~PlaneOptions();

public:
  std::optional<float> size;
  std::optional<float> width;
  std::optional<float> height;
  std::optional<unsigned int> sideOrientation;
  std::optional<Vector4> frontUVs;
  std::optional<Vector4> backUVs;
  std::optional<bool> updatable;
  PlanePtr sourcePlane;

}; // end of class PlaneOptions

//------------------------------------------------------------------------------
// Polyhedron mesh options
//------------------------------------------------------------------------------

/**
 * @brief Options used to create a polyhedron mesh.
 */
class BABYLON_SHARED_EXPORT PolyhedronOptions {

public:
  PolyhedronOptions();
  ~PolyhedronOptions();

public:
  std::optional<unsigned int> type;
  std::optional<float> size;
  std::optional<float> sizeX;
  std::optional<float> sizeY;
  std::optional<float> sizeZ;
  std::optional<Polyhedron> custom;
  std::vector<Vector4> faceUV;
  std::vector<Color4> faceColors;
  std::optional<bool> flat;
  std::optional<bool> updatable;
  std::optional<unsigned int> sideOrientation;
  std::optional<Vector4> frontUVs;
  std::optional<Vector4> backUVs;

}; // end of class PolyhedronOptions

//------------------------------------------------------------------------------
// Polygon mesh options
//------------------------------------------------------------------------------

/**
 * @brief Options used to create a polygon mesh.
 */
class BABYLON_SHARED_EXPORT PolygonOptions {

public:
  PolygonOptions();
  ~PolygonOptions();

public:
  std::vector<Vector3> shape;
  std::vector<std::vector<Vector3>> holes;
  std::optional<float> depth;
  std::vector<Vector4> faceUV;
  std::vector<Color4> faceColors;
  std::optional<bool> updatable;
  std::optional<unsigned int> sideOrientation;
  Vector4 frontUVs;
  Vector4 backUVs;

}; // end of class PolygonOptions

//------------------------------------------------------------------------------
// Ribbon mesh options
//------------------------------------------------------------------------------

/**
 * @brief Options used to create a ribbon mesh.
 */
class BABYLON_SHARED_EXPORT RibbonOptions {

public:
  RibbonOptions();
  ~RibbonOptions();

public:
  std::vector<std::vector<Vector3>> pathArray;
  std::optional<bool> closeArray;
  std::optional<bool> closePath;
  std::optional<size_t> offset;
  std::optional<bool> updatable;
  std::optional<unsigned int> sideOrientation;
  std::optional<Vector4> frontUVs;
  std::optional<Vector4> backUVs;
  MeshPtr instance;
  std::optional<bool> invertUV;
  std::vector<Vector2> uvs;
  std::vector<Color4> colors;

}; // end of class RibbonOptions

//------------------------------------------------------------------------------
// Sphere mesh options
//------------------------------------------------------------------------------

/**
 * @brief Options used to create a sphere mesh.
 */
class BABYLON_SHARED_EXPORT SphereOptions {

public:
  SphereOptions();
  ~SphereOptions();

public:
  std::optional<unsigned int> segments;
  std::optional<float> diameter;
  std::optional<float> diameterX;
  std::optional<float> diameterY;
  std::optional<float> diameterZ;
  std::optional<float> arc;
  std::optional<float> slice;
  std::optional<unsigned int> sideOrientation;
  std::optional<Vector4> frontUVs;
  std::optional<Vector4> backUVs;
  std::optional<bool> updatable;

}; // end of class SphereOptions

//------------------------------------------------------------------------------
// Tiled ground mesh options
//------------------------------------------------------------------------------

/**
 * @brief Options used to create a tiled ground mesh.
 */
class BABYLON_SHARED_EXPORT TiledGroundOptions {

public:
  TiledGroundOptions();
  ~TiledGroundOptions();

public:
  std::optional<float> xmin, zmin;
  std::optional<float> xmax, zmax;
  std::optional<ISize> subdivisions;
  std::optional<ISize> precision;
  std::optional<bool> updatable;

}; // end of class TiledGroundOptions

//------------------------------------------------------------------------------
// Torus mesh options
//------------------------------------------------------------------------------

/**
 * @brief Options used to create a torus mesh.
 */
class BABYLON_SHARED_EXPORT TorusOptions {

public:
  TorusOptions();
  ~TorusOptions();

public:
  std::optional<float> diameter;
  std::optional<float> thickness;
  std::optional<unsigned int> tessellation;
  std::optional<bool> updatable;
  std::optional<unsigned int> sideOrientation;
  std::optional<Vector4> frontUVs;
  std::optional<Vector4> backUVs;

}; // end of class TorusOptions

//------------------------------------------------------------------------------
// TorusKnot mesh options
//------------------------------------------------------------------------------

/**
 * @brief Options used to create a torus knot mesh.
 */
class BABYLON_SHARED_EXPORT TorusKnotOptions {

public:
  TorusKnotOptions();
  ~TorusKnotOptions();

public:
  std::optional<float> radius;
  std::optional<float> tube;
  std::optional<unsigned int> radialSegments;
  std::optional<unsigned int> tubularSegments;
  std::optional<float> p;
  std::optional<float> q;
  std::optional<bool> updatable;
  std::optional<unsigned int> sideOrientation;
  std::optional<Vector4> frontUVs;
  std::optional<Vector4> backUVs;

}; // end of class TorusKnotOptions

//------------------------------------------------------------------------------
// Tube mesh options
//------------------------------------------------------------------------------

/**
 * @brief Options used to create a tube mesh.
 */
class BABYLON_SHARED_EXPORT TubeOptions {

public:
  TubeOptions();
  ~TubeOptions();

public:
  std::vector<Vector3> path;
  std::optional<float> radius;
  std::optional<unsigned int> tessellation;
  std::function<float(unsigned int i, float distance)> radiusFunction;
  std::optional<unsigned int> cap;
  std::optional<float> arc;
  std::optional<bool> updatable;
  std::optional<unsigned int> sideOrientation;
  std::optional<Vector4> frontUVs;
  std::optional<Vector4> backUVs;
  MeshPtr instance;
  std::optional<bool> invertUV;

}; // end of class TubeOptions

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_BUILDERS_BOX_BUILDER_H
