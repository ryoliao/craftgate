
#include "CGGraph_PCH.h"
#include "CGGraphViewer.h"

#include <wx/aboutdlg.h>

IMPLEMENT_APP(CGGraphViewerApp)

bool CGGraphViewerApp::OnInit()
{
    BinLibrary.create();
    Synthetic.create();
    AnimeProvider = new CGAnimeListProvider();
    GraphProvider = new CGGraphListProvider();
    MainFrame = new CGGraphViewer();
    DisplayData.mode = DISPLAY_NONE;

    GLContext = new wxGLContext(MainFrame->Display);
    GLContext->SetCurrent(*(MainFrame->Display));

    if (GLEW_OK != glewInit())
        return false;

    GLEnv = new cg::CGOpenGLGraphicEnvironment();

    SetTopWindow(MainFrame);
    MainFrame->Show();

    return true;
}

int CGGraphViewerApp::OnExit()
{
    AnimeProvider->drop();
    GraphProvider->drop();
    delete GLContext;

    return 0;
}

void CGGraphViewerApp::ActiveBin(long binId, long listType)
{
    cg::CGBinRef bin;
    if ((size_t)binId < BinLibrary->size())
    {
        Synthetic->selectBin(binId);
        bin = BinLibrary->getBin(binId);
        switch (listType)
        {
        case DISPLAY_ANIME:
            AnimeProvider->Reset(bin->getAnimeLibrary());
            MainFrame->DisplayList->Reset(AnimeProvider);
            break;

        case DISPLAY_GRAPH:
            GraphProvider->Reset(bin->getGraphLibrary());
            MainFrame->DisplayList->Reset(GraphProvider);
            break;

        default:
            MainFrame->DisplayList->Reset(0);
            break;
        }
    }
}

void CGGraphViewerApp::ActiveMap(int mapId)
{
    cg::CGOpenGLMapRef map = Synthetic->createMap(mapId);
    if (map.isOK())
    {
        DisplayData.mode = DISPLAY_MAP;
        MainFrame->DisplayList->Reset(0);
        MainFrame->Property->Reset(0);
        MainFrame->Display->Reset(map);
    }
}

void CGGraphViewerApp::ActivePalette(long palId)
{
    Synthetic->selectPalette(palId);
    RefreshDisplay();
}

void CGGraphViewerApp::Open(wxString const & folder)
{
    BinLibrary->reset(folder.c_str());
    Synthetic->reset(BinLibrary);
    MainFrame->Manager->Reset(BinLibrary);
    MainFrame->DisplayList->Reset(0);
    MainFrame->Property->Reset(0);
    MainFrame->Display->Reset(CGDisplayGraph());

    DisplayData.mode = DISPLAY_NONE;

#if _CG_USE_DUMP
    wxString dumpFolder;
    for (size_t i=0; i<BinLibrary->size(); ++i)
    {
        cg::CGBinRef bin = BinLibrary->getBin(i);
        cg::CGAnimeLibraryRef alib = bin->getAnimeLibrary();
        if (alib)
        {
            dumpFolder = wxString::Format("%s\\dump_anim%s.txt", folder.c_str(), bin->getSuffix());
            FILE* fp = fopen(dumpFolder.c_str(), "w");
            if (fp)
                alib->dump(fp);
        }
    }
#endif
}

void CGGraphViewerApp::Export(wxString const & filename)
{
    if (DisplayData.mode == DISPLAY_ANIME)
    {
        CGDisplayPane* display = MainFrame->Display;
        cg::MotionIterator it = Synthetic->findMotion(DisplayData.item, DisplayData.dir, DisplayData.motion);

        if (it.isOK())
            Synthetic->saveGIF(it, filename.c_str());
    }
}

bool CGGraphViewerApp::CanExport()
{
    return (DISPLAY_ANIME == DisplayData.mode);
}

