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


FIRFiltre lpfAcc;

void vmainTask(void const * argument);
uint8_t float_toString(float value, char *ch, uint8_t type);
uint8_t float_toString_dual(float value,float value2 ,char *ch);
uint8_t SerialPrint_Value(float value, uint8_t type);
uint8_t SerialPrint_DualValue(float value1,float value2);

#define WAIT_NEXT_VALUE            1U
#define PRINT_VALUE                0U
int16_t  Acc[3];
/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void vmainTask(void const * argument)
{
    float pi =3.14;
    float value;
    uint16_t freq=150;
    uint16_t Am=1000;
    uint16_t shift =0;
	/*init */




	FIRFiltre_Init(&lpfAcc);





	while(1)
	{

      osDelay(10U); /*100hz*/

      (void)BSP_ACCELERO_GetXYZ(Acc);

      FIRFiltre_Update(&lpfAcc, Acc[1]);

      SerialPrint_DualValue(Acc[1],lpfAcc.out);


#if 0
      for (uint16_t i=0; i< 10000; i++)
      {
    	  value= Am*sin (2*pi*freq*i)+shift;


    	  SerialPrint_Value(value,WAIT_NEXT_VALUE);
    	  freq=400;
    	  value= Am/2*sin (2*pi*freq*i)+shift;
    	  SerialPrint_Value(value,WAIT_NEXT_VALUE);

    	  freq=1200;
    	  value= 2*Am*sin (2*pi*freq*i)+shift;

    	  SerialPrint_Value(value,PRINT_VALUE);

      }

#endif


	}



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

