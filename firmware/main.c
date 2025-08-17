/* USER CODE BEGIN Header */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "lcd4.h"
#include "stm32f4xx_hal.h"
 
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */
 
/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum {
    NORMAL_MODE,
    SET_HOUR,
    SET_MINUTE,
    SET_AMPM
} SystemMode_t;
/* USER CODE END PTD */
 
/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
 
#define ROW1_PORT GPIOA
#define ROW1_PIN  GPIO_PIN_6
#define ROW2_PORT GPIOA
#define ROW2_PIN  GPIO_PIN_7
#define ROW3_PORT GPIOB
#define ROW3_PIN  GPIO_PIN_6
#define ROW4_PORT GPIOC
#define ROW4_PIN  GPIO_PIN_7
 
#define COL1_PORT GPIOA
#define COL1_PIN  GPIO_PIN_9
#define COL2_PORT GPIOA
#define COL2_PIN  GPIO_PIN_8
#define COL3_PORT GPIOB
#define COL3_PIN  GPIO_PIN_10
#define COL4_PORT GPIOB
#define COL4_PIN  GPIO_PIN_4
/* USER CODE END PD */
 
/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */
 
/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
 
TIM_HandleTypeDef htim2;
 
/* USER CODE BEGIN PV */
int hour = 11;
int minute = 59;
int second = 55;
bool ampm = 0; // 1 = PM
 
SystemMode_t currentMode = NORMAL_MODE;
char keypadInput[3] = {0};
uint8_t inputIndex = 0;
bool inputConfirmed = false;
bool errorFlag = false;
bool showPrompt = true;
bool isFahrenheit = false;   // Toggle with button
int roundedTemp = 0;
 
char key_map[4][4] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};
 
void setRowLow(int row) {
    HAL_GPIO_WritePin(ROW1_PORT, ROW1_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ROW2_PORT, ROW2_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ROW3_PORT, ROW3_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ROW4_PORT, ROW4_PIN, GPIO_PIN_SET);
 
    switch (row) {
        case 0: HAL_GPIO_WritePin(ROW1_PORT, ROW1_PIN, GPIO_PIN_RESET); break;
        case 1: HAL_GPIO_WritePin(ROW2_PORT, ROW2_PIN, GPIO_PIN_RESET); break;
        case 2: HAL_GPIO_WritePin(ROW3_PORT, ROW3_PIN, GPIO_PIN_RESET); break;
        case 3: HAL_GPIO_WritePin(ROW4_PORT, ROW4_PIN, GPIO_PIN_RESET); break;
    }
}
 
uint8_t readColumns(void) {
    uint8_t val = 0;
    val |= (HAL_GPIO_ReadPin(COL1_PORT, COL1_PIN) == GPIO_PIN_RESET) ? 0x01 : 0;
    val |= (HAL_GPIO_ReadPin(COL2_PORT, COL2_PIN) == GPIO_PIN_RESET) ? 0x02 : 0;
    val |= (HAL_GPIO_ReadPin(COL3_PORT, COL3_PIN) == GPIO_PIN_RESET) ? 0x04 : 0;
    val |= (HAL_GPIO_ReadPin(COL4_PORT, COL4_PIN) == GPIO_PIN_RESET) ? 0x08 : 0;
    return val;
}
 
char getKeyPressed(void) {
    static uint32_t lastKeyTime = 0;
    uint32_t now = HAL_GetTick();
    if (now - lastKeyTime < 50) return '\0';
 
    for (int r = 0; r < 4; r++) {
        setRowLow(r);
        HAL_Delay(1);
        uint8_t col = readColumns();
        for (int c = 0; c < 4; c++) {
            if (col & (1 << c)) {
                lastKeyTime = now;
                return key_map[r][c];
            }
        }
    }
    return '\0';
}
 
 
 
