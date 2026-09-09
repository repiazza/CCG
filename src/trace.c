/**
 * trace.c
 *
 * Written by Renato Fermi <repiazza@gmail.com>
 *
 * Trace functions and global variables
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

#include <sys_interface.h>
#include <card_game.h>
#include <trace.h>

#if defined(LINUX) || defined(APPLE)
#include <sys/time.h>
#include <sys/types.h>
#include <pthread.h>
#else
#include <windows.h>
#include <process.h>
#endif

char gszTraceFile[2048];
char gszTraceFileDialog[2048];
char gszDebugLevel[32];
char gszConfFile[_MAX_PATH];
int gbTraceOnTerminal = FALSE;

#if defined(LINUX) || defined(APPLE)

static pthread_mutex_t gstTraceMutex = PTHREAD_MUTEX_INITIALIZER;

static void vTraceLock(void)
{
  pthread_mutex_lock(&gstTraceMutex);
}

static void vTraceUnlock(void)
{
  pthread_mutex_unlock(&gstTraceMutex);
}

#else

static CRITICAL_SECTION gstTraceMutex;
static INIT_ONCE gstTraceMutexInit = INIT_ONCE_STATIC_INIT;

static BOOL CALLBACK bInitTraceMutex(PINIT_ONCE pInitOnce,
                                     PVOID pParameter,
                                     PVOID *ppContext)
{
  (void)pInitOnce;
  (void)pParameter;
  (void)ppContext;

  InitializeCriticalSection(&gstTraceMutex);

  return TRUE;
}

static void vTraceLock(void)
{
  InitOnceExecuteOnce(&gstTraceMutexInit,
                      bInitTraceMutex,
                      NULL,
                      NULL);

  EnterCriticalSection(&gstTraceMutex);
}

static void vTraceUnlock(void)
{
  LeaveCriticalSection(&gstTraceMutex);
}

#endif

#ifdef _WIN32

int gettimeofday(struct timeval *tp, void *tzp)
{
  FILETIME ft;
  ULARGE_INTEGER li;
  unsigned __int64 t;

  (void)tzp;

  GetSystemTimeAsFileTime(&ft);

  li.LowPart = ft.dwLowDateTime;
  li.HighPart = ft.dwHighDateTime;

  t = li.QuadPart - 116444736000000000ULL;

  tp->tv_sec = (long)(t / 10000000ULL);
  tp->tv_usec = (long)((t % 10000000ULL) / 10);

  return 0;
}

#endif

static void vTraceMsgInternal(char *szMsg, int bNoNL)
{
  FILE *pfLog = NULL;
  char szDateTimeNow_us[128];
  time_t lTime;
  struct tm st_tm_Now;
  struct timeval tv;

  if (szMsg == NULL)
  {
    return;
  }

  time(&lTime);

#ifdef _WIN32
  if (localtime_s(&st_tm_Now, &lTime) != 0)
  {
    return;
  }
#else
  if (localtime_r(&lTime, &st_tm_Now) == NULL)
  {
    return;
  }
#endif

  if (gettimeofday(&tv, NULL) != 0)
  {
    return;
  }

  snprintf(szDateTimeNow_us,
           sizeof(szDateTimeNow_us),
           "[%02d/%02d/%04d %02d:%02d:%02d.%03ld] ",
           st_tm_Now.tm_mday,
           st_tm_Now.tm_mon + 1,
           st_tm_Now.tm_year + 1900,
           st_tm_Now.tm_hour,
           st_tm_Now.tm_min,
           st_tm_Now.tm_sec,
           (long)(tv.tv_usec / 1000));

  pfLog = fopen(gszTraceFile, "a+");

  if (pfLog == NULL)
  {
    if (gbTraceOnTerminal)
    {
      if (bNoNL)
      {
        fprintf(stdout, "%s", szMsg);
      }
      else
      {
        fprintf(stdout, "%s%s\n", szDateTimeNow_us, szMsg);
      }

      fflush(stdout);
    }

    return;
  }

  if (bNoNL)
  {
    fprintf(pfLog, "%s", szMsg);

    if (gbTraceOnTerminal)
    {
      fprintf(stdout, "%s", szMsg);
    }
  }
  else
  {
    fprintf(pfLog, "%s%s\n", szDateTimeNow_us, szMsg);

    if (gbTraceOnTerminal)
    {
      fprintf(stdout, "%s%s\n", szDateTimeNow_us, szMsg);
    }
  }

  fflush(pfLog);

  fclose(pfLog);
}

void vTraceMsgNoNL(char *szMsg)
{
  vTraceLock();

  vTraceMsgInternal(szMsg, TRUE);

  vTraceUnlock();

} /* vTraceMsgNoNL */

