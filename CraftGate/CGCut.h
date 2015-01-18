
#ifndef _CG_CUT_H
#define _CG_CUT_H

namespace cg
{

class CGCut
{
public:
    struct SData
    {
        u32 refId;
        u16 unknown0;
        u16 from_x;
        u16 from_y;
        u16 width;
        u16 height;
        u16 offset_x;
        u16 offset_y;
    };

    CGCut();
    CGCut(wchar_t const * hint_cut_file);
    ~CGCut();

    void reset();
    void reset(wchar_t const * hint_cut_file);

    u32 getSize() const;
    std::vector<SData> const & getData() const;

private:
    void doReadCut(IStream* s);

    std::vector<SData> data;
};

typedef CGSharedRef<CGCut> CGCutRef;

}

#endif //_CG_CUT_H