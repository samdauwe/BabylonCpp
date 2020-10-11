#ifndef BABYLON_FWD_H
#define BABYLON_FWD_H

namespace BABYLON {

#define FWD_STRUCT_SPTR(className)                                                                 \
  struct className;                                                                                \
  using className##Ptr = std::shared_ptr<className>;

#define FWD_CLASS_SPTR(className)                                                                  \
  class className;                                                                                 \
  using className##Ptr = std::shared_ptr<className>;

} // end of namespace BABYLON

#endif // end of BABYLON_FWD_H
