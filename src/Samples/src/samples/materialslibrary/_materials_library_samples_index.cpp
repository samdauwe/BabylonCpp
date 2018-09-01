#include <babylon/samples/materialslibrary/_materials_library_samples_index.h>

#include <babylon/samples/materialslibrary/cell_material_scene.h>

namespace BABYLON {
namespace Samples {

_MaterialsLibrarySamplesIndex::_MaterialsLibrarySamplesIndex()
{
  // -- Materials Library examples --

  // Cell Material Scene
  _samples["CellMaterialScene"] = ::std::make_tuple(
    true,                                                    //
    [](ICanvas* iCanvas) {                                   //
      return ::std::make_unique<CellMaterialScene>(iCanvas); //
    });                                                      //
}

_MaterialsLibrarySamplesIndex::~_MaterialsLibrarySamplesIndex()
{
}

} // end of namespace Samples
} // end of namespace BABYLON
