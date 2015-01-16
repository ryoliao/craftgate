
#ifndef _CG_DECODER_H_
#define _CG_DECODER_H_

namespace cg
{

class CGRLEDecoder : public IStream
{
public:
    CGRLEDecoder(IStream* stream);
    ~CGRLEDecoder();

    // IStream overrides.
    virtual s32 read(void* ptr, u32 elementSize, u32 count);
    virtual bool isOK() const;

private:
    enum
    {
        REP=0,
        STR,
        INV,
    };

    void doDataForward();
    s32 doReadLength(s32 base, u32 size);

    u32 pattern;
    u32 repeat;
    u32 value;
    IStream* stream;
};

}

#endif