/* USER CODE END PV */
 
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */
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
  MX_ADC1_Init();
  MX_TIM2_Init();
 
  /* USER CODE BEGIN 2 */
   // start the timer interrupt
   HAL_TIM_Base_Start_IT(&htim2);
 
   // configure and initialize the LCD
   lcdSetup(GPIOA, GPIO_PIN_0,   // RS, E, DB4 on PA0, PA1, PA4
            GPIOA, GPIO_PIN_1,
            GPIOA, GPIO_PIN_4,
            GPIOB, GPIO_PIN_0,   // DB5 on PB0
            GPIOC, GPIO_PIN_1,   // DB6 on PC1
            GPIOC, GPIO_PIN_0);  // DB7 on PC0
   lcdInit();
   // your display buffer and timing state
   char     buffer[20];
   uint32_t lastDisplayTime = 0;
   /* USER CODE END 2 */
 
   /* Infinite loop */
   /* USER CODE BEGIN WHILE */
   while (1)
   {
	   char key = getKeyPressed();
 
	           // Handle Set Mode Entry
	           if (key == '*' && currentMode == NORMAL_MODE) {
	               currentMode = SET_HOUR;
	               inputIndex = 0;
	               memset(keypadInput, 0, sizeof(keypadInput));
	               showPrompt = true;
	           }
	           // Handle input during set modes
	           else if (currentMode != NORMAL_MODE && key != '\0') {
	               if (key == '#') {
	                   inputConfirmed = true;
	               } else if ((currentMode == SET_HOUR || currentMode == SET_MINUTE) &&
	                          key >= '0' && key <= '9' && inputIndex < 2) {
	                   keypadInput[inputIndex++] = key;
	                   keypadInput[inputIndex] = '\0';  // Ensure null-terminated string
	                   showPrompt = true;
	               } else if (currentMode == SET_AMPM && (key == '1' || key == '2')) {
	                   keypadInput[0] = key;
	                   keypadInput[1] = '\0';  // Also null-terminate here
	                   inputConfirmed = true;
	               }
	           }
 
	           static uint8_t lastButtonState = 1;
	           uint8_t currentButtonState = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);
 
	           // Toggle unit on falling edge (button press)
	           if (lastButtonState == 1 && currentButtonState == 0) {
	               isFahrenheit = !isFahrenheit;
	               HAL_Delay(200);
	           }
	           lastButtonState = currentButtonState;
 
	           // NORMAL MODE: Show clock on LCD
	           if (currentMode == NORMAL_MODE && HAL_GetTick() - lastDisplayTime >= 1000) {
	                       lastDisplayTime = HAL_GetTick();
	                       // Read ADC and convert to temperature
	                       HAL_ADC_Start(&hadc1);
	                       HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
	                       uint32_t raw = HAL_ADC_GetValue(&hadc1);
 
	                       float voltage = (raw * 3.3) / 4095.0f;  // Convert to volts
	                       float tempC = voltage * 100.0f;         // LM35: 10mV per °C → 0.01V = 1°C
 
	                       roundedTemp = isFahrenheit ? (int)(tempC * 9.0f / 5.0f + 32.5f) : (int)(tempC + 0.5f);
	                       lcdCommand(lcdClear);
	                       lcdSetCursor(0, 0);  // First row, first column
 
	                       // Format: hh:mm:ssXM 72F°
	                       snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d%s %2d%c%c",
	                                hour, minute, second, ampm ? "PM" : "AM",
	                                roundedTemp, isFahrenheit ? 'F' : 'C', 0xDF); // ° symbol
 
	                       lcdString(buffer);
	           }
 
	           // SET MODE HANDLING
	           if (currentMode != NORMAL_MODE) {
	               if (errorFlag) {
	                   lcdCommand(lcdClear);
	                   lcdString("ERROR");
	                   HAL_Delay(2000);
	                   errorFlag = false;
	                   inputIndex = 0;
	                   memset(keypadInput, 0, sizeof(keypadInput));
	                   showPrompt = true;
	                   continue;
	               }
 
	               if (showPrompt) {
	                   lcdCommand(lcdClear);
	                   switch (currentMode) {
	                       case SET_HOUR:
	                           lcdString("HOUR:");
	                           lcdSetCursor(6, 1);
	                           lcdString(keypadInput);
	                           break;
 
	                       case SET_MINUTE:
	                           lcdString("MIN:");
	                           lcdSetCursor(5, 1);
	                           lcdString(keypadInput);
	                           break;
 
	                       case SET_AMPM:
	                           lcdString("AM(1)/PM(2):");
	                           lcdSetCursor(12, 1);
 
	                           lcdString(keypadInput);
	                           break;
	                       default:
	                           break;
	                   }
 
	                   showPrompt = false;
	               }
 
	               if (inputConfirmed) {
	                   switch (currentMode) {
	                       case SET_HOUR: {
	                           int val = atoi(keypadInput);
	                           if (val < 1 || val > 13) {
	                               errorFlag = true;
	                           } else {
	                               hour = val;
	                               currentMode = SET_MINUTE;
	                           }
	                           break;
	                       }
	                       case SET_MINUTE: {
	                           int val = atoi(keypadInput);
	                           if (val < 0 || val > 59) {
	                               errorFlag = true;
	                           } else {
	                               minute = val;
	                               second = 0;
	                               currentMode = SET_AMPM;
	                           }
	                           break;
	                       }
	                       case SET_AMPM:
	                           if (keypadInput[0] == '1') {
	                               ampm = 0; // AM
	                           } else if (keypadInput[0] == '2') {
	                               ampm = 1; // PM
	                           } else {
	                               errorFlag = true;
	                           }
	                           if (!errorFlag) {
	                               currentMode = NORMAL_MODE;
	                           }
	                           break;
	                   }
	                   inputConfirmed = false;
	                   inputIndex = 0;
	                   memset(keypadInput, 0, sizeof(keypadInput));
	                   showPrompt = true;
	               } // your keypad-read + display-update code here
   }
   }
   /* USER CODE END WHILE */
 
   /* USER CODE BEGIN 3 */
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
 
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
 
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
 
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}
 
