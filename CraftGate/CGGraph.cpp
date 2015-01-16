
#include "CGGraph_PCH.h"
#include "CGGraph.h"

namespace cg
{

enum E_CG_GRAPH_FLAGS
{
    ECGF_FORMAT_BIT  = 0x1,
    ECGF_PALETTE_BIT = 0x2,
    ECGF_RAW         = 0x0,
    ECGF_RLE         = 0x1,
};

#if defined(__WIN32__)
#pragma pack(push, 1)
#endif

struct defGraphHeader
{
    c8 trunkId[2];
    u8 flags;
    u8 mask;
};

struct defGraphTrunkInfo
{
    s32 width;
    s32 height;
    u32 szTrunk;
};

struct defGraphTrunkInfoV2
{
    defGraphTrunkInfo v1;
    u32 szPalette;
};

#if defined(__WIN32__)
#pragma pack(pop)
#endif

///////////////////////////////////////////////////////////////////////////////

CGGraphLibrary::CGGraphLibrary()
    : fGraph(0)
{
}

CGGraphLibrary::~CGGraphLibrary()
{
    reset();
}

bool CGGraphLibrary::reset(CGBin* bin)
{
    reset();

    if (!bin ||
        !bin->isOK(ECBC_GRAPHIC) ||
        !bin->isOK(ECBC_GRAPHICINFO))
        return false;

    IRandomStream* fInfo;

    fGraph = bin->getStream(ECBC_GRAPHIC);
    fInfo = bin->getStream(ECBC_GRAPHICINFO);

    defGraphInfo gi;
    graphs.reserve(fInfo->size() / sizeof(defGraphInfo));
    while (1 == fInfo->read(&gi))
        graphs.push_back(gi);

    fInfo->drop();

    return true;
}

void CGGraphLibrary::reset()
{
    graphs.clear();
    if (fGraph)
    {
        fGraph->drop();
        fGraph = 0;
    }
}

static inline bool _CGGraphLibrary_DoReadGraphBuffer(IStream* stream, CGGraphRef readyToReadGraph, u32 szPalette)
{
    defGraphInfo const & desc = readyToReadGraph->desc;
    std::vector<u8> & buffer = readyToReadGraph->buffer;
    u32 szBuffer = desc.width * desc.height;

    buffer.resize(szBuffer);
    for (s32 row = desc.height-1; row >= 0; --row)
    {
        if (desc.width != stream->read(buffer.data() + (row*desc.width), desc.width))
            return false;
    }
    if (szPalette)
    {
        readyToReadGraph->palette.create();
        readyToReadGraph->palette->readFromGraphV2(stream, szPalette);
    }
    return true;
}

CGGraphRef CGGraphLibrary::readGraph(u32 i)
{
    CGGraphRef graph;
    defGraphHeader h;
    defGraphTrunkInfoV2 tdesc;
    defGraphInfo const * desc;
    u32 szGraph;
    bool succeed;

    desc = getInfo(i);
    if (!desc) return graph;

    // limited graph size
    if (desc->width  < 0 || desc->width  > 1024 ||
        desc->height < 0 || desc->height > 1024)
        return graph;

    szGraph = desc->width * desc->height;

    // seek address
    if (0 != fGraph->seek(desc->begFile, SEEK_SET)) return graph;

    // read header
    if (1 != fGraph->read(&h)) return graph;

    // check trunk id
    if (strncmp(h.trunkId, "RD", sizeof(h.trunkId)))
        return graph;

    // read trunk desc
    if (0 != (h.flags & ECGF_PALETTE_BIT))
    {
        if (1 != fGraph->read(&tdesc))
            return graph;
    }
    else
    {
        tdesc.szPalette = 0;
        if (1 != fGraph->read(&tdesc.v1))
            return graph;
    }

    // read graph data
    graph.create();
    graph->desc = *desc;
    if (ECGF_RLE == (h.flags & ECGF_FORMAT_BIT))
    {
        CGRLEDecoder RLE(fGraph);
        succeed = _CGGraphLibrary_DoReadGraphBuffer(&RLE, graph, tdesc.szPalette);
    }
    else
    {
        succeed = _CGGraphLibrary_DoReadGraphBuffer(fGraph, graph, tdesc.szPalette);
    }

    if (!succeed)
        graph.reset();

    return graph;
}

defGraphInfo const * CGGraphLibrary::getInfo(u32 i) const
{
    return &(graphs[i]);
}

u32 CGGraphLibrary::size() const
{
    return graphs.size();
}

#if defined(_CG_USE_DUMP)
void CGGraphLibrary::dump(CGBin* bin)
{
    if (!bin ||
        !bin->isOK(ECBC_GRAPHIC) ||
        !bin->isOK(ECBC_GRAPHICINFO))
        return;

    std::string dbgFilename;
    IRandomStream* fInfo;
    IRandomStream* fGraph;
    FILE* fDbg;

    fGraph = bin->getStream(ECBC_GRAPHIC);
    fInfo = bin->getStream(ECBC_GRAPHICINFO);
    dbgFilename = bin->getFilePath(ECBC_GRAPHICINFO);
    dbgFilename.append(".txt");
    fDbg = fopen(dbgFilename.c_str(), "w");
    if (fDbg)
    {
        fprintf(fDbg, "%8s %8s %8s %4s %4s %3s %8s|%3s %3s %3s %8s %4s|%3s\n",
            "id", "addr", "sz", "w", "h", "tag", "ref", "ver", "zip", "rot", "szt", "pal", "szd");
        defGraphInfo gi;
        while (1 == fInfo->read(&gi))
        {
            fprintf(fDbg, "%8d", gi.Id);
            fprintf(fDbg, " %8x", gi.begFile);
            fprintf(fDbg, " %8x", gi.szBuffer);
            fprintf(fDbg, " %4d", gi.width);
            fprintf(fDbg, " %4d", gi.height);
            fprintf(fDbg, " %3x", (u32)gi.tag);
            fprintf(fDbg, " %8d", (u32)gi.refId);

            if (!fGraph->seek(gi.begFile, SEEK_SET))
            {
                defGraphHeader h;
                if (1 == fGraph->read(&h))
                {
                    s32 haspal = h.flags & ECGF_PALETTE_BIT;
                    s32 format = h.flags & ECGF_FORMAT_BIT;
                    
                    defGraphTrunkInfoV2 ti;
                    bool taken=false;
                    if (!haspal)
                    {
                        if (1 == fGraph->read(&ti.v1))
                            taken=true;
                    }
                    else
                    {
                        if (1 == fGraph->read(&ti))
                            taken=true;
                    }
                    if (taken)
                    {
                        fprintf(fDbg, "|%3x", haspal >> 1);
                        fprintf(fDbg, " %3x", format);
                        fprintf(fDbg, " %3d", (s32)h.mask);
                        fprintf(fDbg, " %8x", (s32)ti.v1.szTrunk);
                        if (haspal)
                        fprintf(fDbg, " %4x", (s32)ti.szPalette);
                        else
                        fprintf(fDbg, " %4s", "n/a");
                        fprintf(fDbg, "|%3s", (gi.szBuffer != ti.v1.szTrunk) ? "x" : "");
                    }
                }
            }
            fprintf(fDbg, "\n");
        }
        fclose(fDbg);
    }
    fInfo->drop();
    fGraph->drop();
}

#endif

} // namespace core
