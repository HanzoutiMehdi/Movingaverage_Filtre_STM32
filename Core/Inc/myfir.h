/**
  ******************************************************************************
  * @file    fir.h
  * @author  Mehdi 
  * @date    21-July-2021
  * @brief   This header file contains the Fir prototype
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FIR_FILTRE_H
#define __FIR_FILTRE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>



#define Nn 81

#define FIR_FILTRE_LENGTH Nn
typedef struct{
	float buf[FIR_FILTRE_LENGTH];
	uint8_t bufIndex;
	float out;
	
	
}FIRFiltre;


void FIRFiltre_Init(FIRFiltre *fir);
float FIRFiltre_Update(FIRFiltre *fir, float inp);



#ifdef __cplusplus
}
#endif

#endif /* __ACCELERO_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
