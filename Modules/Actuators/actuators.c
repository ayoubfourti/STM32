#include"actuators.h"
#include "stm32f4xx_hal.h"
#include "liquidcrystal_i2c.h"
#include <stdio.h>
#include <string.h>


#include "tim.h"

/*  Activer buzzer pendant la Diagnostique   */
void Buzzer_Err(void)
{
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_SET);
    HAL_Delay(300);
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_RESET);
}


/*  Tester la pompe   */
bool Test_Pompe(void)
{
	bool status = true;


    // Afficher le message du test
    HD44780_Clear();
    HD44780_SetCursor(0, 0);
    HD44780_PrintStr("Test Pompe");
    HD44780_SetCursor(0, 1);
    HD44780_PrintStr("ON 3 sec...");

    // Activer la pompe
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
    HAL_Delay(3000);

    // Lire la state
    GPIO_PinState state = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6);

    // Afficher le resultat
    HD44780_Clear();
    HD44780_SetCursor(0, 0);
    HD44780_PrintStr("Pompe");
    HD44780_SetCursor(0, 1);

    if(state == GPIO_PIN_SET)
    {
        HD44780_PrintStr("OK");
    }
    else
    {
        HD44780_PrintStr("ERR");
        status = false;
        Buzzer_Err();
    }

    // Désactiver la pompe
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
    HAL_Delay(1500);

    return status;

}

/*  Tester le ventilateur   */
bool Test_Ventilo(void){

	bool status = true;

	HD44780_Clear();
	HD44780_SetCursor(0, 0);
	HD44780_PrintStr("Test Ventilo");
	HD44780_SetCursor(0, 1);
	HD44780_PrintStr("ON 3 sec...");

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_Delay(3000);

	GPIO_PinState state = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7);

	HD44780_Clear();
	HD44780_SetCursor(0, 0);
	HD44780_PrintStr("Ventilo");
	HD44780_SetCursor(0, 1);

	if (state == GPIO_PIN_SET)
	{
		HD44780_PrintStr("OK");
	}
	else
	{
		HD44780_PrintStr("ERR");
		status = false;
		Buzzer_Err();
	}

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_Delay(1500);

    return status;

}

void Demarrer_Pompe_Urgence(void)
{
    // Affichage
    HD44780_Clear();
    HD44780_SetCursor(0, 0);
    HD44780_PrintStr("POMPE URGENCE");

    // Pompe + LED
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET); // Pompe ON
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET); // LED rouge ON


    // 🔴 SÉCURITÉ TIMER (OBLIGATOIRE)
    HAL_TIM_Base_Stop_IT(&htim7);                  // stop si déjà lancé
    __HAL_TIM_SET_COUNTER(&htim7, 0);              // reset compteur
    __HAL_TIM_CLEAR_FLAG(&htim7, TIM_FLAG_UPDATE); // effacer flag
    HAL_TIM_Base_Start_IT(&htim7);                 // start one-shot
}


void Arreter_Pompe(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET); // Pompe OFF
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET); // LED rouge OFF (adapte)
}



void Demarrer_Ventilo_Urgence(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET); // Ventilo ON
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET); // LED bleue ON (adapte le pin)
}

void Arreter_Ventilo(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET); // Ventilo OFF
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET); // LED bleue OFF
}


