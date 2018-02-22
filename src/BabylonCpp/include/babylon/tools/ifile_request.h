#ifndef BABYLON_TOOLS_IFILE_REQUEST_H
#define BABYLON_TOOLS_IFILE_REQUEST_H

#include <babylon/babylon_global.h>
#include <babylon/tools/observable.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT IFileRequest {
  /**
   * @brief Raised when the request is complete (success or error).
   */
  Observable<IFileRequest> onCompleteObservable;

  /**
   * @brief Aborts the request for a file.
   */
  ::std::function<void()> abort = nullptr;

}; // end of struct IFileRequest

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_IFILE_REQUEST_H
