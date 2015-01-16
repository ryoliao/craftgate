
#ifndef _CG_ANINE_H_
#define _CG_ANINE_H_

namespace cg
{

enum E_CG_ANIME_VERSION
{
    ECAV_VERSION_BIT = 0x1,

    ECAV_VERSION_1 = 0x0,
    ECAV_VERSION_2 = 0x1,
};

#if defined(__WIN32__)
#pragma pack(push, 1)
#endif

struct defAnimeInfo
{
    u32 Id;
    u32 address;
    u16 motions;
    u16 version;
};

struct defAnime
{
    u16 dir;        // direction 0-7.
    u16 motion;
    u32 duration;   // unit milliseconds.
    u32 frames;     // frames in duration
};

struct defAnimeV2
{
    defAnime v1;
    u16 paletteId;
    u16 wrap;
    u32 reserved0;  // value fixed 0xffffffff
};

struct defAnimeFrame
{
    u32 graphId;
    u32 reserved0;
    u16 reserved1;
};

#if defined(__WIN32__)
#pragma pack(pop)
#endif

struct CGMotion
{
    bool reset(defAnimeV2 const & desc, IStream* fAnime);
    u32 getFrame(u32 milliseconds) const;

    defAnimeV2 desc;
    std::vector<defAnimeFrame> frames;
};

struct CGAnime
{
    bool reset(defAnimeInfo const & desc, IRandomStream* fAnime);
    CGMotion const * findMotion(u16 dir, u16 motion) const;

    defAnimeInfo desc;
    std::vector<CGMotion> motions;
};

// forward declare
class CGBin;

class CGAnimeLibrary
{
public:
    CGAnimeLibrary();
    ~CGAnimeLibrary();

    void reset();
    bool reset(CGBin* bin);

    u32 size() const;
    CGAnime const * getAnime(u32 id) const;

#if defined(_CG_USE_DUMP)
    void dump(FILE* fp);
#endif

private:
    std::vector<CGAnime> animes;
};

typedef CGSharedRef<CGAnimeLibrary> CGAnimeLibraryRef;

}

#endif