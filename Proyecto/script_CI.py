import os
import sys
import time
import glob
import serial
from config import BAUD_RATE


def detectar_puerto():
    candidatos = []

    # Puertos comunes en macOS para Arduino y clones
    candidatos.extend(sorted(glob.glob("/dev/cu.usbmodem*")))
    candidatos.extend(sorted(glob.glob("/dev/cu.usbserial*")))
    candidatos.extend(sorted(glob.glob("/dev/cu.wchusbserial*")))
    candidatos.extend(sorted(glob.glob("/dev/cu.SLAB_USBtoUART*")))

    if not candidatos:
        return None

    return candidatos[0]


def validar_archivo(file_path):
    if not file_path.endswith(".org"):
        print("Error: el archivo debe tener extension .org")
        sys.exit(1)

    if not os.path.isfile(file_path):
        print(f"Error: el archivo no existe: {file_path}")
        sys.exit(1)


def enviar_archivo_org(ser, file_path):
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


def leer_respuestas_finales(ser, segundos=4):
    print("\n--- Esperando respuestas del Arduino ---")
    tiempo_final = time.time() + segundos

    while time.time() < tiempo_final:
        if ser.in_waiting > 0:
            respuesta = ser.readline().decode("utf-8", errors="ignore").strip()
            if respuesta:
                print(f"Arduino: {respuesta}")
        time.sleep(0.05)


def main():
    if len(sys.argv) < 2:
        print("Uso: python script_CI.py archivo.org")
        sys.exit(1)

    file_path = sys.argv[1]
    validar_archivo(file_path)

    print(f"Archivo seleccionado: {file_path}")

    puerto = detectar_puerto()

    if not puerto:
        print("Error: no se encontro ningun puerto serial compatible.")
        print("Verifica que el Arduino este conectado.")
        sys.exit(1)

    print(f"Puerto detectado: {puerto}")

    try:
        ser = serial.Serial(puerto, BAUD_RATE, timeout=1)
        time.sleep(4)  # tiempo para que el Arduino reinicie
        ser.reset_input_buffer()
        ser.reset_output_buffer()
        print(f"Conectado al puerto serial {puerto} a {BAUD_RATE} baudios")
    except Exception as e:
        print(f"Error al conectar al puerto serial: {e}")
        sys.exit(1)

    try:
        enviar_archivo_org(ser, file_path)
        leer_respuestas_finales(ser, segundos=4)

    except KeyboardInterrupt:
        print("Interrupcion del usuario")
    except Exception as e:
        print(f"Error durante el envio: {e}")
    finally:
        ser.close()
        print("Puerto serial cerrado")


if __name__ == "__main__":
    main()