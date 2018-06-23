#include <babylon/extensions/hexplanetgeneration/simple_planet.h>

#include <babylon/engine/scene.h>
#include <babylon/extensions/hexplanetgeneration/icosphere.h>
#include <babylon/extensions/hexplanetgeneration/terrain/terrain.h>
#include <babylon/materials/standard_material.h>
#include <babylon/mesh/lines_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/vertex_data.h>

namespace BABYLON {
namespace Extensions {

SimplePlanet::SimplePlanet(float scale, float degree, Scene* scene,
                           const string_t& seed)
    : _degree{degree}
    , _scale{scale}
    , _scene{scene}
    , _seed{seed}
    , _renderDeformedMesh{true}
{
}

SimplePlanet::~SimplePlanet()
{
}

void SimplePlanet::initialize()
{
  _planet.mesh     = Mesh::New("planet", _scene);
  _terrain         = std::make_unique<Terrain>(_seed);
  _icosahedronMesh = Icosphere::generateIcosahedronMesh(
    static_cast<size_t>(_degree), _scale, _random);
}

Vector2 SimplePlanet::calculateUVCoord(const Vector3& p) const
{
  // Calculate the Miller Spherical Projection and map it to UV coordinates
  const float lat = std::asin(p.y / _scale);
  const float lon = std::atan2(p.z, p.x);

  float x = lon / Math::PI;
  float y = (5.f / 4.f)
            * std::log(std::tan((Math::PI / 4.f) + (2.f * lat / 5.f)))
            / 2.2523234430803587f;
  if (y < -1.f) {
    y = 1.f;
  }

  x = (x + 1.f) / 2.f;
  y = (y + 1.f) / 2.f;
  return Vector2(x, 1.f - y);
}

void SimplePlanet::revolve()
{
  _planet.mesh->rotation().y += -0.00005f * _scene->getAnimationRatio();

  if (_planet.selectedBorder) {
    _planet.selectedBorder->setRotation(_planet.mesh->rotation());
  }
}

void SimplePlanet::pickTile(unsigned int faceId)
{
  if (_planet.selectedBorder) {
    _planet.selectedBorder->dispose();
  }

  unsigned int tileId = _planet.faceToTile[faceId];
  PlanetTile& tile    = _planet.tiles[tileId];
  Color3 color(242.f / 255.f, 182.f / 255.f, 64.f / 255.f);
  std::vector<Vector3> linePositions;

  for (auto& tileCorner : tile.corners) {
    linePositions.emplace_back(tileCorner.position);
  }

  linePositions.emplace_back(linePositions[0]);

  _planet.selectedBorder = Mesh::CreateLines("lines", linePositions, _scene);
}

void SimplePlanet::render()
{
  auto material           = StandardMaterial::New("mat", _scene);
  material->specularColor = Color3(0.f, 0.f, 0.f); // No specular color

  IndicesArray indices;
  Float32Array colors;
  Float32Array positions;
  Float32Array uvs;

  // Generate dual polyhedron position and face indices
  for (size_t n = 0; n < _icosahedronMesh.nodes.size(); ++n) {
    const size_t relativeZeroIndex = positions.size() / 3;
    const size_t numFaces          = _icosahedronMesh.nodes[n].f.size();
    PlanetTile tile{n, Vector3::Zero(), {}};

    // Get all the centroids of the faces adjacent to this vertex
    for (size_t f = 0; f < numFaces; ++f) {
      auto& centroid
        = _icosahedronMesh.faces[_icosahedronMesh.nodes[n].f[f]].centroid;
      auto uv = calculateUVCoord(centroid);

      auto height = (_terrain->getHeight(uv.x, uv.y) * 2.f) - 1.f;
      auto normal = centroid.copy().normalize();

      tile.center.addInPlace(centroid.scale(1.f / numFaces));
      // if (_renderDeformedMesh) {
      //  centroid = centroid.add(normal.scaleInPlace(height * 2.f));
      //}

      PlanetCorner corner{centroid, uv};

      tile.corners.emplace_back(corner);

      positions.emplace_back(centroid.x);
      positions.emplace_back(centroid.y);
      positions.emplace_back(centroid.z);
    }
    _planet.tiles.emplace_back(tile);

    auto centerUV = calculateUVCoord(tile.center);
    auto color    = _terrain->getColor(centerUV.x, centerUV.y);

    for (size_t f = 0; f < numFaces; f++) {
      colors.emplace_back(color.r);
      colors.emplace_back(color.g);
      colors.emplace_back(color.b);
      colors.emplace_back(1.f);
    }

    for (size_t i = relativeZeroIndex; i < (positions.size() / 3) - 2; ++i) {
      _planet.faceToTile[indices.size() / 3] = static_cast<unsigned>(n);
      indices.emplace_back(relativeZeroIndex);
      indices.emplace_back(i + 1);
      indices.emplace_back(i + 2);
    }

    // Fix Zipper for Legitimate Diffuse Texture
    for (size_t i = relativeZeroIndex; i < (uvs.size() / 2) - 2; ++i) {
      size_t i1 = relativeZeroIndex * 2;
      size_t i2 = (i + 1) * 2;
      size_t i3 = (i + 2) * 2;

      Vector3 A(uvs[i1], uvs[i1 + 1], 0);
      Vector3 BA = Vector3(uvs[i2], uvs[i2 + 1], 0).subtract(A);
      Vector3 CA = Vector3(uvs[i3], uvs[i3 + 1], 0).subtract(A);

      Vector3 cross = Vector3::Cross(BA, CA);
      if (cross.z < 0.f) {
        if (uvs[i1] < 0.25f) {
          uvs[i1] += 1;
        }
        if (uvs[i2] < 0.25f) {
          uvs[i2] += 1;
        }
        if (uvs[i3] < 0.25f) {
          uvs[i3] += 1;
        }
      }
    }
  }

  auto vertexData = std::make_unique<VertexData>();

  Float32Array normals;
  VertexData::ComputeNormals(positions, indices, normals);
  vertexData->normals   = std::move(normals);
  vertexData->indices   = std::move(indices);
  vertexData->positions = std::move(positions);
  vertexData->colors    = std::move(colors);
  vertexData->applyToMesh(_planet.mesh, false);

  _planet.mesh->material = material;
}

} // end of namespace Extensions
} // end of namespace BABYLON
