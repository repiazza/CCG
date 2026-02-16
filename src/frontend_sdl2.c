#include <string.h>
#include <stdio.h>
#include <frontend_api.h>
#include <frontend_sdl2.h>
#include <sys_interface.h>

#ifdef USE_SDL2
  #include <SDL2/SDL.h>
#endif

static int gbFSDL2_ShouldQuit = FALSE;

#ifdef USE_SDL2
  static Uint32 gui32FSDL2_LastTicks = 0u;
#endif

static int iFSDL2_Init(void) {
  gbFSDL2_ShouldQuit = FALSE;
#ifdef USE_SDL2
  gui32FSDL2_LastTicks = SDL_GetTicks();
#endif
  return TRUE;
}

static void vFSDL2_Shutdown(void) {
  /*
   * MVP note:
   * frontend_sdl2 only resets its local state here.
   * SDL window/renderer destruction is still owned by legacy vSDL_MainQuit()
   * in card_game.c to avoid double-shutdown during incremental migration.
   */
  gbFSDL2_ShouldQuit = FALSE;
}

static void vFSDL2_BeginFrame(void) {
}

static void vFSDL2_EndFrame(void) {
}

static int iFSDL2_MapKeydownEvent(PSTRUCT_CCG_EVENT pstEvOut,
                                  int iSDLKey) {
  switch (iSDLKey) {
    case SDLK_RETURN:
    case SDLK_KP_ENTER: {
      pstEvOut->eType = CCG_EVT_CONFIRM;
      return TRUE;
    }
    case SDLK_ESCAPE: {
      pstEvOut->eType = CCG_EVT_CANCEL;
      return TRUE;
    }
    case SDLK_UP: {
      pstEvOut->eType = CCG_EVT_UP;
      return TRUE;
    }
    case SDLK_DOWN: {
      pstEvOut->eType = CCG_EVT_DOWN;
      return TRUE;
    }
    case SDLK_LEFT: {
      pstEvOut->eType = CCG_EVT_LEFT;
      return TRUE;
    }
    case SDLK_RIGHT: {
      pstEvOut->eType = CCG_EVT_RIGHT;
      return TRUE;
    }
    default: {
      break;
    }
  }

  return FALSE;
}

static int iFSDL2_PollEvent(PSTRUCT_CCG_EVENT pstEvOut) {
#ifdef USE_SDL2
  SDL_Event stSDLEvent;

  if (pstEvOut == NULL) {
    return FALSE;
  }

  memset(pstEvOut, 0x00, sizeof(STRUCT_CCG_EVENT));
  pstEvOut->eType = CCG_EVT_NONE;

  while (SDL_PollEvent(&stSDLEvent)) {
    switch (stSDLEvent.type) {
      case SDL_QUIT: {
        pstEvOut->eType = CCG_EVT_QUIT;
        gbFSDL2_ShouldQuit = TRUE;
        return TRUE;
      }
      case SDL_KEYDOWN: {
        if (iFSDL2_MapKeydownEvent(pstEvOut, stSDLEvent.key.keysym.sym)) {
          return TRUE;
        }
        break;
      }
      case SDL_MOUSEBUTTONDOWN: {
        if (stSDLEvent.button.button == SDL_BUTTON_LEFT) {
          pstEvOut->eType = CCG_EVT_CLICK;
          pstEvOut->iX = stSDLEvent.button.x;
          pstEvOut->iY = stSDLEvent.button.y;
          pstEvOut->iButton = stSDLEvent.button.button;
          return TRUE;
        }
        break;
      }
      default: {
        break;
      }
    }
  }
#endif

  return FALSE;
}

static float fFSDL2_GetDeltaTime(void) {
#ifdef USE_SDL2
  Uint32 ui32CurrentTicks;
  Uint32 ui32ElapsedTicks;

  ui32CurrentTicks = SDL_GetTicks();
  ui32ElapsedTicks = ui32CurrentTicks - gui32FSDL2_LastTicks;
  gui32FSDL2_LastTicks = ui32CurrentTicks;

  return ((float)ui32ElapsedTicks) / 1000.0f;
#else
  return 0.0f;
#endif
}

static int iFSDL2_ShouldQuit(void) {
  return gbFSDL2_ShouldQuit;
}

static const STRUCT_FRONTEND_API gkstFrontendSDL2Api = {
  iFSDL2_Init,
  vFSDL2_Shutdown,
  vFSDL2_BeginFrame,
  vFSDL2_EndFrame,
  iFSDL2_PollEvent,
  fFSDL2_GetDeltaTime,
  iFSDL2_ShouldQuit
};

const STRUCT_FRONTEND_API *pkstFSDL2_GetApi(void) {
  return &gkstFrontendSDL2Api;
}
