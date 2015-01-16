
#ifndef _CG_WX_PROPERTY_WINDOW_H_
#define _CG_WX_PROPERTY_WINDOW_H_

class CGPropertyExchanging : public cg::IRefBase
{
public:
    virtual void SerializeProperty(std::vector<wxPGProperty*> & out)=0;
    //virtual void DeserializeProperty(CGPropertyMap const & in)=0;
    virtual void OnPropertyChanged(wxPGProperty* value)=0;
};

class CGPropertyWindow : public wxPropertyGridManager
{
public:
    CGPropertyWindow(wxWindow* parent=0, wxWindowID id=wxID_ANY);
    ~CGPropertyWindow();

    void Reset(CGPropertyExchanging* tar);

    virtual void Update();

    //! user changed some data in the grid, set the data into the selected item
    void OnPropertyGridChange(wxPropertyGridEvent& e);

private:
    std::vector<wxPGProperty*> properties;
    CGPropertyExchanging* target;

    DECLARE_EVENT_TABLE()
};

#endif //_CG_WX_PROPERTY_WINDOW_H_