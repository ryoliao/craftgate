
#include "CGGraph_PCH.h"

BEGIN_EVENT_TABLE(CGManagerWindow, wxPanel)
    EVT_TREE_SEL_CHANGED(wxCGID_MANAGER_TREE, CGManagerWindow::OnItemSelected)
END_EVENT_TABLE()

class CGManagerItem : public wxTreeItemData
{
public:
    virtual void Activate()=0;
};

class CGManagerMapItem : public CGManagerItem
{
public:
    CGManagerMapItem(int id)
        : bindId(id)
    {}

    virtual void Activate() override
    {
        wxGetApp().ActiveMap(bindId);
    };

private:
    int bindId;
};

class CGManagerGraphItem : public CGManagerItem
{
public:
    CGManagerGraphItem(int id)
        : binId(id)
    {}

    virtual void Activate()
    {
        wxGetApp().ActiveBin(binId, CGGraphViewerApp::DISPLAY_GRAPH);
    }

private:
    int binId;
};

class CGManagerAnimeItem : public CGManagerItem
{
public:
    CGManagerAnimeItem(int id)
        : binId(id)
    {}

    virtual void Activate()
    {
        wxGetApp().ActiveBin(binId, CGGraphViewerApp::DISPLAY_ANIME);
    }

private:
    int binId;
};

class CGManagerPaletteItem : public CGManagerItem
{
public:
    CGManagerPaletteItem(int id)
        : paletteId(id)
    {}

    virtual void Activate()
    {
        wxGetApp().ActivePalette(paletteId);
    }

private:
    int paletteId;
};

CGManagerWindow::CGManagerWindow(wxWindow* parent/*=0*/, wxWindowID id/*=wxID_ANY*/)
    : wxPanel(parent, id)
{
    // main sizer
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Tree Control
    {
        treeCtrl = new wxTreeCtrl(this, wxCGID_MANAGER_TREE, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS|wxTR_HIDE_ROOT);
        treeCtrl->AddRoot("");        
        mainSizer->Add(treeCtrl, 1, wxEXPAND, 10);
    }

    SetSizer(mainSizer);
}

CGManagerWindow::~CGManagerWindow()
{

}

wxString CGManagerWindow::DoGetSuffix(cg::CGBinRef bin)
{
    wxString Suffix(bin->getSuffix());
    if (Suffix.IsEmpty())
        Suffix = "DEFAULT";
    else
        Suffix.Replace("_", "");

    return Suffix;
}

void CGManagerWindow::Reset(cg::CGBinLibraryRef blib)
{
    treeCtrl->DeleteAllItems();

    if (!blib)
        return;

    wxTreeItemId rId = treeCtrl->GetRootItem();
    wxTreeItemId fId = treeCtrl->AppendItem(rId, blib->getFolder());
    wxTreeItemId pId = treeCtrl->AppendItem(fId, "Palettes");
    wxTreeItemId gId = treeCtrl->AppendItem(fId, "Graphics");
    wxTreeItemId aId = treeCtrl->AppendItem(fId, "Animations");
    wxTreeItemId mId = treeCtrl->AppendItem(fId, "Maps");

    binLib = blib;

    for (size_t i=0; i<binLib->getPaletteCount(); ++i)
    {
        cg::CGNamedPalette const & np = binLib->getPalette(i);
        treeCtrl->AppendItem(pId,
            np.name.c_str(), -1, -1, new CGManagerPaletteItem(i));
    }
    for (size_t i=0; i<binLib->size(); ++i)
    {
        cg::CGBinRef bin = binLib->getBin(i);
        if (bin)
        {
            wxString suffix = DoGetSuffix(bin);
            if (bin->getGraphLibrary())
            {
                treeCtrl->AppendItem(gId,
                    wxString::Format("%s:%u:%u",
                        suffix.c_str(),
                        bin->getVersion(cg::ECBC_GRAPHICINFO),
                        bin->getVersion(cg::ECBC_GRAPHIC)),
                    -1, -1, new CGManagerGraphItem(i));
            }
            if (bin->getAnimeLibrary())
            {
                treeCtrl->AppendItem(aId,
                    wxString::Format("%s:%u:%u",
                        suffix.c_str(),
                        bin->getVersion(cg::ECBC_ANIMEINFO),
                        bin->getVersion(cg::ECBC_ANIME)),
                    -1, -1, new CGManagerAnimeItem(i));
            }
        }
    }
    for (auto it : binLib->getMaps())
    {
        treeCtrl->AppendItem(mId,
            wxString::Format("%u", it.first),
            -1, -1, new CGManagerMapItem((int)it.first));
    }

    treeCtrl->Expand(fId);
    treeCtrl->Expand(gId);
    treeCtrl->Expand(aId);
}

void CGManagerWindow::OnItemSelected(wxTreeEvent &e)
{
    CGManagerItem* data = (CGManagerItem*)treeCtrl->GetItemData(e.GetItem());
    if (data)
        data->Activate();
}
