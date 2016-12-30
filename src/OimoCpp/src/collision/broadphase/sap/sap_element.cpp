#include <oimo/collision/broadphase/sap/sap_element.h>

#include <oimo/collision/broadphase/sap/sap_proxy.h>

namespace OIMO {

SAPElement::SAPElement(SAPProxy* _proxy, bool _max)
    : proxy{_proxy}
    , pair{nullptr}
    , min1{nullptr}
    , max1{nullptr}
    , min2{nullptr}
    , max2{nullptr}
    , max{_max}
    , value{0.f}
{
}

SAPElement::~SAPElement()
{
}

} // end of namespace OIMO
