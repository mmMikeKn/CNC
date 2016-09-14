#include <stdlib.h>
#include "global.h"

void manualMode(void) {
	static const char axisName[4] = "XYZE";
#ifdef HAS_HWD_LIMITS
	uint8_t limits = !limits_chk();
#endif
	static const double axisK[4] = {SM_X_STEPS_PER_MM,SM_Y_STEPS_PER_MM, SM_Z_STEPS_PER_MM, SM_E_STEPS_PER_MM};
	int i, st[4] = {SM_X_STEPS_PER_MM, SM_Y_STEPS_PER_MM, SM_Z_STEPS_PER_MM, SM_E_STEPS_PER_MM};
	double k = 1.5;
	uint32_t frq[4] = {SM_MANUAL_MODE_STEPS_X_PER_SEC*K_FRQ,SM_MANUAL_MODE_STEPS_Y_PER_SEC*K_FRQ,
			                 SM_MANUAL_MODE_STEPS_Z_PER_SEC*K_FRQ,SM_MANUAL_MODE_STEPS_E_PER_SEC*K_FRQ};
	uint8_t dir[4] = {0,0,0,0};
	uint32_t steps[4] = {0,0,0,0};

	ili9320_Clear(0); scr_setfullTextWindow();
	scr_gotoxy(1,0); scr_fontColor(Green,Black);
	scr_puts("MANUAL MODE");
	scr_fontColor(Yellow,Black);
	scr_puts("\n  key '6' - [X+]  key '4' - [X-] ");
	scr_puts("\n  key '2' - [Y+]  key '8' - [Y-] ");
	scr_puts("\n  key '1' - [Z+]  key '7' - [Z-] ");
#ifdef HAS_EXTRUDER
	scr_puts("\n  key '3' - [E+]  key '9' - [E-] ");
	_destExtruderT = _smParam.defExtruderTemperature;
#endif
	scr_puts("\n  'A'+.1 'B'-.1 '*'k=1 '#'k+.5");
	scr_puts("\n 0-zero  5-goto'0' C-exit D-stop");
	delayMs(700);
	while(kbd_getKey()>=0) {};
	while(TRUE) {
#ifdef HAS_EXTRUDER
		for(i = 0; i < 4; i++) {
#else
		for(i = 0; i < 3; i++) {
#endif
			double n = (double)stepm_getCurGlobalStepsNum(i)/axisK[i];
			scr_fontColor(Yellow,Black);
			scr_gotoxy(1+i*10,TEXT_Y_MAX-5); scr_printf("%c:%f  ", axisName[i], n);
			scr_fontColor(White,Black);
		 scr_gotoxy(1,TEXT_Y_MAX-4+i); scr_printf("steps %c:%d      ", axisName[i], stepm_getCurGlobalStepsNum(i));
#ifdef HAS_ENCODER
   if(i == 0) {
   	int32_t v = encoderXvalue();
	   scr_gotoxy(25,TEXT_Y_MAX-3); scr_printf("encX:%d    ", v);
	   scr_gotoxy(1,TEXT_Y_MAX-6); scr_printf("errX:%f  ", (double)(v*MM_PER_360)/ENCODER_X_CNT_PER_360 - n);
   }
   if(i == 2) {
   	int32_t v = encoderZvalue();
	   scr_gotoxy(25,TEXT_Y_MAX-2); scr_printf("encZ:%d    ", v);
	   scr_gotoxy(1+2*10,TEXT_Y_MAX-6); scr_printf("errZ:%f  ", (double)(v*MM_PER_360)/ENCODER_Z_CNT_PER_360 - n);
   }
#endif
#ifdef HAS_EXTRUDER
   {
   static uint8_t sec = 0;
   RTC_t rtc;
   rtc_gettime(&rtc);
   if(rtc.sec != sec) {
   	scr_gotoxy(13,0); scr_fontColor(White,Black);
 	 	scr_printf("T[%d]:%d->%d PWR:%d.%d ", _temperatureMAX31855_status, _temperatureHotEnd, _destExtruderT,
 	 			_hotEndPwrPWM/10, _hotEndPwrPWM%10);
 			scr_fontColor(White,Black);
    sec = rtc.sec;
   }
   }
#endif
  	dir[i] = 0;
  	steps[i] = 0;
		}
		scr_fontColor(Blue,Black);
	 scr_gotoxy(2,TEXT_Y_MAX-8); scr_printf("step per key press: %f mm ", k);

		switch(kbd_getKey()) {
		 case KEY_0:
		  while(stepm_inProc() && kbd_getKey() != KEY_C) {}
		  stepm_ZeroGlobalCrd();
#ifdef HAS_ENCODER
		  encodersReset();
#endif
		  break;
		 case KEY_6:
		 	if(stepm_getRemainLines() < 2) {
		 	 steps[0] = k*st[0]; dir[0] = 1; stepm_addMove(steps, frq, dir);
		 	}
		 	break;
		 case KEY_4:
		 	if(stepm_getRemainLines() < 2) {
     steps[0] = k*st[0]; dir[0] = 0; stepm_addMove(steps, frq, dir);
		 	}
		 	break;
		 case KEY_2:
		 	if(stepm_getRemainLines() < 2) {
		 	 steps[1] = k*st[1]; dir[1] = 1; stepm_addMove(steps, frq, dir);
		 	}
		 	break;
		 case KEY_8:
		 	if(stepm_getRemainLines() < 2) {
		 	 steps[1] = k*st[1]; dir[1] = 0; stepm_addMove(steps, frq, dir);
		 	}
		 	break;
		 case KEY_1:
		 	if(stepm_getRemainLines() < 2) {
		 	 steps[2] = k*st[2]; dir[2] = 1; stepm_addMove(steps, frq, dir);
		 	}
		 	break;
		 case KEY_7:
		 	if(stepm_getRemainLines() < 2) {
		 	 steps[2] = k*st[2]; dir[2] = 0; stepm_addMove(steps, frq, dir);
		 	}
		 	break;
#ifdef HAS_EXTRUDER
		 case KEY_3: steps[3] = k*st[3]; dir[3] = 1; stepm_addMove(steps, frq, dir); break;
		 case KEY_9: steps[3] = k*st[3]; dir[3] = 0; stepm_addMove(steps, frq, dir); break;
#endif
		 case KEY_5:
		  while(stepm_inProc() && kbd_getKey() != KEY_C) {}
		  if(stepm_getCurGlobalStepsNum(0) != 0 || stepm_getCurGlobalStepsNum(1) != 0) {
	 	 	steps[0] = labs(stepm_getCurGlobalStepsNum(0)); dir[0] = stepm_getCurGlobalStepsNum(0) < 0;
	 	 	steps[1] = labs(stepm_getCurGlobalStepsNum(1)); dir[1] = stepm_getCurGlobalStepsNum(1) < 0;
	 	 	steps[2] = 0; dir[2] = 0;
		  } else {
		  	steps[0] = steps[1] = 0; dir[0] = dir[1] = 0;
		  	steps[2] = labs(stepm_getCurGlobalStepsNum(2)); dir[2] = stepm_getCurGlobalStepsNum(2) < 0;
		 	}
	 	 stepm_addMove(steps, frq, dir);
		 	break;
		 case KEY_A:
		 	if(k < 2.0) k+=0.1;
		 	break;
		 case KEY_B:
		 	if(k > 0.1) k-=0.1;
		 	break;
		 case KEY_STAR:
		 	k = 1.0;
		 	break;
		 case KEY_DIES:
		 	if(k < 2.0) k+=0.5;
		 	break;
		 case KEY_C:
		 	stepm_EmergeStop();
		 	return;
		 case KEY_D:
		 	stepm_EmergeStop();
		 	break;
		}
#ifdef HAS_HWD_LIMITS
	 if(limits != limits_chk()) {
	 	limits = limits_chk();
	 	GUI_Rectangle(304, 232, 309, 239, limitX_chk()? Red:Green, TRUE);
	 	GUI_Rectangle(310, 232, 315, 239, limitY_chk()? Red:Green, TRUE);
	 	GUI_Rectangle(314, 232, 319, 239, limitZ_chk()? Red:Green, TRUE);
	 }
#endif
	}
}
