#ifndef BABYLON_RENDERING_EDGES_RENDERER_H
#define BABYLON_RENDERING_EDGES_RENDERER_H

#include <unordered_map>

#include <babylon/babylon_api.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class AbstractMesh;
class ShaderMaterial;
class VertexBuffer;
using AbstractMeshPtr   = std::shared_ptr<AbstractMesh>;
using ShaderMaterialPtr = std::shared_ptr<ShaderMaterial>;

namespace GL {
class IGLBuffer;
} // end of namespace GL

/**
 * @brief This class is used to generate edges of the mesh that could then
 * easily be rendered in a scene.
 */
class BABYLON_SHARED_EXPORT EdgesRenderer {

public:
  /**
   * @brief Creates an instance of the EdgesRenderer. It is primarily use to
   * display edges of a mesh. Beware when you use this class with complex
   * objects as the adjacencies computation can be really long.
   * @param  source Mesh used to create edges
   * @param  epsilon sum of angles in adjacency to check for edge
   * @param  checkVerticesInsteadOfIndices
   * @param  generateEdgesLines - should generate Lines or only prepare
   * resources.
   */
  EdgesRenderer(const AbstractMeshPtr& source, float epsilon = 0.95f,
                bool checkVerticesInsteadOfIndices = false,
                bool generateEdgesLines            = true);
  virtual ~EdgesRenderer();

  /**
   * @brief Hidden
   */
  void _rebuild();

  /**
   * @brief Releases the required resources for the edges renderer.
   */
  void dispose();

  void render();

protected:
  void _prepareResources();
  int _processEdgeForAdjacencies(unsigned int pa, unsigned int pb,
                                 unsigned int p0, unsigned int p1,
                                 unsigned int p2);
  int _processEdgeForAdjacenciesWithVertices(const Vector3& pa,
                                             const Vector3& pb,
                                             const Vector3& p0,
                                             const Vector3& p1,
                                             const Vector3& p2);

  /**
   * @brief Checks if the pair of p0 and p1 is en edge.
   * @param faceIndex
   * @param edge
   * @param faceNormals
   * @param  p0
   * @param  p1
   */
  virtual void _checkEdge(size_t faceIndex, int edge,
                          const std::vector<Vector3>& faceNormals,
                          const Vector3& p0, const Vector3& p1);

  /**
   * @brief Generates lines edges from adjacencjes.
   */
  virtual void _generateEdgesLines();

public:
  float edgesWidthScalerForOrthographic;
  float edgesWidthScalerForPerspective;

  /**
   * Gets or sets a boolean indicating if the edgesRenderer is active
   */
  bool isEnabled;

protected:
  AbstractMeshPtr _source;
  Float32Array _linesPositions;
  Float32Array _linesNormals;
  Uint32Array _linesIndices;
  float _epsilon;
  size_t _indicesCount;
  ShaderMaterialPtr _lineShader;
  std::unique_ptr<GL::IGLBuffer> _ib;
  std::unordered_map<unsigned int, std::unique_ptr<VertexBuffer>> _buffers;
  std::unordered_map<std::string, VertexBuffer*> _bufferPtrs;
  bool _checkVerticesInsteadOfIndices;

}; // end of class EdgesRenderer

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_EDGES_RENDERER_H