/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
void MX_ADC1_Init(void)
{
 
  /* USER CODE BEGIN ADC1_Init 0 */
  /* USER CODE END ADC1_Init 0 */
 
  ADC_ChannelConfTypeDef sConfig = {0};
 
  /* USER CODE BEGIN ADC1_Init 1 */
  /* USER CODE END ADC1_Init 1 */
 
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
 
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */
  /* USER CODE END ADC1_Init 2 */
 
}
 
/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
void MX_TIM2_Init(void)
{
 
  /* USER CODE BEGIN TIM2_Init 0 */
  /* USER CODE END TIM2_Init 0 */
 
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
 
  /* USER CODE BEGIN TIM2_Init 1 */
  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 15999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */
  /* USER CODE END TIM2_Init 2 */
 
}
 
/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */
 
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
 
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, DB7_Pin|DB6_Pin|ROW4_Pin, GPIO_PIN_RESET);
 
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, RS_Pin|E_Pin|DB4_Pin|ROW1_Pin
                          |ROW2_Pin, GPIO_PIN_RESET);
 
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, DB5_Pin|ROW3_Pin, GPIO_PIN_RESET);
 
  /*Configure GPIO pins : DB7_Pin DB6_Pin ROW4_Pin */
  GPIO_InitStruct.Pin = DB7_Pin|DB6_Pin|ROW4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
 
  /*Configure GPIO pins : RS_Pin E_Pin DB4_Pin ROW1_Pin
                           ROW2_Pin */
  GPIO_InitStruct.Pin = RS_Pin|E_Pin|DB4_Pin|ROW1_Pin
                          |ROW2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
 
  /*Configure GPIO pins : DB5_Pin ROW3_Pin */
  GPIO_InitStruct.Pin = DB5_Pin|ROW3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
 
  /*Configure GPIO pins : COL3_Pin COL4_Pin */
  GPIO_InitStruct.Pin = COL3_Pin|COL4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
 
  /*Configure GPIO pins : COL2_Pin COL1_Pin */
  GPIO_InitStruct.Pin = COL2_Pin|COL1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
 
/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}
 
/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2 && currentMode == NORMAL_MODE) {
        second++;
 
        if (second >= 60) {
            second = 0;
            minute++;
 
            if (minute >= 60) {
                minute = 0;
                hour++;
 
                if (hour == 12) {
                    ampm = !ampm;
                } else if (hour == 13) {
                    hour = 1;
                }
            }
        }
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
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
