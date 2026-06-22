# SmartHome GT - Sistema de Control Inteligente

Proyecto desarrollado para el curso de **Organización Computacional** de la **Facultad de Ingeniería, Universidad de San Carlos de Guatemala**.

## Descripción

SmartHome GT es una maqueta funcional de casa inteligente que integra control de iluminación por ambientes, ventilación, visualización en pantalla LCD, almacenamiento persistente en EEPROM y comunicación serial/Bluetooth para la activación de modos predefinidos y personalizados.

El sistema permite cargar configuraciones desde un archivo `.org` enviado desde una computadora, interpretar los comandos, almacenarlos en la EEPROM del Arduino y posteriormente activarlos de forma remota mediante Bluetooth.

## Objetivo general

Diseñar e implementar una maqueta funcional de una casa inteligente que permita configurar y activar escenas luminosas predefinidas mediante un archivo `.org`, almacenadas en memoria EEPROM del Arduino, y controlar dispositivos como luces y un ventilador mediante comandos locales o inalámbricos.

## Objetivos específicos

- Implementar almacenamiento persistente de escenas en la EEPROM interna del Arduino.
- Diseñar una interfaz de carga de configuraciones desde archivo `.org`.
- Integrar control remoto por Bluetooth.
- Mostrar retroalimentación en LCD y LEDs de estado.
- Simular un entorno domótico con LEDs, motor DC y servomotor.

---

## Características implementadas

- Control de 5 ambientes:
  - Sala
  - Comedor
  - Cocina
  - Baño
  - Habitación
- Modos predefinidos:
  - `modo_fiesta`
  - `modo_relajado`
  - `modo_noche`
  - `encender_todo`
  - `apagar_todo`
- Carga de configuración desde archivo `.org`
- Almacenamiento en EEPROM
- LCD I2C 16x2 para mostrar estado
- Bluetooth HC-06/HC-05 para control remoto
- LEDs de estado:
  - Azul: sistema activo
  - Verde: operación exitosa
  - Rojo: error
- Base para modos personalizados
- Base para integración de ventilador y puerta

---

## Arquitectura general del sistema

El sistema se divide en los siguientes módulos:

1. **Matriz de iluminación**
   - LEDs distribuidos por ambientes.

2. **Módulo de control central**
   - Arduino Mega 2560.
   - Lee comandos por USB serial y Bluetooth.
   - Escribe y lee configuraciones desde EEPROM.

3. **Pantalla LCD I2C**
   - Muestra estado del modo actual y errores.

4. **Módulo Bluetooth**
   - Permite activar modos desde el celular.

5. **Ventilador (motor DC)**
   - Simula ventilación automática según el modo activo.

6. **Puerta automática (servomotor)**
   - Abre/cierra mediante botón físico.

---

## Componentes utilizados

- Arduino Mega 2560
- Pantalla LCD I2C 16x2
- Módulo Bluetooth HC-06
- LEDs para ambientes
- 3 LEDs de estado
- Resistencias
- Motor DC
- Transistor NPN 2N2222
- Servomotor
- Push button
- Protoboard
- Cables Dupont
- Fuente USB / alimentación

---

## Conexiones principales

### LCD I2C
| LCD | Arduino Mega |
|---|---|
| VCC | 5V |
| GND | GND |
| SDA | 20 |
| SCL | 21 |

### Bluetooth HC-06
| HC-06 | Arduino Mega |
|---|---|
| VCC | 5V |
| GND | GND |
| TX | RX1 (19) |
| RX | TX1 (18) |

### LEDs de estado
| LED | Pin |
|---|---|
| Azul | 28 |
| Verde | 29 |
| Rojo | 30 |

### LEDs de ambientes
| Ambiente | Pines |
|---|---|
| Sala | 2, 3, 4 |
| Comedor | 5, 6, 7 |
| Cocina | 8, 9, 10 |
| Baño | 22, 23, 24 |
| Habitación | 25, 26, 27 |

---

## Estructura del proyecto

```text
Proyecto/
├── .env
├── .gitignore
├── config.py
├── requeriments.txt
├── script_CI.py
├── test_on.org
├── test_off.org
├── test_fiesta.org
├── test_simple.org
├── README.md
└── casa_inteligente_leds/
    └── casa_inteligente_leds.ino
```

