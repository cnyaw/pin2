//
// Bernie Totem gameplay render.
// 2010/04/11 Waync created.
//

#pragma once

#define BERNIE_TOTEM_TEXT_SIZE 20

enum {
  BKGND_TEX_ID = 0,
  FLOWER_TEX_ID = 1,
  TITLE_TEX_ID = 2,
  ICON_TEX_ID = 3,
  SIZER_TEX_ID = 5,
  FRAME_TEX_ID = 6,
  EYE_TEX_ID = 7,
  CLOCK_TEX_ID = 10
};

sw2::StageStack<Game> renderStage;

void draw()
{
  drawBkgnd();
  renderStage.trigger();
}

//
// Draw func.
//

void getTextDim(const char *pText, int &sw, int &sh) const
{
  AppT &app = AppT::getInst();
  for (int i = 0; i < (int)strlen(pText); i++) {
    typename AppT::ImgT img = app.getImage(BERNIE_TOTEM_TEXT_SIZE, (int)pText[i]);
    if (img.isValid()) {
      sw += img.getWidth();
      sh = img.getHeight();
    } else {
      sw += 10;
    }
  }
}

void myDrawNum(int x, int y, int sz, int n) const
{
  char buff[512];
  sprintf(buff, "%d", n);

  int sw = 0, sh = 0;
  getTextDim(buff, sw, sh);

  myTextOutWorker(x + (sz - sw)/2 + 1, y + (sz - sh)/2 + 1, buff, 0xff404040);
  myTextOutWorker(x + (sz - sw)/2, y + (sz - sh)/2, buff);
}

void myBitBlt(int x, int y, int w, int h, int img, int srcx, int srcy, int srcw, int srch, int color = 0xffffffff, float rot = .0f) const
{
  AppT::getInst().drawImage(-1, x, y, img, srcx, srcy, srcw, srch, color, rot, w/(float)srcw, h/(float)srch);
}

void myBitBlt(int x, int y, int w, int h, int img, int srcx, int srcy, int color = 0xffffffff, float rot = .0f) const
{
  myBitBlt(x, y, w, h, img, srcx, srcy, w, h, color, rot);
}

void myFillRect(int x, int y, int w, int h, int color) const
{
  AppT::getInst().fillRect(-1, x, y, w, h, color);
}

int myTextOutWorker(int x, int y, char const* s, int color = 0xffffffff) const
{
  AppT::getInst().drawText(-1, x, y, s, BERNIE_TOTEM_TEXT_SIZE, color);
  return 0;
}

int myTextOut(int x, int y, char const *format, ...) const
{
  char buff[512];

  va_list va;
  va_start(va, format);
  vsnprintf(buff, sizeof(buff), format, va);
  va_end(va);

  return myTextOutWorker(x, y, buff);
}

int myTextOut(int x, int y, int w, int h, char const *format, ...) const
{
  char buff[512];

  va_list va;
  va_start(va, format);
  vsnprintf(buff, sizeof(buff), format, va);
  va_end(va);

  int sw = 0, sh = 0;
  getTextDim(buff, sw, sh);

  return myTextOutWorker(x + (w - sw)/2, y + (h - sh)/2, buff);
}

void drawBkgnd() const
{
  myFillRect(0, 0, BernieTotem::SCREEN_W, BernieTotem::SCREEN_H, 0xff5697FF);
  myBitBlt(0, 0, BernieTotem::SCREEN_W, BernieTotem::SCREEN_H, BKGND_TEX_ID, 0, 0, 212, 160, 0x1fffffff);

  //
  // Title.
  //

  if (renderStage.top() != &Game::sDrawTitle) {
    myBitBlt(335, 270, 140, 40, TITLE_TEX_ID, 0, 0, 211, 84);
    static int rot = 0;
    rot = (1 + rot) % 360;
    AppT::getInst().gx.setAnchor(.5f, .5f);
    myBitBlt(450, 260, 12, 12, ICON_TEX_ID, 0, 0, 32, 32, 0xffffffff, (float)rot);
    AppT::getInst().gx.setAnchor(0, 0);
  }
}

void drawButton(char const* text, URect const& ui, int color) const
{
  int x = ui.left, y = ui.top, w = ui.right, h = ui.bottom;
  myFillRect(x, y, w, h, color);
  myBitBlt(x, y, w, h, FLOWER_TEX_ID, 0, y, COLOR_BTN_BG);
  myTextOut(x, y, w, h, text);
}

