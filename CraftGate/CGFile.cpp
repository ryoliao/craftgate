
#include "CGGraph_PCH.h"

namespace cg
{

CGFile::CGFile()
    : pf(0)
    , szFile(0)
{
}

CGFile::CGFile( wchar_t const * filename )
    : pf(0)
    , szFile(0)
{
    open(filename);
}

CGFile::~CGFile()
{
    close();
}

void CGFile::open( wchar_t const * filename )
{
    close();
    pf = ::_wfopen(filename, L"rb");
    if (pf)
    {
        int df = ::_fileno(pf);
        struct stat st;
        if (!::fstat(df, &st))
            szFile = st.st_size;
    }
}

void CGFile::close()
{
    if (pf)
    {
        fclose(pf);
        pf=0;
        szFile=0;
    }
}

bool CGFile::isOK() const
{
    return (NULL != pf);
}

s32 CGFile::read( void* ptr, u32 elementSize, u32 count )
{
    return fread(ptr, elementSize, count, pf);
}

s32 CGFile::seek( s32 offset, s32 origin )
{
    return fseek(pf, offset, origin);
}

s32 CGFile::tell()
{
    return ftell(pf);
}

u32 CGFile::size()
{
    return szFile;
}

}