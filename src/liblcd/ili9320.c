#include "ili9320.h"
#include "ili9320_font.h"
#include "stm32f10x_fsmc.h"



void ili9320_Delay(unsigned int nCount) {
  for(; nCount != 0; nCount--) __NOP();
}

void Lcd_Configuration(void) {
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE , ENABLE);
 GPIO_InitTypeDef GPIO_InitStructure;
 // PE1-LCD-RST,
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ; 	 //LCD-RST
 GPIO_Init(GPIOE, &GPIO_InitStructure);
 // LIGHT
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
 GPIO_Init(GPIOD, &GPIO_InitStructure);

 //------------------------- FSMC pins ---------------
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
 // PD14-D0, PD15-D1, PD0-D2,   PD1-D3,   PE7-D4,   PE8-D5,  PE9-D6, PE10-D7,
 // PE11-D8, PE12-D9, PE13-D10, PE14-D11, PE15-D12, PD8-D13, PD9-D14, PD10-D15
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_14 | GPIO_Pin_15;
 GPIO_Init(GPIOD, &GPIO_InitStructure);
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 ;
 GPIO_Init(GPIOE, &GPIO_InitStructure);
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14| GPIO_Pin_15 ;
 GPIO_Init(GPIOE, &GPIO_InitStructure);
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 ;
 GPIO_Init(GPIOD, &GPIO_InitStructure);
 //PD11-RS, PD4-nOE, PD5-nWE, PD7-LCD-CS
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_4 |GPIO_Pin_5 | GPIO_Pin_7;
 GPIO_Init(GPIOD, &GPIO_InitStructure);
 //----------------
 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
 FSMC_NORSRAMTimingInitTypeDef  p;

 p.FSMC_AddressSetupTime = 0x02;
 p.FSMC_AddressHoldTime = 0x01;
 p.FSMC_DataSetupTime = 0x05;
 p.FSMC_BusTurnAroundDuration = 0x00;
 p.FSMC_CLKDivision = 0x01;
 p.FSMC_DataLatency = 0x00;
 p.FSMC_AccessMode = FSMC_AccessMode_B;

 FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
 FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
 FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;
 FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
 FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
 FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
 FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
 FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
 FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
 FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
 FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
 FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
 FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
 FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

 FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
  /* Enable FSMC Bank1_SRAM Bank */
 FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
} 

static void LCD_WR_REG(unsigned int index) { 	*(__IO uint16_t *) (Bank1_LCD_C)= index; }
static void LCD_WR_CMD(unsigned int index,unsigned int val) {
	*(__IO uint16_t *) (Bank1_LCD_C)= index;
 *(__IO uint16_t *) (Bank1_LCD_D)= val;
}

static unsigned int LCD_RD_data(void){
	unsigned int a=0;
	a=(*(__IO uint16_t *) (Bank1_LCD_D)); 	//Dummy
	a=*(__IO uint16_t *) (Bank1_LCD_D); //L
	return(a);
}

static void    LCD_WR_Data(unsigned int val) {	*(__IO uint16_t *) (Bank1_LCD_D)= val; }
static void Delay(__IO uint32_t nCount) {  for(; nCount != 0; nCount--) __NOP(); }

