#ifndef FRONTEND_API_H
  #define FRONTEND_API_H

  typedef enum ENUM_CCG_EVT {
    CCG_EVT_NONE = 0,
    CCG_EVT_CONFIRM,
    CCG_EVT_CANCEL,
    CCG_EVT_UP,
    CCG_EVT_DOWN,
    CCG_EVT_LEFT,
    CCG_EVT_RIGHT,
    CCG_EVT_CLICK,
    CCG_EVT_QUIT
  } ENUM_CCG_EVT;

  typedef struct STRUCT_CCG_EVENT {
    ENUM_CCG_EVT eType;
    int iX;
    int iY;
    int iButton;
  } STRUCT_CCG_EVENT, *PSTRUCT_CCG_EVENT;

  typedef struct STRUCT_CCG_RECT {
    int iX;
    int iY;
    int iW;
    int iH;
  } STRUCT_CCG_RECT, *PSTRUCT_CCG_RECT;

  typedef struct STRUCT_CCG_COLOR {
    unsigned char ucR;
    unsigned char ucG;
    unsigned char ucB;
    unsigned char ucA;
  } STRUCT_CCG_COLOR, *PSTRUCT_CCG_COLOR;

  typedef int (*PI_FRONTEND_INIT)(void);
  typedef void (*PFN_FRONTEND_SHUTDOWN)(void);
  typedef void (*PFN_FRONTEND_BEGIN_FRAME)(void);
  typedef void (*PFN_FRONTEND_END_FRAME)(void);
  typedef int (*PI_FRONTEND_POLL_EVENT)(PSTRUCT_CCG_EVENT pstEvOut);
  typedef float (*PF_FRONTEND_GET_DELTA_TIME)(void);
  typedef int (*PI_FRONTEND_SHOULD_QUIT)(void);

  typedef struct STRUCT_FRONTEND_API {
    PI_FRONTEND_INIT piInit;
    PFN_FRONTEND_SHUTDOWN pfnShutdown;
    PFN_FRONTEND_BEGIN_FRAME pfnBeginFrame;
    PFN_FRONTEND_END_FRAME pfnEndFrame;
    PI_FRONTEND_POLL_EVENT piPollEvent;
    PF_FRONTEND_GET_DELTA_TIME pfGetDeltaTime;
    PI_FRONTEND_SHOULD_QUIT piShouldQuit;
  } STRUCT_FRONTEND_API, *PSTRUCT_FRONTEND_API;

#endif
