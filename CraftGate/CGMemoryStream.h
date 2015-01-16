
#ifndef _CG_MEMORY_STREAM_H_
#define _CG_MEMORY_STREAM_H_

namespace cg
{

class CGMemoryStream : public IRandomStream
{
public:
    CGMemoryStream();
    CGMemoryStream(IStream* stream, u32 szRead);
    CGMemoryStream(void* buf, u32 szBuffer);
    ~CGMemoryStream();

    void reset();
    void reset(IStream* stream, u32 szRead);
    void reset(void* buf, u32 szBuffer);

    virtual bool isOK() const;
    virtual s32 read(void* ptr, u32 elementSize, u32 count);
    virtual s32 seek(s32 offset, s32 origin);
    virtual s32 tell();

private:
    std::vector<u8> data;
    s32 curpos;
};

}

#endif //_CG_MEMORY_STREAM_H_