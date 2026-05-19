
//#define UILIB_STATIC //静态库使用请在IDE添加预处理 UILIB_STATIC

#if !defined(DUILIB_SDL) && !defined(DUILIB_GTK)
#define DUILIB_WIN32 //默认的
#endif

#include "compat.h"


#ifdef WIN32
#ifdef UILIB_STATIC
#define UILIB_API 
#else
#if defined(UILIB_EXPORTS)
#	if defined(_MSC_VER)
#		define UILIB_API __declspec(dllexport)
#	else
#		define UILIB_API 
#	endif
#else
#	if defined(_MSC_VER)
#		define UILIB_API __declspec(dllimport)
#	else
#		define UILIB_API 
#	endif
#endif
#endif
#define UILIB_COMDAT __declspec(selectany)

#pragma warning(disable:4505)
#pragma warning(disable:4251)
#pragma warning(disable:4189)
#pragma warning(disable:4121)
#pragma warning(disable:4100)

#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stddef.h>
#include <richedit.h>
#include <tchar.h>
#include <assert.h>
#include <crtdbg.h>
#include <malloc.h>
#include <comdef.h>
#include <gdiplus.h>
#pragma comment( lib, "comctl32.lib" )
#pragma comment( lib, "GdiPlus.lib" )
#pragma comment( lib, "Imm32.lib" )
#else
#define UILIB_API
#define UILIB_COMDAT
#endif //#ifdef WIN32

#ifdef DUILIB_WIN32
	#define UIWND		HWND // HWND
	#define DuiLibWindowWnd	CWindowWin32
	#define CShadowUI	CShadowWin32UI
	#define CDPI		CDpiWin32
	#define UIClip		UIClipWin32
	#define DuiLibPaintManagerUI CPaintManagerWin32UI
	#define CUIApplication CUIApplicationWin32
	#define CUIFrameWnd CUIFrameWndWin32
	#define DuiLibEditWnd CEditWndWin32
	#define CComboWnd CComboWndWin32
	#define CDateTimeWnd CDateTimeWndWin32
	#define CMenuWnd CMenuWndWin32
#elif defined DUILIB_GTK
	#define UIWND		UINT_PTR // GtkWidget*
	#define DuiLibWindowWnd	CWindowGtk
	#define CShadowUI	CShadowGtkUI
	#define CDPI		CDpiGtk
	#define UIClip		UIClipGtk
	#define DuiLibPaintManagerUI CPaintManagerGtkUI
	#define CUIApplication CUIApplicationGtk
	#define CUIFrameWnd CUIFrameWndGtk
	#define DuiLibEditWnd CEditWndGtk
	#define CComboWnd CComboWndGtk
	#define CDateTimeWnd CDateTimeWndGtk
	#define CMenuWnd CMenuWndGtk
#elif defined DUILIB_SDL
	#define UIWND UINT_PTR  // SDL_Window*
	#define DuiLibWindowWnd	CWindowSDL
	#define CShadowUI CShadowSDLUI
	#define CDPI CDpiSDL
	#define UIClip UIClipSDL
	#define DuiLibPaintManagerUI CPaintManagerSDLUI
	#define CUIApplication CUIApplicationSDL
	#define CUIFrameWnd CUIFrameWndSDL
	#define DuiLibEditWnd CEditWndSDL
	#define CComboWnd CComboWndSDL
	#define CDateTimeWnd CDateTimeWndSDL
	#define CMenuWnd CMenuWndSDL
#endif //#ifdef DUILIB_WIN32

#include "Utils/DuiString.h"
#include "Utils/Md5.h"
#include "Utils/Utils.h"
#ifdef WIN32
#include "Utils/unzip.h"
#include "Utils/VersionHelpers.h"
#endif //#ifdef DUILIB_WIN32
#include "Core/UIFile.h"
#include "Core/UIXmlAttribute.h"
#include "Core/UIXmlNode.h"
#include "Core/UIXmlDocument.h"
#include "Utils/observer_impl_base.h"
#include "Utils/UIDelegate.h"
#include "Utils/TrayIcon.h"
#include "Utils/DPI.h"
#include "Utils/DpiWin32.h"
#include "Utils/DpiGtk.h"
#include "Utils/DpiSdl.h"

#include "Core/UIDefine.h"
#include "Utils/UIShadowBase.h"
#include "Utils/UIShadowWin32.h"
#include "Utils/UIShadowGtk.h"
#include "Utils/UIShadowSdl.h"

