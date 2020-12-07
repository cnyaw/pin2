// BernieTotem.cpp : main source file for BernieTotem.exe
//

#include "stdafx.h"

#include <atlframe.h>
#include <atlcrack.h>
#include <atlmisc.h>
#include <atldlgs.h>

#include "resource.h"

#include "rt/rt.h"
#include "app/wtl_player.h"

#include "App.h"

CAppModule _Module;

class CPlayer : public good::rt::CPlayerWindowImpl<CPlayer>, public BernieTotem::Application<CPlayer, good::gx::GLImage>, public BernieTotem::NativeInterface
{
  CPlayer() : bInit(false)
  {
    tip = "Press O to toggle trace messages";
    showFPS = true;
  }

public:

  typedef good::gx::GLImage ImgT;
  typedef CPlayerWindowImpl<CPlayer> BaseT;
  typedef BernieTotem::Application<CPlayer, ImgT> BernieTotemAppT;
  bool bInit;

  static CPlayer& getInst()
  {
    static CPlayer inst;
    return inst;
  }

  BEGIN_MSG_MAP_EX(CPlayer)
    MSG_WM_CHAR(OnChar)
    CHAIN_MSG_MAP(BaseT)
  END_MSG_MAP()

  void onRender(void)
  {
    if ("Bernie Totem" == mRes.mName && -1 != mRoot) {
      if (!bInit) {
        bInit = true;
        BernieTotemAppT::initApp(this);
      }
      BernieTotemAppT::triggerApp();
      BernieTotemAppT::draw();
    }

    BaseT::onRender();
  }

  void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
    if ('O' == nChar) {
      ToggleOutput();
    } else if ('T' == nChar) {
      ToggleTexInfo();
    }
  }

  virtual bool nativeLoadPicture(int type)
  {
    if (1 != type) {
      return false;
    }

    CFileDialog dlg(TRUE, NULL, NULL, OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Image Files(*.bmp,*.jpg,*.gif,*.png)\0*.bmp;*.jpg;" "*.gif;*.png\0");
    if (IDOK != dlg.DoModal()) {
      return false;
    }

    good::gx::GxImage img;
    if (!img.load(dlg.m_szFileName)) {
      return false;
    }

    return BernieTotemAppT::loadPicSel(img);
  }
};

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
    CGLMessageLoop theLoop;
    _Module.AddMessageLoop(&theLoop);

  ::srand((unsigned int)::time(NULL));

    CPlayer& wndMain = CPlayer::getInst();

    if(wndMain.CreateEx() == NULL)
    {
        ATLTRACE(_T("Main window creation failed!\n"));
        return 0;
    }

    if (!wndMain.init("./BernieTotem.txt")) {
      ATLTRACE(_T("Init good failed!\n"));
      return 0;
    }

    wndMain.ShowWindow(SW_SHOW);

    timeBeginPeriod(1);
    int nRet = wndMain.theLoop.Run();
    timeEndPeriod(1);

    _Module.RemoveMessageLoop();
    return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
    HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//    HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
    ATLASSERT(SUCCEEDED(hRes));

    // this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
    ::DefWindowProc(NULL, 0, 0, 0L);

    AtlInitCommonControls(ICC_BAR_CLASSES);    // add flags to support other controls

    hRes = _Module.Init(NULL, hInstance);
    ATLASSERT(SUCCEEDED(hRes));

    int nRet = Run(lpstrCmdLine, nCmdShow);

    _Module.Term();
    ::CoUninitialize();

    return nRet;
}
