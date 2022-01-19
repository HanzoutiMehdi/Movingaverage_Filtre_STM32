/**
  ******************************************************************************
  * @file    rc_filtre.h
  * @author  Mehdi 
  * @date    21-July-2021
  * @brief   This header file contains the Fir prototype
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RC_FILTRE_H
#define __RC_FILTRE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>




typedef struct{
	float coeff[2];
	float out[2];
	
	
}RCFiltre;


void RCFilter_Init(RCFiltre *filt, float cutoffFreqHz, float sampleTimeS) ;
float RCFilter_Update(RCFiltre *filt, float inp);



#ifdef __cplusplus
}
#endif

#endif /* __ACCELERO_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
