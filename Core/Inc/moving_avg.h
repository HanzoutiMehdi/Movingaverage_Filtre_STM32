/**
  ******************************************************************************
  * @file    moving average .h
  * @author  Mehdi 
  * @date    21-July-2021
  * @brief   This header file contains the mov average filter prototype
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MOV_FILTRE_H
#define __MOV_FILTRE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>



#define MOV_FILTRE_LENGTH 20
typedef struct{
	float buf[MOV_FILTRE_LENGTH];
	uint8_t bufIndex;
	float out;
	
	
}MovFiltre;


void MovFiltre_Init(MovFiltre *fir);

float MovFiltre_Update(MovFiltre *fir, float inp);


#ifdef __cplusplus
}
#endif

#endif /* __ACCELERO_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
