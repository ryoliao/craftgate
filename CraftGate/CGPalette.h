
#ifndef _CG_PALETTE_H_
#define _CG_PALETTE_H_

namespace cg
{

class CGPalette
{
public:
    CGPalette();
    CGPalette(CGPalette const & other);
    ~CGPalette();

    CGPalette & operator=(CGPalette const & other);

    void cloneFrom(CGPalette const & other);
    void readFromCGP(IStream* sPal);
    void readFromGraphV2(IStream* sPal, u32 szPalette);
    void readFromMemory(void* p, u32 szPalette);

    CGColor getColor(u8 i) const;
    u32 getARGB(u8 i) const;
    u32 getGL_RGBA(u8 i) const;
    void setColor(u8 i, CGColor clr);

private:
    CGColor colors[0x100];
};

typedef CGSharedRef<CGPalette> CGPaletteRef;

}

#endif //_CG_PALETTE_H_