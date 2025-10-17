# Sistema GPS IoT con ESP32  
Proyecto desarrollado en Visual Studio Code (PlatformIO / Arduino Framework)

---

## 📌 Descripción General
Este proyecto implementa un sistema IoT utilizando un ESP32 integrado con un módulo GPS (GNSS). Su propósito es capturar datos geográficos en tiempo real y procesarlos para su visualización local y transmisión hacia la nube.  
El dispositivo obtiene latitud, longitud, altitud, velocidad, precisión y estado del fix satelital, mostrando dicha información en un display OLED y exportándola como JSON.

---

## 🎯 Objetivos del Proyecto

### Objetivo General
Desarrollar un sistema embebido capaz de obtener datos GPS y transmitirlos a una plataforma remota mediante Wi-Fi.

### Objetivos Específicos
- Procesar tramas NMEA desde un módulo GPS.
- Visualizar datos en un display OLED.
- Enviar información en formato JSON hacia un servicio en la nube.
- Documentar la arquitectura, pruebas y funcionamiento exitoso del sistema.

---

## ⚙️ Funcionalidades Implementadas
- Lectura por UART de GPS (TinyGPS++)  
- Obtención de:
  - Latitud / Longitud (grados decimales)  
  - Altitud (m)  
  - Velocidad (km/h)  
  - Rumbo (°)  
  - Fecha y hora UTC  
  - Satélites en uso  
  - HDOP (precisión) y tipo de fix (2D/3D)  
- Visualización dinámica en OLED SSD1306  
- Envío de datos vía Wi-Fi (HTTP REST o servidor local)  
- Manejo de reconexión Wi-Fi y control de errores  

---

## 🛑 Validación y Resultados del Funcionamiento

Durante las pruebas finales:

| Prueba | Resultado |
|--------|----------|
| Recepción de señal GPS | Coordenadas reales obtenidas con precisión |
| Visualización OLED | Datos mostrados correctamente y en tiempo real |
| Envío a la nube / servidor | Payload JSON recibido correctamente |
| Reconexión Wi-Fi | Reconexión automática sin reiniciar el sistema |

El sistema **funcionó correctamente**, validando la integración entre hardware, firmware y comunicación remota.

---

## 🧩 Componentes del Sistema

### Hardware
| Componente | Detalle |
|------------|---------|
| ESP32 DevKit | Microcontrolador principal |
| Módulo GPS (u-blox NEO-6M/M8N) | Fuente de datos GNSS |
| OLED SSD1306 (I2C) | Pantalla 128x64 |
| Antena GPS | Recepción satelital |
| Cableado Dupont | Conexiones |

### Software y Librerías
| Librería | Uso |
|----------|-----|
| TinyGPS++ | Parseo de tramas NMEA |
| WiFi.h / HTTPClient.h | Comunicación Wi-Fi y HTTP |
| Adafruit_SSD1306 / GFX | Display OLED |
| ArduinoJson | Construcción de JSON |
| PlatformIO / VSCode | Entorno de desarrollo |

---

## 🔌 Diagrama de Conexión (Resumen)

| GPS / OLED | ESP32 |
|------------|------|
| GPS TX | GPIO16 (RX2) |
| GPS RX | GPIO17 (TX2) |
| GPS VCC | 3.3V o 5V |
| GPS GND | GND |
| OLED SDA | GPIO21 |
| OLED SCL | GPIO22 |
| OLED VCC | 3.3V |
| OLED GND | GND |

---

## 📂 Estructura del Proyecto (VSCode)

/Proyecto_GPS_ESP32/
├── src/
│ ├── main.cpp
│ ├── gps_handler.cpp
│ ├── display.cpp
│ ├── wifi_client.cpp
│ └── config.h
├── include/
├── lib/
├── platformio.ini
├── docs/
│ ├── circuito.png
│ └── pruebas.mp4
└── README.md

yaml
Copiar código

---

## 🚀 Ejecución y Carga del Firmware

### 1️⃣ Configurar Wi-Fi y Endpoint en `config.h`
```c
#define WIFI_SSID       "TU_SSID"
#define WIFI_PASS       "TU_PASSWORD"
#define CLOUD_ENDPOINT  "http://tu-servidor.com/api/gps"
#define SEND_INTERVAL_MS 60000
2️⃣ Compilar y Cargar (desde VSCode con PlatformIO)
Conectar ESP32 por USB

Abrir proyecto en VSCode

Click en Build → Upload o usar:

makefile
Copiar código
PlatformIO: Upload
🔁 Flujo del Firmware
Inicialización de Serial, GPS, Wi-Fi y OLED

Intento de conexión Wi-Fi

Lectura continua de datos GPS (NMEA)

Generación de JSON con los datos obtenidos

Envío periódico (intervalo configurado)

Visualización en OLED y salida por Serial Monitor

🗂 Ejemplo de Payload JSON
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


---

🧪 Problemas y Soluciones
Problema	Solución
Pérdida de señal GPS	Uso en exteriores / antena externa
Error TLS en envío	Uso de HTTP o certificado compatible
Bajo heap con JSON	Uso de StaticJsonDocument

🔮 Mejoras Futuras
Integración con MQTT (sub/pub)

Dashboard web con mapa (Leaflet/Mapbox)

Almacenamiento con LittleFS

Actualizaciones OTA (Over-The-Air)



