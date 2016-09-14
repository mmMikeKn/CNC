#ifndef _ILI9320_H_
#define _ILI9320_H_

#include "stm32f10x_conf.h"

// PD13-LIGHT-PWM
#define Lcd_Light_ON   (GPIOD->BSRR = GPIO_Pin_13)
#define Lcd_Light_OFF  (GPIOD->BRR = GPIO_Pin_13)

#define Bank1_LCD_D    ((uint32_t)0x60020000)    //disp Data ADDR
#define Bank1_LCD_C    ((uint32_t)0x60000000)	 //disp Reg ADDR

inline void ili9320_SetCursor(uint16_t x, uint16_t y) {
 *(__IO uint16_t *) (Bank1_LCD_C)= 32;
 *(__IO uint16_t *) (Bank1_LCD_D)= y;

 *(__IO uint16_t *) (Bank1_LCD_C)= 33;
 *(__IO uint16_t *) (Bank1_LCD_D)= 319-x;
}

inline void ili9320_WrVcamLine(uint16_t x, uint16_t y, uint16_t h, uint16_t color[]) {
 int i;
 *(__IO uint16_t *) (Bank1_LCD_C)= 0x20; // 32
 *(__IO uint16_t *) (Bank1_LCD_D)= y;
 *(__IO uint16_t *) (Bank1_LCD_C)= 0x21; // 33;
 *(__IO uint16_t *) (Bank1_LCD_D)= 319-x;
	*(__IO uint16_t *) (Bank1_LCD_C)= 0x0022;
 for(i = 0; i< h; i++) {
 	*(__IO uint16_t *) (Bank1_LCD_D)= color[i];
 }
}


void Lcd_Configuration(void);
unsigned short CheckController(void);
void ili9320_Initializtion(void);
void ili9320_Reset(void);
void ili9320_WriteRegister(unsigned short index,unsigned short dat);
void ili9320_SetWindows(unsigned short StartX,unsigned short StartY,unsigned short EndX,unsigned short EndY);
void ili9320_DrawPicture(unsigned short StartX,unsigned short StartY,unsigned short EndX,unsigned short EndY,unsigned short *pic);
void ili9320_SetPoint(unsigned short x,unsigned short y,unsigned short point);
void ili9320_PutChar(unsigned short x,unsigned short y,unsigned char c,unsigned short charColor,unsigned short bkColor);
void ili9320_Clear(unsigned short dat);
void ili9320_Delay(unsigned int nCount);
void ili9320_Test(void);
unsigned short ili9320_GetCode(void);
void ili9320_WriteData(unsigned short dat);
void ili9320_WriteIndex(unsigned short idx);

void ili9320_VLine(unsigned short x0, unsigned short y0, unsigned short h,unsigned short color);
void ili9320_BackLight(unsigned char status);

unsigned short ili9320_BGR2RGB(unsigned short c);

unsigned short ili9320_GetPoint(unsigned short x,unsigned short y);
unsigned short ili9320_ReadData(void);
unsigned short ili9320_ReadRegister(unsigned short index);

unsigned short GUI_Color565(unsigned int RGB);

void GUI_Text(unsigned short x, unsigned short y, const unsigned char *str, unsigned short len,unsigned short Color, unsigned short bkColor);
void GUI_Line(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1,unsigned short color);  // 画线
void GUI_Circle(unsigned short cx,unsigned short cy,unsigned short r,unsigned short color,unsigned char fill);  // 画园
void GUI_Rectangle(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1,unsigned short color,unsigned char fill); // 画矩形
void GUI_Square(unsigned short x0, unsigned short y0, unsigned short with, unsigned short color,unsigned char fill);  // 画正方形


unsigned short  Touch_MeasurementX(void);
unsigned short  Touch_MeasurementY(void);

// INT   (ADC touch)	- PE12
unsigned char isTouch_pen(void);
unsigned short Touch_ScrX(void);
unsigned short Touch_ScrY(void);

#define White          0xFFFF//白色
#define Black          0x0000//黑色
#define Grey           0xF7DE
#define Blue           0x001F
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0

#endif

