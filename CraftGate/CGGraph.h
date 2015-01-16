
#ifndef _CG_GRAPH_H_
#define _CG_GRAPH_H_

namespace cg
{

#if defined(__WIN32__)
#pragma pack(push, 1)
#endif

struct defGraphInfo
{
    s32 Id;
    u32 begFile;
    u32 szBuffer;
    s32 offX;
    s32 offY;
    s32 width;
    s32 height;
    u8 gridWidth;
    u8 gridHeight;
    u8 tag;
    u8 reserved0;
    s32 reserved1;
    s32 refId;
};

#if defined(__WIN32__)
#pragma pack(pop)
#endif

struct CGGraph
{
    defGraphInfo desc;
    CGPaletteRef palette;
    std::vector<u8> buffer;
};

typedef CGSharedRef<CGGraph> CGGraphRef;

// forward declare
class CGBin;
class CGBinLibrary;
struct defGraphTrunkInfo;
struct defGraphTrunkInfoV2;

class CGGraphLibrary
{
public:
    CGGraphLibrary();
    ~CGGraphLibrary();

    void reset();
    bool reset(CGBin* bin);
    CGGraphRef readGraph(u32 i);
    defGraphInfo const * getInfo(u32 i) const;
    u32 size() const;

#if defined(_CG_USE_DUMP)
    void dump(CGBin* bin);
#endif

    //! internal members
    IRandomStream* fGraph;
    std::vector<defGraphInfo> graphs;
};

typedef CGSharedRef<CGGraphLibrary> CGGraphLibraryRef;

}// namespace core

#endif //_CG_GRAPH_H_