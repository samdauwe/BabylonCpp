#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MATERIALS_TEXTURE_RESERVED_DATA_STORE_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MATERIALS_TEXTURE_RESERVED_DATA_STORE_H

#include <memory>
#include <vector>

#include <babylon/babylon_api.h>

namespace BABYLON {

class SkeletonViewer;
using SkeletonViewerPtr = std::shared_ptr<SkeletonViewer>;

struct BABYLON_SHARED_EXPORT TextureReservedDataStore {
  int width         = 256;
  int height        = 256;
  bool displayRed   = true;
  bool displayGreen = true;
  bool displayBlue  = true;
  bool displayAlpha = true;
  unsigned int face = 0;
}; // end of struct SkeletonReservedDataStore

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MATERIALS_TEXTURE_RESERVED_DATA_STORE_H
