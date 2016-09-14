#ifndef ENCODER_H_
#define ENCODER_H_

#ifdef HAS_ENCODER
void encoder_int();
void encodersReset();

extern int32_t encoderXoffset;

inline int32_t encoderZvalue() { return (0x007FFF-(int32_t)TIM_GetCounter(TIM8))>> 2; }
inline int32_t encoderXvalue() { return ((0x007FFF-(int32_t)TIM_GetCounter(TIM5))>> 2) + encoderXoffset; }

// ��������� ������������ ������ �� �������� ��� ������������ ������� ����������� > ENCODER_CORRECTION_MIN_STEPS.
// ��� ��������� "������" �� �������� ���������, � ������ ���� ��������� ���. 
// ��� ���������� ������� ������ ������� ���������� ������ ���� �� ��� 
// ����������� �� ����������� �� ��� ��������������� � �� ��� ��� ���� �� ���� ���������� � ����� �����
#define ENCODER_CORRECTION_MIN_STEPS 150

#define ENCODER_Z_CNT_PER_360 512
#define ENCODER_X_CNT_PER_360 512

// ���������� �� ����������� ���������� ��� ���������� �������� �������� �������������� � 1/1000 ��.
// ���������� � �������� �� -ENCODER_Z_MIN_MEASURE �� ENCODER_Z_MIN_MEASURE - ����������.
// �������� ����� "�� ����� �����������" (��������� ������, ������ ��������). �� �����. ����� ������.
#define ENCODER_Z_MIN_MEASURE (MM_PER_360*1500/ENCODER_Z_CNT_PER_360) // 0.014 mm
#define ENCODER_X_MIN_MEASURE (MM_PER_360*1500/ENCODER_Z_CNT_PER_360) // 0.014 mm

extern int8_t isEncoderCorrection; // correction On/Off 

// ���������� ���������� ��� ������ �� ������ �� ����� ������. �� ��� ���� ����� �� �����.
// TODO for debug.. will be removed
extern int32_t encoderCorrectionCntUp[3], encoderCorrectionCntDown[3], encoderCorrectionDelta[3], encoderCorrectionMaxDelta[3];
#endif // HAS_ENCODER

#endif
