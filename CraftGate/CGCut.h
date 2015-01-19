
#ifndef _CG_CUT_H
#define _CG_CUT_H

namespace cg
{

class CGCut
{
public:
#if defined(__WIN32__)
#pragma pack(push, 1)
#endif
    struct SData
    {
        u32 refId;
        u16 unknown0;
        u16 from_x;
        u16 from_y;
        u16 width;
        u16 height;
        s16 origin_x;
        s16 orogin_y;
    };
#if defined(__WIN32__)
#pragma pack(pop)
#endif

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