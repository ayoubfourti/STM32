/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2025 STMicroelectronics.
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
#include "adc.h"
#include "crc.h"
#include "dma.h"
#include "i2c.h"
#include "rtc.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ai_aquarium.h"
#include "ai_monitor.h"
#include "sensors.h"
#include "actuators.h"
#include "events.h"
#include "liquidcrystal_i2c.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
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
volatile uint8_t IA_interrupt = 0;
volatile uint8_t diagnostique = 0;
volatile uint8_t urgence = 0;
volatile uint8_t en_test = 0;
volatile uint8_t adc_fin=0;

char capteurs_err[MAX_CAPTEURS][16];
int nb_err = 0;
float water_adc_ema = 0.0f;
float humidity_ema = -1.0f;

uint16_t adcData[4];

// Humidity
uint32_t humidity_ADC;
int humidity_PERCENT;

// Water
uint32_t water_ADC;
const char* water_LEVEL;

// LDR
uint32_t LDR_ADC;
const char* LDR = "Jour";


// Temperature
uint32_t temp_ADC;
float temp;

char line1[20];
char line2[20];

char timeData[10];
char dateData[11];

static uint8_t pump_on = 0;
static uint8_t fan_on  = 0;

static uint32_t alertUntil = 0;
static uint8_t alertActive = 0;


