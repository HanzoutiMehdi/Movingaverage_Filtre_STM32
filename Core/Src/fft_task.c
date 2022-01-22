/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : fft_task.c
  * @brief          : fft task
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "arm_math.h"
#include "hamm128.h"
/*FFT---------------------*/


extern QueueHandle_t xQueue_fft;

/**/
extern SimuleTypeDef  hSim;


void fft_task(void const * argument);
static void dft(FFT_TypeDef *hfft, uint32_t buffer);
static void fft(FFT_TypeDef *hfft, uint32_t buffer,ComplexTypeDef *w);
float32_t AmpResult_fft[FFT_LENGTH];
float32_t Ampinp_fft[FFT_LENGTH];
uint32_t uAmpIn_fft[FFT_LENGTH];
uint32_t uAmpResult_fft[FFT_LENGTH];

static void InitTwiddle(void);

ComplexTypeDef twiddle[FFT_LENGTH];




FFT_TypeDef  in_fft;
FFT_TypeDef  out_fft;

float32_t arm_sig_Input[FFT_LENGTH*2];
float32_t arm_sig_filt[FFT_LENGTH];
arm_cfft_radix4_instance_f32 arm_fft,arm_fft2;    /* ARM CFFT module */



/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
static void dft(FFT_TypeDef *hfft, uint32_t buffer)
{


  uint32_t n,k ;
  uint32_t indice_start;
  uint32_t indice_end;

  if (  buffer==FIRST_HALF)
  {
	indice_start=0 ;
	indice_end  =FFT_LENGTH/2;

  }
  else
  {
		indice_start=FFT_LENGTH/2 ;
		indice_end  =FFT_LENGTH;

  }


   /*Hamming Window*/
  if (hfft->fft_windowing==HM_WIND)
  {
	  for (uint32_t i=indice_start; i<indice_end; i++)
	  {
	   hfft->Input[i].real=hfft->Input[i].real*hamming[i];
	  }


  }


	  for (k=indice_start; k<indice_end; k++)
	  {
		 /*Clear */
		  hfft->Result[k]=0.0f;
		  hfft->Result[k+1]=0.0f;

	     for (n=indice_start ; n<indice_end ; n++)
     	   {
		     hfft->Result[k] += hfft->Input[n].real*cosf(2*PI*k*n/FFT_LENGTH) + hfft->Input[n].imag*sinf(2*M_PI*k*n/FFT_LENGTH);
		     hfft->Result[k+1] += hfft->Input[n].imag*cosf(2*PI*k*n/FFT_LENGTH) - hfft->Input[n].real*sinf(2*M_PI*k*n/FFT_LENGTH);
	       }
	     k++;
      }

}
/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
static void fft(FFT_TypeDef *hfft, uint32_t buffer,ComplexTypeDef *w)
{

	 uint32_t n,k;
	  uint32_t indice_start;
	  uint32_t indice_end;

	  if (  buffer==FIRST_HALF)
	  {
		indice_start=0 ;
		indice_end  =FFT_LENGTH/2;

	  }
	  else
	  {
			indice_start=FFT_LENGTH/2 ;
			indice_end  =FFT_LENGTH;

	  }


	  if (hfft->fft_windowing==HM_WIND)
	  {
		  for (uint32_t i=indice_start; i<indice_end; i++)
		  {
		   hfft->Input[i].real=hfft->Input[i].real*hamming[i];
		  }


	  }







  for (k=indice_start ; k<indice_end ; k++)
   {
		 /*Clear */
		  hfft->Result[k]=0.0f;
		  hfft->Result[k+1]=0.0f;

	  for (n=indice_start ; n<indice_end ; n++)
	  {
		  hfft->Result[k] += hfft->Input[n].real*w[(n*k) % FFT_LENGTH].real   - hfft->Input[n].imag*w[(n*k) % FFT_LENGTH ].imag;
		  hfft->Result[k+1] += hfft->Input[n].imag*w[(n*k) % FFT_LENGTH].real   +  hfft->Input[n].real*w[(n*k) % FFT_LENGTH].imag;
	  }
	   k++;
  }







}
float SAMPLING_FREQ = 100;
float FReqIn = 5.0;

/**
  * @brief  InitTwiddle
  * @retval None
  */

static void InitTwiddle(void)
{

	uint32_t n;

	 for(n=0 ; n<FFT_LENGTH ; n++)
	 {
	 twiddle[n].real = cos(2*PI*n/FFT_LENGTH);
	 twiddle[n].imag = -sin(2*PI*n/FFT_LENGTH);
	 }
}




/**
  * @brief  fft_task
  * @retval None
  */
void fft_task(void const * argument)
{
   uint32_t rx_mesage;
   uint32_t maxValue,maxIndex;




   /*Init FFT */
   InitTwiddle();


   /* Initialize the CFFT/CIFFT module, intFlag = 0, doBitReverse = 1 */
   arm_cfft_radix4_init_f32(&arm_fft, FFT_LENGTH, 0, 1);
   arm_cfft_radix4_init_f32(&arm_fft2, FFT_LENGTH, 0, 1);


   /*init struct*/
   hSim.in_fft=  &in_fft;
   hSim.out_fft=  &out_fft;



	while(1)
	{


		 if( xQueueReceive( xQueue_fft,  &( rx_mesage ),  20 ) == pdPASS )
		 {

           if(hSim.fft.dft_enable==1)
           {



		   	  dft(&in_fft,rx_mesage);

		   	  dft(&out_fft,rx_mesage);
           }


           if(hSim.fft.fft_enable==1)
           {

        	   fft(&in_fft,rx_mesage,twiddle);

        	   fft(&out_fft,rx_mesage,twiddle);
           }



           if(hSim.fft.arm_enable==1)
           {

        	   for (uint32_t i=0,j=0;i<(FFT_LENGTH*2);i++,j++)
        	   {

        		   arm_sig_Input[i]=in_fft.Input[j].real;
        		   arm_sig_Input[i+1]=in_fft.Input[j].real;
        		   i++;

        	   }

               /* Process the data through the CFFT/CIFFT module */
                arm_cfft_radix4_f32(&arm_fft, arm_sig_Input);

                arm_cfft_radix4_f32(&arm_fft2, arm_sig_filt);


        	   fft(&in_fft,rx_mesage,twiddle);

        	   fft(&out_fft,rx_mesage,twiddle);
           }




		   	  /*Calcul amplitud when buffer is already processed */

		   	  if (rx_mesage==SECOND_HALF)
		   	  {

		   		 arm_cmplx_mag_f32(in_fft.Result ,(float*)Ampinp_fft,FFT_LENGTH);
		        /* Calculates maxValue and returns corresponding value */
		        arm_max_f32(Ampinp_fft, FFT_LENGTH, &maxValue, &maxIndex);


		   	   for(uint32_t i=0;i<FFT_LENGTH;i++)
		   		uAmpIn_fft[i]=Ampinp_fft[i];


		   	   arm_cmplx_mag_f32(out_fft.Result ,(float*)AmpResult_fft,FFT_LENGTH);

		   	   for(uint32_t i=0;i<FFT_LENGTH;i++)
		   		uAmpResult_fft[i]=AmpResult_fft[i];



		   	  }





		 }


	}



}





/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
