
#include "CGGraph_PCH.h"
#include <gif_lib.h>

namespace cg
{

const u16 InvalidMapId = 0xffff;

CGPointi CGOpenGLMap::GetPosition(s32 x, s32 y) const
{
    return CGPointi((x + y) * MapGridWidth, (y - x) * MapGridHeight);
}

CGPointi CGOpenGLMap::GetIndex(s32 x, s32 y)
{
    x /= MapGridWidth;
    y /= MapGridHeight;
    return CGPointi((x - y) / 2, (x + y) / 2);
}

CGPointi CGOpenGLMap::GetIndex(CGPointi pos)
{
    return GetIndex(pos.X, pos.Y);
}

CGOpenGLGraph CGOpenGLMap::GetGraph(s32 x, s32 y, u32 layer) const
{
    if (x < 0 || x >= (s32)size.X ||
        y < 0 || y >= (s32)size.Y)
        return CGOpenGLGraph();

    u16 i;
    switch (layer)
    {
    case 0:
        i = mappedData[x + y * size.X].L0;
        break;
    case 1:
        i = mappedData[x + y * size.X].L1;
        break;
    default:
        return CGOpenGLGraph();
    }

    if (i == InvalidMapId)
        return CGOpenGLGraph();

    return graphLib[i];
}

CGSizei CGOpenGLMap::Last() const
{
    return CGSizei(size.X - 1, size.Y - 1);
}

CGPointi CGOpenGLMap::GetCenter() const
{
    return GetPosition(size.X / 2, size.Y / 2);
}

class CGPixelData
{
public:
    CGPixelData(CGGraphRef graph, CGPaletteRef pal)
        : ptr(graph->buffer.data())
        , pitch(graph->desc.width)
        , palette(pal)
    {}

    inline void ConvertRowPixels(long row, GifFileType* gif)
    {
        u8* sp = ptr + (row*pitch);
        ::EGifPutLine(gif, sp, pitch);
    }

