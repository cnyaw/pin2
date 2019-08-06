//
// Bernie Totem gameplay.
// 2010/04/11 Waync created.
//

#pragma once

namespace BernieTotem {

#define NATIVE_LOAD_PIC_FROM_FILE 1
#define NATIVE_LOAD_PIC_FROM_CAMERA 2

struct NativeInterface
{
  virtual bool nativeLoadPicture(int type/*1:gallery,2:camera*/) = 0;
};

template<class AppT>
class Game
{
public:

#include "Render.h"

  State& state;

  sw2::StageStack<Game> stage;

  NativeInterface *native;

  explicit Game(State& state_) : state(state_), native(0)
  {
    state.alnum = true;
    state.flower = true;
    state.picture = false;
  }

  //
  // Main.
  //

  bool init(NativeInterface *native_)
  {
    initUi();

    renderStage.initialize(this, &Game::sDrawTitle);
    stage.initialize(this, &Game::sTitle);

    native = native_;

    return true;
  }

  void trigger()
  {
    stage.trigger();
  }

  void initUi()
  {
    //
    // Title.
    //

    state.uiTitlePlay = URect((SCREEN_W - 180)/2, 215, 180, 32);

    //
    // Play.
    //

    int x = PRACTICE_OFFSET_X, y = PRACTICE_OFFSET_Y;
    int w = PRACTICE_BTN_W, h = PRACTICE_BTN_H;

    state.uiPlaySize3 = URect(x, y, w, h);
    state.uiPlaySize4 = URect(x, y + h, w, h);
    state.uiPlaySize5 = URect(x, y + 2 * h, w, h);
    y += 3 * h + PRACTICE_SPACE_H;

    state.uiPlayNumber = URect(x, y, w, h);
    state.uiPlayFlower = URect(x, y + h, w, h);
    state.uiPlayPicture = URect(x, y + 2 * h, w, h);
    y += 3 * h + PRACTICE_SPACE_H;

    state.uiPlayBack = URect(x, y, w, h);

    state.uiPlayingSpy = URect(5, PRACTICE_OFFSET_Y, 32, 32);
    state.uiPlayRand = URect(335, 270, 140, 40);

    //
    // Picture.
    //

    y -= 3 * h + PRACTICE_SPACE_H;

    state.uiPicGallery = URect(x, y, w, h);
#ifdef _android_
    state.uiPicCamera = URect(x, y + h, w, h);
#endif
    state.uiPicDefault = URect(x, y + 2 * h + PRACTICE_SPACE_H, w, h);
  }

  bool ptInUi(URect const& ui, UPoint pt) const
  {
    return URect(ui.left, ui.top, ui.left + ui.right, ui.top + ui.bottom).ptInRect(pt);
  }

  void defaultSel()
  {
    good::gx::GLImage &img = AppT::getInst().mPicSel;

    state.rcPicRender = URect(PUZZLEX + 1, PUZZLEY + 1, PUZZLEW, PUZZLEH);
    state.rcPicSel = URect(0, 0, img.getWidth(), img.getHeight());
  }

  //
  // Puzzle game.
  //

  bool isGameComplete() const
  {
    if (!state.playing || state.shuffling) {
      return false;
    }

    for (int i = 0; i < state.size * state.size; i++) {
      if (i != state.puzzle[i]) {
        return false;
      }
    }

    return true;
  }

  void reset()
  {
    state.moving = -1;
    for (int i = 0; i < state.size * state.size; i++) {
      state.puzzle[i] = i;
    }
  }

  //
  // Touch.
  //

  void touchBegin(int x, int y)
  {
    HANDLE_TOUCH h(TOUCH_BEGIN, x, y);
    stage.trigger((sw2::uint_ptr)&h);
  }

  void touchEnd(int x, int y)
  {
    HANDLE_TOUCH h(TOUCH_END, x, y);
    stage.trigger((sw2::uint_ptr)&h);
  }

