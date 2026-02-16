/**
 * card_game.c
 *
 * Written by Renato Fermi <repiazza@gmail.com>
 *
 * Description: Main source of the project
 */

#include <stdio.h>
#include <debuff.h>
#include <deck.h>
#include <monster.h>
#ifdef USE_SDL2
  #ifdef APPLE
    #define SDL_MAIN_HANDLED
  #endif
  #include <SDL2/SDL.h>
  #include <SDL2/SDL_ttf.h>
#endif
#include <player.h>
#include <dialog.h>
#include <input.h>
#include <trace.h>
#include <console_api.h>
#ifdef USE_SDL2
  #include <sdl_api.h>
  #include <frontend_sdl2.h>
#endif
#ifdef USE_RAYLIB
  #include <raylib.h>
  #include <frontend_raylib.h>
#endif
#include <sys_interface.h>
#include <card_game.h>
#include <game.h>
#include <welcome.h>
#include <consts.h>
#include <conf.h>
#include <screen.h>
#include <getopt.h>
#include <cmdline.h>
#include <xml.h>
#include <msg.h>
#include <frontend_api.h>
#include <frontend_terminal.h>

/** === Globals === */
char *gkpszProgramName;
int gbLogLevel = 1;
int giLevel;
int gbSDL_Mode;
ENUM_CCG_BACKEND geBackend;
char gszBaseDir[_MAX_PATH];

STRUCT_GLOBAL_PRM gstGlobalPrm;

typedef struct STRUCT_CMDLINE {
  char szTrace[_MAX_PATH];
  char szDebugLevel[32];
  char szConfDir[_MAX_PATH+32];
  char szBaseDir[_MAX_PATH];
} STRUCT_CMDLINE, *PSTRUCT_CMDLINE;

STRUCT_CMDLINE gstCmdLine;

char* gpszOptStr = "hvt:d:sc:r";

#define CMDOPT_HELP     0
#define CMDOPT_VERSION  1
#define CMDOPT_SDL      4
#define CMDOPT_RAYLIB   7

/*
 *  Long Short has_arg pDataType exemple
 *   bSet default pData iDataSize
 *  Help
 */
STRUCT_COMMANDLINE_OPTIONS astCmdOpt[] = {
  /* 00 */
  { "help", 'h', no_argument, CMDLINETYPE_NULL, "",
     FALSE,  "",  NULL,        0,
    "Show this message and exit"
  },
  /* 01 */
  { "version", 'v', no_argument, CMDLINETYPE_NULL, "",
     FALSE,     "",  NULL,        0,
    "Show the version and exit"
  },
  /* 02 */
  { "trace", 't',    required_argument, CMDLINETYPE_STRING, "<number>",
    FALSE,   "", gstCmdLine.szTrace,     sizeof(gstCmdLine.szTrace),
    "<number> Set the trace file path"
  },
  /* 03 */
  { "debug", 'd',    required_argument, CMDLINETYPE_STRING, "<number>",
     FALSE,   "", gstCmdLine.szDebugLevel,     sizeof(gstCmdLine.szDebugLevel),
    "<number> Set the debug level (disable by default)"
  },
  /* 04 */
  { "sdl",   's', no_argument, CMDLINETYPE_NULL, "",
     FALSE,   "",  NULL,        0,
    "Start program in GUI mode (disable by default)"
  },
  /* 05 */
  { "conf-dir",   'c', required_argument,    CMDLINETYPE_NULL, "",
    FALSE,         "",  gstCmdLine.szConfDir, sizeof(gstCmdLine.szConfDir),
    "Set the path of conf directory"
  },
  /* 06 */
  { "base-dir",   'b', optional_argument,    CMDLINETYPE_NULL, "",
    FALSE,         "",  gstCmdLine.szBaseDir, sizeof(gstCmdLine.szBaseDir),
    "Set the root path of the project"
  },
  /* 07 */
  { "raylib", 'r', no_argument, CMDLINETYPE_NULL, "",
    FALSE,      "", NULL,        0,
    "Start program in Raylib GUI mode (disable by default)"
  },
  { NULL, 0, no_argument, CMDLINETYPE_NULL, NULL,
     FALSE, NULL, NULL, 0,
    NULL
  }
};

