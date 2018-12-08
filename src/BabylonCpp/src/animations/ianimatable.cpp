#include <babylon/animations/ianimatable.h>

namespace BABYLON {

IAnimatable::IAnimatable()
    : parent{this, &IAnimatable::get_parent, &IAnimatable::set_parent}
    , nullNode{nullptr}
    , _identityMatrix{Matrix::Identity()}
{
}

IAnimatable::~IAnimatable()
{
}

Node*& IAnimatable::get_parent()
{
  return nullNode;
}

void IAnimatable::set_parent(Node* const& /*parent*/)
{
}

} // end of namespace BABYLON
