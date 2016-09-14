#include <string.h>
#include <stdarg.h>

#include "global.h"
#include "Crc32.h"
#include <stm32f10x_usart.h>

#define TX_BUF_SZ 1024
static volatile unsigned char _send_data_u1[TX_BUF_SZ];
static volatile uint32_t sptr1_u1, sptr2_u1, scnt_u1;


void rs232_init(void) {
//======================== RF UART =======================================
 // USART1_Tx	- PA9	(на Rx интерфейса)
 // USART1_Rx	- PA10	(на Tx интерфейса)
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
 GPIO_InitTypeDef GPIO_InitStructure;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitTypeDef NVIC_InitStructure;
 NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
 GPIO_Init(GPIOA, &GPIO_InitStructure);
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
 GPIO_Init(GPIOA, &GPIO_InitStructure);

 USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART1, &USART_InitStructure);
	USART_Cmd(USART1, ENABLE);
	sptr1_u1 = sptr2_u1 = scnt_u1 = 0;
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
}

void rs232_proc() {
 if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
 	USART_SendData(USART1, USART_ReceiveData(USART1));// echo
 }
	if (USART_GetITStatus(USART1, USART_IT_TXE) != RESET) {
	 if(sptr1_u1 != sptr2_u1) {
	 	USART_SendData(USART1, (uint16_t)_send_data_u1[sptr1_u1]);
	 	sptr1_u1++; scnt_u1--;
	  if(sptr1_u1 >= sizeof(_send_data_u1)) sptr1_u1 = 0;
	 }
	 if(sptr1_u1 == sptr2_u1) USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
	}
}

void rf_putc(char c) {
	while(scnt_u1 == (sizeof(_send_data_u1)-1)) {}
 _send_data_u1[sptr2_u1] = c; sptr2_u1++; scnt_u1++;
 if(sptr2_u1 >= sizeof(_send_data_u1)) sptr2_u1 = 0;
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}

void rf_puts(char *str) {
	for(int i = 0; str[i] != 0; i++) rf_putc(str[i]);
}

static unsigned char rf_itoa(long val, int radix, int len, char *sout, unsigned char ptr) {
	unsigned char c, r, sgn = 0, pad = ' ';
	unsigned char s[20], i = 0;
	unsigned long v;

	if (radix < 0) {
		radix = -radix;
		if (val < 0) {		val = -val;	sgn = '-';	}
	}
	v = val;
	r = radix;
	if (len < 0) {	len = -len;	pad = '0'; }
	if (len > 20) return ptr;
	do {
		c = (unsigned char)(v % r);
		if (c >= 10) c += 7;
		c += '0';
		s[i++] = c;
		v /= r;
	} while (v);
	if (sgn) s[i++] = sgn;
	while (i < len)	s[i++] = pad;
	do	sout[ptr++] = (s[--i]);
	while (i);
	return ptr;
}

void rf_printf(const char* str, ...) {
	va_list arp;
	int d, r, w, s, l;
	va_start(arp, str);
	char sout[256];
	unsigned char ptr = 0;

	while ((d = *str++) != 0) {
			if (d != '%') {	sout[ptr++]=d; continue;	}
			d = *str++; w = r = s = l = 0;
			if (d == '0') {
				d = *str++; s = 1;
			}
			while ((d >= '0')&&(d <= '9')) {
				w += w * 10 + (d - '0');
				d = *str++;
			}
			if (s) w = -w;
			if (d == 'l') {
				l = 1;
				d = *str++;
			}
			if (!d) break;
			if (d == 's') {
				char *s = va_arg(arp, char*);
				while(*s != 0) { sout[ptr++] = *s; s++; }
				continue;
			}
			if (d == 'c') {
				sout[ptr++] = (char)va_arg(arp, int);
				continue;
			}
			if (d == 'u') r = 10;
			if (d == 'd') r = -10;
			if (d == 'X' || d == 'x') r = 16; // 'x' added by mthomas in increase compatibility
			if (d == 'b') r = 2;
			if (!r) break;
			if (l) {
				ptr = rf_itoa((long)va_arg(arp, long), r, w, sout, ptr);
			} else {
				if (r > 0) ptr = rf_itoa((unsigned long)va_arg(arp, int), r, w, sout, ptr);
				else	ptr = rf_itoa((long)va_arg(arp, int), r, w, sout, ptr);
			}
	}
	va_end(arp);
	sout[ptr] = 0;
	rf_puts(sout);
}
