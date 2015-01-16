
#ifndef _CG_FILE_H_
#define _CG_FILE_H_

namespace cg
{

class CGFile : public IRandomStream
{
public:
    CGFile();
    CGFile(wchar_t const * filename);
    ~CGFile();

    void open(wchar_t const * filename);
    void close();

    // override methods
    virtual bool isOK() const;
    virtual s32 read(void* ptr, u32 elementSize, u32 count);
    virtual s32 seek(s32 offset, s32 origin);
    virtual s32 tell();
    virtual u32 size();

private:
    FILE* pf;
    u32 szFile;
};

}

#endif //_CG_FILE_H_