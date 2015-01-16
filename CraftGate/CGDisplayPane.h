
#ifndef _CG_WX_DISPLAY_PANEL_H_
#define _CG_WX_DISPLAY_PANEL_H_

#if _CG_USE_WX_BITMAP
class CGDisplayPanel : public wxPanel
{
public:
    CGDisplayPanel(wxWindow* parent=0, wxWindowID id=wxID_ANY);
    virtual ~CGDisplayPanel();

    void Reset(int frames, int duration);
    void Reset(CGBitmapGraph bitmap);
    void SetFrame(int i, CGBitmapGraph bitmap);

private:
    void OnPaint(wxPaintEvent& event);
    void OnTimerUpdate(wxTimerEvent& event);
    void OnLeftDown(wxMouseEvent& event);

    wxTimer* AnimeteTimer;

    int showframe;
    float relX;
    float relY;
    std::vector<CGBitmapGraph> bitmaps;

    DECLARE_EVENT_TABLE()
};
#endif

#endif //_CG_WX_DISPLAY_PANEL_H_