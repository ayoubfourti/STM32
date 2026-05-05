#include "ai_monitor.h"
#include "ai_aquarium.h"
#include "sensors.h"

#include <string.h>
#include <stdio.h>

#define AI_THRESHOLD  (0.5f)

void AI_Monitor_Init(void)
{
  // rien pour l’instant
}

AiStatus AI_Monitor_Update(float temp_c, const char* water_level_str)
{
  AiStatus st;
  memset(&st, 0, sizeof(st));
  strcpy(st.msg, "OK");

  // Encode water level string -> float
  float water_enc = IA_WaterEncode(water_level_str);

  float score = 0.0f;
  bool ok_run = AI_RunStep(temp_c, water_enc, &score);
  (void)ok_run;

  st.score = score;
  st.type  = ANOM_NONE;
  st.anom  = false;

  // ===============================
  // 1) REGLE METIER EAU (priorité)
  // ===============================
  // Si tu veux que MEDIUM soit déjà une alerte → on force
  if (water_enc < 1.0f)  // MEDIUM ou LOW
  {
    st.anom = true;

      st.type = ANOM_FUITE_EAU;
      strcpy(st.msg, "FUITE EAU");
    st.msg[16] = '\0';
    return st;
  }

  // ===============================
  // 2) Sinon on suit l'IA (HIGH)
  // ===============================
  st.anom = (score > AI_THRESHOLD);
  if (!st.anom) {
    strcpy(st.msg, "OK");
    st.msg[16] = '\0';
    return st;
  }

  // ===============================
  // 3) Interprétation IA
  // ===============================
  if (temp_c > 22.0f) {
    st.type = ANOM_SURCHAUFFE;
    strcpy(st.msg, "SURCHAUFFE");
  }
  else if (temp_c < 15.0f) {
    st.type = ANOM_FROID;
    strcpy(st.msg, "TROP FROID");
  }
  else {
    st.type = ANOM_INCONNUE;
    strcpy(st.msg, "ANOMALIE");
  }

  st.msg[16] = '\0';
  return st;
}
