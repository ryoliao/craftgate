
#ifndef _CG_PCH_
#define _CG_PCH_

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#define NOMINMAX
// virtual leak detector, locally setup required.
// #include <vld.h> 
#include <Windows.h>
#ifndef __WIN32__
#define __WIN32__ 1
#endif
#endif

// glew
#include <GL/glew.h>

// std
#include <vector>
#include <map>
#include <string>
#include <math.h>
#include <algorithm>

// wxWidgets
#ifdef _MSC_VER
#pragma include_alias("wx/setup.h", "msvc/wx/setup.h")
#endif

#include <wx/wx.h>
#include <wx/file.h>
#include <wx/aui/aui.h>
#include <wx/grid.h>
#include <wx/listctrl.h>
#include <wx/rawbmp.h>
#include <wx/treectrl.h>
#include <wx/glcanvas.h>

// wx extensions
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/propgrid/manager.h>

// zlib
#include <zlib.h>

// core
#include "CGConfig.h"
#include "CGCommon.h"
#include "CGCommon2D.h"
#include "EMotions.h"
#include "EReferenceIdTypes.h"
#include "IStream.h"
#include "CGScopedStream.h"
#include "CGMemoryStream.h"
#include "CGFile.h"
#include "CGColor.h"
#include "CGPalette.h"
#include "CGGraph.h"
#include "CGAnime.h"
#include "CGDecoder.h"
#include "CGOpenGLTexture.h"
#include "CGOpenGLGraphicEnvironment.h"
#include "CGBin.h"
#include "CGSynthetic.h"

// ide
#include "CommandIDs.h"
#include "CGPropertyWindow.h"
#include "CGGraphViewer.h"
#include "CGManagerWindow.h"
#include "CGListCtrl.h"
#include "CGDisplayPane.h"
#include "CGOpenGLDisplayPane.h"
#include "CGEventList.h"

#endif //_CG_PCH_