  void touchMove(int x, int y)
  {
    HANDLE_TOUCH h(TOUCH_MOVE, x, y);
    stage.trigger((sw2::uint_ptr)&h);
  }

  //
  // Stage.
  //

  void sMovePiece(int s, sw2::uint_ptr)
  {
    if (JOIN == s) {
      state.timer = 0;
    }

    if (TRIGGER == s && state.timerMovePiece <= ++state.timer) {
      state.moving = -1;
      stage.pop();
      if (isGameComplete()) {
        stage.popAndPush(&Game::sWin);
      }
    }
  }

  void sPlay(int s, sw2::uint_ptr t)
  {
    if (JOIN == s) {
      renderStage.popAndPush(&Game::sDrawPlay);
      state.playing = false;
      state.win = false;
      state.timerMovePiece = TIME_MOVING_PIECE;
      reset();
    }

    if (TRIGGER == s) {
      if (AppT::getInst().isKeyPushed(GOOD_KEYS_ESCAPE)) {
        stage.popAndPush(&Game::sTitle);
        return;
      }
    }

    if (TRIGGER == s && t) {

      HANDLE_TOUCH const* ht = (HANDLE_TOUCH const*)t;
      if (TOUCH_BEGIN != ht->s) {
        return;
      }

      UPoint pt(ht->x, ht->y);

      //
      // Size.
      //

      int size = state.size;
      if (ptInUi(state.uiPlaySize3, pt)) { // 3x3
        if (SIZE3 != state.size) {
          state.size = SIZE3;
        } else {
          state.lack = (state.lack + 1) % (state.size * state.size);
        }
      } else if (ptInUi(state.uiPlaySize4, pt)) { // 4x4
        if (SIZE4 != state.size) {
          state.size = SIZE4;
        } else {
          state.lack = (state.lack + 1) % (state.size * state.size);
        }
      } else if (ptInUi(state.uiPlaySize5, pt)) { // 5x5
        if (SIZE5 != state.size) {
          state.size = SIZE5;
        } else {
          state.lack = (state.lack + 1) % (state.size * state.size);
        }
      }

      if (size != state.size) {
        state.lack = state.size * state.size - 1;
        reset();
      }

      //
      // Type.
      //

      if (ptInUi(state.uiPlayNumber, pt)) {
        if (state.flower || state.picture) {
          state.alnum = !state.alnum;
        }
      } else if (ptInUi(state.uiPlayFlower, pt)) {
        if (state.alnum || state.picture) {
          state.flower = !state.flower;
          if (state.flower) {
            state.picture = false;
          }
        }
      } else if (ptInUi(state.uiPlayPicture, pt)) {
        stage.push(&Game::sPicture);
        return;
      }

      //
      // Back.
      //

      if (ptInUi(state.uiPlayBack, pt)) {
        stage.popAndPush(&Game::sTitle);
        return;
      }

      //
      // Start game.
      //

      int const piecex[] = {PIECE3, PIECE4, PIECE5};
      int piece = piecex[state.size - SIZE3];

      if (URect(PUZZLEX, PUZZLEY, PUZZLEX + state.size * piece, PUZZLEY + state.size * piece).ptInRect(pt)) {
        stage.popAndPush(&Game::sPlaying);
        return;
      }

      //
      // Play with random size and lack piece.
      //

      if (ptInUi(state.uiPlayRand, pt)) {
        state.size = sw2::rangeRand(3, 5);
        state.lack = rand() % (state.size * state.size);
        reset();
        stage.popAndPush(&Game::sPlaying);
      }
    }
  }

