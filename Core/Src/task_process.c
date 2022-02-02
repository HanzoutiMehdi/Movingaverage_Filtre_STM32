/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "usb_device.h"
#include "usb_device.h"
#include "stdlib.h"
#include "myfir.h"
#include "iir_filtre.h"
#include "notsh_filtre.h"
#include "rc_filtre.h"
#include "moving_avg.h"

extern QueueHandle_t xQueue_fft;

/*NOTCH FILTRE*/
NotchFiltre  hnotch;
#define SAMPLE_TIME_MS_TASK            10 /*ms*/  //500 Hz

#define NOTCH_FILTRE_CENTER_HZ         5.0f
#define NOTCH_FILTRE_WIDTH_HZ          0.1f
#define NOTCH_FILTRE_SAMPLETIME_S      (0.001f* SAMPLE_TIME_MS_TASK) /*ms to sencoed*/



/*IIR FILTRE */
IIRFiltre   hiir;
#define APLFA_IIR    0.5f


/*FIR FILTRE*/
FIRFiltre lpfAcc;

void vmainTask(void const * argument);
uint8_t float_toString(float value, char *ch, uint8_t type);
uint8_t float_toString_dual(float value,float value2 ,char *ch);
uint8_t SerialPrint_Value(float value, uint8_t type);
uint8_t SerialPrint_DualValue(float value1,float value2);
uint8_t SerialPrint_ThriValue(float value1,float value2,float value3);
uint8_t float_toString_Third(float value,float value2 ,float value3,char *ch);
static void StoreNewSample(FFT_TypeDef *hfft, float in);
#define WAIT_NEXT_VALUE            1U
#define PRINT_VALUE                0U
int16_t  Acc[3];






MovFiltre movfil;


SimuleTypeDef  hSim;

float Gyro[3];

RCFiltre  rc_filt;

float cutoffFreqHz = 20.0f;
#define ACC







static void filtre_Init(void);
static float Get_NewSample(void);
static float filtre_Sample(float in);


static void Display_Sample(float inputSignal, float output);

extern FFT_TypeDef  in_fft;
extern FFT_TypeDef  out_fft;


/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void vmainTask(void const * argument)
{
   	/*init */
	float inputSignal;
    float filt_out;


   	/*Init Sim*/
  	hSim.In.inputFreq1_Hz=5.0f ;   /*5Hz*/
  	hSim.In.inputFreq2_Hz=30.0f;    /*30Hz*/
  	hSim.In.inputFreq3_Hz=12.0f ;   /*12Hz*/

  	/*Enable Filtre */
  	hSim.fir_enable=1;
    hSim.In.Type=DUAL_SIN_WAVE;

    /*Enable DFT*/
    hSim.fft.dft_enable=1;


    /*Init FIR*/
	FIRFiltre_Init(&lpfAcc);

    /*Init IIR*/
	IIRFiltre_Init(&hiir,APLFA_IIR);

    /*Notch filtre init*/
	NotchFiltre_Init(&hnotch,NOTCH_FILTRE_CENTER_HZ,NOTCH_FILTRE_WIDTH_HZ, NOTCH_FILTRE_SAMPLETIME_S);


	 /*Rc Filter */
	 RCFilter_Init(&rc_filt,  cutoffFreqHz,  NOTCH_FILTRE_SAMPLETIME_S);


	 /*Moving Average Filter Init*/
	 MovFiltre_Init(&movfil);


	while(1)
	{

      osDelay(SAMPLE_TIME_MS_TASK); /*100hz*/



      /*Input STep get new sample (hSim.In.Type) -----------------------------------*/
      inputSignal= Get_NewSample();


     /*---FIltre Step --------------------------------------------------*/
     filt_out= filtre_Sample(inputSignal);


     /*FFT Step---------------------------------------------------------*/
     if(hSim.fft.fft_enable==1U)
     {
   	 // fft(&in_fft, inputSignal);
   	 // fft(&out_fft, filt_out);

     }

     if((hSim.fft.dft_enable==1U) ||(hSim.fft.fft_enable==1U))
     {
    	 StoreNewSample(&in_fft,inputSignal );
    	 StoreNewSample(&out_fft,filt_out );

    	  if (out_fft.current_sample==FFT_LENGTH/2)
    	  {
    		  uint32_t Process_buff=FIRST_HALF;
    		  /*Send HalfBuffer Process*/
    		  xQueueSend(xQueue_fft,  ( void * ) &Process_buff, 0 );
    	  }



    	  if (out_fft.current_sample==0)
    	  {
    		  uint32_t Process_buff=SECOND_HALF;
    		  /*Send HalfBuffer Process*/
    		  xQueueSend(xQueue_fft,  ( void * ) &Process_buff, 0 );
    	  }


     }

      /*Display filtre output value------------------------------------------*/
      Display_Sample(inputSignal,filt_out);


	}



}


