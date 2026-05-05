#ifndef EVENTS_H
#define EVENTS_H


#include"main.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>
#include "sensors.h"



extern volatile uint8_t IA_interrupt;
extern volatile  uint8_t diagnostique;
extern volatile  uint8_t urgence ;
extern volatile  uint8_t en_test ;
extern volatile  uint8_t pompe_urgence_active ;
// Correction du calcul de la période du TIM7
#define POMPE_URGENCE_DUREE_MS 5000  // 5 secondes en millisecondes
#define TIM7_PRESCALER 49999         // 50MHz / 50000 = 1kHz
#define TIM7_PERIOD ((POMPE_URGENCE_DUREE_MS) - 1)  // 4999 pour 5000ms


/// DIAGNOSTIQUE
bool Diagnostic_Capteur(uint32_t adc,SensorId id);

/// Boutton interruption
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

/// Interruption de Timer 7
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);




#endif