void CGGraphViewerApp::DisplayAnime(long item, long dir, long motion, bool prop)
{
    if (-1 == item)
    {
        item = MainFrame->DisplayList->GetNextItem(item,
            wxLIST_NEXT_ALL,
            wxLIST_STATE_SELECTED);
        if (-1 == item)
            return;
    }

    CGDisplayPane* display = MainFrame->Display;
    cg::MotionIterator it = Synthetic->findMotion(item, dir, motion);
    if (it.isOK())
    {
        cg::defAnimeV2 desc = it.data->desc;
        display->Reset(desc.v1.frames, desc.v1.duration);
        for (size_t f=0; f<desc.v1.frames; ++f)
        {
            display->SetFrame(f, Synthetic->CreateOpenGL(it, f));
        }
    }
    else
    {
        display->Reset(CGDisplayGraph());
    }
    if (prop)
    {
        MainFrame->Property->Reset(AnimeProvider);
    }

    DisplayData.mode = DISPLAY_ANIME;
    DisplayData.item = item;
    DisplayData.dir = dir;
    DisplayData.motion = motion;
}

void CGGraphViewerApp::DisplayGraph(long item, bool prop)
{
    if (-1 == item)
    {
        item = MainFrame->DisplayList->GetNextItem(item,
            wxLIST_NEXT_ALL,
            wxLIST_STATE_SELECTED);
        if (-1 == item)
            return;
    }

    MainFrame->Display->Reset(Synthetic->CreateOpenGL(item));

    if (prop)
    {
        MainFrame->Property->Reset(GraphProvider);
    }

    DisplayData.mode = DISPLAY_GRAPH;
    DisplayData.item = item;
}

void CGGraphViewerApp::RefreshDisplay()
{
    switch (DisplayData.mode)
    {
    case DISPLAY_ANIME:
        DisplayAnime();
        break;

    case DISPLAY_GRAPH:
        DisplayGraph();
        break;
    }
}

//// CGMapViewer

BEGIN_EVENT_TABLE(CGGraphViewer, wxFrame)
    EVT_COMMAND (wxID_OPEN,  wxEVT_COMMAND_MENU_SELECTED, CGGraphViewer::OnOpen)
    EVT_COMMAND (wxID_SAVE,  wxEVT_COMMAND_MENU_SELECTED, CGGraphViewer::OnExport)
    EVT_COMMAND (wxID_ABOUT, wxEVT_COMMAND_MENU_SELECTED, CGGraphViewer::OnAbout)
    EVT_COMMAND (wxID_EXIT,  wxEVT_COMMAND_MENU_SELECTED, CGGraphViewer::OnExit)
    EVT_COMMAND (wxCGID_SEARCH, wxEVT_COMMAND_MENU_SELECTED, CGGraphViewer::OnSearch)
END_EVENT_TABLE()

CGGraphViewer::CGGraphViewer()
: wxFrame(NULL, -1, _(cg::AppTitle), wxDefaultPosition, wxSize(800, 600), wxDEFAULT_FRAME_STYLE)
, Aui(this)
{
    CreateMenuBar();

    //Error = new CGEventList(this, wxCGID_ERR);
    Manager = new CGManagerWindow(this, wxCGID_MANAGER);
    DisplayList = new CGListCtrl(this, wxCGID_LIST);
    Display = new CGDisplayPane(this, wxCGID_GRAPHVIEW);
    Property = new CGPropertyWindow(this, wxCGID_PROPERTY);

    Aui.AddPane(Manager,
        wxAuiPaneInfo().Caption(cg::ManagerCaption)
        .DefaultPane()
        .Left()
        .BottomDockable(false)
        .TopDockable(false)
        .CloseButton(false)
        .PinButton()
        .BestSize(250,400));

    Aui.AddPane(DisplayList,
        wxAuiPaneInfo().Caption(cg::ListCaption)
        .DefaultPane()
        .Bottom()
        .Floatable()
        .CloseButton(false)
        .PinButton());

    Aui.AddPane(Property,
        wxAuiPaneInfo().Caption(cg::PropertyCaption)
        .DefaultPane()
        .Bottom()
        .CloseButton(false)
        .PinButton()
        .BestSize(200,200));

    //Aui.AddPane(Error,
    //    wxAuiPaneInfo().Caption(cg::MSGCaption)
    //    .DefaultPane()
    //    .Bottom()
    //    .LeftDockable(false)
    //    .RightDockable(false)
    //    .PinButton()
    //    .BestSize(400,300));

    Aui.AddPane(Display,
        wxAuiPaneInfo().Caption(cg::DisplayCaption)
        .Center()
        .CloseButton(false)
        .PinButton()
        .BestSize(640,480));

    setupAui();
}
 
