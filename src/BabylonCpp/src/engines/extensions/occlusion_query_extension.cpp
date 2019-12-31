#include <babylon/engines/extensions/occlusion_query_extension.h>

#include <babylon/engines/engine.h>

namespace BABYLON {

OcclusionQueryExtension::OcclusionQueryExtension(Engine* engine) : _this{engine}
{
}

OcclusionQueryExtension::~OcclusionQueryExtension() = default;

WebGLQueryPtr createQuery()
{
  return _this->_gl->createQuery();
}

Engine& OcclusionQueryExtension::deleteQuery(const WebGLQueryPtr& query)
{
  _this->_gl->deleteQuery(query.get());

  return *_this;
}

bool OcclusionQueryExtension::isQueryResultAvailable(const WebGLQueryPtr& query)
{
  return _this->_gl->getQueryParameterb(query.get(), GL::QUERY_RESULT_AVAILABLE);
}

unsigned int OcclusionQueryExtension::getQueryResult(const WebGLQueryPtr& query)
{
  return _this->_gl->getQueryParameteri(query.get(), GL::QUERY_RESULT);
}

Engine& OcclusionQueryExtension::beginOcclusionQuery(unsigned int algorithmType,
                                                     const WebGLQueryPtr& query)
{
  const auto glAlgorithm = _this->_getGlAlgorithmType(algorithmType);
  _this->_gl->beginQuery(glAlgorithm, query.get());

  return *_this;
}

Engine& OcclusionQueryExtension::endOcclusionQuery(unsigned int algorithmType)
{
  const auto glAlgorithm = _this->_getGlAlgorithmType(algorithmType);
  _this->_gl->endQuery(glAlgorithm);

  return *_this;
}

std::optional<_TimeToken> OcclusionQueryExtension::startTimeQuery()
{
  return std::nullopt;
}

int OcclusionQueryExtension::endTimeQuery(std::optional<_TimeToken>& /*token*/)
{
  return -1;
}

WebGLQueryPtr OcclusionQueryExtension::_createTimeQuery()
{
  return _this->createQuery();
}

void OcclusionQueryExtension::_deleteTimeQuery(const WebGLQueryPtr& query)
{
  _this->deleteQuery(query);
}

unsigned int OcclusionQueryExtension::_getGlAlgorithmType(unsigned int algorithmType) const
{
  return algorithmType == AbstractMeshConstants::OCCLUSION_ALGORITHM_TYPE_CONSERVATIVE ?
           GL::ANY_SAMPLES_PASSED_CONSERVATIVE :
           GL::ANY_SAMPLES_PASSED;
}

unsigned int OcclusionQueryExtension::_getTimeQueryResult(const WebGLQueryPtr& query)
{
  return _this->getQueryResult(query);
}

bool OcclusionQueryExtension::_getTimeQueryAvailability(const WebGLQueryPtr& query)
{
  return _this->isQueryResultAvailable(query);
}

} // end of namespace BABYLON
