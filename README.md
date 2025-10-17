# GPS
# Sistema GPS IoT con ESP32  
Proyecto desarrollado en Visual Studio Code (PlatformIO / Arduino)

## Descripción General
Este proyecto implementa un sistema IoT basado en ESP32 integrado con un módulo GPS (GNSS) para obtener y transmitir datos geoespaciales en tiempo real. El dispositivo captura y muestra información como latitud, longitud, altitud, velocidad, número de satélites, calidad de la señal y hora UTC. Además, permite enviar esta información a un servicio en la nube mediante Wi-Fi.

El objetivo es demostrar la integración entre hardware embebido, sensores de geolocalización y comunicación con servicios externos mediante protocolos HTTP o MQTT.

---

## Objetivos del Proyecto

### Objetivo General
Desarrollar un dispositivo IoT que obtenga datos de geolocalización desde un módulo GPS y los transmita hacia una plataforma en la nube para su monitoreo o análisis.

### Objetivos Específicos
- Capturar datos GPS y procesarlos en tiempo real.
- Visualizar la información en un display OLED.
- Enviar lecturas mediante Wi-Fi a un endpoint o servicio en la nube.
- Documentar y estructurar el sistema para su réplica y mantenimiento.

---

## Funcionalidades Implementadas
- Lectura de tramas NMEA mediante UART.
- Extracción de:
  - Latitud y Longitud (grados decimales)
  - Altitud (m)
  - Velocidad (m/s y km/h)
  - Rumbo o dirección (°)
  - Fecha y hora UTC del fix
  - Número de satélites en uso
  - Calidad del fix (2D/3D)
  - HDOP (precisión horizontal)
- Visualización en pantalla OLED (SSD1306)
- Envío de datos en formato JSON a la nube
- Reintento automático de conexión Wi-Fi
- Posibilidad de recibir comandos desde la nube (opcional)

---

## Componentes del Sistema

### Hardware
- ESP32 DevKit
- Módulo GPS GNSS (u-blox NEO-6M / NEO-M8N o similar)
- Display OLED SSD1306 (I2C)
- Antena GPS y cables Dupont
- Fuente de 5V o alimentación USB

### Software y Librerías
| Componente | Uso |
|------------|-----------------------------|
| VSCode + PlatformIO | Desarrollo y carga de firmware |
| TinyGPS++ | Parseo de datos NMEA |
| WiFi.h / HTTPClient.h | Conexión Wi-Fi y envío a servicios |
| ArduinoJson | Construcción de payload JSON |
| Adafruit_SSD1306 | Visualización en OLED |
| Wire.h (I2C) | Comunicación con pantalla |

---

## Diagrama de Conexión (Resumen)

| Componente | ESP32 |
|------------|-----------------|
| GPS TX | GPIO16 (RX2) |
| GPS RX | GPIO17 (TX2) |
| GPS VCC | 3.3V o 5V |
| GPS GND | GND |
| OLED SDA | GPIO21 |
| OLED SCL | GPIO22 |
| OLED VCC | 3.3V |
| OLED GND | GND |

---

## Estructura del Repositorio

gps-esp32-project/
├── README.md
├── firmware/
│ ├── src/
│ │ ├── main.cpp
│ │ ├── gps_handler.cpp
│ │ ├── display.cpp
│ │ ├── wifi_client.cpp
│ │ └── config.h.example
│ ├── lib/
│ └── platformio.ini
├── docs/
│ ├── circuit.png
│ ├── demo.mp4
│ └── screenshots/
└── tests/

yaml
Copiar código

---

## Instalación y Configuración

### 1. Clonar repositorio
```bash
git clone <url-del-repositorio>
cd gps-esp32-project/firmware
2. Configurar Wi-Fi y Endpoint
Crear config.h basado en config.h.example:

c
Copiar código
#define WIFI_SSID   "TU_SSID"
#define WIFI_PASS   "TU_PASSWORD"
#define CLOUD_ENDPOINT "https://tu-backend.com/api/gps"
#define SEND_INTERVAL_MS 60000

#define GPS_UART_NUM 2
#define GPS_RX_PIN 16
#define GPS_TX_PIN 17
3. Compilar y Cargar
bash
Copiar código
pio run -t upload
Flujo del Firmware
Inicialización de Serial, GPS, Wi-Fi y OLED.

Intento de conexión Wi-Fi.

Lectura continua de datos NMEA desde GPS.

Construcción de JSON con datos válidos.

Envío a la nube cada SEND_INTERVAL_MS.

Muestreo concurrente en OLED y Serial Monitor.

Ejemplo de Payload JSON
json
Copiar código
{
  "device_id": "ESP32-GPS-01",
  "timestamp_utc": "2025-10-14T12:34:56Z",
  "gps": {
    "fix": true,
    "latitude": -4.598056,
    "longitude": -74.075833,
    "altitude_m": 2567.3,
    "speed_kmh": 12.4,
    "course_deg": 87.5,
    "num_satellites": 7,
    "hdop": 0.9,
    "fix_type": "3D"
  }
}
Pruebas Realizadas
Prueba	Resultado
GPS sin señal	Reporte fix=false
Reconexión Wi-Fi	Reintento automático
Envío HTTP	Payload aceptado
Visual OLED	Cambio de pantallas correcto

Problemas y Soluciones
Problema	Solución
Pérdida de señal GPS	Antena externa / exterior
Errores TLS al enviar	Uso de HTTP o CA correcta
Bajo heap con JSON	StaticJsonDocument optimizado

Mejoras Futuras
Soporte MQTT para comandos remotos

Dashboard en tiempo real con mapa

Almacenamiento local (LittleFS)

OTA (actualización remota)

Créditos
Autor: [Tu Nombre] — Ingeniero de Sistemas
Librerías utilizadas: TinyGPS++, ArduinoJson, Adafruit_SSD1306, Espressif Arduino Core
Licencia: MIT (o seleccionar)