#include "Core/UIResourceManager.h"
#include "Core/UILangManager.h"
#include "Render/IRender.h"
#include "Core/UIScript.h"
#include "Core/UIManager.h"
#include "Core/UIManagerWin32.h"
#include "Core/UIManagerGtk.h"
#include "Core/UIManagerSDL.h"

#include "Core/UIBase.h"
#include "Core/UIWindowWin32.h"
#include "Core/UIWindowGtk.h"
#include "Core/UIWindowSDL.h"

#include "Core/ControlFactory.h"

#include "Control/UIAnimation.h"
#include "Core/UIControl.h"
#include "Core/UIContainer.h"

#include "Core/UIDlgBuilder.h"
#include "Utils/WinImplBase.h"

#include "Layout/UIVerticalLayout.h"
#include "Layout/UIHorizontalLayout.h"
#include "Layout/UITileLayout.h"
#include "Layout/UIDynamicLayout.h"
#include "Layout/UITabLayout.h"
#include "Layout/UIChildLayout.h"


#include "Control/UILabel.h"
#include "Control/UIText.h"

#include "Control/UIEdit.h"

#include "Control/UIGifAnim.h"


#include "Layout/UIAnimationTabLayout.h"
#include "Control/UIButton.h"
#include "Control/UIOption.h"
#include "Control/UITabCtrl.h"

#include "Control/UIList.h"
#include "Control/UICombo.h"
#include "Control/UIScrollBar.h"
#include "Control/UITreeView.h"

#include "Control/UIProgress.h"
#include "Control/UISlider.h"

#include "Control/UIRichEdit.h"
#include "Control/UIDateTime.h"
#include "Control/UIIPAddress.h"
#include "Control/UIIPAddressEx.h"

#include "Control/UIActiveX.h"
#include "Control/UIWebBrowser.h"
#include "Control/UIFlash.h"

#include "Control/UIMenu.h"
#include "Control/UIMenuWndWin32.h"
#include "Control/UIMenuWndGtk.h"
#include "Control/UIMenuWndSdl.h"

#include "Control/UIGroupBox.h"
#include "Control/UIRollText.h"
#include "Control/UIColorPalette.h"
#include "Control/UIListEx.h"
#include "Control/UIHotKey.h"
#include "Control/UIFadeButton.h"
#include "Control/UIRing.h"

#define UIARGB(a,r,g,b)  ((COLORREF)((((BYTE)(b)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(r))<<16))  |(((DWORD)(BYTE)(a))<<24))  )
#define UIARGB_GetAValue(argb)      (LOBYTE((argb)>>24))
#define UIARGB_GetRValue(argb)      (LOBYTE((argb)>>16))
#define UIARGB_GetGValue(argb)      (LOBYTE(((WORD)(argb)) >> 8))
#define UIARGB_GetBValue(argb)      (LOBYTE(argb))

#define UIRGB(r,g,b)	((COLORREF)((((BYTE)(b)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(r))<<16))  |(((DWORD)(BYTE)(255))<<24))  )
#define UIARGB_2_RGB(argb)	(RGB(UIARGB_GetRValue(argb), UIARGB_GetGValue(argb), UIARGB_GetBValue(argb)))
#define RGB_2_UIRGB(rgb)	(UIRGB(GetRValue(rgb), GetGValue(rgb), GetBValue(rgb)))

#include "Control/UIIconButton.h"
#include "Control/UIDateTimeEx.h"
#include "Control/UIComboEx.h"
#include "Control/UIImageBoxEx.h"
#include "Control/UIRollTextEx.h"
#include "Control/UIMsgWnd.h"
#include "Control/UIPicture.h"
#include "Control/UIPictureBox.h"
#include "Control/UIQRCode.h"
#include "Control/UIBarCode.h"
#include "Control/UIGrid.h"
#include "Control/ITreeUI.h"
#include "Control/UITree.h"
#include "Control/UITreeItem.h"
#include "Control/UITracker.h"
#include "Control/UISpin.h"
#include "Layout/UITableLayout.h"
#include "Layout/UIChildWindow.h"

#include "Utils/UIApplication.h"
#include "Utils/UIApplicationWin32.h"
#include "Utils/UIApplicationGtk.h"
#include "Utils/UIApplicationSdl.h"

#include "Utils/UIFrameBase.h"
#include "Utils/UIFrameWnd.h"

#include "Utils/UIFrameWndWin32.h"
#include "Utils/UIFrameWndGtk.h"
#include "Utils/UIFrameWndSdl.h"

#include "Utils/UIForm.h"
#include "Utils/UIDialog.h"

#include "Core/UIGlobal.h"
