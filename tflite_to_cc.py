data = open("aquarium_model.tflite", "rb").read()

with open("aquarium_model.cc", "w") as f:
    f.write("const unsigned char aquarium_model_tflite[] = {")
    for i, b in enumerate(data):
        if i % 12 == 0:
            f.write("\n  ")
        f.write(f"0x{b:02x}, ")
    f.write("\n};\n")
    f.write(f"const unsigned int aquarium_model_tflite_len = {len(data)};\n")