int icbackCCGXml(xmlNodePtr pstNode, void* pData __attribute__((unused)));
STRUCT_XML astCCGXml[] = {
  { "CCG"              , XMLTYPE_PROC  , 0                                     , NULL                          , icbackCCGXml    },
  { "TRACE"            , XMLTYPE_STRING, sizeof(gstGlobalPrm.szTrace)          , gstGlobalPrm.szTrace          , NULL            },
  { "DEBUG_LEVEL"      , XMLTYPE_STRING, sizeof(gstGlobalPrm.szDebugLevel)     , gstGlobalPrm.szDebugLevel     , NULL            },
  { "WRK_DIR"          , XMLTYPE_STRING, sizeof(gstGlobalPrm.szWrkDir)         , gstGlobalPrm.szWrkDir         , NULL            },
  { "BASE_DIR"         , XMLTYPE_STRING, sizeof(gstGlobalPrm.szBaseDir)        , gstGlobalPrm.szBaseDir        , NULL            },
  { "FONT_DIR"         , XMLTYPE_STRING, sizeof(gstGlobalPrm.szFontsDir)       , gstGlobalPrm.szFontsDir       , NULL            },
  { "TRACE_ON_TERMINAL", XMLTYPE_STRING, sizeof(gstGlobalPrm.szTraceOnTerminal), gstGlobalPrm.szTraceOnTerminal, NULL            },
  { "ASSETS_DIR"       , XMLTYPE_STRING, sizeof(gstGlobalPrm.szAssetsDir)      , gstGlobalPrm.szAssetsDir      , NULL            },
  { NULL               , XMLTYPE_NULL  , 0                                     , NULL                          , NULL            }
};
int icbackCCGXml(xmlNodePtr pstNode, void* pData __attribute__((unused))) {
  if ( !strcasecmp((char*)pstNode->name, "CCG") ) {
    memset(&gstGlobalPrm, 0x00, sizeof(gstGlobalPrm));
    iParseXmlFields(pstNode, astCCGXml);
  }
  return 1;
}

STRUCT_CONF_FILE astConfFile[] = {
  { "trace"      , gstGlobalPrm.szTrace     , sizeof(gstGlobalPrm.szTrace)     , DATATYPE_STRING, "log/card_game.log" },
  { "debug-level", gstGlobalPrm.szDebugLevel, sizeof(gstGlobalPrm.szDebugLevel), DATATYPE_STRING, "9"                 },
  { "wrk-dir"    , gstGlobalPrm.szWrkDir    , sizeof(gstGlobalPrm.szWrkDir)    , DATATYPE_STRING, "./"                },
  { "fonts-dir"  , gstGlobalPrm.szFontsDir  , sizeof(gstGlobalPrm.szFontsDir)  , DATATYPE_STRING, "./fonts"           },
  { "conf-dir"   , gstGlobalPrm.szConfDir   , sizeof(gstGlobalPrm.szConfDir)   , DATATYPE_STRING, "./conf"            },
  { NULL         , NULL                     , 0                                , 0              , NULL                }
};

/** === Procedures === */

int iSetBaseDir(void){
  char *pszRootDir = NULL;
  
  memset(gszBaseDir, 0x00, sizeof(gszBaseDir));
  if ( (pszRootDir = getenv("CCG_ROOT_DIR")) != NULL && iDIR_IsDir(pszRootDir) ){
    snprintf(gszBaseDir, sizeof(gszBaseDir), "%s", pszRootDir);
    return 0;
  }
  if ( !bStrIsEmpty(gstGlobalPrm.szBaseDir) && iDIR_IsDir(gstGlobalPrm.szBaseDir) ){
    snprintf(gszBaseDir, sizeof(gszBaseDir), "%s", gstGlobalPrm.szBaseDir);
    return 1;
  }
  if ( !bStrIsEmpty(gstCmdLine.szBaseDir) && iDIR_IsDir(gstCmdLine.szBaseDir) ){
    snprintf(gszBaseDir, sizeof(gszBaseDir), "%s", gstCmdLine.szBaseDir);
    return 2;
  }
  
  return -1;
}

