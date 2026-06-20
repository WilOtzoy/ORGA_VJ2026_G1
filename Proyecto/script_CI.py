import serial
import time
import os
import sys
from config import PUERTO, BAUD_RATE

if len(sys.argv) < 2:
    print("Uso: python script_CI.py archivo.org")
    sys.exit(1)

file_path = sys.argv[1]

if not file_path.endswith(".org"):
    print("El archivo debe tener extension .org")
    sys.exit(1)

if not os.path.isfile(file_path):
    print("El archivo no existe")
    sys.exit(1)

print(f"Archivo seleccionado: {file_path}")

try:
    ser = serial.Serial(PUERTO, BAUD_RATE, timeout=1)
    time.sleep(4)
    ser.reset_input_buffer()
    ser.reset_output_buffer()
    print(f"Conectado al puerto serial {PUERTO} a {BAUD_RATE} baudios")
except Exception as e:
    print(f"Error al conectar al puerto serial: {e}")
    sys.exit(1)

try:
    with open(file_path, "r", encoding="utf-8") as f:
        lines = f.readlines()

    for line in lines:
        line = line.strip()
        if line == "":
            continue

        ser.write((line + "\n").encode("utf-8"))
        print(f"Enviado: {line}")
        time.sleep(0.3)

        timeout = time.time() + 2
        while time.time() < timeout:
            if ser.in_waiting > 0:
                response = ser.readline().decode("utf-8", errors="ignore").strip()
                if response:
                    print(f"Arduino: {response}")
                break
            time.sleep(0.05)

    print("\n--- Esperando respuestas del Arduino ---")
    tiempo_final = time.time() + 4
    while time.time() < tiempo_final:
        if ser.in_waiting > 0:
            respuesta = ser.readline().decode("utf-8", errors="ignore").strip()
            if respuesta:
                print(f"Arduino: {respuesta}")
        time.sleep(0.05)

except KeyboardInterrupt:
    print("Interrupcion del usuario")
except Exception as e:
    print(f"Error durante el envio: {e}")
finally:
    ser.close()
    print("Puerto serial cerrado")