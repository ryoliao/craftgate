
#ifndef _CG_SYNTHETIC_H_
#define _CG_SYNTHETIC_H_

#if _CG_USE_WX_BITMAP

struct CGBitmapGraph
{
    CGBitmapGraph()
    {}

    CGBitmapGraph(wxBitmap bmp, wxPoint off)
        : offset(off)
        , bitmap(bmp)
    {}

    wxPoint offset;
    wxBitmap bitmap;

    void Draw(wxDC & dc, wxPoint pos);
};

#endif

namespace cg
{

#if _CG_USE_OPENGL

struct CGOpenGLGraph
{
    CGPointi offset;
    s32 direction;
    CGOpenGLTextureRef tex;
};

#endif

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
    MotionIterator findMotion(u32 animeId, u16 dir, u16 motion);

    void saveGIF(MotionIterator iter, c8 const * filename);

    // support wxWidgets.
#if _CG_USE_WX_BITMAP

    CGBitmapGraph CreateBitmap(u32 graphId);
    CGBitmapGraph CreateBitmap(MotionIterator iterator, u32 frame);
    CGBitmapGraph CreateBitmap(CGGraphRef graph, bool flipX=false);

#endif

#if _CG_USE_OPENGL

    CGOpenGLGraph CreateOpenGL(u32 graphId);
    CGOpenGLGraph CreateOpenGL(MotionIterator iterator, u32 frame);
    CGOpenGLGraph CreateOpenGL(CGGraphRef graph, s32 direction=CG_LEFT|CG_TOP);

#endif

private:

    void doParsePalettes();
    CGPaletteRef doGetPalette(CGMotion const * mo);

    u32 PaletteId;
    CGBinRef Bin;
    CGBinLibraryRef Lib;

    // key = serial number, value = palette
    std::map<u32, CGPaletteRef> AnimePalettes;
};

typedef CGSharedRef<CGSynthetic> CGSyntheticRef;

}

#endif //_CG_SYNTHETIC_H_