    inline void InvertRowPixels(long row, GifFileType* gif)
    {
        u8* sp = ptr + ((row+1)*pitch);
        for (u32 i=0; i<pitch; ++i)
        {
            --sp;
            ::EGifPutPixel(gif, *sp);
        }
    }

private:
    u8* ptr;
    u32 pitch;
    CGPaletteRef palette;
};

CGSynthetic::CGSynthetic()
    : PaletteId(-1L)
{

}

CGSynthetic::CGSynthetic(CGBinLibraryRef lib)
{
    reset(lib);
}

CGSynthetic::~CGSynthetic()
{

}

bool CGSynthetic::isOK() const
{
    return (Lib && Bin);
}

void CGSynthetic::reset()
{
    PaletteId = -1L;
    Bin.reset();
    Lib.reset();
    AnimePalettes.clear();
    MapPoints.clear();
}

void CGSynthetic::reset(CGBinLibraryRef lib)
{
    reset();

    Lib = lib;
    selectPalette(0);
    doParse();
}

void CGSynthetic::selectBin(u32 binId)
{
    Bin = Lib->getBin(binId);
}

void CGSynthetic::selectPalette(u32 palId)
{
    if (palId < Lib->getPaletteCount())
        PaletteId = palId;
}

MotionIterator CGSynthetic::findMotion(u32 animeId, u16 dir, u16 motion)
{
    MotionIterator it;
    CGAnimeLibraryRef alib = Bin->getAnimeLibrary();
    if (alib)
    {
        CGAnime const * a = alib->getAnime(animeId);
        if (a)
        {
            it.refId = a->desc.Id;
            it.data = a->findMotion(dir, motion);
            if (it.data)
                it.palette = doGetPalette(it.data);
        }
    }
    return it;
}

MapPoint const * CGSynthetic::findMapPoint(u32 refId)
{
    auto it = MapPoints.find(refId);
    if (it == MapPoints.end())
        return nullptr;
    return &it->second;
}

CGPaletteRef CGSynthetic::doGetPalette(CGMotion const * mo)
{
    CGPaletteRef pal;
    if (mo->desc.paletteId)
    {
        auto it = AnimePalettes.find(mo->desc.paletteId);
        if (it != AnimePalettes.end())
            pal = it->second;
    }
    return pal;
}

void CGSynthetic::doParse()
{
    CGBinRef bin;
    CGAnimeLibraryRef alib;
    CGGraphLibraryRef glib;
    CGGraphRef graph;
    defGraphInfo const * gdesc;
    CGAnime const * ani;
    CGMotion const * mo;
    std::map<u32, CGMotion const *> moMap;

    // reset anime palettes
    AnimePalettes.clear();
    // reset map points
    MapPoints.clear();

    // make animation map temporarily
    for (u32 iBin=0; iBin < Lib->size(); ++iBin)
    {
        bin = Lib->getBin(iBin);
        alib = bin->getAnimeLibrary();
        if (alib)
        {
            for (u32 iAni=0; iAni < alib->size(); ++iAni)
            {
                ani = alib->getAnime(iAni);
                if (ani->desc.motions)
                    moMap[ani->desc.Id] = ani->motions.data();
            }
        }
    }

    // build animation and map-points with graph refId
    for (u32 iBin = 0; iBin < Lib->size(); ++iBin)
    {
        bin = Lib->getBin(iBin);
        glib = bin->getGraphLibrary();
        if (glib)
        {
            for (u32 iGraph = 0; iGraph < glib->size(); ++iGraph)
            {
                gdesc = glib->getInfo(iGraph);
                if (ECRID_INVALID != gdesc->refId)
                {
                    switch (refIdType(gdesc->refId))
                    {
                    case ECRID_ANIMATION:
                    {
                        auto it = moMap.find(gdesc->refId);
                        if (it != moMap.end())
                        {
                            mo = it->second;
                            if (mo->desc.paletteId)
                            {
                                graph = glib->readGraph(iGraph);
                                if (graph && graph->palette)
                                    AnimePalettes[mo->desc.paletteId] = graph->palette;
                            }
                        }
                    }
                    break;

                    case ECRID_GRAPH:
                    case ECRID_GRAPH_EX:
                    case ECRID_GRAPH_SE:
                    default:
                    {
                        MapPoints[gdesc->refId] = MapPoint{ iBin, iGraph };
                    }
                    break;
                    };
                }
            }
        }
    }
}

CGOpenGLGraph CGSynthetic::CreateOpenGL(u32 graphId)
{
    CGGraphLibraryRef glib = Bin->getGraphLibrary();
    if (glib)
    {
        return CreateOpenGL(glib->readGraph(graphId));
    }
    return CGOpenGLGraph();
}

CGOpenGLGraph CGSynthetic::CreateOpenGL(MotionIterator iterator, u32 frame)
{
    CGGraphLibraryRef glib = Bin->getGraphLibrary();
    if (glib)
    {
        u32 graphId = iterator.data->frames[frame].graphId;
        CGGraphRef graph = glib->readGraph(graphId);

        if (graph && iterator.palette)
            graph->palette = iterator.palette;

        s32 direction =
            (0 != (iterator.data->desc.wrap & 0x1))
            ? (CG_RIGHT| CG_TOP)
            : (CG_LEFT | CG_TOP);

        return CreateOpenGL(graph, direction);
    }
    return CGOpenGLGraph();
}

CGOpenGLGraph CGSynthetic::CreateOpenGL(CGGraphRef graph, s32 direction)
{
    CGOpenGLGraph oglGraph;
    if (graph)
    {
        defGraphInfo const & desc = graph->desc;

        oglGraph.direction = direction;

        if (direction & CG_RIGHT)
            oglGraph.offset = CGPointi(-(desc.width + desc.offX), desc.offY);
        else
            oglGraph.offset = CGPointi(desc.offX, desc.offY);

        if (!graph->palette &&
            PaletteId != -1L)
            graph->palette = Lib->getPalette(PaletteId).palette;

        oglGraph.tex.create();
        oglGraph.tex->reset(graph);
    }
    return oglGraph;
}

CGOpenGLGraph CGSynthetic::CreateOpenGLWithCut(CGCutRef cut, u32 cutId)
{
    if (cut.isOK())
    {
        auto data = cut->getData();
        if (cutId < data.size())
        {
            auto desc = data.at(cutId);
            CGGraphRef graph = createGraphFromRef(desc.refId);
            if (graph.isOK())
            {
                CGOpenGLGraph oglGraph;
                oglGraph.offset = CGPointi(desc.offset_x, desc.offset_y);
                oglGraph.direction = CG_LEFT | CG_TOP;
                oglGraph.tex.create();
                if (!oglGraph.tex->reset(graph, desc.from_x, desc.from_y, desc.width, desc.height))
                    oglGraph.tex.reset();
                return oglGraph;
            }
        }
    }
    return CGOpenGLGraph();
}

CGOpenGLMapRef CGSynthetic::createMap(u32 id)
{
    CGMapRef map = Lib->readMap(id);

    if (!map.isOK())
        return CGOpenGLMapRef();
    
    CGOpenGLMapRef glMap;

    glMap = createMapFromCut(map);
    if (glMap.isOK())
        return glMap;

    std::map<u16, u16> mapping;
    glMap.create();

    // I'm lazy, so use c++11 as simple.
    glMap->mappedData = std::move(map->getData());
    glMap->size = map->getSize();
    // mapping lambda
    auto mappingSetter = [&](u16 & mapId)
    {
        // skip empty data
        if (0 == mapId)
        {
            mapId = InvalidMapId;
            return;
        }

        auto mapped = mapping.find(mapId);
        if (mapped == mapping.end())
        {
            u32 searchId = mapId;

            // Ex pattern
            if (searchId >= 20000)
                searchId += 200000;

            CGGraphRef graph = createGraphFromRef(searchId);

            if (!graph.isOK())
                mapId = InvalidMapId;
            else
            {
                CGOpenGLGraph gl = CreateOpenGL(graph);
                u16 index = (u16)glMap->graphLib.size();
                glMap->graphLib.push_back(gl);
                mapping[mapId] = index;
                mapId = index;
            }
        }
        else
        {
            mapId = mapped->second;
        }
    };
    // mapping all layers
    for (CGMap::SData & var : glMap->mappedData)
    {
        mappingSetter(var.L0);
        mappingSetter(var.L1);
    }

    // clean cached by createGraphFromRef()
    clearMapCache();

    return glMap;
}

CGOpenGLMapRef CGSynthetic::createMapFromCut(CGMapRef map)
{
    if (!map.isOK())
        return CGOpenGLMapRef();

    CGCutRef cut;
    cut = Lib->readCut(map->getId());

    if (!cut.isOK())
        return CGOpenGLMapRef();

    CGOpenGLMapRef glMap;
    std::map<u16, u16> mapping;

    glMap.create();

    // I'm lazy, so use c++11 as simple.
    glMap->mappedData = std::move(map->getData());
    glMap->size = map->getSize();
    // mapping lambda
    auto mappingSetter = [&](u16 & mapId)
    {
        // skip empty data
        if (0 == mapId)
        {
            mapId = InvalidMapId;
            return;
        }

        auto mapped = mapping.find(mapId);
        if (mapped == mapping.end())
        {
            u32 searchId = mapId;
            CGOpenGLGraph graph = CreateOpenGLWithCut(cut, searchId);

            if (!graph.tex.isOK())
                mapId = InvalidMapId;
            else
            {
                u16 index = (u16)glMap->graphLib.size();
                glMap->graphLib.push_back(graph);
                mapping[mapId] = index;
                mapId = index;
            }
        }
        else
        {
            mapId = mapped->second;
        }
    };
    // mapping all layers
    for (CGMap::SData & var : glMap->mappedData)
    {
        mappingSetter(var.L0);
        mappingSetter(var.L1);
    }

    // clean cached by CreateOpenGLWithCut()
    clearMapCache();

    return glMap;
}

CGGraphRef CGSynthetic::createGraphFromRef(u32 refId)
{
    auto point = MapPoints.find(refId);
    if (point == MapPoints.end())
        return CGGraphRef();

    if (point->second.graphCache.isOK())
        return point->second.graphCache;

    CGBinRef bin = Lib->getBin(point->second.graphLibIndex);
    CGGraphLibraryRef gLib = bin->getGraphLibrary();
    point->second.graphCache = gLib->readGraph(point->second.graphId);
    return point->second.graphCache;
}

void CGSynthetic::clearMapCache()
{
    auto point = MapPoints.begin();
    while (point != MapPoints.end())
    {
        point->second.graphCache.reset();
        ++point;
    }
}

#if defined(__WIN32__)
#pragma pack(push, 1)
#endif

// http://www.w3.org/Graphics/GIF/spec-gif89a.txt referenced
struct SGifGraphicControl
{
    struct
    {
        u8 transparent_color_flag : 1;
        u8 user_input_flag : 1;
        u8 disposal_method : 3;
        u8 reserved : 3;
    };
    u16 delay_time;
    u8 transparent_color_index;
};

// http://www.vurdalakov.net/misc/gif/netscape-looping-application-extension referenced
struct SGifAppNetscape20ExtBlock
{
    SGifAppNetscape20ExtBlock()
    {
        memcpy(netscape_header, "NETSCAPE2.0", sizeof(netscape_header));
    }