CGGraphViewer::~CGGraphViewer()
{
    Aui.UnInit();
}

void CGGraphViewer::CreateMenuBar()
{
    wxMenuBar* mb = new wxMenuBar;

    // file menu
    wxMenu* fileMenu = new wxMenu;
    fileMenu->Append(wxID_OPEN, cg::MenuItemOpen);
    fileMenu->Append(wxID_SAVE, cg::MenuItemExport);
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT, cg::MenuItemExit);

    // edit menu
    wxMenu* editMenu = new wxMenu;
    editMenu->Append(wxCGID_SEARCH, cg::MenuItemSearch);

    // help menu
    wxMenu* helpMenu = new wxMenu;
    helpMenu->Append(wxID_ABOUT, cg::MenuItemAbout);

    // concatenate all menus
    mb->Append(fileMenu, cg::MenuFile);
    mb->Append(editMenu, cg::MenuEdit);
    mb->Append(helpMenu, cg::MenuHelp);

    SetMenuBar(mb);
}

void CGGraphViewer::setupAui()
{
    Aui.SetFlags(wxAUI_MGR_DEFAULT|wxAUI_MGR_ALLOW_ACTIVE_PANE);
    Aui.GetArtProvider()->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, wxAUI_GRADIENT_VERTICAL);
    Aui.GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR, 
        wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT) );
    Aui.GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR, 
        wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT));
    Aui.GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR, 
        wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE ));	

    // tell	the	manager	to "commit"	all	the	changes	just made
    Aui.Update();
}

void CGGraphViewer::OnOpen( wxCommandEvent & e )
{
    wxString filename = wxDirSelector();
    if (!filename.IsEmpty())
    {
        wxGetApp().Open(filename);
    }
}

void CGGraphViewer::OnExport( wxCommandEvent & e )
{
    if (!wxGetApp().CanExport())
    {
        wxMessageBox(
            wxT("please select a component to export."),
            wxT("error"),
            wxICON_ERROR|wxOK|wxCENTRE);
        return;
    }

    wxString filename = wxSaveFileSelector("Export GIF file", "gif");
    if (!filename.IsEmpty())
    {
        wxGetApp().Export(filename);
    }
}

void CGGraphViewer::OnAbout( wxCommandEvent & e )
{
    wxAboutDialogInfo info;
    info.SetName(cg::AppTitle);
    info.SetVersion(cg::VersionName);
    info.SetCopyright(cg::Copyright);
    wxAboutBox(info);
}

void CGGraphViewer::OnExit( wxCommandEvent & e )
{
    Close();
}

void CGGraphViewer::OnSearch(wxCommandEvent & e)
{
    wxString value;
    wxTextEntryDialog dlg(this, wxT("請輸入要查找的地圖編號"), wxT("搜尋"), wxT(""));
    if (dlg.ShowModal() == wxID_OK)
    {
        value = dlg.GetValue();

        unsigned long id;
        if (value.ToULong(&id))
        {
            auto & app = wxGetApp();
            auto point = app.Synthetic->findMapPoint((cg::u32)id);
            if (point != nullptr)
            {
                app.ActiveBin(point->graphLibIndex, CGGraphViewerApp::DISPLAY_GRAPH);
                app.MainFrame->DisplayList->EnsureVisible(point->graphId);
                app.MainFrame->DisplayList->SetItemState(point->graphId, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
            }
        }
    }
}