#ifndef ENCODER_H_
#define ENCODER_H_

#ifdef HAS_ENCODER
void encoder_int();
void encodersReset();

extern int32_t encoderXoffset;

inline int32_t encoderZvalue() { return (0x007FFF-(int32_t)TIM_GetCounter(TIM8))>> 2; }
inline int32_t encoderXvalue() { return ((0x007FFF-(int32_t)TIM_GetCounter(TIM5))>> 2) + encoderXoffset; }

// коррекция производится только на участках где относительно длинное перемещение > ENCODER_CORRECTION_MIN_STEPS.
// это позволяет "забить" на пересчет скоростей, а просто тупо добавлять шаг. 
// При достаточно большем длинне отрезка добавление одного шага на ось 
// практически не сказывается на его прямолинейности и то что все шаги по осям закончатся в одной точке
#define ENCODER_CORRECTION_MIN_STEPS 150

#define ENCODER_Z_CNT_PER_360 512
#define ENCODER_X_CNT_PER_360 512

// отклонение от расчитанной траектории при превышении которого начинаем корректировать в 1/1000 мм.
// отклонения в коридоре от -ENCODER_Z_MIN_MEASURE до ENCODER_Z_MIN_MEASURE - игнорируем.
// Значение взято "из общих соображений" (жесткости станка, люфтов механики). Не догма. можно менять.
#define ENCODER_Z_MIN_MEASURE (MM_PER_360*1500/ENCODER_Z_CNT_PER_360) // 0.014 mm
#define ENCODER_X_MIN_MEASURE (MM_PER_360*1500/ENCODER_Z_CNT_PER_360) // 0.014 mm

extern int8_t isEncoderCorrection; // correction On/Off 

// накопление статистики для показа на экране во время работы. Ни для чего более не нужно.
// TODO for debug.. will be removed
extern int32_t encoderCorrectionCntUp[3], encoderCorrectionCntDown[3], encoderCorrectionDelta[3], encoderCorrectionMaxDelta[3];
#endif // HAS_ENCODER

#endif
