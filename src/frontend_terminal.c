#include <stdio.h>
#include <string.h>
#include <frontend_api.h>
#include <frontend_terminal.h>
#include <input.h>
#include <sys_interface.h>

static int iFTERM_Init(void) {
  return TRUE;
}

static void vFTERM_Shutdown(void) {
}

static void vFTERM_BeginFrame(void) {
}

static void vFTERM_EndFrame(void) {
}

static int iFTERM_PollEvent(PSTRUCT_CCG_EVENT pstEvOut) {
  int iKey;

  if (pstEvOut == NULL) {
    return FALSE;
  }

  memset(pstEvOut, 0x00, sizeof(STRUCT_CCG_EVENT));
  pstEvOut->eType = CCG_EVT_NONE;

  iKey = iPortableGetchar();

  switch (iKey) {
    case '\n':
    case '\r': {
      pstEvOut->eType = CCG_EVT_CONFIRM;
      break;
    }
    case 27: {
      pstEvOut->eType = CCG_EVT_CANCEL;
      break;
    }
    case -1: {
      pstEvOut->eType = CCG_EVT_UP;
      break;
    }
    case -2: {
      pstEvOut->eType = CCG_EVT_DOWN;
      break;
    }
    case -3: {
      pstEvOut->eType = CCG_EVT_RIGHT;
      break;
    }
    case -4: {
      pstEvOut->eType = CCG_EVT_LEFT;
      break;
    }
    case 'q':
    case 'Q': {
      pstEvOut->eType = CCG_EVT_QUIT;
      break;
    }
    default: {
      break;
    }
  }

  return (pstEvOut->eType != CCG_EVT_NONE);
}

static float fFTERM_GetDeltaTime(void) {
  return 0.0f;
}

static int iFTERM_ShouldQuit(void) {
  return FALSE;
}

static const STRUCT_FRONTEND_API gkstFrontendTerminalApi = {
  iFTERM_Init,
  vFTERM_Shutdown,
  vFTERM_BeginFrame,
  vFTERM_EndFrame,
  iFTERM_PollEvent,
  fFTERM_GetDeltaTime,
  iFTERM_ShouldQuit
};

const STRUCT_FRONTEND_API *pkstFTERM_GetApi(void) {
  return &gkstFrontendTerminalApi;
}