    c8 netscape_header[11];
};
struct SGifAppNetscape20ExtSubBlock
{
    SGifAppNetscape20ExtSubBlock()
    {
        hex1 = 0x01;
    }

    u8 hex1;
    u16 loop_times;
};

#if defined(__WIN32__)
#pragma pack(pop)
#endif

static void CGSynthetic_Do_Convert_Palette(ColorMapObject* map, CGPalette const & pal)
{
    for (int i=0; i<map->ColorCount; ++i)
    {
        CGColor c = pal.getColor((u8)i);
        map->Colors[i].Blue  = c.b;
        map->Colors[i].Green = c.g;
        map->Colors[i].Red   = c.r;
    }
}

void CGSynthetic::saveGIF(MotionIterator iter, c8 const * filename)
{
    CGGraphLibraryRef glib;
    CGPaletteRef pal;
    CGRecti screenRect;
    GifFileType* gif=0;
    ColorMapObject* gifPalette=0;
    std::vector<CGGraphRef> frames;

    // gif extensions
    SGifAppNetscape20ExtBlock nsExtBlock;
    SGifAppNetscape20ExtSubBlock nsExtSubBlock;
    SGifGraphicControl gifExtGraphCtrl;

    if (!iter.isOK())
        return;

    glib = Bin->getGraphLibrary();
    if (!glib)
        return;

    if (iter.palette)
        pal = iter.palette;
    else
        if (-1L != PaletteId)
            pal = Lib->getPalette(PaletteId).palette;

    if (!pal)
        return;

    frames.reserve(iter.data->desc.v1.frames);
    for (u32 i=0; i<iter.data->desc.v1.frames; ++i)
    {
        CGGraphRef graph = glib->readGraph(iter.data->frames[i].graphId);
        if (graph)
        {
            defGraphInfo const & desc = graph->desc;
            CGRecti frameRect = CGRecti(CGPointi(desc.offX, desc.offY), CGSizei(desc.width, desc.height));

            if (i)
                screenRect.merge(frameRect);
            else
                screenRect = frameRect;

            frames.push_back(graph);
        }
    }

    gif = ::EGifOpenFileName(filename, false, 0);
    if (!gif)
        return;

    gifPalette = ::GifMakeMapObject(256, NULL);
    if (!gifPalette)
        goto free_gif;

    CGSynthetic_Do_Convert_Palette(gifPalette, pal.get());

    // write screen description.
    if (GIF_OK != ::EGifPutScreenDesc(gif, screenRect.getWidth(), screenRect.getHeight(), 8, 0x00, gifPalette))
        goto free_gif;

    // write loop extension.
    nsExtSubBlock.loop_times = 0; // infinite times

    if (GIF_OK != ::EGifPutExtensionLeader(gif, APPLICATION_EXT_FUNC_CODE))
        goto free_gif;
    if (GIF_OK != ::EGifPutExtensionBlock(gif, sizeof(nsExtBlock), &nsExtBlock))
        goto free_gif;
    if (GIF_OK != ::EGifPutExtensionBlock(gif, sizeof(nsExtSubBlock), &nsExtSubBlock))
        goto free_gif;
    if (GIF_OK != ::EGifPutExtensionTrailer(gif))
        goto free_gif;

    // calculate delay per frame to unit 0.01 sec.
    u32 delay = iter.data->desc.v1.duration / (iter.data->desc.v1.frames * 10u);

    // check need flip x-axis
    bool flipX = (0 != (iter.data->desc.wrap & 0x1));

    // begin write frames
    gifExtGraphCtrl.transparent_color_flag = 1;
    gifExtGraphCtrl.user_input_flag = 0;
    gifExtGraphCtrl.disposal_method = 2;
    gifExtGraphCtrl.reserved = 0;
    gifExtGraphCtrl.delay_time = (u16)delay;
    gifExtGraphCtrl.transparent_color_index = 0;

    for (u32 i=0; i<frames.size(); ++i)
    {
        CGGraphRef graph = frames[i];
        bool hasPalette = graph->palette;
        defGraphInfo const & desc = graph->desc;
        CGPointi offset = screenRect.toClient(CGPointi(desc.offX, desc.offY));
        CGPixelData data(graph, hasPalette ? graph->palette : pal);

        if (hasPalette)
            CGSynthetic_Do_Convert_Palette(gifPalette, graph->palette.get());

        if (flipX)
            offset = screenRect.flipClientPos(CGRecti(offset, CGSizei(desc.width, desc.height)), true, false);

        if (GIF_OK != ::EGifPutExtension(gif, GRAPHICS_EXT_FUNC_CODE, sizeof(gifExtGraphCtrl), &gifExtGraphCtrl))
            goto free_gif;
        if (GIF_OK != ::EGifPutImageDesc(gif, offset.X, offset.Y, desc.width, desc.height, false, hasPalette ? gifPalette : 0))
            goto free_gif;

        if (flipX)
        {
            for (s32 y=0; y<desc.height; ++y)
            {
                data.InvertRowPixels(y, gif);
            }
        }
        else
        {
            for (s32 y=0; y<desc.height; ++y)
            {
                data.ConvertRowPixels(y, gif);
            }
        }
    }

free_gif:
    if (gifPalette)
        ::GifFreeMapObject(gifPalette);
    if (gif)
        ::EGifCloseFile(gif);
}

}
