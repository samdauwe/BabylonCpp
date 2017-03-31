#ifndef BABYLON_MESH_VERTEX_BUFFER_H
#define BABYLON_MESH_VERTEX_BUFFER_H

#include <babylon/babylon_global.h>
#include <babylon/interfaces/idisposable.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT VertexBuffer : public IDisposable {

public:
  static constexpr unsigned int PositionKind             = 1;
  static constexpr unsigned int NormalKind               = 2;
  static constexpr unsigned int TangentKind              = 3;
  static constexpr unsigned int UVKind                   = 4;
  static constexpr unsigned int UV2Kind                  = 5;
  static constexpr unsigned int UV3Kind                  = 6;
  static constexpr unsigned int UV4Kind                  = 7;
  static constexpr unsigned int UV5Kind                  = 8;
  static constexpr unsigned int UV6Kind                  = 9;
  static constexpr unsigned int ColorKind                = 10;
  static constexpr unsigned int MatricesIndicesKind      = 11;
  static constexpr unsigned int MatricesWeightsKind      = 12;
  static constexpr unsigned int MatricesIndicesExtraKind = 13;
  static constexpr unsigned int MatricesWeightsExtraKind = 14;
  static constexpr unsigned int World0Kind               = 15;
  static constexpr unsigned int World1Kind               = 16;
  static constexpr unsigned int World2Kind               = 17;
  static constexpr unsigned int World3Kind               = 18;
  static constexpr unsigned int CellInfoKind             = 19;
  static constexpr unsigned int OptionsKind              = 20;

  static constexpr const char* PositionKindChars        = "position";
  static constexpr const char* NormalKindChars          = "normal";
  static constexpr const char* TangentKindChars         = "tangent";
  static constexpr const char* UVKindChars              = "uv";
  static constexpr const char* UV2KindChars             = "uv2";
  static constexpr const char* UV3KindChars             = "uv3";
  static constexpr const char* UV4KindChars             = "uv4";
  static constexpr const char* UV5KindChars             = "uv5";
  static constexpr const char* UV6KindChars             = "uv6";
  static constexpr const char* ColorKindChars           = "color";
  static constexpr const char* MatricesIndicesKindChars = "matricesIndices";
  static constexpr const char* MatricesWeightsKindChars = "matricesWeights";
  static constexpr const char* MatricesIndicesExtraKindChars
    = "matricesIndicesExtra";
  static constexpr const char* MatricesWeightsExtraKindChars
    = "matricesWeightsExtra";
  static constexpr const char* World0KindChars   = "world0";
  static constexpr const char* World1KindChars   = "world1";
  static constexpr const char* World2KindChars   = "world2";
  static constexpr const char* World3KindChars   = "world3";
  static constexpr const char* CellInfoKindChars = "cellInfo";
  static constexpr const char* OptionsKindChars  = "options";

public:
  VertexBuffer(Engine* engine, const Float32Array& data, unsigned int kind,
               bool updatable, bool postponeInternalCreation = false,
               int stride = -1, bool instanced = false, int offset = -1,
               int size = -1);
  VertexBuffer(Engine* engine, Buffer* buffer, unsigned int kind,
               bool updatable, bool postponeInternalCreation = false,
               int stride = -1, bool instanced = false, int offset = -1,
               int size = -1);
  virtual ~VertexBuffer();

  /** Statics **/
  static std::string KindAsString(unsigned int kind);
  static int KindToStride(unsigned int kind);

  /** Properties **/
  unsigned int getKind() const;
  bool isUpdatable() const;
  Float32Array& getData();
  GL::IGLBuffer* getBuffer();
  int getStrideSize() const;
  unsigned int getOffset() const;
  int getSize() const;
  bool getIsInstanced() const;

  /** Methods **/
  void create();
  void create(const Float32Array& data);
  void update(const Float32Array& data);
  void updateDirectly(const Float32Array& data, int offset);
  void dispose(bool doNotRecurse = false) override;

private:
  Buffer* _getBuffer() const;

private:
  std::unique_ptr<Buffer> _ownedBuffer;
  Buffer* _buffer;
  unsigned int _kind;
  unsigned int _offset;
  int _size;
  int _stride;
  bool _ownsBuffer;

}; // end of class VertexBuffer

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_VERTEX_BUFFER_H
