
/**
  ******************************************************************************
  * @file           : moving average filter
  * @brief          :
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "moving_avg.h"



static float FIR_IMPULSE_RESPONSE[MOV_FILTRE_LENGTH];



/**
  * @brief  TFIRFiltre_Init
  * @retval
  */
void MovFiltre_Init(MovFiltre *fir)
{

	for (uint8_t n=0; n<MOV_FILTRE_LENGTH; n++)
	{
		fir->buf[n]=0.0f;
	}


	/*set buffer index */
	fir->bufIndex = 0 ;

	/*Clear output */

	fir->out = 0.0f;

	/*Init h[n] Moving avearge Filtre*/
	for (uint8_t n=0;n<MOV_FILTRE_LENGTH ;n++)
	{
		FIR_IMPULSE_RESPONSE[n] = (float)1/(float)MOV_FILTRE_LENGTH;
	}



}

/**
  * @brief  FIRFiltre_Update
  * @retval int
  */
float MovFiltre_Update(MovFiltre *fir, float inp)
{

  /*Store latest sample in buffer*/
   fir->buf[fir->bufIndex] = inp;

   /*Increment Buff indew and wrap arround if necessary */
   if (fir->bufIndex<MOV_FILTRE_LENGTH)
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


	for (uint8_t n=0; n<MOV_FILTRE_LENGTH; n++)
	{
		/*Decrement index and wrap if necessary  */
		if (sumIndex>0)
		{
			sumIndex--;
		}
		else
		{
			sumIndex=MOV_FILTRE_LENGTH-1U;

		}
		/*Multiply impulse reponse with shifted sample and add to output*/

		fir->out += FIR_IMPULSE_RESPONSE[n] * fir->buf[sumIndex];


	}

   return fir->out;



}




/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
