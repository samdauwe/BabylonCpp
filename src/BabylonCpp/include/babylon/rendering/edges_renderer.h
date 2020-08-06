#ifndef BABYLON_RENDERING_EDGES_RENDERER_H
#define BABYLON_RENDERING_EDGES_RENDERER_H

#include <unordered_map>

#include <babylon/babylon_api.h>
#include <babylon/maths/vector3.h>
#include <babylon/misc/observer.h>
#include <babylon/rendering/iedges_renderer.h>
#include <babylon/rendering/iedges_renderer_options.h>

namespace BABYLON {

class AbstractMesh;
class Node;
class Scene;
class ShaderMaterial;
class VertexBuffer;
class WebGLDataBuffer;
using AbstractMeshPtr    = std::shared_ptr<AbstractMesh>;
using ShaderMaterialPtr  = std::shared_ptr<ShaderMaterial>;
using VertexBufferPtr    = std::shared_ptr<VertexBuffer>;
using WebGLDataBufferPtr = std::shared_ptr<WebGLDataBuffer>;

/**
 * @brief This class is used to generate edges of the mesh that could then easily be rendered in a
 * scene.
 */
class BABYLON_SHARED_EXPORT EdgesRenderer : public IEdgesRenderer {

public:
  /**
   * @brief Creates an instance of the EdgesRenderer. It is primarily use to display edges of a
   * mesh. Beware when you use this class with complex objects as the adjacencies computation can be
   * really long.
   * @param  source Mesh used to create edges
   * @param  epsilon sum of angles in adjacency to check for edge
   * @param  checkVerticesInsteadOfIndices bases the edges detection on vertices vs indices. Note
   * that this parameter is not used if options.useAlternateEdgeFinder = true
   * @param  generateEdgesLines - should generate Lines or only prepare resources.
   * @param  options The options to apply when generating the edges
   */
  EdgesRenderer(const AbstractMeshPtr& source, float epsilon = 0.95f,
                bool checkVerticesInsteadOfIndices = false, bool generateEdgesLines = true,
                const std::optional<IEdgesRendererOptions>& options = std::nullopt);
  ~EdgesRenderer() override; // = default

  /**
   * @brief Hidden
   */
  void _rebuild();

  /**
   * @brief Checks wether or not the edges renderer is ready to render.
   * @return true if ready, otherwise false.
   */
  bool isReady() override;

  /**
   * @brief Releases the required resources for the edges renderer.
   */
  void dispose(bool doNotRecurse = false, bool disposeMaterialAndTextures = false) override;

  /**
   * @brief Renders the edges of the attached mesh.
   */
  void render() override;

protected:
  void _prepareResources();
  int _processEdgeForAdjacencies(unsigned int pa, unsigned int pb, unsigned int p0, unsigned int p1,
                                 unsigned int p2);
  int _processEdgeForAdjacenciesWithVertices(const Vector3& pa, const Vector3& pb,
                                             const Vector3& p0, const Vector3& p1,
                                             const Vector3& p2);

  /**
   * @brief Checks if the pair of p0 and p1 is en edge.
   * @param faceIndex
   * @param edge
   * @param faceNormals
   * @param  p0
   * @param  p1
   */
  void _checkEdge(size_t faceIndex, int edge, const std::vector<Vector3>& faceNormals,
                  const Vector3& p0, const Vector3& p1);

  /**
   * @brief Push line into the position, normal and index buffer.
   */
  void createLine(const Vector3& p0, const Vector3& p1, uint32_t offset);

  /**
   * @brief Generates lines edges from adjacencjes.
   */
  void _generateEdgesLines();

private:
  static ShaderMaterialPtr GetShader(Scene* scene);

  /**
   * See https://playground.babylonjs.com/#R3JR6V#1 for a visual display of the algorithm
   */
  void _tessellateTriangle(const std::vector<std::vector<std::array<float, 2>>>& edgePoints,
                           size_t indexTriangle, IndicesArray& indices,
                           const IndicesArray& remapVertexIndices);
  void _generateEdgesLinesAlternate();

public:
  /**
   * Define the size of the edges with an orthographic camera
   */
  float edgesWidthScalerForOrthographic;

  /**
   * Define the size of the edges with a perspective camera
   */
  float edgesWidthScalerForPerspective;

protected:
  AbstractMeshPtr _source;
  Float32Array _linesPositions;
  Float32Array _linesNormals;
  Uint32Array _linesIndices;
  float _epsilon;
  size_t _indicesCount;
  ShaderMaterialPtr _lineShader;
  WebGLDataBufferPtr _ib;
  std::unordered_map<std::string, VertexBufferPtr> _buffers;
  std::unordered_map<std::string, VertexBufferPtr> _buffersForInstances;
  bool _checkVerticesInsteadOfIndices;
  std::optional<IEdgesRendererOptions> _options;

private:
  Observer<AbstractMesh>::Ptr _meshRebuildObserver;
  Observer<Node>::Ptr _meshDisposeObserver;

}; // end of class EdgesRenderer

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_EDGES_RENDERER_H
