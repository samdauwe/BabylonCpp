#include <oimo/constraint/constraint.h>

namespace OIMO {

Constraint::Constraint()
    : parent{nullptr}, body1{nullptr}, body2{nullptr}, addedToIsland{false}
{
}

Constraint::~Constraint()
{
}

void Constraint::preSolve(float /*timeStep*/, float /*invTimeStep*/)
{
}

void Constraint::solve()
{
}

void Constraint::postSolve()
{
}

} // end of namespace OIMO
