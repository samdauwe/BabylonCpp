#ifndef BABYLON_RENDERING_LINE_EDGES_RENDERER_H
#define BABYLON_RENDERING_LINE_EDGES_RENDERER_H

#include <babylon/babylon_global.h>
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
  ~LineEdgesRenderer() override;

protected:
  /**
   * @brief Always create the edge since its a line so only important things are
   * p0 and p1.
   * @param  faceIndex not important for LineMesh
   * @param  edge not important for LineMesh
   * @param  faceNormals not important for LineMesh
   * @param  p0 beginnig of line
   * @param  p1 end of line
   */
  void _checkEdge(size_t faceIndex, int edge,
                  const vector_t<Vector3>& faceNormals, const Vector3& p0,
                  const Vector3& p1) override;

private:
  /**
   * @brief Generate edges for each line in LinesMesh. Every Line should be
   * rendered as edge.
   */
  void _generateEdgesLines() override;

}; // end of class LineEdgesRenderer

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_LINE_EDGES_RENDERER_H
