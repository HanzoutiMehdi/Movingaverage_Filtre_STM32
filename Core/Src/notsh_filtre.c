
/**
  ******************************************************************************
  * @file           : NOTCH filtre
  * @brief          :
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "notsh_filtre.h"






/**
  * @brief  Filtre_Init
  * @retval
  */

void  NotchFiltre_Init(NotchFiltre *filt, float CenterFreq_HZ, float notchWidth_Hz,float SampleTime_s)
{

	/*Convert Filtre freq to angular frequyency rad/s*/

	float w0_rps=2.0f*M_PI*CenterFreq_HZ;
	float ww_rps=2.0f*M_PI*notchWidth_Hz;

	/* W* = 2/T tan(w.T/2) */
	float w0_pw_rps= (2.0f / SampleTime_s) * tanf(0.5f * w0_rps*SampleTime_s);

	/*Compute Filtre coef*/
	/*Alpha= 4+ (w0T)² Beta = 2wwT*/
	filt->alpha = 4.0f + w0_pw_rps *w0_pw_rps* SampleTime_s*SampleTime_s;
    filt->beta = 2.0f *  ww_rps*SampleTime_s;



	/*Clear output */
     for(uint8_t i=0;i<3;i++)
     {
    	 filt->x[i] =0.0f;
    	 filt->y[i] =0.0f;
     }




}

/**
  * @brief  FIRFiltre_Update
  * @retval int
  */
float NotchFiltre_Update(NotchFiltre *filt, float inp)
{

  /*Shift samples*/
  filt->x[2]=filt->x[1];
  filt->x[1]=filt->x[0];

  filt->y[2]=filt->y[1];
  filt->y[1]=filt->y[0];

  /*store the new input*/
  filt->x[0]=inp;

  /*Compute new output*/
  //
  filt->y[0]= ((filt->alpha *filt->x[0] + 2.0f* (filt->alpha - 8.0f)*filt->x[1] +  filt->alpha*filt->x[2])
               -(2.0f *(filt->alpha-8.0f)*filt->y[1] +  (filt->alpha -filt->beta) * filt->y[2]))
                 /(filt->alpha+filt->beta);


return filt->y[0];

}









/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
