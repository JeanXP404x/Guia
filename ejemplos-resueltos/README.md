# Ejemplos resueltos

Cada carpeta es un proyecto Wokwi completo: se importa subiendo sus archivos a un proyecto nuevo.
Fueron compilados y probados en la máquina con stubs de Arduino; **pruébalos en Wokwi antes de usarlos**.

| Carpeta | Caso | Placa | Qué muestra |
|---|---|---|---|
| `01-libelium-smart-parking-esp32` | Libelium Smart Parking Spot S1 | ESP32 | Usa la **librería** ModestIoT de Wokwi (`libraries.txt` con hash), Doxygen, `loop` inhibido, máquina de estados (FREE / BUSY / OVERTIME / RESERVED), comandos remotos, tracking en JSON |
| `02-smart-lifewatching-arduino-uno` | Smart Lifemonitor – Smart LifeWatching | Arduino Uno | ModestIoT local, HX711 sin librería, LCD 20x4, `dtostrf` para floats en el Uno |
| `03-aquasense-smart-tank-esp32` | AquaSense Smart Tank Controller | ESP32 | WiFi + HTTP POST con JSON, modo automático / manual, comandos remotos |
| `04-moen-nebia-spa-shower-esp32` | Moen Nebia Spa Shower | ESP32 | 2 DHT22 + 2 potenciómetros, fórmula del anexo (promedio ponderado), estados Inactive / Warm-Up / Active |

## Cómo usarlos para estudiar

1. Lee el enunciado del caso y subraya las palabras clave (ver las guías de la pregunta 1 y la pregunta 2).
2. Abre `*Device.h`: las constantes deben coincidir con los números del enunciado.
3. Sigue el flujo `update()` → `Sensor::on()` → `Device::on()` → actuadores.
4. Ejecútalo en Wokwi y cambia los valores de los sensores para ver cada transición en la consola.

> Antes de entregar cualquier proyecto: cambia `DEVELOPER_NAME` por tus nombres y renombra el zip
> según el enunciado.