void vTraceMsg(char *szMsg)
{
  vTraceLock();

  vTraceMsgInternal(szMsg, FALSE);

  vTraceUnlock();

} /* vTraceMsg */

void _vTraceMsgDialog(char *szMsg, ...)
{
  FILE *pfLog = NULL;
  va_list args;

  if (!DEBUG_DIALOG || szMsg == NULL)
  {
    return;
  }

  vTraceLock();

  pfLog = fopen(gszTraceFileDialog, "a+");

  if (pfLog == NULL)
  {
    vTraceUnlock();
    return;
  }

  va_start(args, szMsg);

  vfprintf(pfLog, szMsg, args);

  va_end(args);

  if (gbTraceOnTerminal)
  {
    va_start(args, szMsg);

    vfprintf(stdout, szMsg, args);

    va_end(args);

    fflush(stdout);
  }

  fflush(pfLog);

  fclose(pfLog);

  vTraceUnlock();

} /* _vTraceMsgDialog */

void vTracePid(char *szMsg, int iMsgLen)
{
  char *pszMyMsg = NULL;
  int iNewMsgLen;
  int iPid;

  if (szMsg == NULL || iMsgLen < 0)
  {
    return;
  }

  iNewMsgLen = iMsgLen + 32;

  iPid = getpid();

  pszMyMsg = (char *)malloc((size_t)iNewMsgLen);

  if (pszMyMsg == NULL)
  {
    return;
  }

  snprintf(pszMyMsg,
           (size_t)iNewMsgLen,
           "%d %s",
           iPid,
           szMsg);

  vTraceMsg(pszMyMsg);

  free(pszMyMsg);

} /* vTracePid */

void _vTraceVarArgsFn(char *pszModuleName,
                      const int kiLine,
                      const char *kpszFunctionName,
                      const char *kpszFmt,
                      ...)
{
  va_list args;
  va_list args_terminal;

  FILE *pfLog = NULL;

  char szPath[_MAX_PATH + _MAX_PATH + 8];
  char szName[_MAX_PATH];
  char szExt[_MAX_PATH];
  char szFullTitle[_MAX_PATH + 16];
  char szMessage[2048];
  char szDbg[2048];

  time_t lTime;
  struct tm st_tm_Now;
  struct timeval tv;

  if (pszModuleName == NULL ||
      kpszFunctionName == NULL ||
      kpszFmt == NULL)
  {
    return;
  }

  memset(szPath, 0x00, sizeof(szPath));
  memset(szName, 0x00, sizeof(szName));
  memset(szExt, 0x00, sizeof(szExt));
  memset(szFullTitle, 0x00, sizeof(szFullTitle));
  memset(szMessage, 0x00, sizeof(szMessage));
  memset(szDbg, 0x00, sizeof(szDbg));

  time(&lTime);

#ifdef _WIN32
  if (localtime_s(&st_tm_Now, &lTime) != 0)
  {
    return;
  }
#else
  if (localtime_r(&lTime, &st_tm_Now) == NULL)
  {
    return;
  }
#endif

  if (gettimeofday(&tv, NULL) != 0)
  {
    return;
  }

  iDIR_SplitFilename(pszModuleName,
                     szPath,
                     szName,
                     szExt);

  snprintf(szFullTitle,
           sizeof(szFullTitle),
           "<%.9s%s:%d>",
           szName,
           szExt,
           kiLine);

  va_start(args, kpszFmt);

  vsnprintf(szMessage,
            sizeof(szMessage),
            kpszFmt,
            args);

  va_end(args);

  snprintf(szDbg,
           sizeof(szDbg),
           "[%02d/%02d/%04d %02d:%02d:%02d.%03ld]%-16.16s(%-24s): %s\n",
           st_tm_Now.tm_mday,
           st_tm_Now.tm_mon + 1,
           st_tm_Now.tm_year + 1900,
           st_tm_Now.tm_hour,
           st_tm_Now.tm_min,
           st_tm_Now.tm_sec,
           (long)(tv.tv_usec / 1000),
           szFullTitle,
           kpszFunctionName,
           szMessage);

  vTraceLock();

  pfLog = fopen(gszTraceFile, "a+");

  if (pfLog != NULL)
  {
    fputs(szDbg, pfLog);
    fflush(pfLog);
    fclose(pfLog);
  }

  if (gbTraceOnTerminal)
  {
    va_start(args_terminal, kpszFmt);

    vfprintf(stdout, szDbg, args_terminal);

    va_end(args_terminal);

    fflush(stdout);
  }

  vTraceUnlock();

} /* _vTraceVarArgsFn */

