#ifndef AI_MONITOR_H
#define AI_MONITOR_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  ANOM_NONE = 0,
  ANOM_SURCHAUFFE,
  ANOM_FROID,
  ANOM_FUITE_EAU,
  ANOM_NIVEAU_BAS,
  ANOM_INCONNUE
} AiAnomaly;

typedef struct {
  float score;          // 0..1
  bool  anom;           // true si anomalie
  AiAnomaly type;       // type interprété
  char  msg[17];        // message prêt LCD (16 chars + '\0')
} AiStatus;

// Appeler 1x au démarrage (après AI_Init)
void AI_Monitor_Init(void);

// Appeler périodiquement (ex: toutes les 1s)
AiStatus AI_Monitor_Update(float temp_c, const char* water_level_str);

#ifdef __cplusplus
}
#endif

#endif
