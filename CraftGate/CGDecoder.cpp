
#include "CGGraph_PCH.h"

namespace cg
{

CGRLEDecoder::CGRLEDecoder(IStream* _stream)
    : stream(_stream)
    , pattern(REP)
    , repeat(0)
    , value(0)
{
    if (stream)
        stream->grab();
}

CGRLEDecoder::~CGRLEDecoder()
{
    if (stream)
        stream->drop();
}

s32 CGRLEDecoder::read(void* ptr, u32 elementSize, u32 count)
{
    if (!elementSize)
        return 0;

    u32 i;
    u32 szRead, szRequire;
    u8* iptr = (u8*)ptr;
    for (i=0; i<count; ++i)
    {
        szRequire = elementSize;
        while (szRequire)
        {
            if (!repeat)
                doDataForward();

            szRead = std::min(repeat, szRequire);
            if (szRead)
            {
                switch (pattern)
                {
                case REP:
                    memset(iptr, value, szRead);
                    break;

                case STR:
                    if (1 != stream->read(iptr, szRead, 1))
                        pattern = INV;
                    break;
                }
                if (INV == pattern)
                    return i;

                iptr += szRead;
                repeat -= szRead;
                szRequire -= szRead;
            }
        }
    }
    return i;
}

void CGRLEDecoder::doDataForward()
{
    s32 lo, hi, fn;
    s32 c = stream->readByte();

    if (c == EOF)
    {
        pattern = INV;
        return;
    }

    lo = (c & 0x0f);
    hi = (c & 0x30) >> 4;
    fn = (c & 0xc0);

    switch (fn)
    {
    case 0x00:
        pattern = STR;
        value = 0;
        break;

    case 0x80:
        pattern = REP;
        value = stream->readByte();
        break;

    case 0xc0:
        pattern = REP;
        value = 0;
        break;

    default:
        pattern = INV;
        break;
    }

    if (EOF == value)
        pattern = INV;

    if (INV != pattern)
        repeat = doReadLength(lo, hi);
}

s32 CGRLEDecoder::doReadLength(s32 base, u32 size)
{
    int c;
    for (u32 i=0; i<size; ++i)
    {
        c = stream->readByte();

        if (EOF == c)
        {
            pattern = INV;
            return 0;
        }

        base = (base<<8)|c;
    }
    return base;
}

bool CGRLEDecoder::isOK() const
{
    return (stream && (INV != pattern));
}

}