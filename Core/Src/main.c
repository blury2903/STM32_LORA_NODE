/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "pzem004t.h"
#include "DHT.h"
#include "Lora.h"
#include "i2c-lcd.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
DHT_DataTypedef DHT11_Data;
pzem PZEM;
LoRa myLoRa;
_values printval;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
float temp_value = 0.0;
float rh_value = 0.0;
float volt = 0.0;
float curr = 0.0;
float power = 0.0;
float energy = 0.0;
float freq = 0.0;
float pf = 0.0;
char value_array[100] = {0};
char i2c_array[100] = {0};
uint8_t start_Lora = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */
/*void SPI_Voltage_Transmit(LoRa* _LoRa, pzem *pzem);
void SPI_Current_Transmit(LoRa* _LoRa, pzem *pzem);
void SPI_Power_Transmit(LoRa* _LoRa, pzem *pzem);
void SPI_Energy_Transmit(LoRa* _LoRa, pzem *pzem);
void SPI_Frequency_Transmit(LoRa* _LoRa, pzem *pzem);
void SPI_pF_Transmit(LoRa* _LoRa, pzem *pzem);*/
void spi_Transmit(void);
void i2c_LCD_1(void);
void i2c_LCD_2(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void spi_Transmit(void)
{
	DHT_GetData(&DHT11_Data);
	temp_value = DHT11_Data.Temperature;
	rh_value = DHT11_Data.Humidity;

	updateValues(&PZEM);
	volt = PZEM.values.voltage;
	curr = PZEM.values.current;
	power = PZEM.values.power;
	energy = PZEM.values.energy;
	freq = PZEM.values.frequency;
	pf = PZEM.values.pf;

	sprintf(value_array, "1,%.1f,%.1f,%.1f,%.3f,%.1f,%.3f,%.1f,%.2f,\r\n", temp_value, rh_value, volt, curr, power, energy, freq, pf);
//	sprintf(value_array, "2,%.1f,%.1f,%.1f,%.3f,%.1f,%.3f,%.1f,%.2f,\r\n", temp_value, rh_value, volt, curr, power, energy, freq, pf);
	LoRa_transmit(&myLoRa, (uint8_t*)value_array, strlen(value_array), 3000); //Default: 3000

	//HAL_UART_Transmit(&huart2, (uint8_t *)value_array, strlen(value_array), 1000);
}

void i2c_LCD_1(void)
{
	updateValues(&PZEM);
	volt = PZEM.values.voltage;
	curr = PZEM.values.current;
	power = PZEM.values.power;
	energy = PZEM.values.energy;
	freq = PZEM.values.frequency;
	pf = PZEM.values.pf;

	lcd_clear_display();
	HAL_Delay(50);

	sprintf(i2c_array, "%.1fV", volt);
	lcd_goto_XY(1, 0);
	lcd_send_string(i2c_array);

	sprintf(i2c_array, "%.3fA", curr);
	lcd_goto_XY(1, 7);
	lcd_send_string(i2c_array);

	sprintf(i2c_array, "%.1fW", power);
	lcd_goto_XY(2, 0);
	lcd_send_string(i2c_array);

	sprintf(i2c_array, "%.3fWh", energy);
	lcd_goto_XY(2, 7);
	lcd_send_string(i2c_array);

	HAL_Delay(2000);
}

void i2c_LCD_2(void)
{
	lcd_clear_display();
	HAL_Delay(50);

	DHT_GetData(&DHT11_Data);
	temp_value = DHT11_Data.Temperature;
	rh_value = DHT11_Data.Humidity;

	updateValues(&PZEM);
	freq = PZEM.values.frequency;
	pf = PZEM.values.pf;

	sprintf(i2c_array, "%.1fHz", freq);
	lcd_goto_XY(1, 0);
	lcd_send_string(i2c_array);

	sprintf(i2c_array, "%.2f", pf);
	lcd_goto_XY(1, 8);
	lcd_send_string(i2c_array);

	sprintf(i2c_array, "%.1fC", temp_value);
	lcd_goto_XY(2, 0);
	lcd_send_string(i2c_array);

	sprintf(i2c_array, "%.1f%%", rh_value);
	lcd_goto_XY(2, 8);
	lcd_send_string(i2c_array);

	HAL_Delay(2000);
}

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
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

	//PZEM CONFIG
	init_pzem(&PZEM, &huart1, 0x01);
	HAL_UART_Receive_IT(PZEM.huart, &PZEM.byteRx[0], 1);

	//LORA CONFIG
	myLoRa = newLoRa();
	myLoRa.CS_port = NSS_GPIO_Port;
	myLoRa.CS_pin = NSS_Pin;
	myLoRa.reset_port = RST_GPIO_Port;
	myLoRa.reset_pin = RST_Pin;
	myLoRa.DIO0_port = DIO0_GPIO_Port;
	myLoRa.DIO0_pin = DIO0_Pin;
	myLoRa.hSPIx = &hspi1;

	myLoRa.frequency = 433;             // default = 433 MHz
	myLoRa.spredingFactor = SF_7;            // default = SF_7
	myLoRa.bandWidth = BW_125KHz;       // default = BW_125KHz
	myLoRa.crcRate = CR_4_5;          // default = CR_4_5
	myLoRa.power = POWER_20db;      // default = 20db
	myLoRa.overCurrentProtection = 130;             // default = 100 mA
	myLoRa.preamble = 10;              // default = 8

	LoRa_reset(&myLoRa);
	if (LoRa_init(&myLoRa) == LORA_OK)
	{
		start_Lora = 1;
	}

	//LCD CONFIG
	lcd_init();
	lcd_goto_XY(1, 1);
	lcd_send_string("INTIALIZING...");
	HAL_Delay(1000);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

//	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 1);
//	  HAL_Delay(2000);
//	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 0);
//	  HAL_Delay(2000);

	  if(updateValues(&PZEM))
	  {
		  i2c_LCD_2();
		  HAL_Delay(1000);
		  i2c_LCD_1();
		  HAL_Delay(1000);
//		  voltage = getVol(&PZEM);
//		  sprintf(voltageArray, "%1.f", voltage);
//		  HAL_UART_Transmit(&huart1, (uint8_t *)voltageArray, 5, 1000);
		  if(start_Lora == 1)
		  {
			  /*SPI_Voltage_Transmit(&myLoRa, &PZEM);
			  SPI_Current_Transmit(&myLoRa, &PZEM);
			  SPI_Power_Transmit(&myLoRa, &PZEM);
			  SPI_Energy_Transmit(&myLoRa, &PZEM);
			  SPI_Frequency_Transmit(&myLoRa, &PZEM);
			  SPI_pF_Transmit(&myLoRa, &PZEM);*/

			  spi_Transmit();
			  //HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
			  HAL_Delay(60000);
//			  HAL_Delay(65000);
		  }
		  //HAL_UART_Transmit(&huart6, (uint8_t *)"\n\nThe measurement result\n\r\n", 25, HAL_MAX_DELAY);
		  /*
		  printVoltage(&huart2, &PZEM);
		  printCurrent(&huart2, &PZEM);
		  printPower(&huart2, &PZEM);
		  printEnergy(&huart2, &PZEM);
		  printFrequency(&huart2, &PZEM);
		  printpF(&huart2, &PZEM);
		  */
		  //HAL_SuspendTick();

		  //HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

		  //HAL_ResumeTick();
	  }
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, NSS_Pin|RST_Pin|Relay_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : NSS_Pin RST_Pin Relay_Pin */
  GPIO_InitStruct.Pin = NSS_Pin|RST_Pin|Relay_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : DIO0_Pin */
  GPIO_InitStruct.Pin = DIO0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DIO0_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(PZEM.rxbufferIndex >30)
	{
		 PZEM.rxbufferIndex=0;
	}

	if (huart->Instance == UART_PZEM) //queried which uart is used
	{
		HAL_UART_Receive_IT(PZEM.huart,&PZEM.byteRx[0], 1);
		PZEM.rxBuffer[PZEM.rxbufferIndex] =PZEM.byteRx[0];
		PZEM.rxbufferIndex++;
	}
}

/* USER CODE END 4 */

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
