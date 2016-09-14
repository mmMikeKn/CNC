#include "global.h"


void kbd_init(void) {
 GPIO_InitTypeDef GPIO_InitStructure;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 GPIO_InitStructure.GPIO_Pin = COL1_PIN; GPIO_Init(COL1_PORT, &GPIO_InitStructure);
 GPIO_InitStructure.GPIO_Pin = COL2_PIN; GPIO_Init(COL2_PORT, &GPIO_InitStructure);
 GPIO_InitStructure.GPIO_Pin = COL3_PIN; GPIO_Init(COL3_PORT, &GPIO_InitStructure);
 GPIO_InitStructure.GPIO_Pin = COL4_PIN; GPIO_Init(COL4_PORT, &GPIO_InitStructure);

 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
 GPIO_InitStructure.GPIO_Pin = ROW0_PIN; GPIO_Init(ROW0_PORT, &GPIO_InitStructure);
 GPIO_InitStructure.GPIO_Pin = ROW1_PIN; GPIO_Init(ROW1_PORT, &GPIO_InitStructure);
 GPIO_InitStructure.GPIO_Pin = ROW2_PIN; GPIO_Init(ROW2_PORT, &GPIO_InitStructure);
 GPIO_InitStructure.GPIO_Pin = ROW3_PIN; GPIO_Init(ROW3_PORT, &GPIO_InitStructure);
}

static volatile int16_t _key;

void kbd_proc(void) {
 static uint8_t scan_stage = 0;
 if(++scan_stage >= 8) scan_stage = 0;
 uint8_t col = scan_stage >> 1;

 if((scan_stage & 0x01) == 0) {
 	COL1_PORT->BRR = COL1_PIN; COL2_PORT->BRR = COL2_PIN;
 	COL3_PORT->BRR = COL3_PIN; COL4_PORT->BRR = COL4_PIN;
  switch(col) {
   case 0: COL1_PORT->BSRR = COL1_PIN; break;
   case 1: COL2_PORT->BSRR = COL2_PIN; break;
   case 2: COL3_PORT->BSRR = COL3_PIN; break;
   case 3: COL4_PORT->BSRR = COL4_PIN; break;
  }
 } else {
 	if(ROW0_PORT->IDR & ROW0_PIN) _key = 0x10|col;
 	if(ROW1_PORT->IDR & ROW1_PIN) _key = 0x20|col;
 	if(ROW2_PORT->IDR & ROW2_PIN) _key = 0x30|col;
 	if(ROW3_PORT->IDR & ROW3_PIN) _key = 0x40|col;
 }
//	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == Bit_RESET) kbd_putKey(0x07F); // key on main board
}

int kbd_getKey(void) {
	int key = _key;
	_key = -1;
 return key;
}
