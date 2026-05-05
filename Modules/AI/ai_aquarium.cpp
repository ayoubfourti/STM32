#include "ai_aquarium.h"

#include <stdio.h>
#include <string.h>

extern "C" {
#include "network.h"
#include "network_data.h"
#include "network_data_params.h"
#include "liquidcrystal_i2c.h"
}

  // pour HD44780_...

// =================== NORMALISATION ===================
// Mets ici EXACTEMENT les mêmes valeurs que Python
#define TEMP_MEAN_PY   21.8873672485f
#define TEMP_STD_PY     5.3754835592f
#define WATER_MEAN_PY   0.7729837298f
#define WATER_STD_PY    0.2768721746f

static float standardize(float x, float mean, float stdv) {
  return (x - mean) / stdv;
}

// =================== X-CUBE-AI =======================
static ai_handle s_network = AI_HANDLE_NULL;
static int32_t   s_last_err = 0;

// Activations buffer (OBLIGATOIRE car ton réseau demande 96 bytes)
AI_ALIGNED(4)
static uint8_t s_activations[AI_NETWORK_DATA_ACTIVATIONS_SIZE];

// I/O buffers float
static ai_float s_in[AI_NETWORK_IN_1_SIZE];     // 2 floats
static ai_float s_out[AI_NETWORK_OUT_1_SIZE];   // 1 float

int32_t AI_LastError(void) {
  return s_last_err;
}

bool AI_Init(void)
{
  s_last_err = 0;

  // weights table pointer (fourni par CubeAI)
  const ai_handle weights[] = { AI_NETWORK_DATA_WEIGHTS_TABLE_GET()[0] };

  // activations buffer pointer
  const ai_handle activations[] = { (ai_handle)s_activations };

  ai_error err = ai_network_create_and_init(&s_network, activations, weights);
  if (err.type != AI_ERROR_NONE) {
    s_network = AI_HANDLE_NULL;
    // pack error into int for debug
    s_last_err = ((int32_t)err.type << 16) | (int32_t)err.code;
    return false;
  }

  return true;
}

bool AI_RunStep(float temp_c, float water_encoded, float* out_score)
{
  if (!s_network) {
    if (out_score) *out_score = -1.0f;
    return false;
  }

  // Normalize exactly like training
  const float temp_norm  = standardize(temp_c, TEMP_MEAN_PY, TEMP_STD_PY);
  const float water_norm = standardize(water_encoded, WATER_MEAN_PY, WATER_STD_PY);

  s_in[0] = temp_norm;
  s_in[1] = water_norm;

  // bind buffers
  ai_buffer* in  = ai_network_inputs_get(s_network, NULL);
  ai_buffer* out = ai_network_outputs_get(s_network, NULL);

  in[0].data  = AI_HANDLE_PTR(s_in);
  out[0].data = AI_HANDLE_PTR(s_out);

  ai_i32 n_batch = ai_network_run(s_network, in, out);
  if (n_batch != 1) {
    // read error for debug
    ai_error e = ai_network_get_error(s_network);
    s_last_err = ((int32_t)e.type << 16) | (int32_t)e.code;
    if (out_score) *out_score = -1.0f;
    return false;
  }

  const float score = s_out[0];
  if (out_score) *out_score = score;

  // Si ton modèle finit par sigmoid → score entre 0 et 1
  // Seuil à 0.5 typiquement
  return (score > 0.5f);
}


// =================== TEST LCD =========================
typedef struct {
  float t;
  float w;
  const char* name;
} TestCase;