---

## Requisitos de software

- Python 3
- Arduino IDE
- Librerías Python:
  - `pyserial`
  - `python-dotenv`
- Librerías Arduino:
  - `Wire`
  - `EEPROM`
  - `LiquidCrystal_I2C`

---

## Instalación del entorno

### Crear entorno virtual
```bash
python3 -m venv .venv
```

### Activarlo
```bash
source .venv/bin/activate
```

### Instalar dependencias
```bash
pip install -r requeriments.txt
```

---

## Archivo `.env`

```env
BAUD_RATE=9600
```

---

## Formato del archivo `.org`

El sistema espera archivos `.org` con inicio y fin explícitos.

### Ejemplo mínimo
```org
conf_ini
encender_todo
conf:fin
```

### Ejemplo extendido
```org
// Configuracion solo LEDs
conf_ini

modo_fiesta
LED'S: Alternandose

modo_relajado
LED'S: OFF

modo_noche
LED'S: OFF

encender_todo
LED'S: ON

apagar_todo
LED'S: OFF

conf:fin
```

### Ejemplo de modo personalizado esperado
```org
conf_ini

modo_custom: "Cena"
Ventilador: OFF
LED'S: sala:ON, comedor:ON, cocina:OFF, baño:OFF, habitacion:OFF

conf:fin
```

---

## Proceso de carga

1. El archivo `.org` se envía por USB serial usando `script_CI.py`.
2. Arduino procesa el archivo línea por línea.
3. Se validan los comandos.
4. Se almacenan en EEPROM.
5. El LED verde parpadea tres veces si la operación es exitosa.
6. El LCD muestra `Configuracion guardada`.

---

## Comandos soportados actualmente

### Por USB / `.org`
- `modo_fiesta`
- `modo_relajado`
- `modo_noche`
- `encender_todo`
- `apagar_todo`

### Por Bluetooth
- `modo_fiesta`
- `modo_relajado`
- `modo_noche`
- `encender_todo`
- `apagar_todo`
- `estado`

### Pendientes por completar
- `modo_custom_1`
- `modo_custom_2`
- integración total de ventilador
- integración total de servomotor

---

## Tabla preliminar de EEPROM

| Dirección EEPROM | Uso |
|---|---|
| 50 | Modo actual |
| 60+ | Reservado para modo personalizado 1 |
| 90+ | Reservado para modo personalizado 2 |


---

## Pruebas realizadas

### Prueba 1 - Encender todo
Archivo:
```org
conf_ini
encender_todo
conf:fin
```

Resultado esperado:
- Todos los LEDs encendidos
- LCD muestra `LEDs ON / Todo encendido`

### Prueba 2 - Apagar todo
Archivo:
```org
conf_ini
apagar_todo
conf:fin
```

Resultado esperado:
- Todos los LEDs apagados
- LCD muestra `LEDs OFF / Todo apagado`

### Prueba 3 - Modo fiesta
Archivo:
```org
conf_ini
modo_fiesta
conf:fin
```

Resultado esperado:
- LEDs alternándose
- LCD muestra `Modo: FIESTA / Alternando`

---

## Problemas encontrados y soluciones

### Error: `No module named 'serial'`
Solución:
```bash
pip install -r requeriments.txt
```

### Error: `could not open port`
Solución:
- verificar que Arduino esté conectado
- cerrar Serial Monitor
- usar autodetección de puerto en el script

### Error LCD I2C
Solución:
- instalar `LiquidCrystal_I2C`
- probar dirección `0x27` o `0x3F`
- ajustar contraste del módulo

---



## Presupuesto estimado
> Completar según compra real de componentes.

| Componente | Cantidad | Precio aproximado |
|---|---:|---:|
| Arduino Mega 2560 | 1 | Q500 |
| LCD I2C 16x2 | 1 | Q124 |
| Bluetooth HC-06 | 1 | Q70 |
| LEDs | varios | Q25 |
| Resistencias | varias | Q30 |
| Servomotor | 1 | Q31 |
| Motor DC | 1 | Q45 |
| Protoboard y cables | varios | Q50 |
|**TOTAL**|Q875|

---



