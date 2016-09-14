#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

void SystemStartup(void);

void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);
void Reset_Device(void);

//extern RCC_ClocksTypeDef RCC_Clocks;

#define USB_DISCONNECT                    GPIOC
#define USB_DISCONNECT_PIN                GPIO_Pin_13
#define BULK_MAX_PACKET_SIZE  0x00000040


#endif  /*__HW_CONFIG_H*/
