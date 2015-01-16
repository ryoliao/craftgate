
#include "CGGraph_PCH.h"

BEGIN_EVENT_TABLE(CGListCtrl, wxListCtrl)
EVT_LIST_ITEM_SELECTED(wxID_ANY, CGListCtrl::OnItemSelected)
END_EVENT_TABLE()

CGListCtrl::CGListCtrl(wxWindow* parent/*=0*/, wxWindowID id/*=wxID_ANY*/)
    : wxListCtrl(parent, id, wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxLC_HRULES)
    , provider(0)
{

}

CGListCtrl::~CGListCtrl()
{
    if (provider)
        provider->drop();
}

void CGListCtrl::Reset(CGListProvider* ptr)
{
    if (ptr)
        ptr->grab();
    if (provider)
        provider->drop();

    provider = ptr;

    ClearAll();
    if (provider)
    {
        CGListDescription && desc = provider->OnGetItemDescription();
        for (size_t i=0; desc.columnNames[i]; ++i)
        {
            InsertColumn(i, desc.columnNames[i]);
        }
        SetItemCount(desc.szItem);
    }
    Refresh();
}

wxString CGListCtrl::OnGetItemText(long item, long column) const
{
    if (provider)
        return provider->OnGetItemText(item, column);

    return wxString();
}

void CGListCtrl::OnItemSelected(wxListEvent & e)
{
    if (provider)
        provider->OnItemSelected(e.GetIndex());
}

///////////////////////////////////////////////////////////////////////////////

CGGraphListProvider::CGGraphListProvider()
    : selection(-1L)
{

}

void CGGraphListProvider::Reset(cg::CGGraphLibraryRef lib)
{
    selection = -1L;
    Graphs = lib;
}

CGListDescription CGGraphListProvider::OnGetItemDescription() const
{
    static char const * const GraphColumnNames[] =
    {
        "ID",
        "Address",
        "Bytes",
        "Size",
        "Offset",
        0x0
    };

    CGListDescription desc;

    desc.szItem = Graphs->size();
    desc.columnNames = GraphColumnNames;

    return desc;
}

wxString CGGraphListProvider::OnGetItemText(long item, long column) const
{
    if (!Graphs ||
        (cg::u32)item >= Graphs->size())
        return wxString();

    cg::defGraphInfo const * gi = Graphs->getInfo(item);
    if (!gi)
        return wxString();

    switch (column)
    {
    case 0:
        return wxString::Format("%u", gi->Id);
    case 1:
        return wxString::Format("0x%08X", gi->begFile);
    case 2:
        return wxString::Format("%u", gi->szBuffer);
    case 3:
        return wxString::Format("%u,%u", gi->width, gi->height);
    case 4:
        return wxString::Format("%d,%d", gi->offX, gi->offY);
    default:
        return wxString();
    }
}

void CGGraphListProvider::OnItemSelected(long item)
{
    selection = item;
    wxGetApp().DisplayGraph(item);
}

void CGGraphListProvider::SerializeProperty(std::vector<wxPGProperty*> & out)
{
    wxPGProperty* prop;

    if (selection != -1L)
    {
        cg::defGraphInfo const * desc = Graphs->getInfo(selection);

        prop = new wxIntProperty("ID", wxPG_LABEL,
            desc->Id);
        prop->ChangeFlag(wxPG_PROP_READONLY, true);
        out.push_back(prop);

        prop = new wxStringProperty("Size", wxPG_LABEL,
            wxString::Format("(%u,%u)", desc->width, desc->height));
        prop->ChangeFlag(wxPG_PROP_READONLY, true);
        out.push_back(prop);

        prop = new wxStringProperty("Offset", wxPG_LABEL,
            wxString::Format("(%d,%d)", desc->offX, desc->offY));
        prop->ChangeFlag(wxPG_PROP_READONLY, true);
        out.push_back(prop);

        prop = new wxIntProperty("Reference", wxPG_LABEL, desc->refId);
        prop->ChangeFlag(wxPG_PROP_READONLY, true);
        out.push_back(prop);
    }
}

void CGGraphListProvider::OnPropertyChanged(wxPGProperty* value)
{
}

///////////////////////////////////////////////////////////////////////////////

CGAnimeListProvider::CGAnimeListProvider()
    : selection(-1L), direction(0), motion(0)

{

}

void CGAnimeListProvider::Reset(cg::CGAnimeLibraryRef lib)
{
    // reset selection data
    selection = -1L;
    direction = 0;
    motion = 0;
    Animes = lib;
}

CGListDescription CGAnimeListProvider::OnGetItemDescription() const
{
    static char const * const AnimeColumnNames[] =
    {
        "ID",
        "Address",
        "Motions",
        0x0
    };

    CGListDescription desc;

    desc.szItem = Animes ? Animes->size() : 0;
    desc.columnNames = AnimeColumnNames;

    return desc;
}

wxString CGAnimeListProvider::OnGetItemText(long item, long column) const
{
    if (!Animes ||
        (cg::u32)item >= Animes->size())
        return wxString();

    cg::CGAnime const * anim = Animes->getAnime(item);
    if (!anim)
        return wxString();

    cg::defAnimeInfo desc = anim->desc;

    switch (column)
    {
    case 0:
        return wxString::Format("%u", desc.Id);
    case 1:
        return wxString::Format("0x%08X", desc.address);
    case 2:
        return wxString::Format("%u", desc.motions);
    default:
        return wxString();
    }
}

void CGAnimeListProvider::OnItemSelected(long item)
{
    selection = item;
    cg::CGAnime const * anim = Animes->getAnime(selection);
    if (anim->desc.motions)
    {
        cg::defAnimeV2 desc = anim->motions[0].desc;
        direction = desc.v1.dir;
        motion = desc.v1.motion;
    }
    wxGetApp().DisplayAnime(selection, direction, motion);
}

void CGAnimeListProvider::SerializeProperty(std::vector<wxPGProperty*> & out)
{
    wxPGProperty* prop;

    if (selection != -1L)
    {
        cg::CGAnime const * anim = Animes->getAnime(selection);

        prop = new wxIntProperty("ID", wxPG_LABEL,
            anim->desc.Id);
        prop->ChangeFlag(wxPG_PROP_READONLY, true);
        out.push_back(prop);

        prop = new wxEnumProperty("Motion", wxPG_LABEL,
            cg::MotionPoseNames, 0, motion);
        out.push_back(prop);

        prop = new wxEnumProperty("Direction", wxPG_LABEL,
            cg::MotionDirectionNames, 0, direction);
        out.push_back(prop);
    }
}

void CGAnimeListProvider::OnPropertyChanged(wxPGProperty* value)
{
    wxString name = value->GetName();
    if (name.IsSameAs("Motion"))
    {
        motion = value->GetValue().GetLong();
        wxGetApp().DisplayAnime(selection, direction, motion, false);
    }
    else
    if (name.IsSameAs("Direction"))
    {
        direction = value->GetValue().GetLong();
        wxGetApp().DisplayAnime(selection, direction, motion, false);
    }
}
