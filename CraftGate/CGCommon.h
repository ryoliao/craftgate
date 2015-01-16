
#ifndef _CG_COMMON_H_
#define _CG_COMMON_H_

namespace cg
{

typedef long               s32;
typedef unsigned long      u32;
typedef short              s16;
typedef unsigned short     u16;
typedef signed char        s8;
typedef unsigned char      u8;
typedef char               c8;
typedef unsigned long long h64;
typedef float              f32;

/*
    TLS macro
*/
#if defined(__WIN32__)

#define CGTLS __declspec(thread)

#else

#define CGTLS

#endif

/*
    class IRefBase

    description:
        reference count based interface for shared object management.
        almost class would inherit IRefBase if need.
*/
class IRefBase
{
public:
    IRefBase()
        : refCount(1)
    {}

    virtual ~IRefBase()
    {}

    void grab()
    {
        ++refCount;
    }

    void drop()
    {
        if (!--refCount)
            delete this;
    }

private:
    u32 refCount;
};

/*
    class CGRef

    description:
        make a reference object for any type.
 */
template<typename T>
class CGRef : public IRefBase
{
public:
    CGRef()
    {}

    // swizzle operation
    template<typename U>
    CGRef(U const & _val)
        : value(_val)
    {}
    template<typename U>
    CGRef(CGRef<U> const & other)
        : value(other.value)
    {}
    template<typename U>
    CGRef<T> & operator= (U const & other)
    {
        value = other.value;
        return *this;
    }
    template<typename U>
    CGRef<T> & operator= (CGRef<U> const & other)
    {
        value = other.value;
        return *this;
    }

    T value;
};

/*
    class CGSharedRef

    description:
        make a shared reference object for any type.
*/
template<typename T>
class CGSharedRef
{
public:
    CGSharedRef()
        : obj(0)
    {
    }

    CGSharedRef(CGSharedRef<T> const & ref)
        : obj(0)
    {
        set(ref);
    }

    ~CGSharedRef()
    {
        reset(0);
    }

    CGSharedRef<T> & operator= (CGSharedRef<T> const & ref)
    {
        set(ref);
        return *this;
    }

    void reset(CGRef<T>* p=0)
    {
        if (obj)
            obj->drop();

        obj = p;
    }

    void set(CGSharedRef<T> const & ref)
    {
        if (ref.obj)
            ref.obj->grab();

        if (obj)
            obj->drop();

        obj = ref.obj;
    }

    void create()
    {
        reset(new CGRef<T>(T()));
    }

    bool isOK() const
    {
        return (0 != obj);
    }

    T & get()
    {
        return obj->value;
    }

    T const & get() const
    {
        return obj->value;
    }

    T * operator-> ()
    {
        return &obj->value;
    }

    T const * operator-> () const
    {
        return &obj->value;
    }

    operator bool() const
    {
        return isOK();
    }

    // swizzle operation
    template<typename U>
    CGSharedRef(U const & val)
        : obj(0)
    {
        reset(new CGRef<T>(val));
    }
    template<typename U>
    CGSharedRef<T> & operator =(U const & val)
    {
        reset(new CGRef<T>(val));
        return *this;
    }

private:
    CGRef<T>* obj;
};

/*
    class CGFileFinder

    description:
        get file list depends on platform rules.
*/
#if defined (_CG_COMPILE_FILEFINDER)
#if defined (__WIN32__)

class CGFileFinder
{
public:
    CGFileFinder()
        : hFind(INVALID_HANDLE_VALUE)
    {
    }

    ~CGFileFinder()
    {
        doClose();
    }

    bool find(wchar_t const * path)
    {
        doClose();
        hFind = FindFirstFile(path, &data);
        return (hFind != INVALID_HANDLE_VALUE);
    }

    bool forward()
    {
        if (hFind != INVALID_HANDLE_VALUE)
        {
            if (!FindNextFile(hFind, &data))
                doClose();
        }
        return (hFind != INVALID_HANDLE_VALUE);
    }

    wchar_t const * getFilename()
    {
        return data.cFileName;
    }

    bool isDirectory()
    {
        return 0 != (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
    }

    static bool isDirectory(wchar_t const * filename)
    {
        DWORD att = ::GetFileAttributes(filename);
        return (att != INVALID_FILE_ATTRIBUTES &&
            0 != (att & FILE_ATTRIBUTE_DIRECTORY));
    }

private:
    void doClose()
    {
        if (hFind != INVALID_HANDLE_VALUE)
        {
            FindClose(hFind);
            hFind = INVALID_HANDLE_VALUE;
        }
    }

    HANDLE hFind;
    WIN32_FIND_DATA data;
};

#else
#error file finder only support on windows platform for now.
#endif
#endif

// event handle
enum E_CG_EVENT_TYPE
{
    ECET_ERROR=0,
    ECET_WARNING,
    ECET_MESSAGE,
};

struct SEvent
{
    u32 EventType;
    std::string Message;
};

class IEventHandler : virtual public IRefBase
{
public:
    virtual void OnEvent(SEvent & e) = 0;
};

class CGError
{
public:
    static void setHandler(IEventHandler* handler);

    static void error(c8 const * format, ...);
    static void warning(c8 const * format, ...);
    static void message(c8 const * format, ...);
};

}

#endif //_CG_COMMON_H_