void vFreeProgramName(){
  if (gkpszProgramName != NULL)
    free(gkpszProgramName);
}
void vSetProgramName(char *argv[]){
  char szPath[_MAX_PATH];
  char szName[_MAX_PATH];
  char szExt [_MAX_PATH];

  iDIR_SplitFilename(argv[0], szPath, szName, szExt);

  if ( !bStrIsEmpty(szName) ) {
    gkpszProgramName = (char *)malloc(strlen(szName) + 1);
    memset(gkpszProgramName, 0, strlen(szName) + 1);
    strcpy(gkpszProgramName, szName);
  }
}

int bInitGlobals(void) {
  char szConfFile[_MAX_PATH+_MAX_PATH] = "";

  memset(szConfFile   , 0x00, sizeof(szConfFile));

  if ( DEBUG_MSGS ) vTraceBegin();

  snprintf(szConfFile   , sizeof(szConfFile), "%s%cccg.xml", gstGlobalPrm.szConfDir, DIR_SEPARATOR);

  giLevel = 1;
  gstGame.iLevel = giLevel;
  bGameSetStatus(STATUS_WELCOME);
  bGameSetState(STATE_NONE);
  bGameSetState(STATE_WELCOME_BEGIN);

  snprintf(gszDebugLevel, sizeof(gszDebugLevel), "%c", DEBUG_LVL_DETAILS);
  gbSDL_Mode = FALSE;
  geBackend = CCG_BACKEND_TERMINAL;
  gkpszProgramName = NULL;

  if ( !bLoadXmlFromFile(szConfFile, astCCGXml) ) {
    snprintf(szConfFile   , sizeof(szConfFile), "%s%cccg.conf", gstCmdLine.szConfDir, DIR_SEPARATOR);
    if ( !bReadConfFile(szConfFile, astConfFile) ) {
      fprintf(stderr, "Falha ao carregar parametros globais!\n");
      return 0;
    }
  }

  if ( !bStrIsEmpty(gstCmdLine.szTrace) ) {
    snprintf(gstGlobalPrm.szTrace, sizeof(gstGlobalPrm.szTrace), "%s", gstCmdLine.szTrace);
  }

  if ( !bStrIsEmpty(gstCmdLine.szDebugLevel) ) {
    snprintf(gstGlobalPrm.szDebugLevel, sizeof(gstGlobalPrm.szDebugLevel), "%s", gstCmdLine.szDebugLevel);
  }

  snprintf(gszDebugLevel, sizeof(gszDebugLevel), "%s", gstGlobalPrm.szDebugLevel);

  if ( !bStrIsEmpty(gstGlobalPrm.szTraceOnTerminal) ) {
    gbTraceOnTerminal = atoi(gstGlobalPrm.szTraceOnTerminal);
  }

  if ( DEBUG_MSGS ) vTraceEnd();

  return 1;
}

static void vShowVersion(void) {
  printf(
    "%s %s %s [%s %s]\n",
    gkpszProgramName,
    CCG_VERSION,
    COPYRIGTH,
    __DATE__,
    __TIME__
  );
#ifdef BUILD_OS_NAME
  printf(
    "%s %s %s %s\n"
    "build with %s %s\n",
    BUILD_OS_NAME,
    BUILD_OS_VERSION,
    BUILD_OS_ARCH,
    BUILD_HOSTNAME,
    BUILD_COMPILER,
    BUILD_COMPILER_VERSION
  );
#endif
}

#ifdef USE_SDL2
int iSDL_OpenShop(SDL_Renderer *pSDL_Renderer, PSTRUCT_PLAYER pstPlayer, PSTRUCT_DECK pstDeck);
#endif

/**
 *
 *  Main
 *
 *  CCG_Main is a Macro defined to use SDL_Main when
 *  USE_SDL2 is defined.
 *
 *  Otherwise(console mode) uses traditional main()
 *
 */
