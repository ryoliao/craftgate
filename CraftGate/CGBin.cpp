
#include "CGGraph_PCH.h"

namespace cg
{

struct SBinCategoryName
{
    SBinCategoryName(wchar_t const * _name)
        : szName(0)
    {
        name = _name;
        if (name)
            szName = wcslen(name);
    }
    wchar_t const * name;
    u32 szName;
};

static SBinCategoryName CGBinCategoryNames[] =
{
    SBinCategoryName(L"anime"),
    SBinCategoryName(L"animeinfo"),
    SBinCategoryName(L"graphic"),
    SBinCategoryName(L"graphicinfo"),
    SBinCategoryName(0)
};

static const u32 MAX_BIN_SUFFIX = 32;

CGBin::CGBin(wchar_t const * hint_bin_file)
{
    reset(hint_bin_file);
}

CGBin::~CGBin()
{
    // do nothing here
}

bool CGBin::isOK(u32 category) const
{
    if (category >= ECGC_COUNT)
        return false;

    if (-1L == version[category] ||
        filepath[category].empty() ||
        -1L == _waccess(filepath[category].c_str(), 0x04)) // read permission
            return false;

    return true;
}

void CGBin::reset()
{
    u32 i;
    suffix.clear();
    GraphLib.reset();
    AnimeLib.reset();
    for (i=0; i<ECGC_COUNT; ++i)
        version[i] = -1L;
    for (i=0; i<ECGC_COUNT; ++i)
        filepath[i].clear();
}

void CGBin::reset(wchar_t const * hint_bin_file)
{
    reset();

    if (!hint_bin_file)
        return;

    wchar_t hint_suffix[MAX_BIN_SUFFIX];
    wchar_t dir[MAX_PATH];
    if (!parseBin(hint_bin_file, 0, 0, hint_suffix, dir))
        return;

    suffix = hint_suffix;

    CGFileFinder ff;
    s32 cat, ver;
    wchar_t searchDir[MAX_PATH];

#if defined (__WIN32__)
    wsprintf(searchDir, L"%s*%s*.bin", dir, hint_suffix);
#else
#error TODO: no implementation on this platform yet.
#endif
    if (ff.find(searchDir)) do 
    {
        if (!ff.isDirectory() &&
            parseBin(ff.getFilename(), &cat, &ver, hint_suffix))
        {
            if (!_wcsicmp(hint_suffix, suffix.c_str()) &&
                ver > version[cat])
            {
                filename[cat] = ff.getFilename();
                filepath[cat] = dir;
                filepath[cat].append(ff.getFilename());
                version[cat] = ver;
            }
        }
    } while (ff.forward());

    GraphLib.create();
    if (!GraphLib->reset(this))
        GraphLib.reset();
    AnimeLib.create();
    if (!AnimeLib->reset(this))
        AnimeLib.reset();
}

wchar_t const * CGBin::getFilePath(u32 category) const
{
    if (category >= ECGC_COUNT)
        return 0;

    return filepath[category].c_str();
}

wchar_t const * CGBin::getSuffix() const
{
    return suffix.c_str();
}

s32 CGBin::getVersion(u32 category) const
{
    if (category >= ECGC_COUNT)
        return -1L;

    return version[category];
}

IRandomStream* CGBin::getStream(u32 category)
{
    if (!isOK(category))
        return 0;

    return new CGFile(filepath[category].c_str());
}

CGGraphLibraryRef CGBin::getGraphLibrary() const
{
    return GraphLib;
}

CGAnimeLibraryRef CGBin::getAnimeLibrary() const
{
    return AnimeLib;
}

///////////////////////////////////////////////////////////////////////////////

CGBinLibrary::CGBinLibrary( wchar_t const * folder/*=0*/ )
{
    reset(folder);
}

CGBinLibrary::~CGBinLibrary()
{

}

void CGBinLibrary::reset()
{
    Bins.clear();
    Palettes.clear();
    Folder.clear();
}

void CGBinLibrary::reset(wchar_t const * folder)
{
    reset();
    if (folder)
    {
        doParseFolder(folder);
        Folder = folder;
    }
}

void CGBinLibrary::doParseFolder(wchar_t const * folder)
{
    if (!folder ||
        !CGFileFinder::isDirectory(folder))
        return;

#if defined(__WIN32__)
    wchar_t buf[MAX_PATH];
    wsprintf(buf, L"%s\\*", folder);
#else
#error no implementation yet.
#endif

    CGFileFinder finder;
    if (finder.find(buf)) do 
    {
        if (finder.isDirectory())
        {
            if (finder.getFilename()[0] != L'.')
            {
                wsprintf(buf, L"%s\\%s", folder, finder.getFilename());
                doParseFolder(buf);
            }
        }
        else
        {
            u32 id;
            wchar_t filename[MAX_PATH];
            wsprintf(filename, L"%s\\%s", folder, finder.getFilename());

            SSplitPath sp(filename);
            if (sp.isOK())
            {
                if (parseCGP(&sp, buf))
                {
                    doInsertCgp(filename, buf);
                }
                else
                if (parseBin(&sp, 0, 0, buf, 0) &&
                    !hasSuffix(buf))
                {
                    doInsertBin(filename);
                }
                else
                if (parseMAP(filename, &sp, &id))
                {
                    doInsertMap(filename, id);
                }
                else
                if (parseCUT(&sp, &id))
                {
                    doInsertCut(filename, id);
                }
            }
        }
    } while (finder.forward());
}

void CGBinLibrary::doInsertBin(wchar_t const * filename)
{
    CGBinRef bin;
    u32 binId;

    binId = Bins.size();

    bin.create();
    bin->reset(filename);
    CGGraphLibraryRef lib = bin->getGraphLibrary();
    if (lib)
    {
        for (u32 i=0; i<lib->size(); ++i)
        {
            defGraphInfo const * desc = lib->getInfo(i);
            if (0 != desc->refId)
            {
                IDRef data;
                data.binId = binId;
                data.graphId = i;
                //GlobalRefs[desc->refId] = data;
            }
        }
        Bins.push_back(bin);
    }
}

void CGBinLibrary::doInsertCgp(wchar_t const * filename, wchar_t const * name)
{
    CGFile f(filename);
    if (f.isOK())
    {
        Palettes.push_back(CGNamedPalette());
        CGNamedPalette & pal = Palettes.back();

        pal.name = name;
        pal.palette.create();
        pal.palette->readFromCGP(&f);
    }
}

void CGBinLibrary::doInsertMap( wchar_t const * filename, u32 id )
{
    Maps[id] = CGMapInfo{ filename };
}

void CGBinLibrary::doInsertCut(wchar_t const * filename, u32 id)
{
    Cuts[id] = CGCutInfo{ filename };
}

u32 CGBinLibrary::size() const
{
    return Bins.size();
}

bool CGBinLibrary::hasSuffix(wchar_t const * name) const
{
    for (u32 i=0; i<Bins.size(); ++i)
    {
        if (!_wcsicmp(name, Bins[i]->getSuffix()))
            return true;
    }
    return false;
}

CGBinRef CGBinLibrary::getBin(u32 i)
{
    return Bins[i];
}
CGBinRef CGBinLibrary::getBinWithSuffix(wchar_t const * name) const
{
    for (auto var : Bins)
    {
        if (!_wcsicmp(name, var->getSuffix()))
            return var;
    }
    return CGBinRef();
}

u32 CGBinLibrary::getPaletteCount() const
{
    return Palettes.size();
}

CGNamedPalette const & CGBinLibrary::getPalette(u32 i) const
{
    return Palettes[i];
}

CGMapRef CGBinLibrary::readMap(u32 mapId) const
{
    auto it = Maps.find(mapId);
    if (it == Maps.end())
        return CGMapRef();

    CGMapRef map;
    map.create();
    map->reset(it->second.file.c_str());

    return map;
}
CGCutRef CGBinLibrary::readCut(u32 mapId) const
{
    auto it = Cuts.find(mapId);
    if (it == Cuts.end())
        return CGCutRef();

    CGCutRef cut;
    cut.create();
    cut->reset(it->second.file.c_str());

    return cut;
}

std::map<u32, CGMapInfo> const & CGBinLibrary::getMaps() const
{
    return Maps;
}

wchar_t const * CGBinLibrary::getFolder() const
{
    return Folder.c_str();
}

///////////////////////////////////////////////////////////////////////////////

bool parseBin(SSplitPath const * sp, s32* category, s32* version, wchar_t* suffix, wchar_t* dir)
{
    // check extension name
    if (_wcsicmp(sp->ext, L".bin"))
        return false;

    // search category
    s32 cate = -1L;
    for (u32 i=0; i<ECGC_COUNT; ++i)
    {
        if (!_wcsnicmp(sp->name,
            CGBinCategoryNames[i].name,
            CGBinCategoryNames[i].szName))
        {
            // FIXED: short name conflict.
            if (-1L == cate ||
                CGBinCategoryNames[i].szName > CGBinCategoryNames[cate].szName)
                cate = i;
        }
    }
    if (-1L == cate)
        return false;

    // return category
    if (category)
        *category = cate;

    // get version
    wchar_t const * sver = wcsrchr(sp->name, L'_');
    if (!sver || !isdigit(*(sver+1)))
        return false;
    
    // return version
    if (version)
        *version = _wtoi(sver+1);

    // return suffix
    if (suffix)
    {
        wchar_t const * beg = sp->name+CGBinCategoryNames[cate].szName;
        u32 szSuffix = sver-beg;
        wcsncpy(suffix, beg, szSuffix);
        suffix[szSuffix] = L'\0';
    }

    // return dir
    if (dir)
        wsprintf(dir, L"%s%s", sp->drive, sp->dir);

    return true;
}

extern bool parseBin(wchar_t const * filename, s32* category/*=0*/, s32* version/*=0*/, wchar_t* suffix/*=0*/, wchar_t* dir/*=0*/)
{
    SSplitPath sp(filename);
    return parseBin(&sp, category, version, suffix, dir);
}

bool parseCGP(SSplitPath const * sp, wchar_t* name/*=0*/)
{
    // check extension name
    if (_wcsicmp(sp->ext, L".cgp"))
        return false;

    if (name)
        wcscpy(name, sp->name);

    return true;
}

bool parseMAP(wchar_t const * filename, SSplitPath const * sp, u32* id/*=0*/)
{
    // FIXME: for client map only
    // check extension name
    if (_wcsicmp(sp->ext, L".dat"))
        return false;

    if (!isdigit(*sp->name))
        return false;

    if (!CGMap::isMap(filename))
        return false;

    if (id)
        *id = (u32)_wtoi(sp->name);

    return true;
}

bool parseCUT(SSplitPath const * sp, u32 * id)
{
    if (_wcsicmp(sp->ext, L".cut"))
        return false;

    if (!isdigit(*sp->name))
        return false;

    if (id)
        *id = (u32)_wtoi(sp->name);

    return true;
}

}