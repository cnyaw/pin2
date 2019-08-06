/*
  gl_code.cpp
  BernieTotem Android platforma implementation.

  Copyright (c) 2014 Waync Cheng.
  All Rights Reserved.

  2014/11/26 Waync created
 */

#define BERNIETOTEM_JNI_ACTIVITY "weilican/BernieTotem/BernieTotemActivity"

#include "app/android_app.h"

#include "../../App.h"

class BernieTotemApp : public good::rt::AndroidApplication<BernieTotemApp>, public BernieTotem::Application<BernieTotemApp>, public BernieTotem::NativeInterface
{
  BernieTotemApp() : bInit(false)
  {
    srand((unsigned int)time(NULL));
  }
public:
  static BernieTotemApp& getInst()
  {
    static BernieTotemApp i;
    return i;
  }

  typedef good::rt::AndroidApplication<BernieTotemApp> BaseT;
  typedef BernieTotem::Application<BernieTotemApp> BernieTotemAppT;
  bool bInit;

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

  virtual bool nativeLoadPicture(int type)
  {
    jclass cls = tmpEnv->FindClass(BERNIETOTEM_JNI_ACTIVITY);
    jmethodID mid = tmpEnv->GetStaticMethodID(cls, "doLoadPicture", "(I)Z");
    if (mid) {
      return tmpEnv->CallStaticBooleanMethod(cls, mid, type);
    }
    return false;
  }
};

BernieTotemApp& g = BernieTotemApp::getInst();

#include "app/android_lib.h"

extern "C" {

JNIEXPORT void JNICALL Java_weilican_BernieTotem_BernieTotemActivity_setPicSel(JNIEnv *env, jobject obj, jintArray iarr)
{
  if (!iarr) {
    return;
  }

  good::gx::GxImage img;

  jint *iarrbody = env->GetIntArrayElements(iarr, 0);
  img.w = iarrbody[0];
  img.h = iarrbody[1];
  img.bpp = 4;
  img.dat = new char[img.w * img.h * img.bpp];
  memcpy(img.dat, iarrbody + 2, sizeof(int) * img.w * img.h);

  env->ReleaseIntArrayElements(iarr, iarrbody, 0);

  if (g.loadPicSel(img)) {
    g.game.defaultSel();
  }
}

}
