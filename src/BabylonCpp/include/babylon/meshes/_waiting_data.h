#ifndef BABYLON_MESHES_WAITING_DATA_H
#define BABYLON_MESHES_WAITING_DATA_H

#include <string>

#include <nlohmann/json_fwd.hpp>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

using json = nlohmann::json;

namespace BABYLON {

struct LODItem {
  std::vector<std::string> ids{};
  Float32Array distances{};
  Float32Array coverages{};
}; // end of struct Lod

/**
 * @brief Hidden
 */
struct BABYLON_SHARED_EXPORT _WaitingData {
  std::optional<LODItem> lods{std::nullopt};
  std::vector<json> actions{};
  std::optional<bool> freezeWorldMatrix{std::nullopt};
}; // end of struct _WaitingData

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_WAITING_DATA_H
