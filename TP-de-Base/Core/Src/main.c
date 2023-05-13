
#include <stdint.h>
#include <stdio.h>
#include "main.h"
#include "cmsis_os.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"
#include "bmp280.h"
#include "lib_lcd.h"
#include "sht31.h"


void SystemClock_Config(void);




BMP280_HandleTypedef bmp280;




/* USER CODE END Includes */


/* USER CODE END Includes */





int main(void)
{

  static rgb_lcd lcdData;
  float temperature=0,pressure=0,humidity=0;
  float hum_value;
  char Data_temp[10];
  char Data_pres[10];
  char Data[20];
  uint8_t buf[15];
  int size;

  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  lcd_init(&hi2c1, &lcdData); // initialise le lcd
  lcd_position(&hi2c1,0,0);
  reglagecouleur(0,0,255);
  bmp280_init_default_params(&bmp280.params);
  bmp280.addr = BMP280_I2C_ADDRESS_1;
  bmp280.i2c = &hi2c1;
  bmp280_init(&bmp280, &bmp280.params);


	while(1){
    while (!bmp280_read_float(&bmp280, &temperature, &pressure)) {
			size = sprintf((char *)Data,"Temperature/pressure reading failed\n\r");
			 HAL_UART_Transmit(&huart2, Data, size, 1000);
			 for(int i=0;i<100000;i++);
			 }

    humidity = humidity_read_value (&hi2c1,&huart2);
	for(int i=0;i<100000;i++);

	sprintf(Data_temp,"%.1f C",temperature);
	sprintf(Data_pres,"%.1f Kpa",pressure/1000);
	lcd_position(&hi2c1,0,0);
	lcd_print(&hi2c1,"Pres: ");
    lcd_position(&hi2c1,7,0);
	lcd_print(&hi2c1,Data_pres);
	lcd_position(&hi2c1,0,1);
	lcd_print(&hi2c1,"Temp: ");
	lcd_position(&hi2c1,7,1);
	lcd_print(&hi2c1,Data_temp);

	 for(int i=0;i<100000;i++);

	}
}





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

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM3 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM3) {
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
