
#ifndef _CG_ENUM_REFERENCE_ID_TYPES_H_
#define _CG_ENUM_REFERENCE_ID_TYPES_H_

namespace cg
{

#define ECRID_DIV_NUMBER 100000
#define ECRID_INVALID    0

enum E_CG_REFID_TYPES
{
    ECRID_GRAPH=0,      // original
    ECRID_ANIMATION,    // animation palette
    ECRID_GRAPH_EX,     // power-up-kit 1
    ECRID_GRAPH_SE,     // square-enix defined
};

static inline E_CG_REFID_TYPES refIdType(u32 refId)
{
    return E_CG_REFID_TYPES(refId / ECRID_DIV_NUMBER);
}

}

#endif //_CG_ENUM_REFERENCE_ID_TYPES_H_