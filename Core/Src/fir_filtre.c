
/**
  ******************************************************************************
  * @file           : fir_filtre
  * @brief          :
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "myfir.h"


#define MOVING_AVG_FILRE

static float FIR_IMPULSE_RESPONSE[FIR_FILTRE_LENGTH];

/**
  * @brief  TFIRFiltre_Init
  * @retval
  */
void FIRFiltre_Init(FIRFiltre *fir)
{

	for (uint8_t n=0; n<FIR_FILTRE_LENGTH; n++)
	{
		fir->buf[n]=0.0f;
	}


	/*set buffer index */
	fir->bufIndex = 0 ;

	/*Clear output */

	fir->out = 0.0f;

#ifdef MOVING_AVG_FILRE
	/*Init h[n] Moving avearge Filtre*/
	for (uint8_t n=0;n<FIR_FILTRE_LENGTH ;n++)
	{
		FIR_IMPULSE_RESPONSE[n] = (float)1/(float)FIR_FILTRE_LENGTH;
	}

#endif


}

/**
  * @brief  FIRFiltre_Update
  * @retval int
  */
float FIRFiltre_Update(FIRFiltre *fir, float inp)
{

  /*Store latest sample in buffer*/
   fir->buf[fir->bufIndex] = inp;

   /*Increment Buff indew and wrap arround if necessary */
   if (fir->bufIndex<FIR_FILTRE_LENGTH)
   {
	   fir->bufIndex++;
   }
   else
   {
	   fir->bufIndex=0.0f;
   }

   /*Compute new output sample (via convulation) */
   fir->out=0.0f;


   uint8_t sumIndex=fir->bufIndex;


	for (uint8_t n=0; n<FIR_FILTRE_LENGTH; n++)
	{
		/*Decrement index and wrap if necessary  */
		if (sumIndex>0)
		{
			sumIndex--;
		}
		else
		{
			sumIndex=FIR_FILTRE_LENGTH-1U;

		}
		/*Multiply impulse reponse with shifted sample and add to output*/

		fir->out += FIR_IMPULSE_RESPONSE[n] * fir->buf[sumIndex];


	}

   return fir->out;



}









/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
