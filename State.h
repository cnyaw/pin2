//
// Bernie Totem gameplay state.
// 2010/04/12 Waync created.
//

#pragma once

namespace BernieTotem {

typedef sw2::IntRect URect;
typedef sw2::IntPoint UPoint;

enum {

  //
  // Metrics.
  //

  FRAME_RATE = 60,
  SCREEN_W = 480,
  SCREEN_H = 320,
  MAXCOL = 5,
  MAXROW = 5,
  PIECE3 = 100,
  PIECE4 = 75,
  PIECE5 = 60,
  PUZZLEX = 30,
  PUZZLEY = (SCREEN_H - PIECE3 * 3)/2,
  PUZZLEW = PIECE3 * 3,
  PUZZLEH = PIECE3 * 3,

  //
  // Timer.
  //

  TIME_MOVING_PIECE = FRAME_RATE / 5,
  TIME_WIN = FRAME_RATE / 2,

  //
  // Game.
  //

  SIZE3 = 3,
  SIZE4 = 4,
  SIZE5 = 5,
  DIR_UP = 0,
  DIR_DOWN,
  DIR_LEFT,
  DIR_RIGHT,
  SHUFFLE_1COUN3 = 50,
  SHUFFLE_1COUN4 = 90,
  SHUFFLE_1COUN5 = 140,

  //
  // Touch.
  //

  TOUCH_BEGIN = 0,
  TOUCH_END,
  TOUCH_MOVE,

  //
  // Render.
  //

  PRACTICE_OFFSET_X = 350,
  PRACTICE_OFFSET_Y = 10,
  PRACTICE_BTN_W = 110,
  PRACTICE_BTN_H = 30,
  PRACTICE_SPACE_H = 12,
  COLOR_BKGND = 0xFF1AA1E1,
  COLOR_BTN1 = 0xFF9AE31A,
  COLOR_BTN2 = 0xFFFF6A00,
  COLOR_BTN3 = 0xFFFF0000,
  COLOR_BTN_BG = 0x30ffffff,

  //
  // Picture selection area opearation mode.
  //

  PICSEL_FRAME_SZ = 16,

  PICSEL_NONE = 0,
  PICSEL_MOVE,
  PICSEL_RESIZE,

  ENDEND
};

struct HANDLE_TOUCH
{
  int s;
  int x, y;
  HANDLE_TOUCH(int s_, int x_, int y_) : s(s_), x(x_), y(y_) {}
};

struct State
{
  //
  // General timer.
  //

  int timer;
  int timerMovePiece;
  int timerPlaying;

  //
  // Puzzle game.
  //

  int puzzle[MAXCOL * MAXROW];
  int size;                             // Current size.

  bool alnum;
  bool flower;
  bool picture;

  URect rcPicSel;                       // Offset and size of selelction.
  URect rcPicRender;                    // Offset and size to render.

  int picSel;
  UPoint ptSelSav, ptLastSel;

  bool playing;
  bool shuffling;
  bool win;
  int lack;                             // Lack piece.

  int moving;                           // Moving piece.
  int dir;                              // Moving dir.

  int counter, counter1;                // Moving count.
  int lastmove1, lastmove2;             // Moving dir, used for random shuffle.

  int moves;                            // Move counts.

  bool spyview;

  //
  // UI.
  //

  URect uiTitlePlay;
  URect uiPlaySize3, uiPlaySize4, uiPlaySize5;
  URect uiPlayNumber, uiPlayFlower, uiPlayPicture, uiPlayBack;
  URect uiPlayingSpy, uiPlayRand;
  URect uiPicGallery, uiPicDefault;
#ifdef _android_
  URect uiPicCamera;
#endif
};

} // namespace BernieTotem

// end of State.h
