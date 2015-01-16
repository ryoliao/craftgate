
#include "CGGraph_PCH.h"

using namespace cg;

static wxListItemAttr _attrError  (wxColour(255,0,0),    wxNullColour, wxNullFont);
static wxListItemAttr _attrWarning(wxColour(255,127,39), wxNullColour, wxNullFont);
static wxListItemAttr _attrMessage(wxColour(0,0,255),    wxNullColour, wxNullFont);

CGEventList::CGEventList(wxWindow* parent/*=0*/, wxWindowID id/*=wxID_ANY*/)
    : wxPanel(parent, id)
{
    DoInitControls();
}

void CGEventList::DoInitControls()
{
    wxBoxSizer* szr = new wxBoxSizer(wxVERTICAL);
    // Tools
    {
        wxBoxSizer* toolSzr = new wxBoxSizer(wxHORIZONTAL);
        wxBitmapButton* clearBtn = new wxBitmapButton(this, wxCGID_ERRTOOL_CLEAN,
            wxIcon("media\\bin.ico", wxBITMAP_TYPE_ICO, 16, 16), wxDefaultPosition, wxSize(24, 24));
        clearBtn->SetToolTip(L"清除所有列表");
        toolSzr->Add(clearBtn, wxSizerFlags().Left());
        szr->Add(toolSzr, 0, wxTOP|wxLEFT|wxBOTTOM, 2);
    }
    // List
    {
        EvtList = new CGEventListCtrl(this, wxCGID_ERRLIST);
        szr->Add(EvtList, 1, wxEXPAND, 10);
    }
    szr->SetSizeHints(this);
    SetSizer(szr);
}

CGEventList::~CGEventList()
{
    // do nothing here.
}

//////////////////////////////////////////////////////////////////////////////
CGEventListCtrl::CGEventListCtrl( wxWindow* parent/*=0*/, wxWindowID id/*=wxID_ANY*/ )
    : wxListCtrl(parent, id, wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxLC_HRULES)
    , imgList(16, 16, true, 3)
{
    InsertColumn(0, "", wxLIST_FORMAT_CENTER, 24);
    InsertColumn(1, L"描述", wxLIST_FORMAT_LEFT, 400);

    imgList.Add(wxIcon("media\\delete.ico", wxBITMAP_TYPE_ICO, 16, 16));
    imgList.Add(wxIcon("media\\warning.ico", wxBITMAP_TYPE_ICO, 16, 16));
    imgList.Add(wxIcon("media\\info.ico", wxBITMAP_TYPE_ICO, 16, 16));
    SetImageList(&imgList, wxIMAGE_LIST_SMALL);

    SEvent e;
    e.EventType = ECET_WARNING;
    e.Message = "just a test.";
    OnEvent(e);
}

CGEventListCtrl::~CGEventListCtrl()
{
    // do nothing here.
}

void CGEventListCtrl::ClearAllEvents()
{
    SetItemCount(0);
    evtdata.clear();
}

void CGEventListCtrl::OnEvent(cg::SEvent & e)
{
    int item = evtdata.size();
    evtdata.push_back(e);   
    SetItemCount(evtdata.size());
    Update();
}

wxString CGEventListCtrl::OnGetItemText(long item, long column) const
{
    SEvent const & e = evtdata[item];
    switch (column)
    {
    case 1:
        switch (e.EventType)
        {
        case ECET_ERROR:
            return wxString::Format("error: %s", e.Message.c_str());
        case ECET_WARNING:
            return wxString::Format("warning: %s", e.Message.c_str());
        case ECET_MESSAGE:
            return wxString::Format("message: %s", e.Message.c_str());
        }
        break;
    }
    return wxString();
}

int CGEventListCtrl::OnGetItemImage(long item) const
{
    SEvent const & e = evtdata[item];
    switch (e.EventType)
    {
    case ECET_ERROR:
        return 0;
    case ECET_WARNING:
        return 1;
    case ECET_MESSAGE:
        return 2;
    }
    return -1L;
}

wxListItemAttr * CGEventListCtrl::OnGetItemAttr(long item) const
{
    SEvent const & e = evtdata[item];
    switch (e.EventType)
    {
    case ECET_ERROR:
        return &_attrError;
    case ECET_WARNING:
        return &_attrWarning;
    case ECET_MESSAGE:
        return &_attrMessage;
    default:
        return NULL;
    }
}
