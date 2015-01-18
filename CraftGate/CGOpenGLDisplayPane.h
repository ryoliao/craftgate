
#ifndef _CG_WX_OPENGL_DISPLAY_PANE_H_
#define _CG_WX_OPENGL_DISPLAY_PANE_H_

class CGOpenGLDisplayPane : public wxGLCanvas
{
public:
    enum E_DISPLAY_MODE
    {
        DISPLAY_GRAPH,
        DISPLAY_MAP,
    };

    CGOpenGLDisplayPane(wxWindow* parent=0, wxWindowID id=wxID_ANY);
    ~CGOpenGLDisplayPane();

    void Reset(cg::CGOpenGLMapRef map);
    void Reset(int frames, int duration);
    void Reset(cg::CGOpenGLGraph graph);
    void SetFrame(int i, cg::CGOpenGLGraph graph);

private:
    void OnPaint(wxPaintEvent& event);
    void OnTimerUpdate(wxTimerEvent& event);
    void OnMouseEvent(wxMouseEvent& event);

    wxTimer* AnimeteTimer;

    E_DISPLAY_MODE mode;
    int showframe;
    cg::CGPointi lastMouse;
    cg::CGPointf Position;
    cg::CGPointi MapPosition;
    std::vector<cg::CGOpenGLGraph> graphs;
    cg::CGOpenGLMapRef Map;

    DECLARE_EVENT_TABLE()
};

#endif //_CG_WX_OPENGL_DISPLAY_PANE_H_