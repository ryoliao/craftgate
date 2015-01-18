
#include "CGGraph_PCH.h"
#include "CGCommon2D_Math.hpp"
#include "CGSynthetic_CGOpenGLMap.hpp"

BEGIN_EVENT_TABLE(CGOpenGLDisplayPane, wxGLCanvas)
    EVT_PAINT(CGOpenGLDisplayPane::OnPaint)
    EVT_TIMER(wxCGID_TIMER_ANIME, CGOpenGLDisplayPane::OnTimerUpdate)
    EVT_MOUSE_EVENTS(CGOpenGLDisplayPane::OnMouseEvent)
END_EVENT_TABLE()

static int _CG_OpenGL_DisplayPane_Attribs[] =
{
    WX_GL_RGBA,
    WX_GL_DOUBLEBUFFER,
    0
};

static cg::CGPointf DefaultGraphPosition(0.5f, 0.65f);

CGOpenGLDisplayPane::CGOpenGLDisplayPane(wxWindow* parent/*=0*/, wxWindowID id/*=wxID_ANY*/)
: wxGLCanvas(parent, id, _CG_OpenGL_DisplayPane_Attribs)
, Position(DefaultGraphPosition)
, mode(DISPLAY_GRAPH)
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
    mode = DISPLAY_GRAPH;
    showframe = 0;
    graphs.assign(frames, cg::CGOpenGLGraph());
    AnimeteTimer->Start(duration/frames);
    Refresh(false);
}

void CGOpenGLDisplayPane::Reset(cg::CGOpenGLGraph graph)
{
    mode = DISPLAY_GRAPH;
    showframe = 0;
    graphs.assign(1, graph);
    AnimeteTimer->Stop();
    Refresh(false);
}

void CGOpenGLDisplayPane::Reset(cg::CGOpenGLMapRef map)
{
    mode = DISPLAY_MAP;
    showframe = 0;
    graphs.clear();
    AnimeteTimer->Stop();

    Map = map;

    if (Map.isOK())
        MapPosition = Map->GetCenter();

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
    using namespace cg;
    wxSize paneSize = GetSize();
    wxGetApp().GLContext->SetCurrent(*this);
    CGOpenGLGraphicEnvironment* Env = wxGetApp().GLEnv;
    Env->setViewport(CGRecti(0, paneSize.x, 0, paneSize.y));

    if (mode == DISPLAY_GRAPH)
    {
        Env->GradientFillLinear(CGColor(0x20, 0x20, 0x20), CGColor(0xf0, 0xf0, 0xf0), CG_BOTTOM);
        if ((size_t)showframe < graphs.size() &&
            graphs[showframe].tex.isOK())
        {
            CGOpenGLGraph & graph = graphs[showframe];
            CGPointi pos = CGPointi(paneSize.x * Position.X, paneSize.y * Position.Y);
            Env->drawImage(graph.tex.get(), pos + graph.offset, graph.direction);
        }
    }
    else if (mode == DISPLAY_MAP)
    {
        Env->FillColor(CGColor(0, 0, 0));
        if (Map.isOK())
        {
            CGPointi index = Map->GetIndex(MapPosition);
            auto drawer = [=](CGOpenGLTexture & tex, CGPointi pt)
            {
                auto pos = pt - MapPosition;
                Env->drawImage(tex, pos);
            };
            Map->EnumerateObjects(CGRecti(MapPosition, CGSizei(paneSize.x, paneSize.y)), drawer);
        }
    }
    SwapBuffers();
    event.Skip();
}

void CGOpenGLDisplayPane::OnTimerUpdate(wxTimerEvent& event)
{
    showframe = (showframe + 1) % graphs.size();
    Refresh(false);
}

void CGOpenGLDisplayPane::OnMouseEvent(wxMouseEvent& event)
{
    wxPoint pos = event.GetPosition();

    if (mode == DISPLAY_GRAPH)
    {
        if (event.LeftDown())
        {
            wxSize winSize = GetSize();
            Position.X = (float)pos.x / winSize.x;
            Position.Y = (float)pos.y / winSize.y;
            Refresh(false);
        }
        if (event.RightDown())
        {
            if (wxGetApp().DisplayData.mode == CGGraphViewerApp::DISPLAY_ANIME)
            {
                wxSize winSize = GetSize();
                cg::CGPointf point, dir;

                point.X = (float)pos.x / winSize.x;
                point.Y = (float)pos.y / winSize.y;

                long dir_case = cg::CGAngleToMotionDirection(point - Position);
                wxGetApp().AnimeProvider->UpdateDirection(dir_case);
            }
        }
    }
    else if (mode == DISPLAY_MAP)
    {
        if (event.LeftDown())
        {
            lastMouse.X = event.GetX();
            lastMouse.Y = event.GetY();
        }
        if (event.Dragging())
        {
            cg::CGPointi cur(event.GetX(), event.GetY());
            MapPosition = MapPosition - (cur - lastMouse);
            lastMouse = cur;
            Refresh(false);
        }
    }
}