// seuils température (hystérésis)
#define TEMP_ON   25.0f
#define TEMP_OFF  24.0f
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void get_time_date(char *time, char *date); //RTC

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
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_ADC2_Init();
  MX_TIM7_Init();
  MX_TIM3_Init();
  MX_TIM2_Init();
  MX_RTC_Init();
  MX_CRC_Init();

   /* USER CODE BEGIN 2 */
  	  	 AI_Init();
  	  	 AI_Monitor_Init();
         HD44780_Init(2);
         HD44780_Clear();
         HD44780_Backlight();
         HD44780_SetCursor(0, 0);
         HD44780_PrintStr("Demarrage System");
         HAL_Delay(2000);
         HD44780_Clear();
         HD44780_PrintStr("Surveillance");
         HD44780_SetCursor(0, 1);
         HD44780_PrintStr("active..");
         HAL_Delay(2000);
         HD44780_Clear();


         HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
         HAL_Delay(1000);
         HAL_ADC_Start_DMA(&hadc2, (uint32_t*) adcData, 4);
         HAL_TIM_Base_Start(&htim3);
         HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);

         uint32_t water_adc_filt = 0;
         uint32_t ldr_adc_filt   = 0;
         uint32_t temp_adc_filt  = 0;


         uint32_t lastLcdUpdate   = 0;
         uint32_t pwm_last_update = 0;
         int32_t  pwm_dc          = 0;
         int32_t  pwm_speed       = 50;

         static uint32_t lastAiUpdate = 0;
         uint32_t lastAlertShow  = 0;

         static AiStatus ai_status;
         ai_status.score = 0.0f;
         memset(&ai_status, 0, sizeof(ai_status));

         while (1)
         {

         	            RTC_TimeTypeDef gTime;
         	            RTC_DateTypeDef gDate;
         	            HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);
         	            HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BIN); // IMPORTANT !!
         	            uint8_t hour = gTime.Hours;

         	            get_time_date(timeData, dateData); // remplit timeData et dateData


         	            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8 |  GPIO_PIN_10, GPIO_PIN_RESET);

         	           HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
         	                if (hour >= 6 && hour < 12)
         	                {
         	                    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET); // Matin
         	                }
         	                else if (hour >= 12 && hour < 18)
         	                {
         	                    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET); // Midi
         	                }
         	                else
         	                {
         	                    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET); // Soir
         	                }



             /* --- Filtrage des ADC (eau, LDR, temperature) --- */
             const float alpha = 0.2f;  // plus petit = plus stable

             if (water_adc_filt == 0 && ldr_adc_filt == 0 && temp_adc_filt == 0)
             {
                 // première initialisation
                 water_adc_filt = adcData[1];
                 ldr_adc_filt   = adcData[2];
                 temp_adc_filt  = adcData[3];
             }
             else
             {
                 water_adc_filt = (uint32_t)(alpha * adcData[1] + (1.0f - alpha) * water_adc_filt);
                 ldr_adc_filt   = (uint32_t)(alpha * adcData[2] + (1.0f - alpha) * ldr_adc_filt);
                 temp_adc_filt  = (uint32_t)(alpha * adcData[3] + (1.0f - alpha) * temp_adc_filt);
             }




             /* --- Animation PWM non bloquante sur TIM2_CH1 --- */
             // ---------- PWM FADE LED SELON LDR ----------
             if (HAL_GetTick() - pwm_last_update >= 3)   // update toutes les 3 ms
             {
                 pwm_last_update = HAL_GetTick();

                 if (strcmp(LDR, "Nuit") == 0)
                 {
                     // Monter progressivement (fade in)
                     if (pwm_dc < 65535)
                         pwm_dc += pwm_speed;
                     else
                         pwm_dc = 65535;
                 }
                 else
                 {
                     // Descendre progressivement (fade out)
                     if (pwm_dc > 0)
                         pwm_dc -= pwm_speed;
                     else
                         pwm_dc = 0;
                 }

                 TIM2->CCR1 = pwm_dc;   // appliquer duty cycle
             }






             /* --- Affichage LCD périodique des capteurs --- */
             if (en_test == 0 && adc_fin == 1 && (HAL_GetTick() - lastLcdUpdate) >= 500)
             {
                 lastLcdUpdate = HAL_GetTick();
                 adc_fin = 0;


                 // Humidité (stable, on prend directement adcData[0])
                 humidity_ADC    = adcData[0];
                 humidity_PERCENT = Humidity_ADC_to_Percent(humidity_ADC);

                 // Niveau d'eau (on utilise la valeur filtrée)
                 water_ADC = water_adc_filt;
                 if (water_adc_ema == 0.0f)  water_adc_ema = (float)water_ADC;
                 water_adc_ema = WATER_EMA_ALPHA * water_ADC + (1.0f - WATER_EMA_ALPHA) * water_adc_ema;
                 uint32_t smooth_adc = (uint32_t)water_adc_ema;
                 water_LEVEL = Water_Level_Label(smooth_adc);


                 // LDR (filtré)
                 LDR_ADC = ldr_adc_filt;
                 if (LDR_ADC < 3400) { LDR = "Jour"; }
                 else                { LDR = "Nuit"; }

                 // Temperature (filtrée)
                 temp_ADC = temp_adc_filt;
                 temp     = Lire_Temperature_LM35(temp_ADC);


                 snprintf(line1, sizeof(line1), "T=%.1fC W=%.5s", temp, water_LEVEL);
                 snprintf(line2, sizeof(line2), "H=%d%% LDR=%.4s", humidity_PERCENT, LDR);



                 if (!alertActive) {
                     HD44780_Clear();
                     HD44780_SetCursor(0, 0);
                     HD44780_PrintStr(line1);
                     HD44780_SetCursor(0, 1);
                     HD44780_PrintStr(line2);
                 }
             }


                                /* PARTIE IA*/
             if (IA_interrupt)
             {
                 if (en_test == 0 && (HAL_GetTick() - lastAiUpdate) >= 1000) {
                        lastAiUpdate = HAL_GetTick();

                        // temp et water_LEVEL doivent déjà avoir une valeur (mise à jour par le bloc ADC)
                        ai_status = AI_Monitor_Update(temp, water_LEVEL);
                    }

                    // ====== ACTIONS (pompe/ventilo) ======

                    // ---- POMPE ----
                    if ((ai_status.type == ANOM_FUITE_EAU) || (ai_status.type == ANOM_NIVEAU_BAS)) {
                        pump_on = 1;
                    }
                    if (pump_on) {
                        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET); // pompe ON

                        // condition d'arrêt : niveau HIGH
                        if (strcmp(water_LEVEL, "HIGH") == 0) {
                            pump_on = 0;
                            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET); // pompe OFF
                        }
                    } else {
                        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET); // pompe OFF
                    }

                    // ---- VENTILO ----
                    if (ai_status.type == ANOM_SURCHAUFFE) {
                        fan_on = 1;
                    }
                    if (fan_on) {
                        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET); // ventilo ON

                        // condition d'arrêt : température redescendue
                        if (temp < TEMP_OFF) {
                            fan_on = 0;
                            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET); // ventilo OFF
                        }
                    } else {
                        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET); // ventilo OFF
                    }

                    // --- Afficher alerte IA séparée (ex: toutes les 3s, pendant 2s) ---
                    // lancer l'alerte (toutes les 3s)
                    if (ai_status.anom && !alertActive && (HAL_GetTick() - lastAlertShow) >= 3000) {
                        lastAlertShow = HAL_GetTick();
                        alertActive = 1;
                        alertUntil = HAL_GetTick() + 2000; // afficher 2s

                        HD44780_Clear();
                        HD44780_SetCursor(0, 0);
                        HD44780_PrintStr("!! ALERTE IA !!");
                        HD44780_SetCursor(0, 1);
                        HD44780_PrintStr(ai_status.msg);
                        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_SET); //buzzer
                        HAL_Delay(500);
                        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_RESET); //buzzer

                    }

                    // arrêter l'alerte et revenir à l'écran capteurs
                    if (alertActive && (HAL_GetTick() >= alertUntil)) {
                        alertActive = 0;
                        // ne rien faire ici : ton écran capteurs se ré-affichera au prochain refresh 500ms
                    }
             }



             /* --- Mode diagnostique --- */
             if (diagnostique && IA_interrupt==0)
             {
                 en_test = 1;
                 diagnostique = 0;
                 HD44780_Clear();
                 HD44780_SetCursor(0, 0);
                 HD44780_PrintStr("Diagnostique");
                 HAL_Delay(1000);

                 nb_err = 0;

                 if (!Diagnostic_Capteur(adcData[0], SENSOR_HUMIDITY))
                     strncpy(capteurs_err[nb_err++], Sensor_Name(SENSOR_HUMIDITY), 15);

                 if (!Diagnostic_Capteur(adcData[1], SENSOR_WATER))
                     strncpy(capteurs_err[nb_err++], Sensor_Name(SENSOR_WATER), 15);

                 if (!Diagnostic_Capteur(adcData[2], SENSOR_LDR))
                     strncpy(capteurs_err[nb_err++], Sensor_Name(SENSOR_LDR), 15);

                 if (!Diagnostic_Capteur(adcData[3], SENSOR_TEMP))
                     strncpy(capteurs_err[nb_err++], Sensor_Name(SENSOR_TEMP), 15);

                 if (!Diagnostic_Capteur(0, ACT_VENTILO))
                     strncpy(capteurs_err[nb_err++], Sensor_Name(ACT_VENTILO), 15);

                 if (!Diagnostic_Capteur(0, ACT_POMPE))
                     strncpy(capteurs_err[nb_err++], Sensor_Name(ACT_POMPE), 15);

                 for (int i = 0; i < nb_err; ++i)
                     capteurs_err[i][15] = '\0';

                 HD44780_Clear();
                 HD44780_SetCursor(0, 0);

                 if (nb_err == 0)
                 {
                     HD44780_PrintStr("Tout est bon");
                 }
                 else
                 {
                     for (int i = 0; i < nb_err; i++)
                     {
                         HD44780_PrintStr(capteurs_err[i]);
                         if (i < nb_err - 1)
                             HD44780_PrintStr(",");
                     }
                     HD44780_SetCursor(0, 1);
                     HD44780_PrintStr("A verifier");
                 }

                 HAL_Delay(3000);
                 HD44780_Clear();
                 HD44780_SetCursor(0, 0);
                 HD44780_PrintStr("Surveillance");
                 HD44780_SetCursor(0, 1);
                 HD44780_PrintStr("active..");

                 en_test = 0;
             }
             if (urgence && IA_interrupt==0)
             {
                 en_test = 1;
                 urgence = 0;



                 HD44780_Clear();
                 HD44780_SetCursor(0, 0);
                 HD44780_PrintStr("MODE URGENCE");
                 HD44780_SetCursor(0, 1);
                 HD44780_PrintStr("Verification..");
                 HAL_Delay(1500);

                 // Vérification si la pompe doit démarrer
                 if (adcData[1] < WATER_HIGH_ADC) // LOW ou MEDIUM
                 {
                     Demarrer_Pompe_Urgence();
                 }
                 else
                 {
                     HD44780_Clear();
                     HD44780_SetCursor(0, 0);
                     HD44780_PrintStr("Pompe OK");
                     HD44780_SetCursor(0, 1);
                     HD44780_PrintStr("Niveau eau OK");
                     HAL_Delay(1500);
                 }

                 const float alphaT = 0.7f;   // Coefficient de filtrage
                 char buf[17];

                 float temp_filt_local = (float)adcData[3];
                 float temp_local = Lire_Temperature_LM35((uint32_t)temp_filt_local);

                 while (temp_local >= 22.0f)
                 {
                     HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);   // Ventilo ON
                     HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);  // LED bleue ON
                     // HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_SET); // Buzzer ON (si besoin)

                     HD44780_Clear();
                     HD44780_SetCursor(0, 0);
                     HD44780_PrintStr("Temp haute   !");
                     HD44780_SetCursor(0, 1);
                     snprintf(buf, sizeof(buf), "T=%.1fC Vent:ON", temp_local);
                     HD44780_PrintStr(buf);

                     HAL_Delay(150);

                     // Mise à jour du filtre pendant l'urgence
                     temp_filt_local = alphaT * (float)adcData[3]
                                     + (1.0f - alphaT) * temp_filt_local;

                     temp_local = Lire_Temperature_LM35((uint32_t)temp_filt_local);
                 }

                 // Sortie de la boucle : température < 20°C
                 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);   // Ventilo OFF
                 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);  // LED bleue OFF
                 HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_RESET);   // Buzzer OFF

                 HD44780_Clear();
                 HD44780_SetCursor(0, 0);
                 HD44780_PrintStr("Temp normale");
                 HD44780_SetCursor(0, 1);
                 snprintf(buf, sizeof(buf), "T=%.1fC Vent:OFF", temp_local);
                 HD44780_PrintStr(buf);

                 HAL_Delay(3000);

                 HD44780_Clear();
                 HD44780_SetCursor(0, 0);
                 HD44780_PrintStr("Surveillance");
                 HD44780_SetCursor(0, 1);
                 HD44780_PrintStr("active..");

                 en_test = 0;
             }

         }

  /* USER CODE END 2 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_HSE_DIV24;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
    /*
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;  // déclenchement quand le bouton est pressé
     GPIO_InitStruct.Pull = GPIO_PULLUP;           // pull-up interne activé

  */



void get_time_date(char *time, char *date)
{
    RTC_DateTypeDef gDate;
    RTC_TimeTypeDef gTime;

    HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BIN);

    sprintf(time,"%02d:%02d:%02d", gTime.Hours, gTime.Minutes, gTime.Seconds);
    sprintf(date,"%02d-%02d-%04d", gDate.Date, gDate.Month, 2000 + gDate.Year);
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
