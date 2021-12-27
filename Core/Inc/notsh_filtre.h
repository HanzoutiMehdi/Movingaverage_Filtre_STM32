/**
  ******************************************************************************
  * @file    notsh_filtre.h
  * @author  Mehdi 
  * @date    21-July-2021
  * @brief   This header file contains the Fir prototype
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NOTSH_FILTRE_H
#define __NOTSH_FILTRE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "math.h"




typedef struct{
   /*filtre coeffecion*/
	float alpha;
	float beta;
	 /*Input arrary*/
	float x[3];

	/*Output arrary*/
	float y[3];

	
	
}NotchFiltre;


void  NotchFiltre_Init(NotchFiltre *filt, float CenterFReq, float notchWidth_Hz,float SampleTime_s);
float NotchFiltre_Update(NotchFiltre *filt, float inp);



#ifdef __cplusplus
}
#endif

#endif /* NOTCH_FILTRE */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
