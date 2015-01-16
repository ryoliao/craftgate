
#ifndef _CG_WX_OPENGL_DISPLAY_PANE_H_
#define _CG_WX_OPENGL_DISPLAY_PANE_H_

#if _CG_USE_OPENGL

class CGOpenGLDisplayPane : public wxGLCanvas
{
public:
    CGOpenGLDisplayPane(wxWindow* parent=0, wxWindowID id=wxID_ANY);
    ~CGOpenGLDisplayPane();

    void Reset(int frames, int duration);
    void Reset(cg::CGOpenGLGraph graph);
    void SetFrame(int i, cg::CGOpenGLGraph graph);

private:
    void OnPaint(wxPaintEvent& event);
    void OnTimerUpdate(wxTimerEvent& event);
    void OnLeftDown(wxMouseEvent& event);

    wxTimer* AnimeteTimer;

    int showframe;
    float relX;
    float relY;
    std::vector<cg::CGOpenGLGraph> graphs;

    DECLARE_EVENT_TABLE()
};

#endif

#endif //_CG_WX_OPENGL_DISPLAY_PANE_H_