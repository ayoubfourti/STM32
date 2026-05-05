#ifndef ACTUATORS_H
#define ACTUATORS_H

#include"main.h"
#include <stdbool.h>

extern volatile uint8_t diagnostique;
extern volatile uint8_t urgence ;
extern volatile  uint8_t en_test ;
extern volatile uint8_t pompe_urgence_active ;

// Buzzer
void Buzzer_Err(void);

// Test des actionneurs
bool Test_Pompe(void);
bool Test_Ventilo(void);
// Démarrage de la pompe en urgence
void Demarrer_Pompe_Urgence(void);
void Arreter_Pompe(void);


void Demarrer_Ventilo_Urgence(void);
void Arreter_Ventilo(void);




#endif