void vSetLogFileTitle(void)
{
  memset(gszTraceFile, 0, sizeof(gszTraceFile));

  snprintf(gszTraceFile,
           sizeof(gszTraceFile),
           "%s.log",
           gkpszProgramName);

  memset(gszTraceFileDialog, 0, sizeof(gszTraceFileDialog));

  snprintf(gszTraceFileDialog,
           sizeof(gszTraceFileDialog),
           "%s_dialog.log",
           gkpszProgramName);

} /* vSetLogFile */

void vInitLogs(char *pszTrace, const char *pszDebugLevel)
{
  char szPath[_MAX_PATH + 8];
  char szPath2[_MAX_PATH + 8];
  char szName[_MAX_PATH];
  char szExt[_MAX_PATH];

  memset(szPath, 0x00, sizeof(szPath));
  memset(szPath2, 0x00, sizeof(szPath2));
  memset(szName, 0x00, sizeof(szName));
  memset(szExt, 0x00, sizeof(szExt));

  if (bStrIsEmpty(pszTrace))
  {
    vSetLogFileTitle();

    iDIR_SplitFilename(gszTraceFile,
                       szPath,
                       szName,
                       szExt);

    snprintf(szPath,
             sizeof(szPath),
             "%s/log",
             gszBaseDir);

    snprintf(gszTraceFile,
             sizeof(gszTraceFile),
             "%s/%s%s",
             szPath,
             szName,
             szExt);
  }
  else
  {
    iDIR_SplitFilename(pszTrace,
                       szPath,
                       szName,
                       szExt);

    snprintf(gszTraceFile,
             sizeof(gszTraceFile),
             "%s",
             pszTrace);
  }

  if (strlen(szPath) > 1 &&
      szPath[strlen(szPath) - 1] == '/')
  {
    szPath[strlen(szPath) - 1] = 0;
  }

  if (iDIR_IsDir(szPath) <= 0)
  {
    if (!iDIR_MkDir(szPath))
    {
      fprintf(stderr,
              "E: Impossible create dir [%s]!\n%s\n",
              szPath,
              strerror(errno));

      exit(EXIT_FAILURE);
    }
  }

  if (pszDebugLevel)
  {
    snprintf(gszDebugLevel,
             sizeof(gszDebugLevel),
             "%s",
             pszDebugLevel);
  }

  iDIR_SplitFilename(gszTraceFileDialog,
                     szPath2,
                     szName,
                     szExt);

  snprintf(gszTraceFileDialog,
           sizeof(gszTraceFileDialog),
           "%s/%s%s",
           szPath,
           szName,
           szExt);

  if (DEBUG_MORE_MSGS)
  {
    vTraceVarArgsFn("Load OK=[%s]", gszTraceFile);
  }

} /* vInitLogs */

void vTraceMainLoopInit(void)
{
  vTraceVarArgsFn("===========================================");
  vTraceVarArgsFn("=====    ***   Init Main LOOP  ***    =====");
  vTraceVarArgsFn("=====    ***     Mode=%s       ***    =====",
                  gbSDL_Mode ? "SDL" : "CONSOLE");
  vTraceVarArgsFn("=====                                 =====");
}

void vTraceMainLoopEnd(void)
{
  vTraceVarArgsFn("=====                                 =====");
  vTraceVarArgsFn("=====    ***   End Main LOOP   ***    =====");
  vTraceVarArgsFn("=====    ***     Mode=%s       ***    =====",
                  gbSDL_Mode ? "SDL" : "CONSOLE");
  vTraceVarArgsFn("===========================================");
}