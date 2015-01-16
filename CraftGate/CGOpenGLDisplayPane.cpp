
#include "CGGraph_PCH.h"

#if _CG_USE_OPENGL

BEGIN_EVENT_TABLE(CGOpenGLDisplayPane, wxGLCanvas)
    EVT_PAINT(CGOpenGLDisplayPane::OnPaint)
    EVT_TIMER(wxCGID_TIMER_ANIME, CGOpenGLDisplayPane::OnTimerUpdate)
    EVT_LEFT_DOWN(CGOpenGLDisplayPane::OnLeftDown)
END_EVENT_TABLE()

static int _CG_OpenGL_DisplayPane_Attribs[] =
{
    WX_GL_RGBA,
    WX_GL_DOUBLEBUFFER,
    0
};

CGOpenGLDisplayPane::CGOpenGLDisplayPane(wxWindow* parent/*=0*/, wxWindowID id/*=wxID_ANY*/)
: wxGLCanvas(parent, id, _CG_OpenGL_DisplayPane_Attribs)
, relX(0.5f)
, relY(0.65f)
{
    AnimeteTimer = new wxTimer(this, wxCGID_TIMER_ANIME);
}

CGOpenGLDisplayPane::~CGOpenGLDisplayPane()
{
    AnimeteTimer->Stop();
    delete AnimeteTimer;
}

void CGOpenGLDisplayPane::Reset(int frames, int duration)
{
    showframe = 0;
    graphs.assign(frames, cg::CGOpenGLGraph());
    AnimeteTimer->Start(duration/frames);
    Refresh(false);
}

void CGOpenGLDisplayPane::Reset(cg::CGOpenGLGraph graph)
{
    showframe = 0;
    graphs.assign(1, graph);
    AnimeteTimer->Stop();
    Refresh(false);
}

void CGOpenGLDisplayPane::SetFrame(int i, cg::CGOpenGLGraph graph)
{
    if ((size_t)i < graphs.size())
    {
        graphs[i] = graph;
    }
}

void CGOpenGLDisplayPane::OnPaint(wxPaintEvent& event)
{
    wxSize szPanel = GetSize();
    cg::CGPointi pos = cg::CGPointi(szPanel.x*relX, szPanel.y*relY);

    wxGetApp().GLContext->SetCurrent(*this);
    cg::CGOpenGLGraphicEnvironment* Env = wxGetApp().GLEnv;

    Env->setViewport(cg::CGRecti(0, szPanel.x, 0, szPanel.y));
    Env->GradientFillLinear(cg::CGColor(0x20, 0x20, 0x20), cg::CGColor(0xf0, 0xf0, 0xf0), cg::CG_BOTTOM);

    if ((size_t)showframe < graphs.size() &&
        graphs[showframe].tex.isOK())
    {
        cg::CGOpenGLGraph & graph = graphs[showframe];
        Env->drawImage(graph.tex.get(), pos+graph.offset, graph.direction);
    }

    SwapBuffers();
    event.Skip();
}

void CGOpenGLDisplayPane::OnTimerUpdate(wxTimerEvent& event)
{
    showframe = (showframe + 1) % graphs.size();
    Refresh(false);
}

void CGOpenGLDisplayPane::OnLeftDown(wxMouseEvent& event)
{
    wxSize winSize = GetSize();
    wxPoint pos = event.GetPosition();

    relX = (float)pos.x / winSize.x;
    relY = (float)pos.y / winSize.y;

    Refresh(false);
}

#endif