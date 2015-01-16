
#ifndef _CG_COLOR_H_
#define _CG_COLOR_H_

namespace cg
{

#if defined(__WIN32__)
#pragma pack(push, 1)
#endif

struct CGColor
{
    CGColor()
    {}

    CGColor(u8 _r, u8 _g, u8 _b)
        : r(_r), g(_g), b(_b)
    {}

    u8 b;
    u8 g;
    u8 r;
};

#if defined(__WIN32__)
#pragma pack(pop)
#endif

}

#endif //_CG_COLOR_H_