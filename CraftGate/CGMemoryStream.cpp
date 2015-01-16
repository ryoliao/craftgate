
#include "CGGraph_PCH.h"

namespace cg
{
    
CGMemoryStream::CGMemoryStream()
    : curpos(-1L)
{
}

CGMemoryStream::CGMemoryStream(IStream* stream, u32 szRead)
{
    reset(stream, szRead);
}

CGMemoryStream::CGMemoryStream(void* buf, u32 szBuffer)
    : curpos(-1L)
{
    reset(buf, szBuffer);
}

CGMemoryStream::~CGMemoryStream()
{
    // nothing behavior required in here.
}

void CGMemoryStream::reset()
{
    data.clear();
    curpos = -1L;
}

void CGMemoryStream::reset(IStream* stream, u32 szRead)
{
    reset();

    if (szRead)
    {
        data.resize(szRead);
        if (szRead == stream->read(data.data(), 1, szRead))
            curpos = 0;
        else
            data.clear();
    }
}

void CGMemoryStream::reset(void* buf, u32 szBuffer)
{
    reset();

    if (szBuffer)
    {
        data.resize(szBuffer);
        memcpy(data.data(), buf, szBuffer);
        curpos = 0;
    }
}

bool CGMemoryStream::isOK() const
{
    return (-1L != curpos && !data.empty());
}

s32 CGMemoryStream::read(void* ptr, u32 elementSize, u32 count)
{
    if (!isOK())
        return 0;

    count = std::min(count, (data.size() - curpos)/elementSize);

    if (count <= 0)
        return 0;

    elementSize *= count;
    memcpy(ptr, data.data()+curpos, elementSize);
    curpos += elementSize;

    return count;
}

s32 CGMemoryStream::seek(s32 offset, s32 origin)
{
    s32 pos;
    switch (origin)
    {
    case SEEK_SET:
        pos = offset;
        break;
    case SEEK_CUR:
        pos = curpos+offset;
        break;
    case SEEK_END:
        pos = data.size()-offset;
        break;
    }

    if (pos < 0 || (u32)pos > data.size())
        return -1L;

    curpos = pos;
    return 0;
}

s32 CGMemoryStream::tell()
{
    return curpos;
}

}