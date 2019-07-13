#include <babylon/meshes/builders/lines_builder.h>

#include <babylon/meshes/_creation_data_storage.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/lines_mesh.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/meshes/vertex_data.h>

namespace BABYLON {

LinesMeshPtr LinesBuilder::CreateLineSystem(const std::string& name,
                                            LineSystemOptions& options,
                                            Scene* scene)
{
  const auto& instance = options.instance;
  const auto& lines    = options.lines;
  const auto& colors   = options.colors;

  if (instance) { // lines update
    auto positions = instance->getVerticesData(VertexBuffer::PositionKind);
    Float32Array vertexColor;
    std::vector<Color4> lineColors;
    if (!colors.empty()) {
      vertexColor = instance->getVerticesData(VertexBuffer::ColorKind);
    }
    size_t i = 0;
    size_t c = 0;
    for (size_t l = 0; l < lines.size(); ++l) {
      const auto& points = lines[l];
      for (size_t p = 0; p < points.size(); p++) {
        positions[i]     = points[p].x;
        positions[i + 1] = points[p].y;
        positions[i + 2] = points[p].z;
        if (!colors.empty() && !vertexColor.empty()) {
          lineColors         = colors[l];
          vertexColor[c]     = lineColors[p].r;
          vertexColor[c + 1] = lineColors[p].g;
          vertexColor[c + 2] = lineColors[p].b;
          vertexColor[c + 3] = lineColors[p].a;
          c += 4;
        }
        i += 3;
      }
    }
    instance->updateVerticesData(VertexBuffer::PositionKind, positions, false,
                                 false);
    if (!colors.empty() && !vertexColor.empty()) {
      instance->updateVerticesData(VertexBuffer::ColorKind, vertexColor, false,
                                   false);
    }
    return instance;
  }

  // line system creation
  auto useVertexColor = (!colors.empty()) ? true : false;
  auto lineSystem
    = LinesMesh::New(name, scene, nullptr, nullptr, true, useVertexColor,
                     options.useVertexAlpha.value_or(true));
  auto vertexData = VertexData::CreateLineSystem(options);
  vertexData->applyToMesh(*lineSystem, options.updatable);
  return lineSystem;
}

LinesMeshPtr LinesBuilder::CreateLines(const std::string& name,
                                       LinesOptions& options, Scene* scene)
{
  LineSystemOptions lineSystemOptions(options);
  return LinesBuilder::CreateLineSystem(name, lineSystemOptions, scene);
}

LinesMeshPtr LinesBuilder::CreateDashedLines(const std::string& name,
                                             DashedLinesOptions& options,
                                             Scene* scene)
{
  const auto& points   = options.points;
  const auto& instance = options.instance;
  const auto gapSize   = options.gapSize.value_or(1.f);
  const auto dashSize  = options.dashSize.value_or(3.f);

  if (instance) { //  dashed lines update
    const auto positionFunction = [&](Float32Array& positions) -> void {
      auto curvect  = Vector3::Zero();
      auto nbSeg    = positions.size() / 6;
      auto lg       = 0.f;
      auto nb       = 0u;
      auto shft     = 0.f;
      auto dashshft = 0.f;
      auto curshft  = 0.f;
      auto p        = 0u;
      auto j        = 0u;
      for (std::size_t i = 0; i < points.size() - 1; ++i) {
        points[i + 1].subtractToRef(points[i], curvect);
        lg += curvect.length();
      }
      shft          = lg / static_cast<float>(nbSeg);
      auto dashSize = instance->_creationDataStorage->dashSize;
      auto gapSize  = instance->_creationDataStorage->gapSize;
      dashshft      = dashSize * shft / static_cast<float>(dashSize + gapSize);
      std::size_t i = 0;
      for (i = 0; i < points.size() - 1; ++i) {
        points[i + 1].subtractToRef(points[i], curvect);
        nb = static_cast<unsigned int>(std::floor(curvect.length() / shft));
        curvect.normalize();
        j = 0;
        while (j < nb && p < positions.size()) {
          curshft          = shft * static_cast<float>(j);
          positions[p]     = points[i].x + curshft * curvect.x;
          positions[p + 1] = points[i].y + curshft * curvect.y;
          positions[p + 2] = points[i].z + curshft * curvect.z;
          positions[p + 3] = points[i].x + (curshft + dashshft) * curvect.x;
          positions[p + 4] = points[i].y + (curshft + dashshft) * curvect.y;
          positions[p + 5] = points[i].z + (curshft + dashshft) * curvect.z;
          p += 6;
          ++j;
        }
      }
      while (p < positions.size()) {
        positions[p]     = points[i].x;
        positions[p + 1] = points[i].y;
        positions[p + 2] = points[i].z;
        p += 3;
      }
    };
    instance->updateMeshPositions(positionFunction, false);
    return instance;
  }
  // dashed lines creation
  auto dashedLines = LinesMesh::New(name, scene);
  auto vertexData  = VertexData::CreateDashedLines(options);
  vertexData->applyToMesh(*dashedLines, options.updatable);

  dashedLines->_creationDataStorage = std::make_shared<_CreationDataStorage>();
  dashedLines->_creationDataStorage->dashSize = dashSize;
  dashedLines->_creationDataStorage->gapSize  = gapSize;
  return dashedLines;
}

} // end of namespace BABYLON
