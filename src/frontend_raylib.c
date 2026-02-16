#include <string.h>
#include <stdio.h>
#include <frontend_api.h>
#include <frontend_raylib.h>
#include <consts.h>

#define FRL_WINDOW_WIDTH  800
#define FRL_WINDOW_HEIGHT 600

#ifdef USE_RAYLIB
  #include <raylib.h>
#endif

static int gbFRL_ShouldQuit = FALSE;

static int iFRL_Init(void) {
  gbFRL_ShouldQuit = FALSE;

#ifdef USE_RAYLIB
  InitWindow(FRL_WINDOW_WIDTH, FRL_WINDOW_HEIGHT, "CCG - Raylib MVP");
  SetTargetFPS(60);
#endif

  return TRUE;
}

static void vFRL_Shutdown(void) {
#ifdef USE_RAYLIB
  if (IsWindowReady()) {
    CloseWindow();
  }
#endif
  gbFRL_ShouldQuit = FALSE;
}

static void vFRL_BeginFrame(void) {
#ifdef USE_RAYLIB
  BeginDrawing();
  ClearBackground(BLACK); /* ou outra cor simples */
#endif
}

static void vFRL_EndFrame(void) {
#ifdef USE_RAYLIB
  DrawText("CCG - Raylib MVP", 20, 20, 30, RAYWHITE);
  DrawText("Feche a janela para sair", 20, 60, 20, GRAY);
  EndDrawing();
#endif
}

static int iFRL_PollEvent(PSTRUCT_CCG_EVENT pstEvOut) {
#ifdef USE_RAYLIB
  if (pstEvOut == NULL) {
    return FALSE;
  }

  memset(pstEvOut, 0x00, sizeof(STRUCT_CCG_EVENT));
  pstEvOut->eType = CCG_EVT_NONE;

  if (WindowShouldClose()) {
    pstEvOut->eType = CCG_EVT_QUIT;
    gbFRL_ShouldQuit = TRUE;
    return TRUE;
  }

  if (IsKeyPressed(KEY_ENTER)) {
    pstEvOut->eType = CCG_EVT_CONFIRM;
    return TRUE;
  }

  if (IsKeyPressed(KEY_ESCAPE)) {
    pstEvOut->eType = CCG_EVT_CANCEL;
    return TRUE;
  }

  if (IsKeyPressed(KEY_UP)) {
    pstEvOut->eType = CCG_EVT_UP;
    return TRUE;
  }

  if (IsKeyPressed(KEY_DOWN)) {
    pstEvOut->eType = CCG_EVT_DOWN;
    return TRUE;
  }

  if (IsKeyPressed(KEY_LEFT)) {
    pstEvOut->eType = CCG_EVT_LEFT;
    return TRUE;
  }

  if (IsKeyPressed(KEY_RIGHT)) {
    pstEvOut->eType = CCG_EVT_RIGHT;
    return TRUE;
  }

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    pstEvOut->eType = CCG_EVT_CLICK;
    pstEvOut->iX = GetMouseX();
    pstEvOut->iY = GetMouseY();
    pstEvOut->iButton = MOUSE_LEFT_BUTTON;
    return TRUE;
  }
#else
  if (pstEvOut != NULL) {
    memset(pstEvOut, 0x00, sizeof(STRUCT_CCG_EVENT));
    pstEvOut->eType = CCG_EVT_NONE;
  }
#endif

  return FALSE;
}



static float fFRL_GetDeltaTime(void) {
#ifdef USE_RAYLIB
  return GetFrameTime();
#else
  return 0.0f;
#endif
}

static int iFRL_ShouldQuit(void) {
#ifdef USE_RAYLIB
  return (gbFRL_ShouldQuit || WindowShouldClose());
#else
  return gbFRL_ShouldQuit;
#endif
}

static const STRUCT_FRONTEND_API gkstFrontendRaylibApi = {
  iFRL_Init,
  vFRL_Shutdown,
  vFRL_BeginFrame,
  vFRL_EndFrame,
  iFRL_PollEvent,
  fFRL_GetDeltaTime,
  iFRL_ShouldQuit
};

const STRUCT_FRONTEND_API *pkstFRL_GetApi(void) {
  return &gkstFrontendRaylibApi;
}
