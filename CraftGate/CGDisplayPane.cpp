
#include "CGGraph_PCH.h"

BEGIN_EVENT_TABLE(CGDisplayPanel, wxPanel)
    EVT_PAINT(CGDisplayPanel::OnPaint)
    EVT_TIMER(wxCGID_TIMER_ANIME, CGDisplayPanel::OnTimerUpdate)
    EVT_LEFT_DOWN(CGDisplayPanel::OnLeftDown)
END_EVENT_TABLE()

CGDisplayPanel::CGDisplayPanel( wxWindow* parent/*=0*/, wxWindowID id/*=wxID_ANY*/ )
: wxPanel(parent, id)
, relX(0.5f), relY(0.65f)
, showframe(0)
{
    SetDoubleBuffered(true);
    AnimeteTimer = new wxTimer(this, wxCGID_TIMER_ANIME);
}

CGDisplayPanel::~CGDisplayPanel()
{
    AnimeteTimer->Stop();
    delete AnimeteTimer;
}

void CGDisplayPanel::Reset(int frames, int duration)
{
    showframe = 0;
    bitmaps.assign(frames, CGBitmapGraph());
    AnimeteTimer->Start(duration/frames);
    Refresh(false);
}

void CGDisplayPanel::Reset( CGBitmapGraph bitmap )
{
    showframe = 0;
    bitmaps.assign(1, bitmap);
    AnimeteTimer->Stop();
    Refresh(false);
}

void CGDisplayPanel::SetFrame(int i, CGBitmapGraph bitmap)
{
    if ((size_t)i < bitmaps.size())
    {
        bitmaps[i] = bitmap;
    }
}

void CGDisplayPanel::OnPaint(wxPaintEvent& event)
{
    wxSize szPanel = GetSize();
    wxPoint pos = wxPoint(szPanel.x*relX, szPanel.y*relY);

    wxPaintDC dc(this);
    dc.GradientFillLinear(szPanel, wxColour(0x20, 0x20, 0x20), wxColour(0xf0, 0xf0, 0xf0), wxUP);
    if ((size_t)showframe < bitmaps.size())
        bitmaps[showframe].Draw(dc, pos);
}

void CGDisplayPanel::OnTimerUpdate(wxTimerEvent& event)
{
    showframe = (showframe + 1) % bitmaps.size();
    Refresh(false);
}

void CGDisplayPanel::OnLeftDown(wxMouseEvent& event)
{
    wxSize winSize = GetSize();
    wxPoint pos = event.GetPosition();

    relX = (float)pos.x / winSize.x;
    relY = (float)pos.y / winSize.y;

    Refresh(false);
}
