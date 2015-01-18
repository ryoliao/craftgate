
#include "CGGraph_PCH.h"

namespace cg
{

struct SCutHeader
{
    c8 headerId[6];
    u16 count0;
    u16 count1;
    u16 count2;
};

CGCut::CGCut()
{
    reset();
}

CGCut::CGCut(wchar_t const * hint_cut_file)
{
    reset(hint_cut_file);
}

CGCut::~CGCut()
{

}

void CGCut::reset()
{
    data.clear();
}

void CGCut::reset(wchar_t const * hint_cut_file)
{
    if (!hint_cut_file)
    {
        reset();
        return;
    }

    CGFile f(hint_cut_file);
    if (f.isOK())
        doReadCut(&f);
    else
        reset();
}

u32 CGCut::getSize() const
{
    return data.size();
}

std::vector<CGCut::SData> const & CGCut::getData() const
{
    return data;
}

void CGCut::doReadCut(IStream* s)
{
    SCutHeader header;
    s->read(&header);

    if (_stricmp(header.headerId, "CUT""\x20""\x0c"))
    {
        reset();
        return;
    }

    u32 nData = header.count0 + header.count1 + header.count2;
    data.resize(nData);

    u32 nRead = s->read(data.data(), nData);
    if (nRead < nData)
        data.reserve(nRead);
}

}