#include <stdlib.h>
#include <string.h>
#include "global.h"

#define STEPS_BUF_SZ 16
typedef struct {
	uint32_t steps[4];
	uint32_t pscValue[4], arrValue[4], f[4];
	uint8_t dir[4];
} LINE_DATA;

static volatile LINE_DATA steps_buf[STEPS_BUF_SZ];
static int32_t steps_buf_sz, steps_buf_p1, steps_buf_p2;

static LINE_DATA cur_steps_buf; // for debug only

volatile struct {
	int32_t globalSteps;
	uint32_t steps;
 uint8_t clk, dir, isInProc;
} step_motors[4];

static void stepm_powerOff() {
	steps_buf_sz = steps_buf_p1 = steps_buf_p2 = 0;
	MOTOR_OFF;
}

void stepm_init(void) {
	for(int i = 0; i < 4; i++) {
		step_motors[i].steps = 0;
		step_motors[i].clk = TRUE; step_motors[i].isInProc = FALSE;
		step_motors[i].globalSteps = 0;
	}
	steps_buf_sz = steps_buf_p1 = steps_buf_p2 = 0;

	GPIO_InitTypeDef GPIO_InitStructure;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

 GPIO_InitStructure.GPIO_Pin = M_EN_PIN;   GPIO_Init(M_EN_PORT, &GPIO_InitStructure);

 GPIO_InitStructure.GPIO_Pin = M0_DIR_PIN;  GPIO_Init(M0_DIR_PORT, &GPIO_InitStructure);
 GPIO_InitStructure.GPIO_Pin = M0_STEP_PIN; GPIO_Init(M0_STEP_PORT, &GPIO_InitStructure);
 stepm_powerOff();

 GPIO_InitStructure.GPIO_Pin = M1_DIR_PIN;  GPIO_Init(M1_DIR_PORT, &GPIO_InitStructure);
 GPIO_InitStructure.GPIO_Pin = M1_STEP_PIN; GPIO_Init(M1_STEP_PORT, &GPIO_InitStructure);

 GPIO_InitStructure.GPIO_Pin = M2_DIR_PIN;  GPIO_Init(M2_DIR_PORT, &GPIO_InitStructure);
 GPIO_InitStructure.GPIO_Pin = M2_STEP_PIN; GPIO_Init(M2_STEP_PORT, &GPIO_InitStructure);

 GPIO_InitStructure.GPIO_Pin = M3_DIR_PIN;  GPIO_Init(M3_DIR_PORT, &GPIO_InitStructure);
 GPIO_InitStructure.GPIO_Pin = M3_STEP_PIN; GPIO_Init(M3_STEP_PORT, &GPIO_InitStructure);

 TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 TIM_TimeBaseStructure.TIM_Prescaler = 1799; // any
 TIM_TimeBaseStructure.TIM_Period = 100; // any
 TIM_TimeBaseStructure.TIM_ClockDivision = 0;
 TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

 NVIC_InitTypeDef NVIC_InitStructure;
 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;

 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
 // TIM2 - motor 0
 NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn;
 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
 NVIC_Init(&NVIC_InitStructure);
 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
 TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
 TIM2->EGR = TIM_PSCReloadMode_Update; TIM_ARRPreloadConfig(TIM2, ENABLE);
 TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); TIM_Cmd(TIM2, ENABLE);
 // TIM3 - motor 1
 NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn;
 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
 NVIC_Init(&NVIC_InitStructure);
 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
 TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
 TIM3->EGR = TIM_PSCReloadMode_Update; TIM_ARRPreloadConfig(TIM3, ENABLE);
 TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); TIM_Cmd(TIM3, ENABLE);
 // TIM4 - motor 2
 NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn;
 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
 NVIC_Init(&NVIC_InitStructure);
 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
 TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
 TIM4->EGR = TIM_PSCReloadMode_Update; TIM_ARRPreloadConfig(TIM4, ENABLE);
 TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); TIM_Cmd(TIM4, ENABLE);
 // TIM6 - motor 3
 NVIC_InitStructure.NVIC_IRQChannel=TIM6_IRQn;
 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
 NVIC_Init(&NVIC_InitStructure);
 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
 TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
 TIM6->EGR = TIM_PSCReloadMode_Update; TIM_ARRPreloadConfig(TIM6, ENABLE);
 TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE); TIM_Cmd(TIM6, ENABLE);
}

