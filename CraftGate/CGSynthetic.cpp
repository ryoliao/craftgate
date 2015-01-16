
#include "CGGraph_PCH.h"
#include <gif_lib.h>

#if _CG_USE_WX_BITMAP

void CGBitmapGraph::Draw(wxDC & dc, wxPoint pos)
{
    if (bitmap.IsOk())
    {
        pos += offset;
        dc.DrawBitmap(bitmap, pos);
    }
}

#endif

namespace cg
{

class CGPixelData
{
public:
    CGPixelData(CGGraphRef graph, CGPaletteRef pal)
        : ptr(graph->buffer.data())
        , pitch(graph->desc.width)
        , palette(pal)
    {}

#if _CG_USE_WX_BITMAP

    inline void ConvertRowPixels(long row, wxAlphaPixelData::Iterator p)
    {
        u8* sp = ptr + (row*pitch);
        for (u32 i=0; i<pitch; ++i)
        {
            p.Data() = palette->getARGB(*sp);
            ++p; ++sp;
        }
    }

    inline void InvertRowPixels(long row, wxAlphaPixelData::Iterator p)
    {
        u8* sp = ptr + ((row+1)*pitch);
        for (u32 i=0; i<pitch; ++i)
        {
            --sp;
            p.Data() = palette->getARGB(*sp);
            ++p;
        }
    }

#endif

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
}

void CGSynthetic::reset(CGBinLibraryRef lib)
{
    reset();

    Lib = lib;
    selectPalette(0);
    doParsePalettes();
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

void CGSynthetic::doParsePalettes()
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

    // build animation palette map
    for (u32 iBin=0; iBin < Lib->size(); ++iBin)
    {
        bin = Lib->getBin(iBin);
        glib = bin->getGraphLibrary();
        if (glib)
        {
            for (u32 iGraph=0; iGraph < glib->size(); ++iGraph)
            {
                gdesc = glib->getInfo(iGraph);
                if (ECRID_INVALID != gdesc->refId &&
                    ECRID_ANIMATION == refIdType(gdesc->refId))
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
            }
        }
    }
}

#if _CG_USE_WX_BITMAP

CGBitmapGraph CGSynthetic::CreateBitmap(u32 graphId)
{
    CGGraphLibraryRef glib = Bin->getGraphLibrary();
    if (glib)
    {
        return CreateBitmap(glib->readGraph(graphId));
    }
    return CGBitmapGraph();
}

CGBitmapGraph CGSynthetic::CreateBitmap(CGGraphRef graph, bool flipX)
{
    CGBitmapGraph wxGraph;
    if (graph)
    {
        defGraphInfo desc = graph->desc;
        CGPaletteRef pal;

        if (graph->palette)
            pal = graph->palette;
        else
        if (-1L != PaletteId)
            pal = Lib->getPalette(PaletteId).palette;
        
        if (!pal)
            return wxGraph;

        wxGraph.bitmap.Create(desc.width, desc.height, 32);
        wxGraph.bitmap.UseAlpha();

        u8* ip = graph->buffer.data();
        wxAlphaPixelData data(wxGraph.bitmap);
        if (data)
        {
            CGColor clr;
            wxAlphaPixelData::Iterator p(data), py;
            CGPixelData sdata(graph, pal);
            if (flipX)
            {
                for (long y=0; y<desc.height; ++y)
                {
                    py = p;
                    py.OffsetY(data, y);
                    sdata.InvertRowPixels(y, py);
                }
                wxGraph.offset = wxPoint(-(desc.width + desc.offX), desc.offY);
            }
            else
            {
                for (long y=0; y<desc.height; ++y)
                {
                    py = p;
                    py.OffsetY(data, y);
                    sdata.ConvertRowPixels(y, py);
                }
                wxGraph.offset = wxPoint(desc.offX, desc.offY);
            }
        }
    }
    return wxGraph;
}

CGBitmapGraph CGSynthetic::CreateBitmap(MotionIterator iterator, u32 frame)
{
    CGGraphLibraryRef glib = Bin->getGraphLibrary();
    if (glib)
    {
        u32 graphId = iterator.data->frames[frame].graphId;

        CGGraphRef graph = glib->readGraph(graphId);

        if (graph && iterator.palette)
            graph->palette = iterator.palette;

        bool flipX = (0 != (iterator.data->desc.wrap & 0x1));
        return CreateBitmap(graph, flipX);
    }
    return CGBitmapGraph();
}

#endif

#if _CG_USE_OPENGL

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

#endif 

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
