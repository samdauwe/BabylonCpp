#include <babylon/materials/material_stencil_state.h>

#include <babylon/core/json_util.h>
#include <babylon/engines/constants.h>

namespace BABYLON {

MaterialStencilState::MaterialStencilState()
{
}

MaterialStencilState::~MaterialStencilState() = default;

void MaterialStencilState::reset()
{
  enabled = false;
  mask    = 0xFF;

  func     = Constants::ALWAYS;
  funcRef  = 1;
  funcMask = 0xFF;

  opStencilFail      = Constants::KEEP;
  opDepthFail        = Constants::KEEP;
  opStencilDepthPass = Constants::REPLACE;
}

unsigned int MaterialStencilState::get_func() const
{
  return _func;
}

void MaterialStencilState::set_func(unsigned int value)
{
  _func = value;
}

unsigned int MaterialStencilState::get_funcRef() const
{
  return _funcRef;
}

void MaterialStencilState::set_funcRef(unsigned int value)
{
  _funcRef = value;
}

unsigned int MaterialStencilState::get_funcMask() const
{
  return _funcMask;
}

void MaterialStencilState::set_funcMask(unsigned int value)
{
  _funcMask = value;
}

unsigned int MaterialStencilState::get_opStencilFail() const
{
  return _opStencilFail;
}

void MaterialStencilState::set_opStencilFail(unsigned int value)
{
  _opStencilFail = value;
}

unsigned int MaterialStencilState::get_opDepthFail() const
{
  return _opDepthFail;
}

void MaterialStencilState::set_opDepthFail(unsigned int value)
{
  _opDepthFail = value;
}

unsigned int MaterialStencilState::get_opStencilDepthPass() const
{
  return _opStencilDepthPass;
}

void MaterialStencilState::set_opStencilDepthPass(unsigned int value)
{
  _opStencilDepthPass = value;
}

unsigned int MaterialStencilState::get_mask() const
{
  return _mask;
}

void MaterialStencilState::set_mask(unsigned int value)
{
  _mask = value;
}

bool MaterialStencilState::get_enabled() const
{
  return _enabled;
}

void MaterialStencilState::set_enabled(bool value)
{
  _enabled = value;
}

std::string MaterialStencilState::getClassName() const
{
  return "MaterialStencilState";
}

void MaterialStencilState::copyTo(MaterialStencilState& /*stencilState*/)
{
}

json MaterialStencilState::serialize() const
{
  return nullptr;
}

MaterialStencilStatePtr MaterialStencilState::Parse(const json& /*source*/, Scene* /*scene*/,
                                                    const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace BABYLON
