
#ifndef _CG_INTERFACE_STREAM_H_
#define _CG_INTERFACE_STREAM_H_

namespace cg
{

/*
    interface IStream, IRandomStream

    description:
        1) all interface's methods follow standard arguments and return value.
           for example, return value as EOF, arguments as SEEK_SET, SEEK_CUR, SEEK_END.
        2) only support less then 32-bits file size.
*/

class IStream : public virtual IRefBase
{
public:
    // interface methods
    virtual s32 read(void* ptr, u32 elementSize, u32 count)=0;
    virtual bool isOK() const=0;

    // interface utilities

    /*
        read()

        description:
            read a POD structure to given pointer.
            endian order depends on platform.
    */
    template<typename T>
    s32 read(T* ptr, u32 count=1)
    {
        return read(ptr, sizeof(T), count);
    }

    /*
        readByte()

        description:
            read a byte and return it.

        result:
            on success, value as read byte is returned.
            on failure, EOF is returned.
    */
    virtual s32 readByte()
    {
        u8 data;
        if (1 != read(&data))
            return EOF;
        return data;
    }
};

///////////////////////////////////////////////////////////////////////////////

class IRandomStream : public IStream
{
public:
    // interface methods
    virtual s32 seek(s32 offset, s32 origin)=0;
    virtual s32 tell()=0;
    virtual u32 size()=0;
};

}

#endif //_CG_INTERFACE_STREAM_H_