void stepm_proc(uint8_t id) {
#ifdef HAS_HWD_LIMITS
	if(limits_chk()) {	stepm_EmergeStop(); return;	}
#endif
	if(step_motors[id].isInProc) {
  switch(id) {
   case 0:
   	if(step_motors[id].clk) M0_STEP_PORT->BSRR = M0_STEP_PIN;
   	else M0_STEP_PORT->BRR = M0_STEP_PIN;
   	break;
   case 1:
   	if(step_motors[id].clk) M1_STEP_PORT->BSRR = M1_STEP_PIN;
   	else M1_STEP_PORT->BRR = M1_STEP_PIN;
   	break;
   case 2:
   	if(step_motors[id].clk) M2_STEP_PORT->BSRR = M2_STEP_PIN;
   	else M2_STEP_PORT->BRR = M2_STEP_PIN;
   	break;
   case 3:
   	if(step_motors[id].clk) M3_STEP_PORT->BSRR = M3_STEP_PIN;
   	else M3_STEP_PORT->BRR = M3_STEP_PIN;
   	break;
  }
  if(!step_motors[id].clk) {
 		if(step_motors[id].steps!=0) step_motors[id].steps--;
  	if(step_motors[id].dir) step_motors[id].globalSteps++;
  	else	step_motors[id].globalSteps--;
  } else {
  	if(step_motors[id].steps == 0) step_motors[id].isInProc = FALSE;
  }
  step_motors[id].clk = !step_motors[id].clk;
	} else {
		if(steps_buf_sz > 0 && !step_motors[0].isInProc && !step_motors[1].isInProc &&
				 !step_motors[2].isInProc && !step_motors[3].isInProc) {
	 	__disable_irq();
			LINE_DATA *p = (LINE_DATA *)(&steps_buf[steps_buf_p1]);
			memcpy(&cur_steps_buf, p, sizeof(cur_steps_buf)); // for debug
	 	for(int i = 0; i < 4; i++)	{ step_motors[i].steps = p->steps[i]; step_motors[i].dir = p->dir[i]; }
	 	if(step_motors[0].steps) {
    //------------------------
#ifdef HAS_ENCODER
	 		if(isEncoderCorrection && step_motors[0].steps > ENCODER_CORRECTION_MIN_STEPS) {
		 		int32_t enVal = encoderXvalue();
		 		int32_t globalSteps = stepm_getCurGlobalStepsNum(0);
	 			int32_t dv = enVal*MM_PER_360*1000L/ENCODER_X_CNT_PER_360 - globalSteps*MM_PER_360*1000L/SM_X_STEPS_PER_360; // delta in 1/1000 mm

	 			encoderCorrectionDelta[0] = dv; // TODO for debug.. will be removed
	 			if(labs(encoderCorrectionMaxDelta[0]) < labs(encoderCorrectionDelta[0]))
	 				encoderCorrectionMaxDelta[0] = encoderCorrectionDelta[0];

	 			if(dv > ENCODER_X_MIN_MEASURE) {
	 				encoderCorrectionCntDown[0]++; // TODO for debug.. will be removed
	 				step_motors[0].globalSteps++;
	 				if(step_motors[0].dir) step_motors[0].steps--;
	 				else step_motors[0].steps++;
	 			}
	 			if(dv < -ENCODER_X_MIN_MEASURE) {
	 				encoderCorrectionCntUp[0]++; // TODO for debug.. will be removed
	 				step_motors[0].globalSteps--;
	 				if(step_motors[0].dir) step_motors[0].steps++;
	 				else step_motors[0].steps--;
	 			}
	 		}
#endif
    //------------------------
	 		step_motors[0].isInProc = TRUE;
	 	 GPIO_WriteBit(M0_DIR_PORT, M0_DIR_PIN, p->dir[0]? Bit_SET:Bit_RESET);
	 	 MOTOR_ON;
	 	 TIM2->PSC = p->pscValue[0];	TIM_SetAutoreload(TIM2, p->arrValue[0]);
	 	}
	 	if(step_motors[1].steps) {
	 		step_motors[1].isInProc = TRUE;
 	 	GPIO_WriteBit(M1_DIR_PORT, M1_DIR_PIN, p->dir[1]? Bit_SET:Bit_RESET);
	 	 MOTOR_ON;
 	 	TIM3->PSC = p->pscValue[1];	TIM_SetAutoreload(TIM3, p->arrValue[1]);
	 	}
	 	if(step_motors[2].steps) {
	 		//------------------------
#ifdef HAS_ENCODER
	 		if(isEncoderCorrection && step_motors[2].steps > ENCODER_CORRECTION_MIN_STEPS) {
		 		int32_t enVal = encoderZvalue();
		 		int32_t globalSteps = stepm_getCurGlobalStepsNum(2);
	 			int32_t dv = enVal*MM_PER_360*1000L/ENCODER_Z_CNT_PER_360 - globalSteps*MM_PER_360*1000L/SM_Z_STEPS_PER_360; // delta in 1/1000 mm

	 			encoderCorrectionDelta[2] = dv; // TODO for debug.. will be removed
	 			if(labs(encoderCorrectionMaxDelta[2]) < labs(encoderCorrectionDelta[2]))
	 				encoderCorrectionMaxDelta[2] = encoderCorrectionDelta[2];

	 			if(dv > ENCODER_Z_MIN_MEASURE) {
	 				encoderCorrectionCntDown[2]++; // TODO for debug.. will be removed
	 				step_motors[2].globalSteps++;
	 				if(step_motors[2].dir) step_motors[2].steps--;
	 				else step_motors[2].steps++;
	 			}
	 			if(dv < -ENCODER_Z_MIN_MEASURE) {
	 				encoderCorrectionCntUp[2]++; // TODO for debug.. will be removed
	 				step_motors[2].globalSteps--;
	 				if(step_motors[2].dir) step_motors[2].steps++;
	 				else step_motors[2].steps--;
	 			}
	 		}
#endif
	 		//------------------------
	 		step_motors[2].isInProc = TRUE;
	 	 GPIO_WriteBit(M2_DIR_PORT, M2_DIR_PIN,
#ifdef HAS_EXTRUDER
	 	 		p->dir[2]? Bit_SET:Bit_RESET);
#else
	 		  p->dir[2]? Bit_RESET:Bit_SET);
#endif
	 	 MOTOR_ON;
	 	 TIM4->PSC = p->pscValue[2];	TIM_SetAutoreload(TIM4, p->arrValue[2]);
	 	}
	 	if(step_motors[3].steps) {
	 		step_motors[3].isInProc = TRUE;
	 	 GPIO_WriteBit(M3_DIR_PORT, M3_DIR_PIN, p->dir[3]? Bit_RESET:Bit_SET);
	 	 MOTOR_ON;
	 	 TIM5->PSC = p->pscValue[3];	TIM_SetAutoreload(TIM5, p->arrValue[3]);
	 	}
			steps_buf_p1++;
			if(steps_buf_p1 >= STEPS_BUF_SZ) steps_buf_p1 = 0;
			steps_buf_sz--;
	 	__enable_irq();
		}
	}
}