void ili9320_Initializtion(){
 GPIO_ResetBits(GPIOE, GPIO_Pin_1); // RESET LCD
 ili9320_Delay(0xAFFf);
 GPIO_SetBits(GPIOE, GPIO_Pin_1 );
 ili9320_Delay(0xAFFf);

	LCD_WR_CMD(0x00E3, 0x3008); // Set internal timing
	LCD_WR_CMD(0x00E7, 0x0012); // Set internal timing
	LCD_WR_CMD(0x00EF, 0x1231); // Set internal timing
	LCD_WR_CMD(0x0000, 0x0001); // Start Oscillation
	LCD_WR_CMD(0x0001, 0x0100); // set SS and SM bit
	LCD_WR_CMD(0x0002, 0x0700); // set 1 line inversion

	LCD_WR_CMD(0x0003, 0x1030); // set GRAM write direction and BGR=0,262K colors,1 transfers/pixel.
	LCD_WR_CMD(0x0004, 0x0000); // Resize register
	LCD_WR_CMD(0x0008, 0x0202); // set the back porch and front porch
	LCD_WR_CMD(0x0009, 0x0000); // set non-display area refresh cycle ISC[3:0]
	LCD_WR_CMD(0x000A, 0x0000); // FMARK function
	LCD_WR_CMD(0x000C, 0x0000); // RGB interface setting
	LCD_WR_CMD(0x000D, 0x0000); // Frame marker Position
	LCD_WR_CMD(0x000F, 0x0000); // RGB interface polarity
//Power On sequence
	LCD_WR_CMD(0x0010, 0x0000); // SAP, BT[3:0], AP, DSTB, SLP, STB
	LCD_WR_CMD(0x0011, 0x0007); // DC1[2:0], DC0[2:0], VC[2:0]
	LCD_WR_CMD(0x0012, 0x0000); // VREG1OUT voltage
	LCD_WR_CMD(0x0013, 0x0000); // VDV[4:0] for VCOM amplitude
	Delay(200); // Dis-charge capacitor power voltage
	LCD_WR_CMD(0x0010, 0x1690); // SAP, BT[3:0], AP, DSTB, SLP, STB
	LCD_WR_CMD(0x0011, 0x0227); // R11h=0x0221 at VCI=3.3V, DC1[2:0], DC0[2:0], VC[2:0]
	Delay(50); // Delay 50ms
	LCD_WR_CMD(0x0012, 0x001C); // External reference voltage= Vci;
	Delay(50); // Delay 50ms
	LCD_WR_CMD(0x0013, 0x1800); // R13=1200 when R12=009D;VDV[4:0] for VCOM amplitude
	LCD_WR_CMD(0x0029, 0x001C); // R29=000C when R12=009D;VCM[5:0] for VCOMH
	LCD_WR_CMD(0x002B, 0x000D); // Frame Rate = 91Hz
	Delay(50); // Delay 50ms
	LCD_WR_CMD(0x0020, 0x0000); // GRAM horizontal Address
	LCD_WR_CMD(0x0021, 0x0000); // GRAM Vertical Address
// ----------- Adjust the Gamma Curve ----------//
	LCD_WR_CMD(0x0030, 0x0007);
	LCD_WR_CMD(0x0031, 0x0302);
	LCD_WR_CMD(0x0032, 0x0105);
	LCD_WR_CMD(0x0035, 0x0206);
	LCD_WR_CMD(0x0036, 0x0808);
	LCD_WR_CMD(0x0037, 0x0206);
	LCD_WR_CMD(0x0038, 0x0504);
	LCD_WR_CMD(0x0039, 0x0007);
	LCD_WR_CMD(0x003C, 0x0105);
	LCD_WR_CMD(0x003D, 0x0808);
//------------------ Set GRAM area ---------------//
	LCD_WR_CMD(0x0050, 0x0000); // Horizontal GRAM Start Address
	LCD_WR_CMD(0x0051, 0x00EF); // Horizontal GRAM End Address
	LCD_WR_CMD(0x0052, 0x0000); // Vertical GRAM Start Address
	LCD_WR_CMD(0x0053, 0x013F); // Vertical GRAM Start Address
	LCD_WR_CMD(0x0060, 0xA700); // Gate Scan Line
	LCD_WR_CMD(0x0061, 0x0001); // NDL,VLE, REV
	LCD_WR_CMD(0x006A, 0x0000); // set scrolling line
//-------------- Partial Display Control ---------//
	LCD_WR_CMD(0x0080, 0x0000);
	LCD_WR_CMD(0x0081, 0x0000);
	LCD_WR_CMD(0x0082, 0x0000);
	LCD_WR_CMD(0x0083, 0x0000);
	LCD_WR_CMD(0x0084, 0x0000);
	LCD_WR_CMD(0x0085, 0x0000);
//-------------- Panel Control -------------------//
	LCD_WR_CMD(0x0090, 0x0010);
	LCD_WR_CMD(0x0092, 0x0000);
	LCD_WR_CMD(0x0093, 0x0003);
	LCD_WR_CMD(0x0095, 0x0110);
	LCD_WR_CMD(0x0097, 0x0000);
	LCD_WR_CMD(0x0098, 0x0000);
	LCD_WR_CMD(0x0007, 0x0133); // 262K color and display ON

 LCD_WR_CMD(32, 0);
 LCD_WR_CMD(33, 0x013F);
	*(__IO uint16_t *) (Bank1_LCD_C)= 34;
	for(int n = 0; n<76800; n++) LCD_WR_Data(0xf17f);
}

