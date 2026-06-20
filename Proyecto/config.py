from dotenv import load_dotenv
import os

load_dotenv()

PUERTO = os.getenv("PUERTO")
BAUD_RATE = int(os.getenv("BAUD_RATE", "9600"))

if not PUERTO:
    raise ValueError("PUERTO debe estar definido en el archivo .env")
