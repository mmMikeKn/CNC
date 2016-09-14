// win_stepmotor_test.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "gcode.h"

uint8_t limitX_chk(void) { return FALSE; }
uint8_t limitY_chk(void) { return FALSE; }
uint8_t limitZ_chk(void) { return FALSE; }
uint8_t limits_chk(void) { return FALSE; }
void stepm_EmergeStop(void) {
}
void ili9320_Clear(int n) {}
void stepm_init() {}
long RTC_GetCounter() {return 0;}
int stepm_inProc() { return FALSE; }
int stepm_getRemSteps(int crd) {return 0;}
void scr_fontColor(int x, int y) {}
void scr_gotoxy(int x, int y) {}
void scr_printf(const char* str, ...) {}

int GlobalSteps[4]= {0,0,0,0};
int stepm_getCurGlobalStepsNum(int n) {
 return GlobalSteps[n];
}
void step_dump() {}
int isEncoderCorrection;

int extrudT_getTemperatureReal() { return 0; }
int kbd_getKey() { return -1; }
void stepm_stopAll();
void scr_puts(char *str) {}
void scr_clrEndl() {}
void stepm_stopAll() {}

FILE *f_in;
int f_open(FIL *f, char *fileName, int mode) { 
 f_in = fopen(fileName, "rt"); return 0;  
}
char *f_gets(char *str, int len, FIL *f) { return fgets(str, len, f_in); }
void f_close(FIL *f) { fclose(f_in); }

void win_showErrorWin() {}
int SD_errno;
void GUI_Rectangle(int a, int b,int c, int d, int e, int f) {}
void GUI_Line(int a, int b, int c, int s, int e) {}
char *str_trim(char *str) {
 for(int n = 0; str[n] != 0; n++) {
  if(str[n] == '\n' || str[n] == '\r') {
   str[n] = 0; break;
  }
 }
 return str;
}
int32_t lround(double v) { return (int32_t)v; }
void extrudT_setTemperature(int b) {}
int extrudT_isReady() { return 1; }
int extrudT_getTemperatureWait(void) { return 1; }

void stepm_addMove(uint32_t steps[4], uint32_t frq[4], uint8_t dir[4]) {
 for(int i = 0; i < 4; i++)
  GlobalSteps[i] += dir[i]? steps[i]:-(int32_t)steps[i];
 DBG("\n               GX:%.2f(%d) GY:%.2f(%d) GZ:%.2f(%d)", (double)GlobalSteps[0]/SM_X_STEPS_PER_MM, GlobalSteps[0], (double)GlobalSteps[1]/SM_Y_STEPS_PER_MM, GlobalSteps[1], (double)GlobalSteps[2]/SM_Z_STEPS_PER_MM, GlobalSteps[2]);
}

int32_t stepm_getRemainLines(void) {
	return 0;
}

uint32_t stepm_LinesBufferIsFull(void) {
	return 0;
}

void cnc_extruder_on(void) {
}


void cnc_extruder_t(int temperature, int isWait) {
}

void cnc_extruder_stop(void) {
}


static void initSmParam(void) {
 _smParam.smoothStartF_from0[0] = SM_SMOOTH_START_X*K_FRQ;
 _smParam.smoothStartF_from0[1] = SM_SMOOTH_START_Y*K_FRQ;
 _smParam.smoothStartF_from0[2] =  SM_SMOOTH_START_Z*K_FRQ;
 _smParam.smoothStopF_to0[0] = SM_SMOOTH_STOP_X*K_FRQ;
 _smParam.smoothStopF_to0[1] = SM_SMOOTH_STOP_Y*K_FRQ;
 _smParam.smoothStopF_to0[2] = SM_SMOOTH_STOP_Z*K_FRQ;
 _smParam.smoothAF[0]= SM_SMOOTH_DFEED_X*SM_X_STEPS_PER_MM*SM_SMOOTH_TFEED*K_FRQ/1000;
 _smParam.smoothAF[1]= SM_SMOOTH_DFEED_Y*SM_Y_STEPS_PER_MM*SM_SMOOTH_TFEED*K_FRQ/1000;
 _smParam.smoothAF[2]= SM_SMOOTH_DFEED_Z*SM_Z_STEPS_PER_MM*SM_SMOOTH_TFEED/1000*K_FRQ;
 _smParam.maxFeedRate[0] = SM_X_MAX_STEPS_PER_SEC*K_FRQ;
 _smParam.maxFeedRate[1] = SM_Y_MAX_STEPS_PER_SEC*K_FRQ;
 _smParam.maxFeedRate[2] = SM_Z_MAX_STEPS_PER_SEC*K_FRQ;
// _smParam.maxSpindleTemperature = MAX_SPINDEL_TEMPERATURE;
}


int main(int argc, char* argv[]) {
 initSmParam();
 cnc_gfile("D:\\CNC\\cnc_workspace\\win_stepmotor_test\\Screw Holder_export.ngc", GFILE_MODE_MASK_EXEC);
 return 0;
}

