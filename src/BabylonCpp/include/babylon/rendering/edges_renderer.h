#ifndef BABYLON_RENDERING_EDGES_RENDERER_H
#define BABYLON_RENDERING_EDGES_RENDERER_H

#include <babylon/babylon_global.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT EdgesRenderer {

public:
  EdgesRenderer(AbstractMesh* source, float epsilon = 0.95f,
                bool checkVerticesInsteadOfIndices = false);
  virtual ~EdgesRenderer();

  void _rebuild();
  void dispose();
  void render();

private:
  void _prepareResources();
  int _processEdgeForAdjacencies(unsigned int pa, unsigned int pb,
                                 unsigned int p0, unsigned int p1,
                                 unsigned int p2);
  int _processEdgeForAdjacenciesWithVertices(const Vector3& pa,
                                             const Vector3& pb,
                                             const Vector3& p0,
                                             const Vector3& p1,
                                             const Vector3& p2);
  void _checkEdge(unsigned int faceIndex, int edge,
                  vector_t<Vector3> faceNormals, const Vector3& p0,
                  const Vector3& p1);
  void _generateEdgesLines();

public:
  float edgesWidthScalerForOrthographic;
  float edgesWidthScalerForPerspective;

  /**
   * Gets or sets a boolean indicating if the edgesRenderer is active
   */
  bool isEnabled;

private:
  AbstractMesh* _source;
  Float32Array _linesPositions;
  Float32Array _linesNormals;
  Uint32Array _linesIndices;
  float _epsilon;
  size_t _indicesCount;
  ShaderMaterialPtr _lineShader;
  unique_ptr_t<GL::IGLBuffer> _ib;
  unordered_map_t<unsigned int, unique_ptr_t<VertexBuffer>> _buffers;
  unordered_map_t<string_t, VertexBuffer*> _bufferPtrs;
  bool _checkVerticesInsteadOfIndices;

}; // end of class EdgesRenderer

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_EDGES_RENDERER_H
