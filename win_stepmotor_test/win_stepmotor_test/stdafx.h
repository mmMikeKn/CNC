// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <math.h>

#define M_PI		3.14159265358979323846

#ifndef isnan
  #define isnan(x) ((x) != (x))
#endif

//#include <stdint.h>

typedef int int32_t;
typedef short int16_t;
typedef char int8_t;
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
typedef int32_t  s32;
typedef int16_t s16;
typedef int8_t  s8;
typedef unsigned long long int uint64_t; 
typedef long long int int64_t; 
typedef unsigned short u16;
typedef unsigned char u8;
typedef unsigned char uint8_t;
typedef unsigned char	BYTE;
typedef char XCHAR;

#define Yellow 0
#define Black 0
#define Green 0
#define Red 0
#define Cyan 0
#define Blue 0
#define White 0
#define KEY_A 1
#define KEY_B 2
#define KEY_C 3
#define KEY_0 10
#define KEY_1 11
#define KEY_7 17
#define KEY_8 18

void limits_init(void);
uint8_t limitX_chk(void);
uint8_t limitY_chk(void);
uint8_t limitZ_chk(void);

uint8_t limits_chk(void);

void ili9320_Clear(int n);
void gc_init(); 
void stepm_init();
long RTC_GetCounter();
int stepm_inProc();
int stepm_getRemSteps(int crd);
void scr_fontColor(int, int);
void scr_gotoxy(int, int);
void scr_printf(const char* str, ...);
int stepm_getCurGlobalStepsNum(int);
int extrudT_getTemperatureReal();
int kbd_getKey();
void stepm_stopAll();
void scr_puts(char *); 
void scr_clrEndl();

typedef FILE FIL;
typedef unsigned int FRESULT;
int f_open(FIL *f, char *fileName, int);
void f_close(FIL *f);
#define FA_READ 0
#define FR_OK 0
void win_showErrorWin();
extern int SD_errno;
void GUI_Rectangle(int, int,int, int, int, int);
void GUI_Line(int, int, int, int, int);
char *f_gets(char *str, int len, FIL *f);
char *str_trim(char *str);
void stepm_EmergeStop(void);
void stepm_addMove(uint32_t steps[], uint32_t frq[], uint8_t dir[]);
uint32_t stepm_LinesBufferIsFull(void);
int32_t stepm_getRemainLines(void);
int32_t lround(double v);
void extrudT_setTemperature(int);
int extrudT_getTemperatureWait(void);
int extrudT_isReady();

#define DBG(...) { printf(__VA_ARGS__); }
#define hypot _hypot
void step_dump();
extern int isEncoderCorrection;

// TODO: reference additional headers your program requires here
