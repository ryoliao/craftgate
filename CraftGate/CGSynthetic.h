
#ifndef _CG_SYNTHETIC_H_
#define _CG_SYNTHETIC_H_

namespace cg
{

const s32 MapGridWidth = 32;
const s32 MapGridHeight = 24;

struct CGOpenGLGraph
{
    CGPointi offset;
    s32 direction;
    CGOpenGLTextureRef tex;
};

struct CGOpenGLMap
{
    CGSizei size;
    std::vector<CGMap::SData> mappedData;
    std::vector<CGOpenGLGraph> graphLib;

    static CGPointi GetIndex(s32 x, s32 y);
    static CGPointi GetIndex(CGPointi pos);

    CGPointi GetPosition(s32 x, s32 y) const;
    CGOpenGLGraph GetGraph(s32 x, s32 y, u32 layer) const;
    CGSizei Last() const;

    CGPointi GetCenter() const;

    template<typename Fn>
    inline void EnumerateObjects(CGRecti rect, Fn & f);
};

typedef CGSharedRef<CGOpenGLMap> CGOpenGLMapRef;

struct MotionIterator
{
    MotionIterator()
        : data(0)
        , refId(0)
    {}

    bool isOK() const
    {
        return (0 != data);
    }

    u32 getFrame(u32 milliseconds) const
    {
        return data->getFrame(milliseconds);
    }

    u32 refId;
    CGMotion const * data;
    CGPaletteRef palette;
};

struct MapPoint
{
    u32 graphLibIndex;
    u32 graphId;
    CGGraphRef graphCache;
};

class CGSynthetic
{
public:
    CGSynthetic();
    CGSynthetic(CGBinLibraryRef lib);

    ~CGSynthetic();

    bool isOK() const;

    void reset();
    void reset(CGBinLibraryRef lib);
    
    void selectBin(u32 binId);
    void selectPalette(u32 palId);
    CGOpenGLMapRef createMap(u32 mapId);
    CGOpenGLMapRef createMapFromCut(CGMapRef map);
    CGGraphRef createGraphFromRef(u32 refId);
    void clearMapCache();

    MotionIterator findMotion(u32 animeId, u16 dir, u16 motion);
    MapPoint const * findMapPoint(u32 refId);

    void saveGIF(MotionIterator iter, c8 const * filename);

    CGOpenGLGraph CreateOpenGL(u32 graphId);
    CGOpenGLGraph CreateOpenGL(MotionIterator iterator, u32 frame);
    CGOpenGLGraph CreateOpenGL(CGGraphRef graph, s32 direction=CG_LEFT|CG_TOP);
    CGOpenGLGraph CreateOpenGLWithCut(CGCutRef cut, u32 cutId);

private:

    void doParse();
    CGPaletteRef doGetPalette(CGMotion const * mo);

    u32 PaletteId;
    CGBinRef Bin;
    CGBinLibraryRef Lib;

    // key = serial number, value = palette
    std::map<u32, CGPaletteRef> AnimePalettes;
    std::map<u32, MapPoint> MapPoints;
};

typedef CGSharedRef<CGSynthetic> CGSyntheticRef;

}

#endif //_CG_SYNTHETIC_H_