import numpy as np
import pandas as pd

# ==========================
# PARAMÈTRES GLOBAUX
# ==========================
N_NORMAL = 800
N_ANOM   = 800

np.random.seed(42)  # pour des résultats reproductibles (optionnel)

# ==========================
# Encodage du niveau d'eau
# ==========================
def encode_water(level: str) -> float:
    if level == "LOW":
        return 0.0
    if level == "MID":
        return 0.5
    return 1.0  # HIGH


# =========================================================
# 1) DONNÉES NORMALES (label = 0)
# =========================================================
# Hypothèse :
#  - Température confortable : surtout entre 20°C et 25°C
#  - Parfois un peu plus bas (18°C) ou un peu plus haut (27°C)
#  - Niveau d'eau : surtout HIGH, parfois MID, jamais LOW
#  - Ajout d'un petit bruit sur temp et level
# =========================================================

temps_normal = np.random.uniform(15.0, 24.0, size=N_NORMAL)

waters_normal = []
for _ in range(N_NORMAL):
    level_str = np.random.choice(["MID", "HIGH"], p=[0.2, 0.8])
    waters_normal.append(encode_water(level_str))

temps_normal = np.array(temps_normal)
waters_normal = np.array(waters_normal)

# Bruit réaliste (capteur imparfait)
temps_normal += np.random.normal(0.0, 0.25, size=N_NORMAL)   # bruit sur temp
waters_normal += np.random.normal(0.0, 0.03, size=N_NORMAL)  # bruit sur niveau
waters_normal = np.clip(waters_normal, 0.0, 1.0)

labels_normal = np.zeros(N_NORMAL, dtype=int)


# =========================================================
# 2) DONNÉES ANORMALES (label = 1)
# =========================================================
# On va créer plusieurs types d'anomalies :
#
#  A) Surchauffe progressive  (temp_pre_high)
#     - temp = 25–29°C (zone limite)
#     - water = MID ou HIGH
#
#  B) Surchauffe franche (temp_high)
#     - temp = 29–35°C
#     - water = MID ou HIGH
#
#  C) Eau froide (temp_low)
#     - temp = 10–15°C
#     - water = MID ou HIGH
#
#  D) Fuite progressive (leak_progressive)
#     -> séquences de niveau d'eau qui descend (HIGH -> LOW)
#
#  E) Eau basse stable (water_low)
#     - temp normale
#     - water ≈ LOW
#
# On répartit les 800 anomalies entre ces catégories.
# =========================================================

temps_anom = []
waters_anom = []
labels_anom = []

# ---------- A) Surchauffe progressive ----------
N_A = 200
for _ in range(N_A):
    t = np.random.uniform(25.0, 28.0)
    w = encode_water(np.random.choice(["MID", "HIGH"]))
    t += np.random.normal(0.0, 0.2)
    w += np.random.normal(0.0, 0.03)
    w = np.clip(w, 0.0, 1.0)

    temps_anom.append(t)
    waters_anom.append(w)
    labels_anom.append(1)

# ---------- B) Surchauffe franche ----------
N_B = 200
for _ in range(N_B):
    t = np.random.uniform(29.0, 35.0)
    w = encode_water(np.random.choice(["MID", "HIGH"]))
    t += np.random.normal(0.0, 0.3)
    w += np.random.normal(0.0, 0.03)
    w = np.clip(w, 0.0, 1.0)

    temps_anom.append(t)
    waters_anom.append(w)
    labels_anom.append(1)

# ---------- C) Eau froide ----------
N_C = 100
for _ in range(N_C):
    t = np.random.uniform(10.0, 15.0)
    w = encode_water(np.random.choice(["MID", "HIGH"]))
    t += np.random.normal(0.0, 0.3)
    w += np.random.normal(0.0, 0.03)
    w = np.clip(w, 0.0, 1.0)

    temps_anom.append(t)
    waters_anom.append(w)
    labels_anom.append(1)

# ---------- D) Fuite progressive (scénarios séquentiels) ----------
# On simule plusieurs fuites : pour chaque fuite, une séquence de "steps" points
# Niveau d'eau descend de HIGH -> LOW, temp reste plutôt normale.
N_SCENARIOS_LEAK = 15      # 15 scénarios de fuite
STEPS_PER_LEAK   = 20      # 20 points par scénario -> 300 points

for _ in range(N_SCENARIOS_LEAK):
    # temp autour de 20–25°C
    temp_start = np.random.uniform(19.0, 25.0)
    temp_end   = temp_start + np.random.uniform(-1.0, 1.0)

    temps_seq = np.linspace(temp_start, temp_end, STEPS_PER_LEAK)

    water_start = 1.0   # HIGH
    water_end   = 0.0   # LOW
    water_seq   = np.linspace(water_start, water_end, STEPS_PER_LEAK)

    # bruit sur les séquences
    temps_seq  += np.random.normal(0.0, 0.2, STEPS_PER_LEAK)
    water_seq  += np.random.normal(0.0, 0.05, STEPS_PER_LEAK)
    water_seq   = np.clip(water_seq, 0.0, 1.0)

    for t, w in zip(temps_seq, water_seq):
        temps_anom.append(t)
        waters_anom.append(w)
        labels_anom.append(1)

# On vérifie combien on a d'anomalies pour compléter jusqu'à 800
current_anom = len(labels_anom)
remaining = N_ANOM - current_anom

# ---------- E) Eau basse stable (niveau LOW) ----------
# Si besoin de compléter jusqu'à 800 anomalies, on crée des points
# avec water LOW et temp normale
for _ in range(remaining):
    t = np.random.uniform(18.0, 25.0)   # temp normale
    w = encode_water("LOW")            # eau basse
    t += np.random.normal(0.0, 0.2)
    w += np.random.normal(0.0, 0.03)
    w = np.clip(w, 0.0, 1.0)

    temps_anom.append(t)
    waters_anom.append(w)
    labels_anom.append(1)


# =========================================================
# 3) ASSEMBLAGE + MÉLANGE
# =========================================================

temps_anom  = np.array(temps_anom)
waters_anom = np.array(waters_anom)
labels_anom = np.array(labels_anom, dtype=int)

# Sanity check
print("Anomalies générées :", len(labels_anom))

temp_all  = np.concatenate([temps_normal, temps_anom])
water_all = np.concatenate([waters_normal, waters_anom])
label_all = np.concatenate([labels_normal, labels_anom])

indices = np.arange(len(temp_all))
np.random.shuffle(indices)

df = pd.DataFrame({
    "temp":  temp_all[indices],
    "water": water_all[indices],
    "label": label_all[indices]
})

# =========================================================
# 4) SAUVEGARDE CSV
# =========================================================

df.to_csv("aquarium_data_solid.csv", index=False)

print("Dataset généré : aquarium_data_solid.csv")
print(df.head())
print("Nombre total de lignes :", len(df))
print("Normales :", (df['label'] == 0).sum(),
      " | Anormales :", (df['label'] == 1).sum())
