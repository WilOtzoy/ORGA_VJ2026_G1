#include <EEPROM.h>

#define SALA_L1     2
#define SALA_L2     3
#define SALA_L3     4

#define COMEDOR_L1  5
#define COMEDOR_L2  6
#define COMEDOR_L3  7

#define COCINA_L1   8
#define COCINA_L2   9
#define COCINA_L3   10

#define BANO_L1     22
#define BANO_L2     23
#define BANO_L3     24

#define HAB_L1      25
#define HAB_L2      26
#define HAB_L3      27

#define LED_AZUL    28
#define LED_VERDE   29
#define LED_ROJO    30

const int LEDS_AMBIENTES[] = {
  SALA_L1, SALA_L2, SALA_L3,
  COMEDOR_L1, COMEDOR_L2, COMEDOR_L3,
  COCINA_L1, COCINA_L2, COCINA_L3,
  BANO_L1, BANO_L2, BANO_L3,
  HAB_L1, HAB_L2, HAB_L3
};
const int TOTAL_LEDS = 15;

#define EEPROM_MODO_ACTUAL  50

#define MODO_NINGUNO        0
#define MODO_FIESTA         1
#define MODO_RELAJADO       2
#define MODO_NOCHE          3
#define MODO_ENCENDER_TODO  4
#define MODO_APAGAR_TODO    5

int modoActual = MODO_NINGUNO;
bool cargandoArchivo = false;
String lineaSerial = "";
String lineaBluetooth = "";

unsigned long tiempoAnteriorFiesta = 0;
bool estadoFiesta = false;
const long INTERVALO_FIESTA = 300;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  for (int i = 0; i < TOTAL_LEDS; i++) {
    pinMode(LEDS_AMBIENTES[i], OUTPUT);
    digitalWrite(LEDS_AMBIENTES[i], LOW);
  }

  pinMode(LED_AZUL, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_ROJO, OUTPUT);

  digitalWrite(LED_AZUL, LOW);
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_ROJO, LOW);

  modoActual = EEPROM.read(EEPROM_MODO_ACTUAL);
  if (modoActual < MODO_NINGUNO || modoActual > MODO_APAGAR_TODO) {
    modoActual = MODO_NINGUNO;
  }

  if (modoActual != MODO_NINGUNO) {
    aplicarModo(modoActual);
  }

  digitalWrite(LED_AZUL, HIGH);
  Serial.println("Sistema listo. Esperando comandos.");
}

void loop() {
  leerSerialUSB();
  leerBluetooth();

  if (modoActual == MODO_FIESTA) {
    actualizarFiesta();
  }
}

void leerSerialUSB() {
  while (Serial.available() > 0) {
    char c = Serial.read();

    if (c == '\n') {
      lineaSerial.trim();
      if (lineaSerial.length() > 0) {
        procesarLineaOrg(lineaSerial);
      }
      lineaSerial = "";
    } else {
      lineaSerial += c;
    }
  }
}

void leerBluetooth() {
  if (cargandoArchivo) return;

  while (Serial1.available() > 0) {
    char c = Serial1.read();

    if (c == '\n') {
      lineaBluetooth.trim();
      if (lineaBluetooth.length() > 0) {
        procesarComandoBluetooth(lineaBluetooth);
      }
      lineaBluetooth = "";
    } else {
      lineaBluetooth += c;
    }
  }
}

void procesarLineaOrg(String linea) {
  if (linea.startsWith("//")) return;
  if (linea.length() == 0) return;

  int posComentario = linea.indexOf("//");
  if (posComentario > 0) {
    linea = linea.substring(0, posComentario);
    linea.trim();
  }

  if (linea == "conf_ini") {
    cargandoArchivo = true;
    Serial.println("OK: Inicio de archivo detectado.");
    return;
  }

  if (linea == "conf:fin") {
    if (!cargandoArchivo) {
      mostrarError();
      Serial.println("ERROR: Sin conf_ini");
      return;
    }

    cargandoArchivo = false;
    parpadearVerde();
    Serial.println("OK: Configuracion guardada.");
    aplicarModo(modoActual);
    return;
  }

  if (!cargandoArchivo) return;

  if (linea == "modo_fiesta") {
    modoActual = MODO_FIESTA;
    EEPROM.write(EEPROM_MODO_ACTUAL, MODO_FIESTA);
    Serial.println("OK: modo_fiesta guardado.");
    return;
  }

  if (linea == "modo_relajado") {
    modoActual = MODO_RELAJADO;
    EEPROM.write(EEPROM_MODO_ACTUAL, MODO_RELAJADO);
    Serial.println("OK: modo_relajado guardado.");
    return;
  }

  if (linea == "modo_noche") {
    modoActual = MODO_NOCHE;
    EEPROM.write(EEPROM_MODO_ACTUAL, MODO_NOCHE);
    Serial.println("OK: modo_noche guardado.");
    return;
  }

  if (linea == "encender_todo") {
    modoActual = MODO_ENCENDER_TODO;
    EEPROM.write(EEPROM_MODO_ACTUAL, MODO_ENCENDER_TODO);
    Serial.println("OK: encender_todo guardado.");
    return;
  }

  if (linea == "apagar_todo") {
    modoActual = MODO_APAGAR_TODO;
    EEPROM.write(EEPROM_MODO_ACTUAL, MODO_APAGAR_TODO);
    Serial.println("OK: apagar_todo guardado.");
    return;
  }

  if (linea.startsWith("LED'S:")) return;
  if (linea.startsWith("Mensaje en LCD:")) return;

  mostrarError();
  Serial.print("ERROR: linea no reconocida: ");
  Serial.println(linea);
}

