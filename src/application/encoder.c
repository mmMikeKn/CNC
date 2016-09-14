#include "global.h"

#ifdef HAS_ENCODER
int32_t encoderCorrectionCntUp[3], encoderCorrectionCntDown[3], encoderCorrectionDelta[3], encoderCorrectionMaxDelta[3];
int32_t encoderXoffset;
int8_t isEncoderCorrection = FALSE;

void encoder_int() {
 GPIO_InitTypeDef GPIO_InitStructure;
 TIM_TimeBaseInitTypeDef timer_base;

 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
 // encoder on Z. PC6, PC7
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; GPIO_Init(GPIOC, &GPIO_InitStructure);
 // encoder on X. PA0, PA1
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; GPIO_Init(GPIOA, &GPIO_InitStructure);

 RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE); RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);
 TIM_DeInit(TIM8); TIM_DeInit(TIM5);

 TIM_TimeBaseStructInit(&timer_base);
 timer_base.TIM_Period = 0xFFFF;
 timer_base.TIM_CounterMode = TIM_CounterMode_Up;
 TIM_TimeBaseInit(TIM8, &timer_base);
 TIM_TimeBaseInit(TIM5, &timer_base);

 TIM_ICInitTypeDef TIM_ICInitStruct;
 //Debounce filter
 TIM_ICInitStruct.TIM_Channel=TIM_Channel_1;
 TIM_ICInitStruct.TIM_ICFilter=3;
 TIM_ICInit(TIM8, &TIM_ICInitStruct); TIM_ICInit(TIM5, &TIM_ICInitStruct);
 TIM_ICInitStruct.TIM_Channel=TIM_Channel_2;
 TIM_ICInitStruct.TIM_ICFilter=3;
 TIM_ICInit(TIM8, &TIM_ICInitStruct); TIM_ICInit(TIM5, &TIM_ICInitStruct);

 TIM_EncoderInterfaceConfig(TIM8, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
 TIM_EncoderInterfaceConfig(TIM5, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

 // ≈сли рабочее поле больше 0x7FFF/4 шелчков энкодера, то нужно ловить переполнение

 TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
 NVIC_EnableIRQ(TIM5_IRQn);
 encoderXoffset = 0;

 TIM_Cmd(TIM8, ENABLE); TIM_Cmd(TIM5, ENABLE);
 encodersReset();
}

void encodersReset() {
	TIM8->CNT = 0x7FFF;
	TIM5->CNT = 0x7FFF;
 for(int i = 0; i < 3; i++) {
 	encoderCorrectionCntUp[i] = encoderCorrectionCntDown[i] = encoderCorrectionDelta[i] = encoderCorrectionMaxDelta[i] = 0;
 }
 encoderXoffset = 0;
}

#endif //HAS_ENCODER
