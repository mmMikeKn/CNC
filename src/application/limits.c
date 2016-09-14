#include "global.h"

#ifdef HAS_HWD_LIMITS
void limits_init(void) {
 GPIO_InitTypeDef GPIO_InitStructure;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;

 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
 GPIO_InitStructure.GPIO_Pin = XPIN; GPIO_Init(XPORT, &GPIO_InitStructure);
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
 GPIO_InitStructure.GPIO_Pin = YPIN; GPIO_Init(YPORT, &GPIO_InitStructure);
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
 GPIO_InitStructure.GPIO_Pin = ZPIN; GPIO_Init(ZPORT, &GPIO_InitStructure);
}

#endif