void procesarComandoBluetooth(String cmd) {
  if (cmd == "modo_fiesta") {
    modoActual = MODO_FIESTA;
    EEPROM.write(EEPROM_MODO_ACTUAL, MODO_FIESTA);
    aplicarModo(MODO_FIESTA);
    parpadearVerde();
    Serial1.println("OK: Modo FIESTA activado.");

  } else if (cmd == "modo_relajado") {
    modoActual = MODO_RELAJADO;
    EEPROM.write(EEPROM_MODO_ACTUAL, MODO_RELAJADO);
    aplicarModo(MODO_RELAJADO);
    parpadearVerde();
    Serial1.println("OK: Modo RELAJADO activado.");

  } else if (cmd == "modo_noche") {
    modoActual = MODO_NOCHE;
    EEPROM.write(EEPROM_MODO_ACTUAL, MODO_NOCHE);
    aplicarModo(MODO_NOCHE);
    parpadearVerde();
    Serial1.println("OK: Modo NOCHE activado.");

  } else if (cmd == "encender_todo") {
    modoActual = MODO_ENCENDER_TODO;
    EEPROM.write(EEPROM_MODO_ACTUAL, MODO_ENCENDER_TODO);
    aplicarModo(MODO_ENCENDER_TODO);
    parpadearVerde();
    Serial1.println("OK: Todo encendido.");

  } else if (cmd == "apagar_todo") {
    modoActual = MODO_APAGAR_TODO;
    EEPROM.write(EEPROM_MODO_ACTUAL, MODO_APAGAR_TODO);
    aplicarModo(MODO_APAGAR_TODO);
    parpadearVerde();
    Serial1.println("OK: Todo apagado.");

  } else if (cmd == "estado") {
    String respuesta = "Modo actual: ";
    switch (modoActual) {
      case MODO_FIESTA: respuesta += "FIESTA"; break;
      case MODO_RELAJADO: respuesta += "RELAJADO"; break;
      case MODO_NOCHE: respuesta += "NOCHE"; break;
      case MODO_ENCENDER_TODO: respuesta += "ENCENDER_TODO"; break;
      case MODO_APAGAR_TODO: respuesta += "APAGAR_TODO"; break;
      default: respuesta += "NINGUNO"; break;
    }
    Serial1.println(respuesta);

  } else {
    Serial1.println("ERROR: Modo invalido");
    mostrarError();
  }
}

void aplicarModo(int modo) {
  switch (modo) {
    case MODO_FIESTA:
      break;
    case MODO_RELAJADO:
      apagarTodosLEDs();
      break;
    case MODO_NOCHE:
      apagarTodosLEDs();
      break;
    case MODO_ENCENDER_TODO:
      encenderTodosLEDs();
      break;
    case MODO_APAGAR_TODO:
      apagarTodosLEDs();
      break;
  }
}

void actualizarFiesta() {
  unsigned long tiempoActual = millis();

  if (tiempoActual - tiempoAnteriorFiesta >= INTERVALO_FIESTA) {
    tiempoAnteriorFiesta = tiempoActual;
    estadoFiesta = !estadoFiesta;

    digitalWrite(SALA_L1,   estadoFiesta ? HIGH : LOW);
    digitalWrite(SALA_L2,   estadoFiesta ? HIGH : LOW);
    digitalWrite(SALA_L3,   estadoFiesta ? HIGH : LOW);

    digitalWrite(COCINA_L1, estadoFiesta ? HIGH : LOW);
    digitalWrite(COCINA_L2, estadoFiesta ? HIGH : LOW);
    digitalWrite(COCINA_L3, estadoFiesta ? HIGH : LOW);

    digitalWrite(HAB_L1,    estadoFiesta ? HIGH : LOW);
    digitalWrite(HAB_L2,    estadoFiesta ? HIGH : LOW);
    digitalWrite(HAB_L3,    estadoFiesta ? HIGH : LOW);

    digitalWrite(COMEDOR_L1, estadoFiesta ? LOW : HIGH);
    digitalWrite(COMEDOR_L2, estadoFiesta ? LOW : HIGH);
    digitalWrite(COMEDOR_L3, estadoFiesta ? LOW : HIGH);

    digitalWrite(BANO_L1,    estadoFiesta ? LOW : HIGH);
    digitalWrite(BANO_L2,    estadoFiesta ? LOW : HIGH);
    digitalWrite(BANO_L3,    estadoFiesta ? LOW : HIGH);
  }
}

void encenderTodosLEDs() {
  for (int i = 0; i < TOTAL_LEDS; i++) {
    digitalWrite(LEDS_AMBIENTES[i], HIGH);
  }
}

void apagarTodosLEDs() {
  for (int i = 0; i < TOTAL_LEDS; i++) {
    digitalWrite(LEDS_AMBIENTES[i], LOW);
  }
}

void parpadearVerde() {
  digitalWrite(LED_AZUL, LOW);
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_VERDE, HIGH);
    delay(200);
    digitalWrite(LED_VERDE, LOW);
    delay(200);
  }
  digitalWrite(LED_AZUL, HIGH);
}

void mostrarError() {
  digitalWrite(LED_AZUL, LOW);
  for (int i = 0; i < 4; i++) {
    digitalWrite(LED_ROJO, HIGH);
    delay(200);
    digitalWrite(LED_ROJO, LOW);
    delay(200);
  }
  digitalWrite(LED_AZUL, HIGH);
}