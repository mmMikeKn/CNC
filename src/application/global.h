#ifndef GLOBAL_H_
#define GLOBAL_H_

//#define MY_3DPRINTER

#ifdef MY_3DPRINTER
#define HAS_TERMOMETER_MAX31855
#define MOTOR_EN_INVERTER
#define HAS_EXTRUDER // 3D printer
#else // my cnc
#define HAS_ENCODER
#endif
#define HAS_HWD_LIMITS

#ifdef HAS_ENCODER
#define ENCODER_MAX_STOP_ERR 0.9 // 0.9mm error = PAUSE
#endif



/*----- keyboard ----------------------------------------
*    1x 2x 3x 4x
* x0  1  2  3  A
* x1  4  5  6  B
* x2  7  8  9  C
* x3  *  0  #  D
*  column[0..3]: PC2, PE6, PC3, PC1
*  row   [0..3]: PE2, PE3, PE4, PE5
* ------- stepmotor -------------------------------------
*    EN    DIR   STEP
* 0: PB11  PA2   PA3
* 1: PB11  PB9   PB1
* 2: PB11  PB12  PD12
* 3: PB11  PD6   PB10
* ----- position switch ----------------------------------
*  X,Y,Z: PE0,PB8,PD3
* ----- sensors ---------------------------------------
* PC6, PC7 - encoder	Z TIM8_CH1,2 (5 V tolerant!)
* PA0, PA1 - encoder X TIM5_CH1,2 (not 5 V tolerant!!!)
* ---- TERMOMETER_MAX31855
* T_CS=PB14, SPI1_SCK=PA5, SPI1_MISO=PA6, SPI1_MOSI=PA7
* ------ hot end power ------------
* PB15 - power software PWM (SysTickHandler 0..1000)
* PB0,PC4 - hotend temperature tune (encoder for manual adjusting)
*/

#define MAX31855_CS_PIN GPIO_Pin_14
#define MAX31855_CS_PORT GPIOB

#define HOTEND_PWR_PIN GPIO_Pin_15
#define HOTEND_PWR_PORT GPIOB

#define HOTEND_TUNE_ENCODER_CHA_PIN GPIO_Pin_0
#define HOTEND_TUNE_ENCODER_CHA_PORT GPIOB

#define HOTEND_TUNE_ENCODER_CHB_PIN GPIO_Pin_4
#define HOTEND_TUNE_ENCODER_CHB_PORT GPIOC

//----- limit switch ----------------------------------
#define XPORT GPIOE
#define XPIN GPIO_Pin_0
#define ZPORT GPIOB
#define ZPIN GPIO_Pin_8
#define YPORT GPIOD
#define YPIN GPIO_Pin_3

//------- stepmotor -------------------------------------
// 74hc14 - inverter on the step motors board. STEP on falling edge
//  _______        ______
//         x      |
//         |______|
#define M0_DIR_PORT GPIOA
#define M0_DIR_PIN GPIO_Pin_2
#define M0_STEP_PORT GPIOA
#define M0_STEP_PIN GPIO_Pin_3

#define M1_DIR_PORT GPIOB
#define M1_DIR_PIN GPIO_Pin_9
#define M1_STEP_PORT GPIOB
#define M1_STEP_PIN GPIO_Pin_1

#define M2_DIR_PORT GPIOB
#define M2_DIR_PIN GPIO_Pin_12
#define M2_STEP_PORT GPIOD
#define M2_STEP_PIN GPIO_Pin_12

#define M3_DIR_PORT GPIOD
#define M3_DIR_PIN GPIO_Pin_6
#define M3_STEP_PORT GPIOB
#define M3_STEP_PIN GPIO_Pin_10

#define M_EN_PORT GPIOB
#define M_EN_PIN GPIO_Pin_11

#ifdef MOTOR_EN_INVERTER
#define MOTOR_OFF { M_EN_PORT->BSRR = M_EN_PIN; }
#define MOTOR_ON { M_EN_PORT->BRR = M_EN_PIN; }
#else
#define MOTOR_ON { M_EN_PORT->BSRR = M_EN_PIN; }
#define MOTOR_OFF { M_EN_PORT->BRR = M_EN_PIN; }
#endif // HAS_MOTOR_EN_INVERTER

//----- keyboard ----------------------------------------
#define ROW0_PORT GPIOE
#define ROW0_PIN GPIO_Pin_2
#define ROW1_PORT GPIOE
#define ROW1_PIN GPIO_Pin_3
#define ROW2_PORT GPIOE
#define ROW2_PIN GPIO_Pin_4
#define ROW3_PORT GPIOE
#define ROW3_PIN GPIO_Pin_5

#define COL1_PORT GPIOC
#define COL1_PIN GPIO_Pin_2
#define COL2_PORT GPIOE
#define COL2_PIN GPIO_Pin_6
#define COL3_PORT GPIOC
#define COL3_PIN GPIO_Pin_3
#define COL4_PORT GPIOC
#define COL4_PIN GPIO_Pin_1

#ifdef _WINDOWS
#else
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "stm32f10x_spi.h"
#include "hw_config.h"
#include "scr_io.h"
#include "rtc.h"
#include "ff.h"
#include "sdcard.h"
#include "rs232_interface.h"
#include "keyboard.h"
#include "gcode.h"
#include "stepmotor.h"
#include "limits.h"
#include "encoder.h"
//#define DEBUG_MODE
#ifdef DEBUG_MODE
 #define DBG(...) { rf_printf(__VA_ARGS__); }
#else
 #define DBG(...) {}
#endif

#endif

void delayMs(uint32_t msec);
char *str_trim(char *str);
uint8_t questionYesNo(char *msg, char *param);

void manualMode(void);

void showCriticalStatus(char *msg, int st);
uint16_t calcColor(uint8_t val);

#ifdef HAS_TERMOMETER_MAX31855
extern volatile int16_t _temperatureHotEnd, _temperatureChip;
extern volatile uint8_t _temperatureMAX31855_status;
#endif // HAS_TERMOMETER_MAX31855

#ifdef HAS_EXTRUDER
extern uint16_t _destExtruderT, _hotEndPwrPWM;
void adjHotEnd();
#endif //HAS_EXTRUDER

#endif /* GLOBAL_H_ */
