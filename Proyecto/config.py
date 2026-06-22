from dotenv import load_dotenv
import os

load_dotenv()


BAUD_RATE = int(os.getenv("BAUD_RATE", "9600"))

