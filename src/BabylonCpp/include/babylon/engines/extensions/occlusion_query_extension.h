#ifndef BABYLON_ENGINES_EXTENSIONS_OCCLUSION_QUERY_EXTENSION_H
#define BABYLON_ENGINES_EXTENSIONS_OCCLUSION_QUERY_EXTENSION_H

#include <memory>
#include <optional>

#include <babylon/babylon_api.h>
#include <babylon/instrumentation/_time_token.h>
#include <babylon/meshes/abstract_mesh_constants.h>

namespace BABYLON {

class Engine;
class InternalTexture;
using WebGLQuery         = GL::IGLQuery;
using InternalTexturePtr = std::shared_ptr<InternalTexture>;
using WebGLQueryPtr      = std::shared_ptr<WebGLQuery>;

/**
 * @brief Hidden
 */
struct BABYLON_SHARED_EXPORT _OcclusionDataStorage {

  /** Hidden */
  int occlusionInternalRetryCounter = 0;

  /** Hidden */
  bool isOcclusionQueryInProgress = false;

  /** Hidden */
  bool isOccluded = false;

  /** Hidden */
  int occlusionRetryCount = -1;

  /** Hidden */
  unsigned int occlusionType = AbstractMeshConstants::OCCLUSION_TYPE_NONE;

  /** Hidden */
  unsigned int occlusionQueryAlgorithmType
    = AbstractMeshConstants::OCCLUSION_ALGORITHM_TYPE_CONSERVATIVE;

}; // end of struct _OcclusionDataStorage

/**
 * @brief Hidden
 */
class BABYLON_SHARED_EXPORT OcclusionQueryExtension {

public:
  OcclusionQueryExtension(Engine* engine);
  ~OcclusionQueryExtension();

  /**
   * @brief Create a new webGL query (you must be sure that queries are supported by checking
   * getCaps() function).
   * @return the new query
   */
  WebGLQueryPtr createQuery();

  /**
   * @brief Delete and release a webGL query.
   * @param query defines the query to delete
   * @return the current engine
   */
  Engine& deleteQuery(const WebGLQueryPtr& query);

  /**
   * @brief Check if a given query has resolved and got its value.
   * @param query defines the query to check
   * @returns true if the query got its value
   */
  bool isQueryResultAvailable(const WebGLQueryPtr& query);

  /**
   * @brief Gets the value of a given query.
   * @param query defines the query to check
   * @returns the value of the query
   */
  unsigned int getQueryResult(const WebGLQueryPtr& query);

  /**
   * @brief Initiates an occlusion query.
   * @param algorithmType defines the algorithm to use
   * @param query defines the query to use
   * @returns the current engine
   * @see http://doc.babylonjs.com/features/occlusionquery
   */
  Engine& beginOcclusionQuery(unsigned int algorithmType, const WebGLQueryPtr& query);

  /**
   * @brief Ends an occlusion query.
   * @see http://doc.babylonjs.com/features/occlusionquery
   * @param algorithmType defines the algorithm to use
   * @returns the current engine
   */
  Engine& endOcclusionQuery(unsigned int algorithmType);

  /**
   * @brief Starts a time query (used to measure time spent by the GPU on a specific frame)
   * Please note that only one query can be issued at a time
   * @returns a time token used to track the time span
   */
  std::optional<_TimeToken> startTimeQuery();

  /**
   * @brief Ends a time query.
   * @param token defines the token used to measure the time span
   * @returns the time spent (in ns)
   */
  int endTimeQuery(std::optional<_TimeToken>& token);

  /**
   * @brief Hidden
   */
  WebGLQueryPtr _createTimeQuery();

  /**
   * @brief Hidden
   */
  void _deleteTimeQuery(const WebGLQueryPtr& query);

  /**
   * @brief Hidden
   */
  unsigned int _getGlAlgorithmType(unsigned int algorithmType) const;

  /**
   * @brief Hidden
   */
  unsigned int _getTimeQueryResult(const WebGLQueryPtr& query);

  /**
   * @brief Hidden
   */
  bool _getTimeQueryAvailability(const WebGLQueryPtr& query);

public:
  /** @hidden */
  std::optional<_TimeToken> _currentNonTimestampToken = std::nullopt;

private:
  Engine* _this;

}; // end of class OcclusionQueryExtension

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_EXTENSIONS_OCCLUSION_QUERY_EXTENSION_H
