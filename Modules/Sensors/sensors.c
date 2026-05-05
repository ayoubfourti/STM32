#include "sensors.h"

/* ==== DEFINITIONS DES VARIABLES ==== */
extern char capteurs_err[MAX_CAPTEURS][16];
extern int nb_err;
extern float water_adc_ema ;
extern float humidity_ema ;
#include <string.h>



/* ======= IMPLEMENTATIONS ========= */

int Humidity_ADC_to_Percent(uint32_t adc)
{
    if (adc > HUMIDITY_MIN) adc = HUMIDITY_MIN;
    if (adc < HUMIDITY_MAX) adc = HUMIDITY_MAX;

    float percent = ((float)(HUMIDITY_MIN - adc)) * 100.0f / (float)(HUMIDITY_MIN - HUMIDITY_MAX);

    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;

    if (humidity_ema < 0.0f)
        humidity_ema = percent;

    humidity_ema = 0.7f * percent + 0.3f * humidity_ema;

    return (int)humidity_ema;
}


float Lire_Temperature_LM35(uint32_t adc)
{
    static uint8_t first = 1;
    static float temp_ema = 0.0f;          // valeur filtrée

    // 1) Température brute avec ta formule
    float temp_raw = adc * 0.0595664f;

    // 2) Coefficients de filtre
    const float alpha = 0.3f;          // plus petit = plus stable, plus grand = plus réactif

        if (first)
        {
            temp_ema = temp_raw;           // init première fois
            first = 0;
        }
        else
        {
            temp_ema = alpha * temp_raw + (1.0f - alpha) * temp_ema;
        }

        return temp_ema;                   // on renvoie la valeur filtrée
    }



const char* Water_Level_Label(uint32_t adc)
{
    if (adc < WATER_LOW_ADC)
        return "LOW";
    else if (adc <= WATER_HIGH_ADC)
        return "MEDIUM";
    else
        return "HIGH";
}

float IA_WaterEncode(const char* level)
{
    if (level == NULL) return 0.5f;

    if (strcmp(level, "LOW") == 0) return 0.0f;
    if (strcmp(level, "MED") == 0) return 0.5f;
    if (strcmp(level, "HIGH") == 0) return 1.0f;

    return 0.5f;
}


const char* Sensor_Name(SensorId id)
{
    switch (id)
    {
        case SENSOR_HUMIDITY: return "Humidity";
        case SENSOR_WATER:    return "Water";
        case SENSOR_LDR:      return "LDR";
        case SENSOR_TEMP:     return "Temp";
        case ACT_VENTILO:     return "Ventilo";
        case ACT_POMPE:       return "Pompe";
        default:              return "Unknown";
    }
}