void stepm_EmergeStop(void) {
	stepm_powerOff();
	for(int i = 0; i < 4; i++) {
		step_motors[i].isInProc = FALSE;	step_motors[i].steps = 0;
	}
	steps_buf_sz = steps_buf_p1 = steps_buf_p2 = 0;
}

void stepm_addMove(uint32_t steps[4], uint32_t frq[4], uint8_t dir[4]) {
	if(steps[0] == 0 && steps[1] == 0 && steps[2] == 0
#ifdef HAS_EXTRUDER
			&& steps[3] == 0
#endif
			) return;

	while(steps_buf_sz >= STEPS_BUF_SZ) __NOP();

	LINE_DATA *p = (LINE_DATA *)(&steps_buf[steps_buf_p2]);

#ifdef HAS_EXTRUDER
	for(int i = 0; i < 4;i++) {
#else
	for(int i = 0; i < 3;i++) {
#endif
		uint32_t f = frq[i];
	 if(f > (15000*K_FRQ)) f = 15000*K_FRQ; //15kHz
	 if(f < K_FRQ) f = K_FRQ; //1Hz
	 // 72Mhz/(psc*arr) = frq
	 uint32_t pscValue=1;
	 uint32_t arrValue=(72000000UL/2*K_FRQ)/f; // (1 falling age on 2 IRQ)
	 while((arrValue & 0xffff0000) != 0) { pscValue = pscValue << 1; arrValue = arrValue >> 1; }
	 pscValue--;
	 p->f[i] = frq[i]; // for debug
	 p->arrValue[i] = arrValue;
	 p->pscValue[i] = pscValue;
	 p->dir[i] = dir[i]; p->steps[i] = steps[i];
	}
	steps_buf_p2++;
	if(steps_buf_p2 >= STEPS_BUF_SZ) steps_buf_p2 = 0;
	__disable_irq(); steps_buf_sz++; __enable_irq();
}

int32_t stepm_getRemainLines(void) {
	return steps_buf_sz;
}

int32_t stepm_inProc(void) {
	if(steps_buf_sz > 0) return TRUE;
	for(int i = 0; i < 4; i++)
  if(step_motors[i].isInProc) return TRUE;
	return FALSE;
}

uint32_t stepm_LinesBufferIsFull(void) {
	return steps_buf_sz >= STEPS_BUF_SZ;
}

int32_t stepm_getCurGlobalStepsNum(uint8_t id) {
	return step_motors[id].globalSteps;
}

void stepm_ZeroGlobalCrd(void) {
	for(int i = 0; i < 4; i++) {
		step_motors[i].globalSteps = 0;
	}
}

void step_dump() {
	LINE_DATA *p = (LINE_DATA *)(&cur_steps_buf);
  for(int i = 0; i < 4; i++) {
	  scr_gotoxy(1,8+i);	scr_printf("%d,%d,%d,%d [%d]   ",	p->steps[i], p->dir[i], p->pscValue[i], p->arrValue[i], p->f[i]); // TODO
  }
}

