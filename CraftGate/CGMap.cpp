
#include "CGGraph_PCH.h"

namespace cg
{

static const CGMap::SData nullMapData{ 0u, 0u, 0u };

struct SClientMapHeader
{
    c8 headerId[4];
    u32 reserved0;
    u32 reserved1;
    u32 width;
    u32 height;
};

// Tip: all server map data is use BE
struct SServerMapHeader
{
    c8 headerId[6];
    u16 mapId;
    c8 mapName[32];
    u16 width;
    u16 height;
};

CGMap::CGMap(wchar_t const * hint_map_file)
{
    reset(hint_map_file);
}

CGMap::~CGMap()
{
    // do nothing
}

void CGMap::reset()
{
    id = -1;
    size = CGSizei{ 0, 0 };
    name.clear();
    data.clear();
}

void CGMap::reset(wchar_t const * hint_map_file)
{
    s32 type;
    if (!isMap(hint_map_file, &type))
    {
        reset();
        return;
    }

    CGFile f(hint_map_file);
    if (type == ECMT_CLIENT_MAP)
    {
        SSplitPath path(hint_map_file);
        doReadClientMap(&f, _wtoi(path.name));
    }
    else
    {
        doReadServerMap(&f);
    }
}

u32 CGMap::getId() const
{
    return id;
}

std::string const & CGMap::getName() const
{
    return name;
}

CGSizei CGMap::getSize() const
{
    return size;
}

std::vector<CGMap::SData> & CGMap::getData()
{
    return data;
}

void CGMap::doReadServerMap(IRandomStream* s)
{
    // TODO: not implement
}

void CGMap::doReadClientMap(IRandomStream* s, u32 fid)
{
    SClientMapHeader header;
    s->read(&header);

    size.X = header.width;
    size.Y = header.height;

    u32 nData = size.Volume();
    data.assign(nData, nullMapData);
    name = "(null)";
    id = fid;

    SData * vmap = data.data();
    // read L0 data
    for (u32 i = 0; i < nData; ++i)
        s->read(&(vmap[i].L0));
    // read L1 data
    for (u32 i = 0; i < nData; ++i)
        s->read(&(vmap[i].L1));
    // read flags
    for (u32 i = 0; i < nData; ++i)
        s->read(&(vmap[i].flags));
}

bool CGMap::isMap(wchar_t const * file, s32* mapType)
{
    if (!file)
        return false;

    CGFile f(file);
    if (!f.isOK())
        return false;

    IRandomStream* s = &f;

    c8 headId[6];
    if (6 != s->read(headId, 6))
        return false;

    if (!strncmp(headId, "LS2MAP", 6))
    {
        if (mapType) *mapType = ECMT_SERVER_MAP;
        return true;
    }
    else if (!strncmp(headId, "MAP\1", 4))
    {
        if (mapType) *mapType = ECMT_CLIENT_MAP;
        return true;
    }
    return false;
}

} // namespace cg
