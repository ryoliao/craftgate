
#ifndef _CG_WX_GRAPH_VIEWER_H_
#define _CG_WX_GRAPH_VIEWER_H_

class CGGraphViewer;
class CGGraphList;
class CGListCtrl;
class CGListProvider;
class CGGraphListProvider;
class CGAnimeListProvider;
class CGGraphPane;
class CGDisplayPanel;
class CGOpenGLDisplayPane;
class CGEventList;
class CGPropertyWindow;
class CGManagerWindow;

#ifdef _CG_USE_OPENGL

typedef CGOpenGLDisplayPane CGDisplayPane;
typedef cg::CGOpenGLGraph CGDisplayGraph;

#else

typedef CGDisplayPanel CGDisplayPane;
typedef CGBitmapGraph CGDisplayGraph;

#endif

class CGGraphViewerApp : public wxApp
{
public:
    virtual bool OnInit();
    virtual int OnExit();

    void ActiveBin(long binId, long listType=-1L);
    void ActivePalette(long paletteId);
    void Open(wxString const & folder);
    void Export(wxString const & filename);
    bool CanExport();
    void DisplayAnime(long item=-1, long dir=0, long motion=0, bool prop=true);
    void DisplayGraph(long item=-1, bool prop=true);
    void RefreshDisplay();

    enum 
    {
        DISPLAY_NONE=0,
        DISPLAY_ANIME,
        DISPLAY_GRAPH,
    };

    CGGraphViewer* MainFrame;
    cg::CGBinLibraryRef BinLibrary;
    cg::CGSyntheticRef Synthetic;
    CGGraphListProvider* GraphProvider;
    CGAnimeListProvider* AnimeProvider;

#if _CG_USE_OPENGL
    wxGLContext* GLContext;
    cg::CGOpenGLGraphicEnvironment* GLEnv;
#endif

    struct
    {
        long mode;
        long item;
        // for anime display data
        long dir;
        long motion;
    } DisplayData;
};

DECLARE_APP(CGGraphViewerApp);

class CGGraphViewer : public wxFrame
{
public:
    CGGraphViewer();
    virtual ~CGGraphViewer();

    // Initializer
    void CreateMenuBar();

    // Common event
    void OnPaint(wxPaintEvent& event);

    // Menu Event
    void OnOpen(wxCommandEvent & e);
    void OnExport(wxCommandEvent & e);
    void OnAbout(wxCommandEvent & e);
    void OnExit(wxCommandEvent & e);

    // setup
    void setupAui();

    //CGEventList* Error;
    CGManagerWindow* Manager;
    CGListCtrl* DisplayList;
    CGPropertyWindow* Property;
    CGDisplayPane* Display;

private:
    wxAuiManager Aui;

    DECLARE_EVENT_TABLE()
};

#endif //_CG_WX_GRAPH_VIEWER_H_