  void sPlaying(int s, sw2::uint_ptr t)
  {
    if (JOIN == s) {
      state.playing = true;
      state.moves = 0;
      state.timerPlaying = 0;
      stage.push(&Game::sShuffle);
      state.spyview = false;
    }

    if (TRIGGER == s) {

      if (0 < state.moves) {
        state.timerPlaying += 1;
      }

      if (0 == t) {
        if (AppT::getInst().isKeyPushed(GOOD_KEYS_ESCAPE)) {
          stage.popAndPush(&Game::sPlay);
        }
        return;
      }

      HANDLE_TOUCH const* ht = (HANDLE_TOUCH const*)t;

      UPoint pt(ht->x, ht->y);

      //
      // Back.
      //

      if (ptInUi(state.uiPlayBack, pt)) {
        stage.popAndPush(&Game::sPlay);
        return;
      }

      //
      // Spy view.
      //

      if (TOUCH_BEGIN == ht->s && ptInUi(state.uiPlayingSpy, pt)) {
        state.spyview = true;
        return;
      } else if (TOUCH_END == ht->s && state.spyview) {
        state.spyview = false;
        return;
      }

      //
      // Puzzle.
      //

      int const piecex[] = {PIECE3, PIECE4, PIECE5};
      int piece = piecex[state.size - SIZE3];

      int col = (pt.x - PUZZLEX) / piece;
      int row = (pt.y - PUZZLEY) / piece;
      int idx = row * state.size + col;

      if (TOUCH_BEGIN == ht->s) {

        if (!URect(PUZZLEX, PUZZLEY, PUZZLEX + state.size * piece, PUZZLEY + state.size * piece).ptInRect(pt)) {
          return;
        }

        if (state.puzzle[idx] == state.lack) {
          return;
        }

        if (0 < row && state.puzzle[idx - state.size] == state.lack) { // Up.
          std::swap(state.puzzle[idx], state.puzzle[idx - state.size]);
          state.moving = idx - state.size;
          state.dir = DIR_UP;
        } else if (state.size > row + 1 && // Down.
                    state.puzzle[idx + state.size] == state.lack) {
          std::swap(state.puzzle[idx], state.puzzle[idx + state.size]);
          state.moving = idx + state.size;
          state.dir = DIR_DOWN;
        } else if (0 < col && state.puzzle[idx - 1] == state.lack) { // Left.
          std::swap(state.puzzle[idx], state.puzzle[idx - 1]);
          state.moving = idx - 1;
          state.dir = DIR_LEFT;
        } else if (state.size > col + 1 && // Right.
                  state.puzzle[idx + 1] == state.lack) {
          std::swap(state.puzzle[idx], state.puzzle[idx + 1]);
          state.moving = idx + 1;
          state.dir = DIR_RIGHT;
        }

        if (-1 != state.moving) {
          stage.push(&Game::sMovePiece);
          state.moves += 1;
        }
      }
    }
  }

