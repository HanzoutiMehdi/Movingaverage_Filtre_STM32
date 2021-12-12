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

uint8_t Task_Init(void);
void vmainTask(void const * argument);

/*error code*/
#define ICARE_PLUS_TASK_CREATION_FAIL    1U

/**
  * @brief  The application entry point.
  * @retval int
  */
uint8_t Task_Init(void)
{
	BaseType_t xReturned;
	uint8_t ret;

	/* Creat Startup Task */
	xReturned = xTaskCreate(vmainTask,"Main_Task",400U ,NULL,(tskIDLE_PRIORITY+1U), NULL );

	if (xReturned!=pdPASS )
	{
		GlobalError(ICARE_PLUS_TASK_CREATION_FAIL);
	}




	ret=BSP_ACCELERO_Init();
	if (ret !=0)
	{
		while(1);
	}



  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}


void GlobalError(uint16_t err)
{

   switch (err)
   {

   case ICARE_PLUS_TASK_CREATION_FAIL:
   {
	   while(1);
   }

   default:
   {
	   while(1);
   }


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
