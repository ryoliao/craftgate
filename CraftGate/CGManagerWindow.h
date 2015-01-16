
#ifndef _CG_WX_MANAGER_WINDOW_H_
#define _CG_WX_MANAGER_WINDOW_H_

class CGManagerWindow : public wxPanel
{
public:
    CGManagerWindow(wxWindow* parent=0, wxWindowID id=wxID_ANY);
    ~CGManagerWindow();

    void Reset(cg::CGBinLibraryRef blib);

private:
    void OnItemSelected(wxTreeEvent &e);
    wxString DoGetSuffix(cg::CGBinRef bin);

    
    cg::CGBinLibraryRef binLib;
    wxTreeCtrl* treeCtrl;
    //wxImageList treeImageList;

    DECLARE_EVENT_TABLE()
};

#endif //_CG_WX_MANAGER_WINDOW_H_