void myBitBltUi(URect const& ui, int img, int srcx, int srcy) const
{
  myBitBlt(ui.left, ui.top, ui.right, ui.bottom, img, srcx, srcy);
}

//
// Stage.
//

void sDrawTitle(int s, sw2::uint_ptr)
{
  if (sw2::TRIGGER == s) {

    myBitBlt(20, 50, 420, 120, TITLE_TEX_ID, 0, 0, 211, 84);

    static int rot = 0;
    rot = (1 + rot) % 360;

    AppT::getInst().gx.setAnchor(.5f, .5f);
    myBitBlt(400, 60, 32, 32, ICON_TEX_ID, 0, 0, 0xffffffff, (float)rot);
    AppT::getInst().gx.setAnchor(0, 0);

    drawButton("Start", state.uiTitlePlay, COLOR_BTN3);
  }
}

void sDrawPlay(int s, sw2::uint_ptr t)
{
  if (sw2::TRIGGER == s)
  {
    //
    // Puzzle.
    //

    int const piecex[] = {PIECE3, PIECE4, PIECE5};
    int piece = piecex[state.size - SIZE3];

    myFillRect(PUZZLEX, PUZZLEY, state.size * piece + 1, state.size * piece + 1, 0xff000000);

    for (int i = 0; i < state.size * state.size; ++i) {

      if (!state.win) {
        if (!state.spyview && state.lack == state.puzzle[i]) {
          continue;
        } else if (state.spyview && state.lack == i) {
          continue;
        }
      }

      int col = i % state.size, row = i / state.size;

      int x = PUZZLEX + piece * (i % state.size);
      int y = PUZZLEY + piece * (i / state.size);

      bool shadow = false;
      int sx = 0, sy  = 0;

      bool moving = i == state.moving;  // Animate move.

      if (moving && !state.spyview) {
        int offset = (int)(piece * sin((state.timer / (float)state.timerMovePiece) * (3.1415926/2.0f)));
        int p = piece;
        switch (state.dir)
        {
        case DIR_UP:
          y = y + p - offset;
          break;
        case DIR_DOWN:
          y = y - p + offset;
          break;
        case DIR_LEFT:
          x = x + p - offset;
          break;
        case DIR_RIGHT:
          x = x - p + offset;
          break;
        }
      }

      URect const &rs = state.rcPicSel;
      int srcw = (int)(rs.right / (float)state.size);
      int srch = (int)(rs.bottom / (float)state.size);
      int srcx, srcy;

      if (state.flower) {
        if (state.spyview) {
          srcx = (i % state.size) * srcw;
          srcy = (i / state.size) * srch;
        } else {
          srcx = (state.puzzle[i] % state.size) * srcw;
          srcy = (state.puzzle[i] / state.size) * srch;
        }
        myBitBlt(x + 1, y + 1, piece - 1, piece - 1, FLOWER_TEX_ID, srcx, srcy, srcw, srch);
        if (state.alnum) {
          myDrawNum(x, y, piece, state.spyview ? i + 1 : state.puzzle[i] + 1);
        }
      } else if (state.picture) {
        if (state.spyview) {
          srcx = rs.left + (i % state.size) * srcw;
          srcy = rs.top + (i / state.size) * srch;
        } else {
          srcx = rs.left + (state.puzzle[i] % state.size) * srcw;
          srcy = rs.top + (state.puzzle[i] / state.size) * srch;
        }

        AppT &app = AppT::getInst();
        app.gx.drawImage(x + 1, y + 1, app.mPicSel, srcx, srcy, srcw, srch, 0xffffffff, .0f, (piece - 1)/(float)srcw , (piece - 1)/(float)srch);

        if (state.alnum) {
          myDrawNum(x, y, piece, state.spyview ? i + 1 : state.puzzle[i] + 1);
        }
      } else if (state.alnum) {
        myFillRect(x + 1, y + 1, piece - 1, piece - 1, COLOR_BTN1);
        myDrawNum(x, y, piece, state.spyview ? i + 1 : state.puzzle[i] + 1);
      }

      if (state.win && TIME_WIN >state.timer && state.lack == state.puzzle[i]) {
        float t = state.timer / (float)TIME_WIN;
        int a = (int)(0xff * (1 - sin(3.1415926 * t / 2.0f)));
        myFillRect(x + 1, y + 1, piece - 1, piece - 1, a << 24);
      }
    }

    //
    // Frame on lack piece.
    //

    int xlack = PUZZLEX + piece * (state.lack % state.size);
    int ylack = PUZZLEY + piece * (state.lack / state.size);

    myBitBlt(xlack, ylack, piece, piece, FRAME_TEX_ID, 0, 0, 100, 100);

    //
    // Option buttons.
    //

    if (state.playing) {
      int sec, min, hr;
      sec = state.timerPlaying / FRAME_RATE;
      hr = sec / 3600;
      min = sec / 60;
      sec %= 60;
      int x = PRACTICE_OFFSET_X;
      myBitBlt(x, 110, 20, 20, CLOCK_TEX_ID, 0, 0);
      myTextOut(x + 26, 114, "%02d:%02d:%02d", hr, min, sec);
      myTextOut(x, 140, "Moves : %d", state.moves);
      if (!state.shuffling) {
        drawButton(state.win ? "Win!" : "Back", state.uiPlayBack, COLOR_BTN3);
      }
    } else {
      drawButton("3x3", state.uiPlaySize3, SIZE3 == state.size ? COLOR_BTN1 : COLOR_BTN2);
      drawButton("4x4", state.uiPlaySize4, SIZE4 == state.size ? COLOR_BTN1 : COLOR_BTN2);
      drawButton("5x5", state.uiPlaySize5, SIZE5 == state.size ? COLOR_BTN1 : COLOR_BTN2);
      drawButton("123", state.uiPlayNumber, state.alnum ? COLOR_BTN1 : COLOR_BTN2);
      drawButton("Flower", state.uiPlayFlower, state.flower ? COLOR_BTN1 : COLOR_BTN2);
      drawButton("Picture", state.uiPlayPicture, state.picture ? COLOR_BTN1 : COLOR_BTN2);
      drawButton("Back", state.uiPlayBack, COLOR_BTN3);
    }

    //
    // Spy view.
    //

    if (state.playing && !state.shuffling && !state.win) {
      myBitBltUi(state.uiPlayingSpy, EYE_TEX_ID, 0, 0);
    }
  }
}

