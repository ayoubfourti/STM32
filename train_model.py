import numpy as np
import pandas as pd
import tensorflow as tf
from tensorflow import keras
from sklearn.model_selection import train_test_split

# 1) Charger la dataset
df = pd.read_csv("aquarium_data_solid.csv")

# Features (entrées IA) : temp, water
X = df[["temp", "water"]].values.astype(np.float32)

# Label (sortie IA) : 0 = normal, 1 = anomalie
y = df["label"].values.astype(np.float32)

# 2) Normalisation simple (IMPORTANT : on fera la même sur STM32)
# On centre et met à l'échelle pour stabiliser l'entraînement
temp_mean = X[:,0].mean()
temp_std  = X[:,0].std() + 1e-6

water_mean = X[:,1].mean()
water_std  = X[:,1].std() + 1e-6

X[:,0] = (X[:,0] - temp_mean) / temp_std
X[:,1] = (X[:,1] - water_mean) / water_std

print("temp_mean =", temp_mean, " temp_std =", temp_std)
print("water_mean =", water_mean, " water_std =", water_std)

# On sauvegarde ces paramètres pour les recopier dans le STM32
with open("normalization_params.txt", "w") as f:
    f.write(f"{temp_mean},{temp_std},{water_mean},{water_std}\n")

# 3) Split train / test
X_train, X_test, y_train, y_test = train_test_split(
    X, y, test_size=0.2, random_state=42, stratify=y
)

# 4) Définition du modèle Keras (petit réseau adapté STM32F4)
model = keras.Sequential([
    keras.layers.Input(shape=(2,)),   # [temp_norm, water_norm]
    keras.layers.Dense(16, activation="relu"),
    keras.layers.Dense(8, activation="relu"),
    keras.layers.Dense(4, activation="relu"),
    keras.layers.Dense(1, activation="sigmoid")  # probabilité d'anomalie
])

model.compile(
    optimizer=keras.optimizers.Adam(learning_rate=0.001),
    loss="binary_crossentropy",
    metrics=["accuracy"]
)

model.summary()

# 5) Entraînement
history = model.fit(
    X_train, y_train,
    validation_data=(X_test, y_test),
    epochs=30,
    batch_size=32,
    verbose=1
)

# 6) Évaluation
loss, acc = model.evaluate(X_test, y_test, verbose=0)
print("Accuracy sur le test :", acc)

# 7) Sauvegarde du modèle Keras
model.save("aquarium_model_keras.h5")

# 8) Conversion en TensorFlow Lite
converter = tf.lite.TFLiteConverter.from_keras_model(model)
# Optimisation pour embarqué (optionnel, float32 simple)
converter.optimizations = [tf.lite.Optimize.DEFAULT]
tflite_model = converter.convert()

with open("aquarium_model.tflite", "wb") as f:
    f.write(tflite_model)

print("Modèle TFLite généré : aquarium_model.tflite")
