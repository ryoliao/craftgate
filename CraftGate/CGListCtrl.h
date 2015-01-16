
#ifndef _CG_WX_LIST_CONTROL_H_
#define _CG_WX_LIST_CONTROL_H_

struct CGListDescription
{
    size_t szItem;
    char const * const * columnNames;
};

class CGListProvider : public CGPropertyExchanging
{
public:
    virtual CGListDescription OnGetItemDescription() const=0;
    virtual wxString OnGetItemText(long item, long column) const=0;
    virtual void OnItemSelected(long item)=0;
};

class CGListCtrl : public wxListCtrl
{
public:
    CGListCtrl(wxWindow* parent=0, wxWindowID id=wxID_ANY);
    virtual ~CGListCtrl();

    void Reset(CGListProvider* ptr);

    // these functions are only used for virtual list view controls, i.e. the
    // ones with wxLC_VIRTUAL style

    // return the text for the given column of the given item
    virtual wxString OnGetItemText(long item, long column) const;

private:
    void OnItemSelected(wxListEvent & e);

    CGListProvider* provider;

    DECLARE_EVENT_TABLE()
};

///////////////////////////////////////////////////////////////////////////////

class CGGraphListProvider : public CGListProvider
{
public:
    CGGraphListProvider();

    void Reset(cg::CGGraphLibraryRef lib);

    // CGListProvider overrides
    virtual CGListDescription OnGetItemDescription() const;
    virtual wxString OnGetItemText(long item, long column) const;
    virtual void OnItemSelected(long item);

    // CGPropertyExchanging overrides.
    virtual void SerializeProperty(std::vector<wxPGProperty*> & out);
    virtual void OnPropertyChanged(wxPGProperty* value);

private:
    cg::CGGraphLibraryRef Graphs;
    long selection;
};

///////////////////////////////////////////////////////////////////////////////

class CGAnimeListProvider : public CGListProvider
{
public:
    CGAnimeListProvider();

    void Reset(cg::CGAnimeLibraryRef lib);

    // CGListProvider overrides
    virtual CGListDescription OnGetItemDescription() const;
    virtual wxString OnGetItemText(long item, long column) const;
    virtual void OnItemSelected(long item);

    // CGPropertyExchanging overrides.
    virtual void SerializeProperty(std::vector<wxPGProperty*> & out);
    virtual void OnPropertyChanged(wxPGProperty* value);

private:
    cg::CGAnimeLibraryRef Animes;
    long selection;
    long direction;
    long motion;
};

#endif //_CG_WX_LIST_CONTROL_H_