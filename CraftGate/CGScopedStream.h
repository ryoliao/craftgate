
#ifndef _CG_SCOPED_STREAM_H_
#define _CG_SCOPED_STREAM_H_

namespace cg
{

/*
    class CGFileRanged

    description:
        range protected file.
*/
class CGScopedStream : public IRandomStream
{
public:
    CGScopedStream(IRandomStream* stream, u32 protectSize);
    CGScopedStream(IRandomStream* stream, s32 offset, s32 origin, u32 protectSize);
    ~CGScopedStream();

    // override methods
    virtual bool isOK() const;
    virtual s32 read(void* ptr, u32 elementSize, u32 count);
    virtual s32 seek(s32 offset, s32 origin);
    virtual s32 tell();

private:
    IRandomStream* stream;
    s32 offset;
    s32 size;
};

}

#endif //_CG_SCOPED_STREAM_H_