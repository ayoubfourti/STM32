#include"events.h"
#include "stm32f4xx_hal.h"
#include "liquidcrystal_i2c.h"
#include <stdio.h>
#include <string.h>
#include "actuators.h"
#include "sensors.h"




extern ADC_HandleTypeDef hadc2;
extern TIM_HandleTypeDef htim7;
extern volatile uint8_t adc_fin;

extern float water_adc_ema ;


/* ================== INTERRUPTION BOUTON ================== */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    static uint32_t last6 = 0, last7 = 0, last15 = 0;
    uint32_t now = HAL_GetTick();

    // si tu veux bloquer pendant tests, OK
    if (en_test) return;

    if (GPIO_Pin == GPIO_PIN_6) {
        if (now - last6 < 80) return;   // debounce 80ms
        last6 = now;

        // Vérifie que c'est un vrai appui (niveau bas car pull-up)
        if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6) == GPIO_PIN_RESET) {
            urgence = 1;
            diagnostique = 0;
        }
        return;
    }

    if (GPIO_Pin == GPIO_PIN_7) {
        if (now - last7 < 80) return;
        last7 = now;

        if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7) == GPIO_PIN_RESET) {
            diagnostique = 1;
            urgence = 0;
        }
        return;
    }

    if (GPIO_Pin == GPIO_PIN_15) {
        if (now - last15 < 200) return;
        last15 = now;

        // adapte le port si ton bouton est sur un autre GPIO
        // exemple si c'est PA15:
        if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_15) == GPIO_PIN_RESET)
        IA_interrupt = !IA_interrupt;
        return;
    }
}

/* ================== DIAGNOSTIQUE ================== */

bool Diagnostic_Capteur(uint32_t adc, SensorId id)
{
    char buf[17];
    bool status_ok = true;


    HD44780_Clear();
    HD44780_SetCursor(0, 0);
    HD44780_PrintStr(Sensor_Name(id));

    switch (id)
    {
    case SENSOR_TEMP:
    {
        float temp = Lire_Temperature_LM35(adc);
        snprintf(buf, sizeof(buf), "Temp:%.2fC", temp);
        break;
    }

        case SENSOR_HUMIDITY:
        {
            int hum = Humidity_ADC_to_Percent(adc);
            snprintf(buf, sizeof(buf), "Hum:%3d%%", hum);
            break;
        }
        case SENSOR_WATER:
        {
            if (water_adc_ema == 0.0f)
                water_adc_ema = (float)adc;

            water_adc_ema = WATER_EMA_ALPHA * adc + (1.0f - WATER_EMA_ALPHA) * water_adc_ema;
            uint32_t smooth_adc = (uint32_t)water_adc_ema;

            const char* lvl = Water_Level_Label(smooth_adc);
            snprintf(buf, sizeof(buf), "%s", lvl);
            break;
        }

        case SENSOR_LDR:
        {
            if (adc < 3400)
                snprintf(buf, sizeof(buf), "Il fait jour");
            else
                snprintf(buf, sizeof(buf), "Il fait nuit");
            break;
        }
        case ACT_VENTILO:
        {
            snprintf(buf, sizeof(buf), "Test Ventilo");
            break;
        }
        case ACT_POMPE:
        {
            snprintf(buf, sizeof(buf), "Test Pompe");
            break;
        }
        default:
            snprintf(buf, sizeof(buf), "Inconnu");
            break;
    }

    HD44780_SetCursor(0, 1);
    HD44780_PrintStr(buf);
    HAL_Delay(2000);

    if (id == SENSOR_HUMIDITY)
    {
        HD44780_Clear();
        HD44780_SetCursor(0, 0);
        HD44780_PrintStr("Humidity");

        int hum = Humidity_ADC_to_Percent(adc);
        HD44780_SetCursor(0, 1);

        if (hum < 60)
        {
            HD44780_PrintStr("OK");
        }
        else
        {
            HD44780_PrintStr("ERR");
            status_ok = false;
            Buzzer_Err();
        }
        HAL_Delay(1500);
    }
    else if (id == SENSOR_WATER)
    {
        const char* niveau = Water_Level_Label(adc);

        HD44780_Clear();
        HD44780_SetCursor(0, 0);
        HD44780_PrintStr("Water");

        HD44780_SetCursor(0, 1);
        HD44780_PrintStr(niveau);

        if (adc < WATER_LOW_ADC)
        {
            HD44780_PrintStr(" ERR");
            status_ok = false;
            Buzzer_Err();
        }
        else
        {
            HD44780_PrintStr(" OK");
        }
        HAL_Delay(1500);
    }
    else if (id == ACT_POMPE)
    {
    	status_ok=Test_Pompe();
    }
    else if (id == ACT_VENTILO)
    {
    	status_ok=Test_Ventilo();
    }

    return status_ok;
}

/* Callback du timer - S'exécute UNE SEULE FOIS  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim7)
    {

        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);

        HAL_TIM_Base_Stop_IT(&htim7);   // important !
    }
}



void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC2)
    {
        adc_fin = 1;   // signale à la boucle principale que de nouvelles données sont dispo
    }
}

