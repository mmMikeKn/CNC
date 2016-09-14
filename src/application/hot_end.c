#include <stdlib.h>
#include "global.h"

#ifdef HAS_EXTRUDER
uint16_t _destExtruderT = 215, _hotEndPwrPWM = 1000;
static int32_t integralPID = 0, differentialPID = 0;

#define GRAPH_Y_BOTTOM 220
#define GRAPH_Y_TOP 18
#define GRAPH_Y_START 80

static void drawScr() {
	ili9320_Clear(0); scr_setfullTextWindow();
	scr_gotoxy(0,TEXT_Y_MAX-1);	scr_fontColor(Yellow,Black);
	scr_puts("'1'-[T+1] '4'-[T-1] '2'-[T+5] '5'-[T-5]");
}

extern volatile uint16_t MAX31855_v1, MAX31855_v2;

void adjHotEnd(void) {
	uint16_t x = 0, lastDestT = _destExtruderT;

 drawScr();
	while(kbd_getKey()>=0) {};
	_destExtruderT = _smParam.defExtruderTemperature;

	while(TRUE) {
	 static uint8_t sec = 0;
	 uint16_t y;
	 RTC_t rtc;
	 rtc_gettime(&rtc);
 	scr_gotoxy(1,1); scr_printf("%X.%X        ", MAX31855_v1,MAX31855_v2); // TODO for debug
 	if(rtc.sec != sec || lastDestT != _destExtruderT) {
	 	scr_gotoxy(0,0);
	 	if(_temperatureMAX31855_status != 0) scr_fontColor(Red, Black);
	 	else scr_fontColor(White,Black);
  	scr_printf("T[%d]: %d->%d  PWR:%d.%d INT:%d", _temperatureMAX31855_status, _temperatureHotEnd, _destExtruderT,
 			_hotEndPwrPWM/10, _hotEndPwrPWM%10, _temperatureChip);
	 	scr_clrEndl();
  	lastDestT = _destExtruderT;
 	}

	 if(rtc.sec != sec || lastDestT != _destExtruderT) {
	 	sec = rtc.sec;
	 	uint16_t tmp = _temperatureHotEnd+195;
	 	y = GRAPH_Y_BOTTOM - (tmp-GRAPH_Y_START);
	 	if(y > GRAPH_Y_BOTTOM) y = GRAPH_Y_BOTTOM;
	 	if(y < GRAPH_Y_TOP) y = GRAPH_Y_TOP;
	 	ili9320_SetPoint(x, y, Red);

	 	y = GRAPH_Y_BOTTOM - (_destExtruderT-GRAPH_Y_START);
	 	if(y > GRAPH_Y_BOTTOM) y = GRAPH_Y_BOTTOM;
	 	if(y < GRAPH_Y_TOP) y = GRAPH_Y_TOP;
	 	ili9320_SetPoint(x, y, Green);

	 	y = GRAPH_Y_BOTTOM - _hotEndPwrPWM/10;
	 	ili9320_SetPoint(x, y+1, Yellow);

	 	y = GRAPH_Y_BOTTOM - integralPID/1000;
	 	ili9320_SetPoint(x, y+2, Blue);
	 	y = GRAPH_Y_BOTTOM - differentialPID/1000;
	 	ili9320_SetPoint(x, y+3, Cyan);

	 	x++;
	 	if(x >= 320) {
	 		x = 0;
	 		drawScr();
	 	}
	 }
		switch(kbd_getKey()) {
 	 case KEY_1:
 	 	if(_destExtruderT < 270)	_destExtruderT++;
 	 	break;
 	 case KEY_4:
 	 	if(_destExtruderT > 100)	_destExtruderT--;
 	 	break;
 	 case KEY_2:
 	 	if(_destExtruderT < 265)	_destExtruderT+=5;
 	 	break;
 	 case KEY_5:
 	 	if(_destExtruderT > 105)	_destExtruderT-=5;
 	 	break;
 	 case KEY_C:
 	 	_smParam.defExtruderTemperature = _destExtruderT;
	  	return;
		}
	}
}

static int32_t Kp = 5000, Ki = 20, Kd = 15;

void extruderWarmUp() { // PID
 	static int32_t prevError = 0, integrVal = 0;
  int32_t v, error;

  if(_destExtruderT == 0) {
  	_hotEndPwrPWM = 0; return;
  }

  error = _destExtruderT -_temperatureHotEnd;
  v = Kp * error;
  v += (integrVal += Ki * error);
  if(integrVal > 50000) integrVal = 50000;
  integralPID = integrVal;
  v += (differentialPID = Kd * (error - prevError));
  prevError = error;
  v /= 100;
  if(v > 1000) _hotEndPwrPWM = 1000;
  else if(v < 0) _hotEndPwrPWM = 0;
  else _hotEndPwrPWM = v;
}

#endif
