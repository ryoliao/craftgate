
#include "CGGraph_PCH.h"

namespace cg
{

static CGColor LowSystemColors[] =
{
    CGColor(0x00, 0x00, 0x00),
    CGColor(0x00, 0x00, 0x80),
    CGColor(0x00, 0x80, 0x00),
    CGColor(0x00, 0x80, 0x80),
    CGColor(0x80, 0x00, 0x00),
    CGColor(0x80, 0x00, 0x80),
    CGColor(0x80, 0x80, 0x00),
    CGColor(0xc0, 0xc0, 0xc0),
    CGColor(0xc0, 0xdc, 0xc0),
    CGColor(0xf0, 0xca, 0xa6),
    CGColor(0x00, 0x00, 0xde),
    CGColor(0x00, 0x5f, 0xff),
    CGColor(0xa0, 0xff, 0xff),
    CGColor(0xd2, 0x5f, 0x00),
    CGColor(0xff, 0xd2, 0x50),
    CGColor(0x28, 0xe1, 0x28),
};

static CGColor HighSystemColors[] =
{
    CGColor(0x96, 0xc3, 0xf5),
    CGColor(0x5f, 0xa0, 0x1e),
    CGColor(0x46, 0x7d, 0xc3),
    CGColor(0x1e, 0x55, 0x9b),
    CGColor(0x37, 0x41, 0x46),
    CGColor(0x1e, 0x23, 0x28),
    CGColor(0xf0, 0xfb, 0xff),
    CGColor(0xa5, 0x6e, 0x3a),
    CGColor(0x80, 0x80, 0x80),
    CGColor(0x00, 0x00, 0xff),
    CGColor(0x00, 0xff, 0x00),
    CGColor(0x00, 0xff, 0xff),
    CGColor(0xff, 0x00, 0x00),
    CGColor(0xff, 0x80, 0xff),
    CGColor(0xff, 0xff, 0x00),
    CGColor(0xff, 0xff, 0xff),
};

CGPalette::CGPalette()
{

}

CGPalette::CGPalette(CGPalette const & other)
{
    cloneFrom(other);
}

CGPalette::~CGPalette()
{

}

CGPalette & CGPalette::operator=(CGPalette const & other)
{
    cloneFrom(other);
    return *this;
}

CGColor CGPalette::getColor(u8 i) const
{
    return colors[i];
}

void CGPalette::setColor(u8 i, CGColor clr)
{
    colors[i] = clr;
}

void CGPalette::cloneFrom(CGPalette const & other)
{
    memcpy(colors, other.colors, sizeof(colors));
}

void CGPalette::readFromCGP(IStream* sPal)
{
    // assign system colors.
    memcpy(&colors[0x00], LowSystemColors, sizeof(LowSystemColors));
    memcpy(&colors[0xf0], HighSystemColors, sizeof(HighSystemColors));

    // PAL file only have 224 custom colors, 0x00~0x0f and 0xf0~0xff use system colors
    sPal->read(&colors[0x10], 224);
}

void CGPalette::readFromGraphV2(IStream* sPal, u32 szPalette)
{
    szPalette = std::min(szPalette, (u32)sizeof(colors));
    sPal->read(colors, szPalette, 1);
}

void CGPalette::readFromMemory( void* p, u32 szPalette )
{
    szPalette = std::min(szPalette, (u32)sizeof(colors));
    memcpy(colors, p, szPalette);
}

u32 CGPalette::getARGB(u8 i) const
{
    if (0 == i)
        return 0;

    // 2, 1, 0, 3
    // r, g, b, a
    CGColor c = colors[i];
    return (0xff << 24)|(c.r << 16)|(c.g << 8)|(c.b);
}

u32 CGPalette::getGL_RGBA(u8 i) const
{
    if (0 == i)
        return 0;

    // 0, 1, 2, 3
    // r, g, b, a
    CGColor c = colors[i];
    return (0xff << 24)|(c.b << 16)|(c.g << 8)|(c.r);
}

}