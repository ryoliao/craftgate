
#include "CGGraph_PCH.h"

namespace cg
{

CGScopedStream::CGScopedStream(IRandomStream* s, u32 protectSize)
    : stream(s)
    , size(protectSize)
{
    if (stream)
    {
        stream->grab();
        offset = stream->tell();
        if (-1L == offset)
        {
            stream->drop();
            stream = 0;
        }
    }
}

CGScopedStream::CGScopedStream(IRandomStream* s, s32 _offset, s32 origin, u32 protectSize)
    : stream(s)
    , size(protectSize)
{
    if (stream)
    {
        stream->grab();
        if (stream->seek(offset, origin))
        {
            stream->drop();
            stream = 0;
        }
        else
            offset = stream->tell();
    }
}

CGScopedStream::~CGScopedStream()
{
    if (stream)
        stream->drop();
}

bool CGScopedStream::isOK() const
{
    return ((0 != stream) && stream->isOK());
}

s32 CGScopedStream::read(void* ptr, u32 elementSize, u32 count)
{
    if (!isOK())
        return 0;

    s32 pos = stream->tell();
    if (-1L == pos)
        return 0;

    count = std::min(count, (size-pos)/elementSize);
    if (count <= 0)
        return 0;

    return stream->read(ptr, elementSize, count);
}

s32 CGScopedStream::seek(s32 _offset, s32 origin)
{
    const s32 SEEK_ERROR_VALUE = -1L;

    if (!isOK())
        return SEEK_ERROR_VALUE;

    s32 pos;
    switch (origin)
    {
    case SEEK_SET:
        if (_offset < 0 || _offset > size)
            return SEEK_ERROR_VALUE;
        pos = offset + _offset;
        break;

    case SEEK_CUR:
        pos = stream->tell();
        if (SEEK_ERROR_VALUE == pos)
            return SEEK_ERROR_VALUE;
        pos += _offset;
        if (pos > offset+size || pos < offset)
            return SEEK_ERROR_VALUE;
        break;

    case SEEK_END:
        if (_offset < 0 || _offset > size)
            return SEEK_ERROR_VALUE;
        pos = offset + size - _offset;
        break;
    }
    return stream->seek(pos, SEEK_SET);
}

s32 CGScopedStream::tell()
{
    const s32 TELL_ERROR_VALUE = -1L;

    if (!isOK())
        return TELL_ERROR_VALUE;

    s32 pos = stream->tell();
    if (TELL_ERROR_VALUE != pos)
    {
        pos -= offset;
        if (pos < 0 || pos > size)
            return TELL_ERROR_VALUE;
    }
    return pos;
}

}