/****************************************************************************
* 名    称：void ili9320_SetWindows(u16 StartX,u16 StartY,u16 EndX,u16 EndY)
* 功    能：设置窗口区域
* 入口参数：StartX     行起始座标
*           StartY     列起始座标
*           EndX       行结束座标
*           EndY       列结束座标
* 出口参数：无
* 说    明：
* 调用方法：ili9320_SetWindows(0,0,100,100)；
****************************************************************************/
__inline void ili9320_SetWindows(u16 StartX,u16 StartY,u16 EndX,u16 EndY) {
 *(__IO uint16_t *) (Bank1_LCD_C)= 0x50;
 *(__IO uint16_t *) (Bank1_LCD_D)= StartY;

 *(__IO uint16_t *) (Bank1_LCD_C)= 0x51;
 *(__IO uint16_t *) (Bank1_LCD_D)= EndX;

 *(__IO uint16_t *) (Bank1_LCD_C)= 0x52;
 *(__IO uint16_t *) (Bank1_LCD_D)= 399-StartX;

 *(__IO uint16_t *) (Bank1_LCD_C)= 0x53;
 *(__IO uint16_t *) (Bank1_LCD_D)= EndY;
}

/****************************************************************************
* 名    称：void ili9320_Clear(u16 dat)
* 功    能：将屏幕填充成指定的颜色，如清屏，则填充 0xffff
* 入口参数：dat      填充值
* 出口参数：无
* 说    明：
* 调用方法：ili9320_Clear(0xffff);
****************************************************************************/
void ili9320_Clear(u16 dat) {
 u32 i;
 *(__IO uint16_t *) (Bank1_LCD_C)= 0x50;
 *(__IO uint16_t *) (Bank1_LCD_D)= 0;
 *(__IO uint16_t *) (Bank1_LCD_C)= 0x51;
 *(__IO uint16_t *) (Bank1_LCD_D)= 239;
 *(__IO uint16_t *) (Bank1_LCD_C)= 0x52;
 *(__IO uint16_t *) (Bank1_LCD_D)= 0;
 *(__IO uint16_t *) (Bank1_LCD_C)= 0x53;
 *(__IO uint16_t *) (Bank1_LCD_D)= 319;
 *(__IO uint16_t *) (Bank1_LCD_C)= 32;
 *(__IO uint16_t *) (Bank1_LCD_D)= 0;
 *(__IO uint16_t *) (Bank1_LCD_C)= 33;
 *(__IO uint16_t *) (Bank1_LCD_D)= 0;
 *(__IO uint16_t *) (Bank1_LCD_C)= 34;
 for(i=0;i<76800;i++){
 	*(__IO uint16_t *) (Bank1_LCD_D)= dat;
 }
}

void ili9320_WriteData(u16 dat)
{
  *(__IO uint16_t *) (Bank1_LCD_D)= dat;
  //LCD_WR_Data(dat);
}