/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
static void StoreNewSample(FFT_TypeDef *hfft, float in)
{
   uint32_t nbre_sample  =hfft->current_sample;

  uint32_t n,k;


  /*Prepare Input Buffer*/
  hfft->Input[nbre_sample].real=in;
  hfft->Input[nbre_sample].imag=0.0;

  hfft->current_sample++;


  if (nbre_sample>=FFT_LENGTH)
  {
	  /*Send Second HalfBuffer Process*/
	  hfft->current_sample=0;

  }

}




/**
  * @brief  Display_Sample
  * @retval Send to USB

  */
static void Display_Sample(float inputSignal, float output)
{

   	  SerialPrint_DualValue(inputSignal,   output);

}

/**
  * @brief  filtre_Sample
  * @retval Process Filtre depending hSim.fir / notch/iir ..

  */
static float filtre_Sample(float inputSignal)
{
	float out=0.0;

	switch (hSim.filtre_Type)
	{

    	case FIR_FILTRE:
    	{
    	   /*FIR update*/
    	   FIRFiltre_Update(&lpfAcc,inputSignal);
    	   out=lpfAcc.out;
     		break;
    	}

    	case IIR_FILTRE:
    	{
           /*IIR update*/
           IIRFiltre_Update(&hiir, inputSignal);
           out= hiir.out;
    		break;
    	}
    	case MOVING_AVG_FILTRE:
    	{

    		out=MovFiltre_Update(&movfil,inputSignal);

    		break;
    	}

    	case NOTCH_FILTRE:
    	{
    		out=NotchFiltre_Update(&hnotch,inputSignal);
    		break;
    	}
    	case RC_FILTRE:
    	{
    		RCFilter_Update(&rc_filt,inputSignal);
    		out= rc_filt.out[0];
    		break;

    	}
    	default:
    	{
    		while(1);
    	}

	}


	return out;

 }





/**
  * @brief  Get_NewSample
  * @retval Get New Sample of an input Signa according to hSim.In.Type

  */
static void filtre_Init(void)
{

}

/**
  * @brief  Get_NewSample
  * @retval Get New Sample of an input Signa according to hSim.In.Type

  */
static float Get_NewSample(void)
{
	float inputSignal;
	    /*get time ----*/
	    uint32_t time_s=HAL_GetTick();

	    float inputSin = 100.0f*sin(2.0f*M_PI*hSim.In.inputFreq1_Hz*(0.001f*time_s ));


	    float inputSin2 = 100.0f*sin(2.0f*M_PI*hSim.In.inputFreq2_Hz*(0.001f*time_s ));

	    float inputSin3 = 100.0f*sin(2.0f*M_PI*hSim.In.inputFreq3_Hz*(0.001f*time_s ));

	   if (hSim.In.Type==DUAL_SIN_WAVE)
	   {
	 	     /**/
	  	inputSignal=inputSin+inputSin2;
	   }

	  else if (hSim.In.Type==THRID_SIN_WAVE)
	    {
	   	     /**/
	  	inputSignal=inputSin+inputSin2+inputSin3;
	    }

	  else if (hSim.In.Type==SIN_WAVE)
	    {
	   	     /**/
	  	inputSignal=inputSin;
	    }
	    else if (hSim.In.Type==GYRO_WAVE)
	    {
	  	//    (void)BSP_GYRO_GetXYZ(Gyro);
	  	 // inputSignal=Gyro[1];

	    }
	    else
	  {
	  	  (void)BSP_ACCELERO_GetXYZ(Acc);
	  	inputSignal=Acc[1];
	  }


	   return  inputSignal;

}





/**
  * @brief  Serial Print to Oscilo
  * @retval type: 0 print stored value
  *             : 1 store value and not print, wait for other value to be sent
  */
uint8_t SerialPrint_DualValue(float value1,float value2)
{
#define CH_LENGHT          100U
	uint8_t ret;
	static char ch[CH_LENGHT];

	/*Clear ch*/
	for (uint8_t i=0;i<CH_LENGHT;i++)
	{
		ch[i]=0;
	}

	/*float to string on shot value :print only one value*/
	ret=float_toString_dual(value1,value2,ch);
	if (ret !=0)
	{
		return 1;
	}

	uint16_t len=strlen(ch);

	/*send buffer to USB CDC class */
	ret=CDC_Transmit_FS(ch,len);



	return ret;

}


/**
  * @brief  Serial Print to Oscilo
  * @retval type: 0 print stored value
  *             : 1 store value and not print, wait for other value to be sent
  */
uint8_t SerialPrint_ThriValue(float value1,float value2,float value3)
{
#define CH_LENGHT          100U
	uint8_t ret;
	static char ch[CH_LENGHT];

	/*Clear ch*/
	for (uint8_t i=0;i<CH_LENGHT;i++)
	{
		ch[i]=0;
	}

	/*float to string on shot value :print only one value*/
	ret=float_toString_Third(value1,value2,value3,ch);
	if (ret !=0)
	{
		return 1;
	}

	uint16_t len=strlen(ch);

	/*send buffer to USB CDC class */
	ret=CDC_Transmit_FS(ch,len);



	return ret;

}



