#ifndef AI_AQUARIUM_H
#define AI_AQUARIUM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

// Init network (returns true if OK)
bool AI_Init(void);

// Run inference
// temp_c: temperature in °C
// water_encoded: 0.0=LOW, 0.5=MID, 1.0=HIGH (comme toi)
// out_score: output brut du modèle (0..1 si sigmoid)
bool AI_RunStep(float temp_c, float water_encoded, float* out_score);



// Get last AI error code (for debug)
int32_t AI_LastError(void);

#ifdef __cplusplus
}
#endif

#endif
