
#include "CGGraph_PCH.h"

BEGIN_EVENT_TABLE(CGPropertyWindow, wxPropertyGridManager)
    EVT_PG_CHANGED(wxID_ANY, CGPropertyWindow::OnPropertyGridChange)
END_EVENT_TABLE()

CGPropertyWindow::CGPropertyWindow(wxWindow* parent, wxWindowID id)
: wxPropertyGridManager(parent, id, wxDefaultPosition, wxDefaultSize, wxPGMAN_DEFAULT_STYLE)
, target(0)
{
    AddPage();
}

CGPropertyWindow::~CGPropertyWindow()
{
    if (target)
        target->drop();
}

void CGPropertyWindow::OnPropertyGridChange(wxPropertyGridEvent& e)
{
    if (target)
        target->OnPropertyChanged(e.GetProperty());
}

void CGPropertyWindow::Reset(CGPropertyExchanging* tar)
{
    if (tar)
        tar->grab();
    if (target)
        target->drop();

    target = tar;

    Update();
}

void CGPropertyWindow::Update()
{
    ClearPage(0);
    properties.clear();

    // main category
    Append(new wxPropertyCategory("General"));

    if (target)
    {
        target->SerializeProperty(properties);
        for (size_t i=0; i<properties.size(); ++i)
        {
            Append(properties[i]);
        }
    }
}
