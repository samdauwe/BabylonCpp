#ifndef BABYLON_MESH_SIMPLIFICATION_QUADRATIC_ERROR_SIMPLIFICATION_H
#define BABYLON_MESH_SIMPLIFICATION_QUADRATIC_ERROR_SIMPLIFICATION_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief An implementation of the Quadratic Error simplification algorithm.
 * Original paper : http://www1.cs.columbia.edu/~cs4162/html05s/garland97.pdf
 * Ported mostly from QSlim and
 * http://voxels.blogspot.de/2014/05/quadric-mesh-simplification-with-source.html
 * to babylon JS
 * @author RaananW
 */
class BABYLON_SHARED_EXPORT QuadraticErrorSimplification : public ISimplifier {

public:
  QuadraticErrorSimplification();
  ~QuadraticErrorSimplification();

}; // end of class QuadraticErrorSimplification

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_SIMPLIFICATION_QUADRATIC_ERROR_SIMPLIFICATION_H