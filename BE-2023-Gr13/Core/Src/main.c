/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bmp280.h"
#include "sht31.h"
#include "ST7735.h"
#include "task.h"
#include "queue.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
BMP280_HandleTypedef bmp280;
float temperature=0,pressure=0,humidity=0;
uint32_t ReadTaskProfiler=0, PrintTaskProfiler=0;
QueueHandle_t xPrintQueue_temp;
QueueHandle_t xPrintQueue_press;
QueueHandle_t xPrintQueue_hum;
TaskHandle_t xTempReadHandle = NULL;
TaskHandle_t xTempPrintHandle = NULL;
float hum_value;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */
void vPrintTemperatureValueTask(void * pvParameters);
void vReadTemperature_pressure_humidity_ValuesTask(void * pvParameters);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
    ST7735_Init();
    bmp280_init_default_params(&bmp280.params);
    bmp280.addr = BMP280_I2C_ADDRESS_1;
    bmp280.i2c = &hi2c1;
    bmp280_init(&bmp280, &bmp280.params);
    xPrintQueue_temp = xQueueCreate(50, sizeof(float));
    xPrintQueue_press = xQueueCreate(50, sizeof(float));
    xPrintQueue_hum = xQueueCreate(50, sizeof(float));

    xTaskCreate(vReadTemperature_pressure_humidity_ValuesTask,
      		      "Reading temperature",
      		  	  100,
      			  NULL,
      			  1,
      			  &xTempReadHandle);

    xTaskCreate(vPrintTemperatureValueTask,
       		      "Printing temperature",
       		  	  300,
       			  NULL,
       			  1,
       			  &xTempPrintHandle);
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();

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

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void vReadTemperature_pressure_humidity_ValuesTask(void * pvParameters){

	uint8_t Data[40];
	uint16_t size;
	//float temperature=0,pressure=0,humidity=0;
	while(1){
    while (!bmp280_read_float(&bmp280, &temperature, &pressure)) {
			size = sprintf((char *)Data,"Temperature/pressure reading failed\n\r");
			 HAL_UART_Transmit(&huart2, Data, size, 1000);
			 for(int i=0;i<100000;i++);
			 }
    xQueueSend(xPrintQueue_press, &pressure, 0);
	xQueueSend(xPrintQueue_temp, &temperature, 0);
    humidity = humidity_read_value (&hi2c1,&huart2);
    xQueueSend(xPrintQueue_hum, &humidity, 0);
	ReadTaskProfiler++;
	for(int i=0;i<1400000;i++);
	}
}


void vPrintTemperatureValueTask(void * pvParameters){
	//float temperature=0,pressure=0,humidity=0;
	 char Data_temp[30];
	 char Data_pres[30];
	 char Data_hum[30];
	 uint8_t buf[15];
	 int size;
	while(1){
		if ((xQueueReceive(xPrintQueue_temp,&temperature,portMAX_DELAY) == pdPASS)&&(xQueueReceive(xPrintQueue_press,&pressure,portMAX_DELAY))&&(xQueueReceive(xPrintQueue_hum,&humidity,portMAX_DELAY) == pdPASS)){
			sprintf(Data_temp,"Temp:%.1f C",temperature);
			sprintf(Data_pres,"Press:%.1f Kpa",pressure/1000);
			sprintf(Data_hum,"Humidity: %.1f %",humidity);
			ST7735_DrawString(1,4,Data_temp,ORANGE);
			ST7735_DrawString(1,6,Data_pres,ORANGE);
			ST7735_DrawString(1,8,Data_hum,ORANGE);
			sprintf(Data_temp,"Temp:%.1f C \n\r",temperature);
			sprintf(Data_pres,"Press:%.1f Kpa \n\r",pressure/1000);
			sprintf(Data_hum,"Humidity: %.1f % \n\r",humidity);
			HAL_UART_Transmit(&huart3,(char*)Data_temp, sizeof(Data_temp),100);
			for(int i=0;i<10000;i++);
			HAL_UART_Transmit(&huart3,(char*)Data_pres, sizeof(Data_pres),100);
			for(int i=0;i<10000;i++);
			HAL_UART_Transmit(&huart3,(char*)Data_hum, sizeof(Data_hum),100);
			for(int i=0;i<10000;i++);
		}
		else {
			size= sprintf( (char*)buf, "error2 \n\r");
			HAL_UART_Transmit(&huart2, buf, size, 1000);

		}
		PrintTaskProfiler++;
		}

}

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM2 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM2) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
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
