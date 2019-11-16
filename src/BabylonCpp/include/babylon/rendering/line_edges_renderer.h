#ifndef BABYLON_RENDERING_LINE_EDGES_RENDERER_H
#define BABYLON_RENDERING_LINE_EDGES_RENDERER_H

#include <babylon/babylon_api.h>
#include <babylon/rendering/edges_renderer.h>

namespace BABYLON {

/**
 * @brief LineEdgesRenderer for LineMeshes to remove unnecessary triangulation.
 */
class BABYLON_SHARED_EXPORT LineEdgesRenderer : public EdgesRenderer {

public:
  /**
   * @brief This constructor turns off auto generating edges line in Edges
   * Renderer to make it here.
   * @param  source LineMesh used to generate edges
   * @param  epsilon not important (specified angle for edge detection)
   * @param  checkVerticesInsteadOfIndices not important for LineMesh
   */
  LineEdgesRenderer(const AbstractMeshPtr& source, float epsilon = 0.95f,
                    bool checkVerticesInsteadOfIndices = false);
  ~LineEdgesRenderer(); // = default

private:
  /**
   * @brief Generate edges for each line in LinesMesh. Every Line should be
   * rendered as edge.
   */
  void _generateEdgesLines_specialized();

}; // end of class LineEdgesRenderer

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_LINE_EDGES_RENDERER_H
