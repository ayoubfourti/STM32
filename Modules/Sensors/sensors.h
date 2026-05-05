#ifndef SENSORS_H
#define SENSORS_H

#include <stdint.h>
#include "stm32f4xx_hal.h"



/* ==== CONSTANTES DES CAPTEURS ==== */
#define MAX_CAPTEURS 8
#define WATER_EMA_ALPHA 0.60f


#define WATER_LOW_ADC    3100
#define WATER_HIGH_ADC   340

#define HUMIDITY_MIN 3050
#define HUMIDITY_MAX 1130

#define LDR_DAY_MIN 3100
#define LDR_DAY_MAX 2300

#define TEMP_MIN_DEG 5.0f
#define TEMP_MAX_DEG 30.0f

#define WATER_LOW_POINT   1600
#define WATER_MED_POINT   2040

#define TEMP_OFFSET (-0.0f)


/* ================== TYPE DE CAPTEUR ================== */
typedef enum
{
    SENSOR_HUMIDITY = 0,
    SENSOR_WATER,
    SENSOR_LDR,
    SENSOR_TEMP,
    ACT_VENTILO,
    ACT_POMPE
} SensorId;



int Humidity_ADC_to_Percent(uint32_t adc);
float Lire_Temperature_LM35(uint32_t adc);
const char* Water_Level_Label(uint32_t adc);
float IA_WaterEncode(const char* level);
const char* Sensor_Name(SensorId id);


#endif
