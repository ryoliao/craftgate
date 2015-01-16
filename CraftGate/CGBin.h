
#ifndef _CG_BIN_H_
#define _CG_BIN_H_

namespace cg
{

/*
    class CGBin

    description:
        Cross Gate (MAOMAOCG2013 version) bin file descriptor.
*/
enum E_CG_BIN_CATEGORY
{
    ECBC_ANIME=0,
    ECBC_ANIMEINFO,
    ECBC_GRAPHIC,
    ECBC_GRAPHICINFO,

    ECGC_COUNT, // not using, but count the bin category.
};

struct SSplitPath;

extern bool parseBin(wchar_t const * filename, s32* category = 0, s32* version = 0, wchar_t* suffix = 0, wchar_t* dir = 0);
extern bool parseBin(SSplitPath const * sp, s32* category = 0, s32* version = 0, wchar_t* suffix = 0, wchar_t* dir = 0);
extern bool parseCGP(SSplitPath const * sp, wchar_t* name = 0);
extern bool parseMAP(SSplitPath const * sp, u32* id=0);

class CGGraphLibrary;
class CGAnimeLibrary;

class CGBin
{
public:
    CGBin(wchar_t const * hint_bin_file = 0);
    ~CGBin();

    bool isOK(u32 category) const;
    void reset();
    void reset(wchar_t const * hint_bin_file);

    wchar_t const * getSuffix() const;
    wchar_t const * getFilePath(u32 category) const;
    s32 getVersion(u32 category) const;

    CGGraphLibraryRef getGraphLibrary() const;
    CGAnimeLibraryRef getAnimeLibrary() const;

    // for internal only
    IRandomStream* getStream(u32 category);

private:
    CGGraphLibraryRef GraphLib;
    CGAnimeLibraryRef AnimeLib;

    std::wstring suffix;
    std::wstring filename[ECGC_COUNT];
    std::wstring filepath[ECGC_COUNT];
    s32 version[ECGC_COUNT];
};

typedef CGSharedRef<CGBin> CGBinRef;

struct CGNamedPalette
{
    std::wstring name;
    CGPaletteRef palette;
};

class CGBinLibrary
{
public:
    CGBinLibrary(wchar_t const * folder = 0);
    ~CGBinLibrary();

    void reset();
    void reset(wchar_t const * folder);

    u32 size() const;
    bool hasSuffix(wchar_t const * name) const;
    CGBinRef getBin(u32 i);
    wchar_t const * getFolder() const;

    u32 getPaletteCount() const;
    CGNamedPalette const & getPalette(u32 i) const;

private:

    void doParseFolder(wchar_t const * folder);
    void doInsertBin(wchar_t const * filename);
    void doInsertCgp(wchar_t const * filename, wchar_t const * name);
    void doInsertMap(wchar_t const * filename, u32 id);

    struct IDRef
    {
        u32 binId;
        u32 graphId;
    };

    std::vector<CGBinRef> Bins;
    std::vector<CGNamedPalette> Palettes;
    std::wstring Folder;
};

typedef CGSharedRef<CGBinLibrary> CGBinLibraryRef;

}

#endif