/**
  * @brief  Serial Print to Oscilo
  * @retval type: 0 print stored value
  *             : 1 store value and not print, wait for other value to be sent
  */
uint8_t SerialPrint_Value(float value, uint8_t type)
{
#define CH_LENGHT          100U
	uint8_t ret;
	static char ch[CH_LENGHT];

	/*Clear ch*/
	for (uint8_t i=0;i<CH_LENGHT;i++)
	{
		ch[i]=0;
	}

	/*float to string on shot value :print only one value*/
	ret=float_toString(value,ch,type);
	if (ret !=0)
	{
		return 1;
	}

	uint16_t len=strlen(ch);

	/*send buffer to USB CDC class */
	ret=CDC_Transmit_FS(ch,len);

	if (type==0)
	{
		ch[0]='\r';
        ch[1]='\n';
	  ret=CDC_Transmit_FS(ch,2);
	}


	return ret;

}


uint8_t float_toString_Third(float value,float value2 ,float value3,char *ch)
{
	char *tmpSign = (value < 0) ? "-" : "";
	float tmpVal = (value < 0) ? -value : value;

	int tmpInt1 = tmpVal;                  // Get the integer
	float tmpFrac = tmpVal - tmpInt1;      // Get fraction
	int tmpInt2 = trunc(tmpFrac * 10000);  // Turn into integer


	/*value2*/
	char *tmpSign2 = (value2 < 0) ? "-" : "";
	float tmpVal2 = (value2 < 0) ? -value2 : value2;

	int tmpInt1_2 = tmpVal2;                  // Get the integer
	float tmpFrac2 = tmpVal2 - tmpInt1_2;      // Get fraction
	int tmpInt2_2 = trunc(tmpFrac2 * 10000);  // Turn into integer

    /*value 3*/
	char *tmpSign3 = (value3 < 0) ? "-" : "";
	float tmpVal3 = (value3 < 0) ? -value3 : value3;

	int tmpInt1_3 = tmpVal3;                  // Get the integer
	float tmpFrac3 = tmpVal3 - tmpInt1_3;      // Get fraction
	int tmpInt2_3 = trunc(tmpFrac3 * 10000);  // Turn into integer




	if ((tmpInt2<10000U) && (tmpInt2_2<10000U)&& (tmpInt2_3<10000U))
    {

	 sprintf (ch, "%s%d.%04d,%s%d.%04d,%s%d.%04d\r\n", tmpSign, tmpInt1, tmpInt2, tmpSign2, tmpInt1_2, tmpInt2_2,
			  			                               tmpSign3, tmpInt1_3, tmpInt2_3 );
	return 0;
    }
	else
	{
		return 1; /*so big value to convert*/
	}
}




uint8_t float_toString_dual(float value,float value2 ,char *ch)
{
	char *tmpSign = (value < 0) ? "-" : "";
	float tmpVal = (value < 0) ? -value : value;

	int tmpInt1 = tmpVal;                  // Get the integer
	float tmpFrac = tmpVal - tmpInt1;      // Get fraction
	int tmpInt2 = trunc(tmpFrac * 10000);  // Turn into integer


	/*value2*/
	char *tmpSign2 = (value2 < 0) ? "-" : "";
	float tmpVal2 = (value2 < 0) ? -value2 : value2;

	int tmpInt1_2 = tmpVal2;                  // Get the integer
	float tmpFrac2 = tmpVal2 - tmpInt1_2;      // Get fraction
	int tmpInt2_2 = trunc(tmpFrac2 * 10000);  // Turn into integer


	if ((tmpInt2<10000U) && (tmpInt1<10000U))
    {

	 sprintf (ch, "%s%d.%04d,%s%d.%04d\r\n", tmpSign, tmpInt1, tmpInt2,tmpSign2, tmpInt1_2, tmpInt2_2);
	return 0;
    }
	else
	{
		return 1; /*so big value to convert*/
	}
}



uint8_t float_toString(float value, char *ch, uint8_t type)
{
	char *tmpSign = (value < 0) ? "-" : "";
	float tmpVal = (value < 0) ? -value : value;

	int tmpInt1 = tmpVal;                  // Get the integer
	float tmpFrac = tmpVal - tmpInt1;      // Get fraction
	int tmpInt2 = trunc(tmpFrac * 10000);  // Turn into integer

	if ((tmpInt2<10000U) && (tmpInt1<10000U))
    {

	if (type==0) /*one shot value*/
	{
	 sprintf (ch, "%s%d.%04d\r\n", tmpSign, tmpInt1, tmpInt2);
	}
	else
	{
		 sprintf (ch, "%s%d.%04d,", tmpSign, tmpInt1, tmpInt2);
	}
	return 0;
    }
	else
	{
		return 1; /*so big value to convert*/
	}
}


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/