  void sShuffle(int s, sw2::uint_ptr)
  {
    if (JOIN == s) {
      int const count1[] = {SHUFFLE_1COUN3, SHUFFLE_1COUN4, SHUFFLE_1COUN5};
      state.counter = count1[state.size - SIZE3];
      state.counter1 = state.counter;
      state.lastmove1 = -1;
      state.shuffling = true;
    }

    if (LEAVE == s) {
      state.shuffling = false;
    }

    if (TRIGGER == s) {

      if (0 >= state.counter) {
        stage.pop();                    // Done, back to play.
        state.timerMovePiece = TIME_MOVING_PIECE;
        return;
      }

      int u = 8;
      int b = 2;
      int c = state.counter1 - state.counter;
      int c3 = state.counter1 / 5;
      if (c3 <= c && 4 * c3 > c) {
        state.timerMovePiece = b;
      } else if (c3 > c) {
        state.timerMovePiece = (int)(b + u * cos(c / (float)c3 * 3.1415f / 2.0f));
      } else if (4 * c3 < c) {
        state.timerMovePiece = (int)(b + u * sin((c - 4 * c3) / (float)c3 * 3.1415f / 2.0f));
      }

      while (true)
      {
        int move = rand() % 4;
        if (state.lastmove1 == move) {
          continue;
        }

        for (int i = 0; i < state.size * state.size; i++) {
          if (state.lack == state.puzzle[i]) {
            int col = i % state.size, row = i / state.size;
            if (DIR_UP == move && 0 < row) {
              std::swap(state.puzzle[i], state.puzzle[i - state.size]);
              state.moving = i;
              state.dir = DIR_DOWN;
            } else if (DIR_DOWN == move && state.size > row + 1) {
              std::swap(state.puzzle[i], state.puzzle[i + state.size]);
              state.moving = i;
              state.dir = DIR_UP;
            } else if (DIR_LEFT == move && 0 < col) {
              std::swap(state.puzzle[i], state.puzzle[i - 1]);
              state.moving = i;
              state.dir = DIR_RIGHT;
            } else if (DIR_RIGHT == move && state.size > col + 1) {
              std::swap(state.puzzle[i], state.puzzle[i + 1]);
              state.moving = i;
              state.dir = DIR_LEFT;
            }
            break;
          }
        }

        if (-1 != state.moving) {
          int const invmove[] = {DIR_DOWN, DIR_UP, DIR_RIGHT, DIR_LEFT};
          state.lastmove1 = invmove[move];
          state.counter -= 1;
          stage.push(&Game::sMovePiece);
          break;
        }
      }
    }
  }

  void sTitle(int s, sw2::uint_ptr t)
  {
    if (JOIN == s) {
      renderStage.popAndPush(&Game::sDrawTitle);
    }

    if (0 != t) {
      HANDLE_TOUCH const* ht = (HANDLE_TOUCH const*)t;
      if (TOUCH_BEGIN == ht->s) {
        UPoint pt(ht->x, ht->y);
        if (ptInUi(state.uiTitlePlay, pt)) { // Play.
          state.size = SIZE3;
          state.lack = state.size * state.size - 1;
          stage.popAndPush(&Game::sPlay);
        }
      }
    }
  }

  void sWin(int s, sw2::uint_ptr t)
  {
    if (JOIN == s) {
      state.timer = 0;
      state.win = true;
    }

    if (TRIGGER == s) {
      if (TIME_WIN > state.timer) {
        state.timer += 1;
      }
      if (0 == t) {
        if (AppT::getInst().isKeyPushed(GOOD_KEYS_ESCAPE)) {
          stage.popAndPush(&Game::sPlay);
        }
      } else {
        HANDLE_TOUCH const* ht = (HANDLE_TOUCH const*)t;
        if (TOUCH_BEGIN == ht->s) {
          stage.popAndPush(&Game::sPlay);
        }
      }
    }
  }