void ili9320_WriteIndex(u16 idx)
{
  *(__IO uint16_t *) (Bank1_LCD_C)= idx;
  //LCD_WR_REG(idx);
}

u16 ili9320_ReadData(void)
{
  u16 val=0;
  val=LCD_RD_data();
  return val;
}

u16 ili9320_ReadRegister(u16 index)
{
  u16 tmp;
  tmp= *(volatile unsigned int *)(0x60000000);

  return tmp;
}

void ili9320_WriteRegister(u16 index,u16 dat)
{
 /************************************************************************
  **                                                                    **
  ** nCS       ----\__________________________________________/-------  **
  ** RS        ------\____________/-----------------------------------  **
  ** nRD       -------------------------------------------------------  **
  ** nWR       --------\_______/--------\_____/-----------------------  **
  ** DB[0:15]  ---------[index]----------[data]-----------------------  **
  **                                                                    **
  ************************************************************************/
  *(__IO uint16_t *) (Bank1_LCD_C)= index;
  *(__IO uint16_t *) (Bank1_LCD_D)= dat;
  //LCD_WR_CMD(index,dat);
}

u16 ili9320_GetPoint(u16 x,u16 y) {
 ili9320_SetCursor(x,y);
 *(__IO uint16_t *) (Bank1_LCD_C)= 34;
 //LCD_WR_REG(34);
 //temp = ili9320_ReadData(); //dummy
 //temp = ili9320_ReadData();

 //return (ili9320_BGR2RGB(ili9320_ReadData()));
 return (ili9320_BGR2RGB(ili9320_ReadData()));
}

/****************************************************************************
* 名    称：void ili9320_SetPoint(u16 x,u16 y,u16 point)
* 功    能：在指定座标画点
* 入口参数：x      行座标
*           y      列座标
*           point  点的颜色
* 出口参数：无
* 说    明：
* 调用方法：ili9320_SetPoint(10,10,0x0fe0);
****************************************************************************/
void ili9320_SetPoint(u16 x,u16 y,u16 point) {
	if ( (y>=240)||(x>=320) ) return;
 *(__IO uint16_t *) (Bank1_LCD_C)= 32;
 *(__IO uint16_t *) (Bank1_LCD_D)= y;

 *(__IO uint16_t *) (Bank1_LCD_C)= 33;
 *(__IO uint16_t *) (Bank1_LCD_D)= 319-x;

 *(__IO uint16_t *) (Bank1_LCD_C)= 34;
 *(__IO uint16_t *) (Bank1_LCD_D)= point;
}

/****************************************************************************
* 名    称：void ili9320_DrawPicture(u16 StartX,u16 StartY,u16 EndX,u16 EndY,u16 *pic)
* 功    能：在指定座标范围显示一副图片
* 入口参数：StartX     行起始座标
*           StartY     列起始座标
*           EndX       行结束座标
*           EndY       列结束座标
            pic        图片头指针
* 出口参数：无
* 说    明：图片取模格式为水平扫描，16位颜色模式
* 调用方法：ili9320_DrawPicture(0,0,100,100,(u16*)demo);
****************************************************************************/
void ili9320_DrawPicture(u16 StartX,u16 StartY,u16 EndX,u16 EndY,u16 *pic) {
 u16  i,temp;
 ili9320_SetWindows(StartX,StartY,EndX,EndY);
 ili9320_SetCursor(StartX,StartY);
 //for (i=0;i<(EndX*EndY);i++) LCD_WR_Data(*pic++);
 //while(1);
  //LCD_WR_CMD(0x0003,0x1018);   //左下起
 /*ili9320_SetWindows(StartX,StartY,EndX,EndY);
 LCD_WR_CMD(0x0050, 0); // Horizontal GRAM Start Address
 LCD_WR_CMD(0x0051, 60); // Horizontal GRAM End Address
 LCD_WR_CMD(0x0052, 0); // Vertical GRAM Start Address
 LCD_WR_CMD(0x0053, 399); // Vertical GRAM Start Address
 LCD_WR_CMD(0x0020,(0x0000));
 LCD_WR_CMD(0x0021,(0x18f));
  */

	LCD_WR_REG(34);
	i=0;
	while(i<40000)
	{
	    temp=(uint16_t)(pic[i]<<8)+pic[i+1];
		LCD_WR_Data(temp);
		i=i+2;
 	}
}


