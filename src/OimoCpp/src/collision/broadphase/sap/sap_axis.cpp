#include <oimo/collision/broadphase/sap/sap_axis.h>

#include <cmath>

#include <oimo/collision/broadphase/sap/sap_element.h>

namespace OIMO {

SAPAxis::SAPAxis() : numElements{0}, bufferSize{256}
{
  elements.resize(bufferSize);
}

SAPAxis::~SAPAxis()
{
}

void SAPAxis::addElements(SAPElement* min, SAPElement* max)
{
  if (numElements + 2 >= bufferSize) {
    bufferSize *= 2;
    elements.resize(bufferSize);
  }
  elements[numElements++] = min;
  elements[numElements++] = max;
}

void SAPAxis::removeElements(SAPElement* min, SAPElement* max)
{
  int minIndex = -1;
  int maxIndex = -1;
  for (unsigned int i = 0; i < numElements; ++i) {
    SAPElement* e = elements[i];
    if (e == min || e == max) {
      if (minIndex == -1) {
        minIndex = static_cast<int>(i);
      }
      else {
        maxIndex = static_cast<int>(i);
        break;
      }
    }
  }
  unsigned int _maxIndex
    = (maxIndex < 0) ? 0 : static_cast<unsigned int>(maxIndex);
  for (unsigned int i = _maxIndex + 1; i < _maxIndex; ++i) {
    elements[i - 1] = elements[i];
  }
  for (unsigned int i = static_cast<unsigned int>(maxIndex + 1);
       i < numElements; ++i) {
    elements[i - 2] = elements[i];
  }
  numElements -= 2;
}

void SAPAxis::sort()
{
  unsigned int count     = 0;
  unsigned int threshold = 1;
  while ((numElements >> threshold) != 0) {
    ++threshold;
  }
  threshold = threshold * numElements >> 2;
  count     = 0;

  bool giveup     = false;
  SAPElement *tmp = nullptr, *tmp2 = nullptr;
  float pivot = 0.f;
  for (unsigned int i = 1; i < numElements; ++i) { // try insertion sort
    tmp              = elements[i];
    pivot            = tmp->value;
    SAPElement* _tmp2 = elements[i - 1];
    if (_tmp2->value > pivot) {
      unsigned int j = i;
      do {
        elements[j] = _tmp2;
        if (--j == 0) {
          break;
        }
        _tmp2 = elements[j - 1];
      } while (_tmp2->value > pivot);
      elements[j] = tmp;
      count += i - j;
      if (count > threshold) {
        giveup = true; // stop and use quick sort
        break;
      }
    }
  }
  if (!giveup) {
    return;
  }
  count    = 2;
  stack[0] = 0;
  stack[1] = numElements - 1;

  SAPElement *ei = nullptr, *ej = nullptr;
  unsigned int i = 0, j = 0;
  while (count > 0) {
    unsigned int right = stack[--count];
    unsigned int left  = stack[--count];
    unsigned int diff  = right - left;
    if (diff > 16) { // quick sort
      // var mid=left+(diff>>1);
      unsigned int mid = left + static_cast<unsigned int>(
                                  std::floor(static_cast<float>(diff) * 0.5f));
      tmp             = elements[mid];
      elements[mid]   = elements[right];
      elements[right] = tmp;
      pivot           = tmp->value;
      i               = left - 1;
      j               = right;
      while (true) {
        do {
          ei = elements[++i];
        } while (ei->value < pivot);
        do {
          ej = elements[--j];
        } while (pivot < ej->value && j != left);
        if (i >= j)
          break;
        elements[i] = ej;
        elements[j] = ei;
      }

      elements[right] = elements[i];
      elements[i]     = tmp;
      if (i - left > right - i) {
        stack[count++] = left;
        stack[count++] = i - 1;
        stack[count++] = i + 1;
        stack[count++] = right;
      }
      else {
        stack[count++] = i + 1;
        stack[count++] = right;
        stack[count++] = left;
        stack[count++] = i - 1;
      }
    }
    else {
      for (i = left + 1; i <= right; i++) {
        tmp   = elements[i];
        pivot = tmp->value;
        tmp2  = elements[i - 1];
        if (tmp2->value > pivot) {
          j = i;
          do {
            elements[j] = tmp2;
            if (--j == 0)
              break;
            tmp2 = elements[j - 1];
          } while (tmp2->value > pivot);
          elements[j] = tmp;
        }
      }
    }
  }
}

int SAPAxis::calculateTestCount() const
{
  int num = 1;
  int sum = 0;
  for (unsigned int i = 1; i < numElements; ++i) {
    if (elements[i]->max) {
      --num;
    }
    else {
      sum += num;
      ++num;
    }
  }
  return sum;
}

} // end of namespace OIMO