  void sPicture(int s, sw2::uint_ptr t)
  {
    if (JOIN == s) {
      renderStage.push(&Game::sDrawPicture);
      state.picSel = PICSEL_NONE;
      state.flower = false;
      state.picture = true;
    }

    if (TRIGGER == s) {
      if (AppT::getInst().isKeyPushed(GOOD_KEYS_ESCAPE)) {
        stage.pop();
        renderStage.pop();
        return;
      }
    }

    if (TRIGGER == s && 0 != t) {
      HANDLE_TOUCH const* ht = (HANDLE_TOUCH const*)t;
      if (TOUCH_BEGIN == ht->s) {

        UPoint pt(ht->x, ht->y);

        //
        // Back.
        //

        if (ptInUi(state.uiPlayBack, pt)) {
          stage.pop();
          renderStage.pop();
          return;
        }

        //
        // Set default selection.
        //

        if (ptInUi(state.uiPicDefault, pt)) {
          defaultSel();
          return;
        }

        //
        // Select picture from gallery.
        //

        if (ptInUi(state.uiPicGallery, pt)) {
          if (native->nativeLoadPicture(NATIVE_LOAD_PIC_FROM_FILE)) {
            defaultSel();
            return;
          }
        }

        //
        // Select picture from camera.
        //

#ifdef _android_
        if (ptInUi(state.uiPicCamera, pt)) {
          if (native->nativeLoadPicture(NATIVE_LOAD_PIC_FROM_CAMERA)) {
            defaultSel();
            return;
          }
        }
#endif

        //
        // Move or re-size selection.
        //

        URect const &rr = state.rcPicRender;

        URect rcRender = URect(rr.left, rr.top, rr.left + rr.right, rr.top + rr.bottom);
        if (rcRender.ptInRect(pt)) {

          good::gx::GLImage &img = AppT::getInst().mPicSel;

          //
          // Click in selection area. Get dragging mode.
          //

          URect const &rs = state.rcPicSel;

          int w = (int)(rs.right * rr.right / (float)img.getWidth());
          int h = (int)(rs.bottom * rr.bottom / (float)img.getHeight());
          int sx = (int)(rs.left * rr.right / (float)img.getWidth());
          int sy = (int)(rs.top * rr.bottom / (float)img.getHeight());

          if (URect(rr.left + sx + w - PICSEL_FRAME_SZ, rr.top + sy + h - PICSEL_FRAME_SZ, rr.left + sx + w, rr.top + sy + h).ptInRect(pt)) {
            state.picSel = PICSEL_RESIZE;
          } else if (URect(rr.left + sx, rr.top + sy, rr.left + sx + w, rr.top + sy + h).ptInRect(pt)) {
            state.picSel = PICSEL_MOVE;
          } else {
            return;
          }

          //
          // Enter dragging mode.
          //

          state.ptLastSel = pt;
          state.ptSelSav = UPoint(state.rcPicSel.left, state.rcPicSel.top);
          stage.push(&Game::sPictureSel);
        }
      }
    }
  }

  void sPictureSel(int s, sw2::uint_ptr t)
  {
    if (TRIGGER != s || 0 == t) {
      return;
    }

    HANDLE_TOUCH const* ht = (HANDLE_TOUCH const*)t;
    if (TOUCH_MOVE != ht->s) {
      stage.pop();
      state.picSel = PICSEL_NONE;
      return;
    }

    UPoint pt(ht->x, ht->y);

    good::gx::GLImage &img = AppT::getInst().mPicSel;
    URect const &rr = state.rcPicRender;
    URect &rs = state.rcPicSel;

    switch (state.picSel)
    {
    case PICSEL_MOVE:
      {
        rs.left = state.ptSelSav.x + (int)((pt.x - state.ptLastSel.x) * img.getWidth() / (float)rr.right);
        rs.top = state.ptSelSav.y + (int)((pt.y - state.ptLastSel.y) * img.getHeight() / (float)rr.bottom);

        if (0 > rs.left) {
          rs.left -= rs.left;
        }

        if (0 > rs.top) {
          rs.top -= rs.top;
        }

        if (rs.left + rs.right >= img.getWidth()) {
          rs.left -= rs.left + rs.right - img.getWidth();
        }

        if (rs.top + rs.bottom >= img.getHeight()) {
          rs.top -= rs.top + rs.bottom - img.getHeight();
        }
      }
      break;

    case PICSEL_RESIZE:
      {
        int left = rr.left + (int)(rs.left * rr.right / (float)img.getWidth());
        int top = rr.top + (int)(rs.top * rr.bottom / (float)img.getHeight());

        int w = (int)((pt.x - left) * img.getWidth() / (float)rr.right);
        int h = (int)((pt.y - top) * img.getHeight() / (float)rr.bottom);
        int delta = (w + h) / 2;

        if ((int)(PICSEL_FRAME_SZ * img.getWidth() / (float)rr.right) > delta ||
            img.getWidth() < rs.left + delta || img.getHeight() < rs.top + delta) {
          break;
        }

        rs.right = rs.bottom = delta;
      }
      break;
    }
  }
};

} // namespace BernieTotem

// end of Game.h
