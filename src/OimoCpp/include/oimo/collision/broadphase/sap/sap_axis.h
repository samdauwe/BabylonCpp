#ifndef OIMO_COLLISION_BROADPHASE_SAP_SAP_AXIS_H
#define OIMO_COLLISION_BROADPHASE_SAP_SAP_AXIS_H

#include <array>
#include <vector>

namespace OIMO {

struct SAPElement;

/**
 * @brief A projection axis for sweep and prune broad-phase.
 */
class SAPAxis {

public:
  SAPAxis();
  ~SAPAxis();

  void addElements(SAPElement* min, SAPElement* max);
  void removeElements(SAPElement* min, SAPElement* max);
  void sort();
  int calculateTestCount() const;

public:
  unsigned int numElements;
  unsigned int bufferSize;
  std::vector<SAPElement*> elements;
  std::array<unsigned int, 64> stack;

}; // end of class SAPAxis

} // end of namespace OIMO

#endif // end of OIMO_COLLISION_BROADPHASE_SAP_SAP_AXIS_H
