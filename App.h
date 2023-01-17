//
// Bernie Totem game application.
// 2014/11/26 Waync created.
//

#pragma once

#include <time.h>
#include <math.h>

#include <fstream>
#include <vector>

#include "swGeometry.h"
#include "swStageStack.h"
#include "swUtil.h"

#include "State.h"
#include "Game.h"

#ifdef WIN32
# define GOOD_SUPPORT_GDIPLUS_IMG
#endif

#include "gx/img.h"

namespace BernieTotem {

template<class AppT, class ImgT, class BaseT>
class Application : public BaseT
{
public:

  bool bInit;
  State state;
  Game<AppT> game;
  ImgT mPicSel;

  Application() : bInit(false), game(state), mPicSel(0)
  {
  }

  void onRender(void)
  {
    if ("Bernie Totem" == BaseT::mRes.mName && -1 != BaseT::mRoot) {
      if (!bInit) {
        bInit = true;
        initApp((AppT*)&AppT::getInst());
      }
      triggerApp();
      draw();
    }
    BaseT::onRender();
  }

  void initApp(NativeInterface *native)
  {
    game.init(native);

    AppT &app = AppT::getInst();
    std::stringstream ss;
    if (app.loadFile("res/flower.png", ss)) {
      mPicSel = ImgT::getImage("BernieTotem.PicSel", ss.str());
      app.state.rcPicRender = sw2::IntRect(BernieTotem::PUZZLEX + 1, BernieTotem::PUZZLEY + 1, BernieTotem::PUZZLEW, BernieTotem::PUZZLEH);
      app.state.rcPicSel = sw2::IntRect(0, 0, mPicSel.getWidth(), mPicSel.getHeight());
    }
  }

  void triggerApp()
  {
    AppT &app = AppT::getInst();

    if (app.isKeyPushed(GOOD_KEYS_LBUTTON)) {
      game.touchBegin(app.mMousePos.x, app.mMousePos.y);
    }

    if (app.isKeyDown(GOOD_KEYS_LBUTTON)) {
      game.touchMove(app.mMousePos.x, app.mMousePos.y);
    }

    if (app.isKeyPressed(GOOD_KEYS_LBUTTON)) {
      game.touchEnd(app.mMousePos.x, app.mMousePos.y);
    }

    game.trigger();
  }

  void draw()
  {
    game.draw();
  }

  bool loadPicSel(good::gx::GxImage &img)
  {
    img.convert32();

    if (img.w > img.h) {
      img.resize((int)(mPicSel.getHeight() * img.w / (float)img.h), mPicSel.getHeight());
    } else if (img.w < img.h) {
      img.resize(mPicSel.getWidth(), (int)(mPicSel.getWidth() * img.h / (float)img.w));
    } else {
      img.resize(mPicSel.getWidth(), mPicSel.getHeight());
    }

    mPicSel.draw(0, 0, img, 0, 0, mPicSel.getWidth(), mPicSel.getHeight());
    AppT::getInst().gx.restoreSur();

    return true;
  }
};

} // namespace BernieTotem

// end of App.h
