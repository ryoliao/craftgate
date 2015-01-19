
#ifndef _CG_WX_COMMAND_IDs_H_
#define _CG_WX_COMMAND_IDs_H_

#define _wxCT(x, str) \
    static wxChar const * x = wxT(str)

enum
{
    // main frame
    wxCGID_ERR = wxID_HIGHEST+1,
    wxCGID_FILTER,
    wxCGID_GRAPHVIEW,
    wxCGID_LIST,
    wxCGID_PROPERTY,
    wxCGID_ERRTOOL,
    wxCGID_ERRTOOL_CLEAN,
    wxCGID_MANAGER,
    wxCGID_MANAGER_TREE,
    wxCGID_MANAGER_ITEM_ACTIVE,
    wxCGID_ERRLIST,
    wxCGID_TIMER_ANIME,
    wxCGID_SEARCH,
};

namespace cg
{

_wxCT(MSGCaption,     "Message");
_wxCT(AppTitle,       "CraftGate by RYO");
_wxCT(MenuFile,       "File");
_wxCT(MenuEdit,       "Edit");
_wxCT(MenuItemExit,   "Exit\tAlt+F4");
_wxCT(MenuHelp,       "Help");
_wxCT(MenuItemAbout,  "About");
_wxCT(MenuItemOpen,   "Open...\tCtrl+O");
_wxCT(MenuItemExport, "Export...\tCtrl+E");
_wxCT(MenuItemSearch, "Search...\tCtrl+S");
_wxCT(ManagerCaption, "Manager");
_wxCT(DisplayCaption, "Display");
_wxCT(PropertyCaption,"Properties");
_wxCT(ListCaption,    "List");

_wxCT(VersionName,    "0.0.5b");
_wxCT(Copyright,      "(C) 2013 Liao Yen Peng <ryodci@gmail.com>");

};

#endif //_CG_WX_COMMAND_IDs_H_