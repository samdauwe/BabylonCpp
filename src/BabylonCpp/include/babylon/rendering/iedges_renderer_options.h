#ifndef BABYLON_RENDERING_IEDGES_RENDERER_OPTIONS_H
#define BABYLON_RENDERING_IEDGES_RENDERER_OPTIONS_H

#include <optional>

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Defines the additional options of the edges renderer.
 */
struct BABYLON_SHARED_EXPORT IEdgesRendererOptions {

  /**
   * Gets or sets a boolean indicating that the alternate edge finder algorithm must be used
   * If not defined, the default value is true
   */
  std::optional<bool> useAlternateEdgeFinder = std::nullopt;

  /**
   * Gets or sets a boolean indicating that the vertex merger fast processing must be used.
   * If not defined, the default value is true.
   * You should normally leave it undefined (or set it to true), except if you see some artifacts in
   * the edges rendering (can happen with complex geometries) This option is used only if
   * useAlternateEdgeFinder = true
   */
  std::optional<bool> useFastVertexMerger = std::nullopt;

  /**
   * During edges processing, the vertices are merged if they are close enough: epsilonVertexMerge
   * is the limit whithin which vertices are considered to be equal. The default value is 1e-6 This
   * option is used only if useAlternateEdgeFinder = true
   */
  std::optional<float> epsilonVertexMerge = std::nullopt;

  /**
   * Gets or sets a boolean indicating that tessellation should be applied before finding the edges.
   * You may need to activate this option if your geometry is a bit unusual, like having a vertex of
   * a triangle in-between two vertices of an edge of another triangle. It happens often when using
   * CSG to construct meshes. This option is used only if useAlternateEdgeFinder = true
   */
  std::optional<bool> applyTessellation = std::nullopt;

  /**
   * The limit under which 3 vertices are considered to be aligned. 3 vertices PQR are considered
   * aligned if distance(PQ) + distance(QR) - distance(PR) < epsilonVertexAligned The default value
   * is 1e-6 This option is used only if useAlternateEdgeFinder = true
   */
  std::optional<float> epsilonVertexAligned = std::nullopt;

}; // end of struct IEdgesRendererOptions

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_IEDGES_RENDERER_OPTIONS_H
