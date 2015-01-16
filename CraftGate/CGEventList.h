
#ifndef _CG_WX_EVENT_LIST_H_
#define _CG_WX_EVENT_LIST_H_

class CGEventListCtrl;

class CGEventList : public wxPanel
{
public:
    CGEventList(wxWindow* parent=0, wxWindowID id=wxID_ANY);
    virtual ~CGEventList();

private:
    // creation
    void DoInitControls();

    CGEventListCtrl* EvtList;
};

class CGEventListCtrl : public wxListCtrl, public cg::IEventHandler
{
public:
    CGEventListCtrl(wxWindow* parent=0, wxWindowID id=wxID_ANY);
    virtual ~CGEventListCtrl();

    void ClearAllEvents();

    // these functions are only used for virtual list view controls, i.e. the
    // ones with wxLC_VIRTUAL style

    // return the text for the given column of the given item
    virtual wxString OnGetItemText(long item, long column) const;

    // return the icon for the given item. In report view, OnGetItemImage will
    // only be called for the first column. See OnGetItemColumnImage for
    // details.
    virtual int OnGetItemImage(long item) const;

    // return the attribute for the item (may return NULL if none)
    virtual wxListItemAttr *OnGetItemAttr(long item) const;

private:
    // IEventHandler override
    virtual void OnEvent(cg::SEvent & e);

    wxImageList imgList;
    std::vector<cg::SEvent> evtdata;
};

#endif //_CG_WX_EVENT_LIST_H_