int CCG_Main(int argc, char *argv[]){
  STRUCT_DECK stDeck;
  STRUCT_MONSTER astMonsters[MAX_MONSTERS];
  int bRunning = TRUE;
  int iMonsterCount;
  const STRUCT_FRONTEND_API *pkstFrontendApi;
#ifdef USE_RAYLIB
  STRUCT_CCG_EVENT stFrontendEvent;
  int iHasEvent;
#endif
#ifdef USE_SDL2
  SDL_Window *pSDL_Wndw = NULL;
  SDL_Renderer *pSDL_Rnd = NULL;
  SDL_Event SDL_Ev;
#endif
  char* pszRootDir = NULL;

  memset(&gstGlobalPrm, 0x00, sizeof(gstGlobalPrm));
  memset(&gstGame     , 0x00, sizeof(gstGame     ));
  memset(&gstCmdLine  , 0x00, sizeof(gstCmdLine  ));
  pkstFrontendApi = NULL;
#ifdef USE_RAYLIB
  memset(&stFrontendEvent, 0x00, sizeof(stFrontendEvent));
  iHasEvent = FALSE;
#endif

  vSetProgramName(argv);
  /**
   * TODO: Rever logica default '.'
   */
  if ( iSetBaseDir() < 0 ){
    sprintf(gszBaseDir, "%s", ".");
  }
  pszRootDir = gszBaseDir;

  snprintf(gstCmdLine.szConfDir, sizeof(gstCmdLine.szConfDir), "%s%cconf", pszRootDir, DIR_SEPARATOR);

  /** Reading cmdline options like using SDL2 or not, debug level ... */
  if ( !bParseCommandLine(argc, argv, astCmdOpt) ) {
    vPrintUsage(argv, astCmdOpt);
    return -1;
  }

  if ( !bStrIsEmpty(gstCmdLine.szConfDir) ) {
    snprintf(gstGlobalPrm.szConfDir, sizeof(gstGlobalPrm.szConfDir), "%s", gstCmdLine.szConfDir);
  }

  if ( !bInitGlobals() ) {
    return -1;
  }

  if ( astCmdOpt[CMDOPT_HELP].bSet ) {
    vPrintUsage(argv, astCmdOpt);
    return 0;
  }

  if ( astCmdOpt[CMDOPT_VERSION].bSet ) {
    vShowVersion();
    return 0;
  }

  if ( astCmdOpt[CMDOPT_SDL].bSet && astCmdOpt[CMDOPT_RAYLIB].bSet ) {
    fprintf(stderr, "Conflito de argumentos: use apenas --sdl ou --raylib.\n");
    return -1;
  }

  if ( astCmdOpt[CMDOPT_SDL].bSet ) {
    geBackend = CCG_BACKEND_SDL2;
  }
  if ( astCmdOpt[CMDOPT_RAYLIB].bSet ) {
    geBackend = CCG_BACKEND_RAYLIB;
  }

  if ( geBackend == CCG_BACKEND_SDL2 ) {
    #ifndef USE_SDL2
      fprintf(stderr, "Backend SDL2 solicitado, mas este binario nao foi compilado com USE_SDL2.\n");
      return -1;
    #endif
    gbSDL_Mode = TRUE; /** legacy mirror of geBackend */
    #ifdef USE_SDL2
      pkstFrontendApi = pkstFSDL2_GetApi();
      if (pkstFrontendApi == NULL || pkstFrontendApi->piInit == NULL) {
        fprintf(stderr, "Falha ao carregar frontend SDL2.\n");
        return -1;
      }
      if (pkstFrontendApi->piInit() == FALSE) {
        fprintf(stderr, "Falha ao inicializar frontend SDL2.\n");
        return -1;
      }
    #endif
  }

  if ( geBackend == CCG_BACKEND_TERMINAL ) {
    /* MVP step: terminal frontend is initialized for future event unification; console loop still uses vCNSL_MainLoop. */
    pkstFrontendApi = pkstFTERM_GetApi();
    if (pkstFrontendApi == NULL || pkstFrontendApi->piInit == NULL) {
      fprintf(stderr, "Falha ao carregar frontend terminal.\n");
      return -1;
    }
    if (pkstFrontendApi->piInit() == FALSE) {
      fprintf(stderr, "Falha ao inicializar frontend terminal.\n");
      return -1;
    }
  }

  if ( geBackend == CCG_BACKEND_RAYLIB ) {
    #ifndef USE_RAYLIB
      fprintf(stderr, "Backend Raylib solicitado, mas este binario nao foi compilado com USE_RAYLIB.\n");
      return -1;
    #else
      pkstFrontendApi = pkstFRL_GetApi();
      if (pkstFrontendApi == NULL || pkstFrontendApi->piInit == NULL) {
        fprintf(stderr, "Falha ao carregar frontend Raylib.\n");
        return -1;
      }
      if (pkstFrontendApi->piInit() == FALSE) {
        fprintf(stderr, "Falha ao inicializar frontend Raylib.\n");
        return -1;
      }

      fprintf(stdout, "Raylib MVP inicializado. Feche a janela para sair.\n");
      while (!pkstFrontendApi->piShouldQuit()) {
        iHasEvent = pkstFrontendApi->piPollEvent(&stFrontendEvent);
        if (iHasEvent && stFrontendEvent.eType == CCG_EVT_QUIT) {
          break;
        }
        WaitTime(0.01);
      }

      if (pkstFrontendApi->pfnShutdown != NULL) {
        pkstFrontendApi->pfnShutdown();
      }
      return 0;
    #endif
  }
  
  vInitLogs(gstGlobalPrm.szTrace, gstGlobalPrm.szDebugLevel);

  if ( !bLoadMsgXml() ) {
    fprintf(stderr, "Erro ao ler o arquivo msg.xml!\n");
    return 0;
  }
     
  #ifdef USE_SDL2
    if ( gbSDL_Mode ) {
      if ( !bLoadScreenXml() ) {
        return -1;
      }
      vSDL_SetupMain(&pSDL_Rnd, &pSDL_Wndw);
    }
    else
      vShowInitDialog();
  #else
    vShowInitDialog();
  #endif

  #ifdef USE_SDL2
    if ( gbSDL_Mode ){
      if ( iSDL_OpenWelcome(pSDL_Rnd) == FINISH_PROGRAM ) {
        vSDL_MainQuit(&pSDL_Wndw);
        return 0;
      }
    }
  #endif

  vInitBasicDeck(&stDeck);
  iDrawMultipleCard(INIT_HAND_CARDS, &stDeck);
  vInitPlayer(&stDeck, !gbSDL_Mode);
  vInitMonstersForLevel(astMonsters, giLevel, &iMonsterCount);
  vInitDialog();
  
  if ( gbLoadGameFromFile )
    vGameLoadCtxVars(&gstGame.stGameContext, &stDeck, &gstPlayer, astMonsters, &iMonsterCount);
  
    
  #ifdef FAKE
    vFakeOpenShopEarly(&stDeck);
  #endif

  if ( DEBUG_MORE_MSGS ) vTraceMainLoopInit(); 

  #ifdef USE_SDL2
    if ( gbSDL_Mode ){
      vSDL_DrawBegin(pSDL_Rnd, &stDeck, astMonsters, iMonsterCount);
      vSDL_MainLoop(&bRunning, &SDL_Ev, pSDL_Rnd, &stDeck, astMonsters, iMonsterCount);
    }
    else 
      vCNSL_MainLoop(&bRunning, &stDeck, astMonsters, iMonsterCount);
  #else
    vCNSL_MainLoop(&bRunning, &stDeck, astMonsters, iMonsterCount);
  #endif

  if ( DEBUG_MORE_MSGS ) vTraceMainLoopEnd();
  
  vFreeDialog();
  vFreeProgramName();

  if ( pkstFrontendApi != NULL &&
       pkstFrontendApi->pfnShutdown != NULL ) {
    pkstFrontendApi->pfnShutdown();
  }
  
  #ifdef USE_SDL2
    if ( gbSDL_Mode ) 
      vSDL_MainQuit(&pSDL_Wndw);
  #endif

  return 0;
}
