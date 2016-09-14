#include "global.h"
#include "core_cm3.h"
#include "mass_mal.h"


void USB_Init(void);

ErrorStatus HSEStartUpStatus ;
RCC_ClocksTypeDef RCC_Clocks ;

#ifdef  __cplusplus
extern "C" {
#endif
uint32_t GetCpuClock()
  {
    return RCC_Clocks.SYSCLK_Frequency ;
  }
#ifdef  __cplusplus
  }
#endif

void SystemStartup(void) {
 NVIC_InitTypeDef NVIC_InitStructure;
 /* Unlock the internal flash */
 FLASH_Unlock();

 /* RCC system reset(for debug purpose) */
 RCC_DeInit();

 /* Enable HSE */
 RCC_HSEConfig(RCC_HSE_ON);

 /* Wait till HSE is ready */
 HSEStartUpStatus = RCC_WaitForHSEStartUp();

 if (HSEStartUpStatus == SUCCESS) {
   FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);/* Enable Prefetch Buffer */
   FLASH_SetLatency(FLASH_Latency_2);/* Flash 2 wait state */
   RCC_HCLKConfig(RCC_SYSCLK_Div1);/* HCLK = SYSCLK */
   RCC_PCLK2Config(RCC_HCLK_Div1);/* PCLK2 = HCLK */
   RCC_PCLK1Config(RCC_HCLK_Div2); /* PCLK1 = HCLK/2 */
   RCC_ADCCLKConfig(RCC_PCLK2_Div6);     //ADCCLK = PCLK2/6 = 12MHz
   RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9 );/* PLLCLK = 8MHz * 9 = 72 MHz */
   RCC_PLLCmd(ENABLE); /* Enable PLL */
   /* Wait till PLL is ready */
   while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

   RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);/* Select PLL as system clock source */

   /* Wait till PLL is used as system clock source */
   while (RCC_GetSYSCLKSource() != 0x08);
 }
 RCC_GetClocksFreq( &RCC_Clocks ) ;
 SysTick_Config(SystemFrequency / 1000);
 //-------------  GPIO  configure ----------
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|
			RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE, ENABLE);

 GPIO_InitTypeDef GPIO_InitStructure;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
 GPIO_Init(GPIOB, &GPIO_InitStructure); // LED on board
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; // LCD LIGHT
 GPIO_Init(GPIOD, &GPIO_InitStructure);
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
 GPIO_Init(GPIOB, &GPIO_InitStructure); // KEY on board

 rtc_init();
 Lcd_Configuration(); ili9320_Initializtion(); ili9320_Clear(0); Lcd_Light_ON;
 win_showMsgWin(); delayMs(500);
 scr_puts(" ---- SystemStartup -----");

 rs232_init();
 kbd_init();
 scr_puts("\nstep 1.");
 stepm_init();
 scr_puts("\nstep 2.");
 // extruder_t.c removed 20.03.13
 // extrudT_init();
#ifdef HAS_ENCODER
 encoder_int();
#endif
#ifdef HAS_TERMOMETER_MAX31855
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
 // T_CS=PB14, SPI1_SCK=PA5, SPI1_MISO=PA6, SPI1_MOSI=PA7
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 GPIO_InitStructure.GPIO_Pin = MAX31855_CS_PIN; GPIO_Init(MAX31855_CS_PORT, &GPIO_InitStructure);

 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_7; GPIO_Init(GPIOA,&GPIO_InitStructure);

 // SPI1 LCD TSC2046N CS off
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; GPIO_Init(GPIOB,&GPIO_InitStructure);
 GPIOB->BSRR = GPIO_Pin_7;

 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; GPIO_Init(GPIOA,&GPIO_InitStructure);

  //SPI1 Periph clock enable
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
 SPI_I2S_DeInit(SPI1);
 SPI_Cmd(SPI1, DISABLE);
 SPI_InitTypeDef   SPI_InitStructure;
 SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
 SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
 SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
 SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
 SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
 SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
 SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
 SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
 SPI_Init(SPI1,&SPI_InitStructure);
 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
 NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;
 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 NVIC_Init(&NVIC_InitStructure);
 SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_RXNE,ENABLE);
 SPI_Cmd(SPI1,ENABLE);
#endif
 scr_puts("\nstep 3.");
#ifdef HAS_HWD_LIMITS
 limits_init();
#endif
#ifdef HAS_EXTRUDER
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 GPIO_InitStructure.GPIO_Pin = HOTEND_PWR_PIN; GPIO_Init(HOTEND_PWR_PORT, &GPIO_InitStructure);
 //* PB0,PC4 - hotend temperature tune (encoder for manual adjusting)
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
 GPIO_InitStructure.GPIO_Pin = HOTEND_TUNE_ENCODER_CHA_PIN;
 GPIO_Init(HOTEND_TUNE_ENCODER_CHA_PORT, &GPIO_InitStructure);
 GPIO_InitStructure.GPIO_Pin = HOTEND_TUNE_ENCODER_CHB_PIN;
 GPIO_Init(HOTEND_TUNE_ENCODER_CHB_PORT, &GPIO_InitStructure);
#endif
 scr_puts("\nstep 4.");
 MAL_Init(0);
 scr_puts("\nstep 5.");

 // USB configure
 /* USB_DISCONNECT_PIN used as USB pull-up */
 GPIO_InitStructure.GPIO_Pin = USB_DISCONNECT_PIN;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
 GPIO_Init(USB_DISCONNECT, &GPIO_InitStructure);

 RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
 RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);

 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
 NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 NVIC_Init(&NVIC_InitStructure);
 NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn;
 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 NVIC_Init(&NVIC_InitStructure);
 USB_Init();
 scr_puts("\nstep 6.");
 //while(bDeviceState != CONFIGURED);
}