void ili9320_VLine(u16 x0, u16 y0, u16 h,u16 color) {
 int i;
	ili9320_SetCursor(x0,y0);
	*(__IO uint16_t *) (Bank1_LCD_C)= 0x0022;
 for(i = 0; i<h;i++) {
 	*(__IO uint16_t *) (Bank1_LCD_D)= color;
 }
}

/****************************************************************************
* 名    称：void ili9320_PutChar(u16 x,u16 y,u8 c,u16 charColor,u16 bkColor)
* 功    能：在指定座标显示一个8x16点阵的ascii字符
* 入口参数：x          行座标
*           y          列座标
*           charColor  字符的颜色
*           bkColor    字符背景颜色
* 出口参数：无
* 说    明：显示范围限定为可显示的ascii码
* 调用方法：ili9320_PutChar(10,10,'a',0x0000,0xffff);
****************************************************************************/
void ili9320_PutChar(u16 x,u16 y,u8 c,u16 charColor,u16 bkColor)
{
  u16 i=0;
  u16 j=0;
  
  u8 tmp_char=0;

  for (i=0;i<16;i++)
  {
    tmp_char=ascii_8x16[((c-0x20)*16)+i];
    for (j=0;j<8;j++)
    {
      if ( ((tmp_char >> (7-j)) & 0x01) == 0x01)
        {
          ili9320_SetPoint(x+j,y+i,charColor); // 字符颜色
        }
        else
        {
          ili9320_SetPoint(x+j,y+i,bkColor); // 背景颜色
        }
    }
  }
}

/****************************************************************************
* 名    称：void ili9320_Test()
* 功    能：测试液晶屏
* 入口参数：无
* 出口参数：无
* 说    明：显示彩条，测试液晶屏是否正常工作
* 调用方法：ili9320_Test();
****************************************************************************/
void ili9320_Test() {
 u16 i,j;
 ili9320_SetCursor(0,0);

 for(i=0;i<400;i++)
   for(j=0;j<240;j++)
   {
     if(i>279)LCD_WR_Data(0x0000);
     else if(i>239)LCD_WR_Data(0x001f);
     else if(i>199)LCD_WR_Data(0x07e0);
     else if(i>159)LCD_WR_Data(0x07ff);
     else if(i>119)LCD_WR_Data(0xf800);
     else if(i>79)LCD_WR_Data(0xf81f);
     else if(i>39)LCD_WR_Data(0xffe0);
     else LCD_WR_Data(0xffff);
   }
 
}

u16 ili9320_BGR2RGB(u16 c) {
  u16  r, g, b, rgb;

  b = (c>>0)  & 0x1f;
  g = (c>>5)  & 0x3f;
  r = (c>>11) & 0x1f;
  
  rgb =  (b<<11) + (g<<5) + (r<<0);

  return( rgb );
}


void ili9320_Reset() {
  /***************************************
   **                                   **
   **  -------\______________/-------   **
   **         | <---Tres---> |          **
   **                                   **
   **  Tres: Min.1ms                    **
   ***************************************/
    
//  	Set_Rst;;
//    ili9320_Delay(50000);
//  	Clr_Rst;
//    ili9320_Delay(50000);
//  	Set_Rst;
//    ili9320_Delay(50000);
}

void ili9320_BackLight(u8 status) {
  if ( status >= 1 )  Lcd_Light_ON;
  else Lcd_Light_OFF;
}