void sDrawPicture(int s, sw2::uint_ptr)
{
  if (sw2::TRIGGER == s) {

    //
    // Draw picture.
    //

    myFillRect(PUZZLEX, PUZZLEY, PUZZLEW + 2, PUZZLEH + 2, 0xdc000000);

    AppT &app = AppT::getInst();
    typename AppT::ImgT &img = app.mPicSel;
    URect const &rs = state.rcPicSel;
    URect const &rr = state.rcPicRender;

    app.gx.drawImage(rr.left, rr.top, app.mPicSel, 0, 0, img.getWidth(), img.getHeight(), 0xffffffff, .0f, rr.right/(float)img.getWidth(), rr.bottom/(float)img.getHeight());
    myFillRect(rr.left, rr.top, rr.right, rr.bottom, 0xafffffff);

    //
    // Draw selection.
    //

    int x = rr.left + (int)(rs.left * rr.right / (float)img.getWidth());
    int y = rr.top + (int)(rs.top * rr.bottom / (float)img.getHeight());
    int w = (int)(rs.right * rr.right / (float)img.getWidth());
    int h = (int)(rs.bottom * rr.bottom / (float)img.getHeight());

    app.gx.drawImage(x, y, app.mPicSel, rs.left, rs.top, rs.right, rs.bottom, 0xffffffff, .0f, rr.right/(float)img.getWidth(), rr.bottom/(float)img.getHeight());

    myBitBlt(x + w - PICSEL_FRAME_SZ, y + h - PICSEL_FRAME_SZ, PICSEL_FRAME_SZ, PICSEL_FRAME_SZ, SIZER_TEX_ID, 0, 0);

    if (PICSEL_RESIZE == state.picSel) {
      myFillRect(x + w - PICSEL_FRAME_SZ, y + h - PICSEL_FRAME_SZ, PICSEL_FRAME_SZ, PICSEL_FRAME_SZ, 0xafff0000);
    }

    //
    // Buttons.
    //

    drawButton("Gallery", state.uiPicGallery, COLOR_BTN2);
#ifdef _android_
    drawButton("Camera", state.uiPicCamera, COLOR_BTN2);
#endif
    drawButton("Default", state.uiPicDefault, COLOR_BTN1);
    drawButton("Back", state.uiPlayBack, COLOR_BTN3);
  }
}

// end of Render.h
