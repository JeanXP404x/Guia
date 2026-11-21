# Código base – explicación paso a paso

Este proyecto **compila y funciona tal cual** en Wokwi (ESP32). Hace lo siguiente: un potenciómetro da un
valor de 0 a 100; si el valor pasa de 80 se enciende un LED de alerta, y el LCD muestra el valor y el estado.
Sirve de molde: se reemplaza el ejemplo por los sensores, actuadores y reglas del enunciado.

## Archivos

```
codigo-base/
├── sketch.ino          1. punto de entrada (mínimo)
├── ModestIoT.h/.cpp    2. núcleo del framework (no se modifica)
├── MyIoTDevice.h/.cpp  3. el Device: aquí van TODAS las reglas del caso
├── AnalogSensor.h/.cpp 4. ejemplo de Sensor
├── Led.h/.cpp          5. ejemplo de Actuator
├── LcdDisplay.h/.cpp   6. ejemplo de Actuator con texto
├── diagram.json        7. circuito
├── libraries.txt       8. librerías
└── wokwi-project.txt   9. descripción del proyecto
```

---

## 1. `sketch.ino`: solo crea el Device

```cpp
const MyIoTDeviceWiring DEVICE_WIRING = { 34, 2, 0x27 };  // pines en un solo lugar
MyIoTDevice device(DEVICE_WIRING);                         // UN objeto Device

void setup() { Serial.begin(SERIAL_BAUD_RATE); device.begin(); }
void loop()  { device.update(); }
```

**Por qué:** el constraint dice que `sketch.ino` debe enfocarse en crear el Device y llamar su comportamiento.
Aquí no va ninguna regla de negocio. Los pines van en un struct para no tener magic numbers sueltos.

> Caso Libelium: pide "no lógica en `loop`, solo inhibirlo". Con la librería ModestIoT de ESP32 se usa
> `vTaskDelay(pdMS_TO_TICKS(60000));` en `loop()` (ver el ejemplo resuelto 01).

## 2. `ModestIoT.h/.cpp`: el framework

| Clase | Rol | Patrón |
|---|---|---|
| `Event` | Lo que un sensor avisa (`identifier` + `sourceId` = pin) | Value Object |
| `Command` | La orden que recibe un actuador | Command |
| `EventHandler` | Interfaz: `on(Event)` | Observer |
| `CommandHandler` | Interfaz: `handle(Command)` | Command |
| `Sensor` | Base de las entradas. `on()` llama a `processEvent()` y, si no se suprime, avisa al Device | Template Method |
| `Actuator` | Base de las salidas. `handle()` llama a `executeCommand()` | Template Method |
| `Device` | Base del dispositivo: `begin()`, `update()`, `on()` | Mediator |

**Cómo fluye:**
```
update() --MEASURE_DATA_REQUESTED--> Sensor.on() --> processEvent() lee el hardware
                                                     |-- sin cambio o error: SUPPRESSED (no avisa)
                                                     |-- con cambio: DATA_READ(sourceId = pin) --> Device.on()
Device.on() --> aplica la regla --> actuador.handle(Command) --> executeCommand() mueve el hardware
```
Este archivo **no se modifica**: es igual en todos los exámenes.

## 3. `MyIoTDevice.h/.cpp`: el corazón del examen

Está marcado con `STEP 1` a `STEP 6`:

| Paso | Qué hacer con tu caso |
|---|---|
| STEP 1 `...Wiring` | Un campo por cada pin del diagrama |
| STEP 2 `enum class OperationState` | Los estados del enunciado (`Free, Busy, Reserved`...) |
| STEP 3 constantes | **Cada número del enunciado** (umbrales, rangos, tiempos, dimensiones del LCD) |
| STEP 4 miembros | Un objeto por sensor y actuador. Se les pasa `this` para que avisen al Device |
| STEP 5 estado | Variables internas (estado actual, marcas de tiempo `millis()`) |
| STEP 6 métodos | Un método por regla: `evaluateState()`, `applyState()`, `refreshDisplay()` |

Métodos clave:
- **`begin()`**: imprime la información de la empresa, inicializa el LCD, el servo y el DHT, y pinta el estado inicial.
- **`update()`**: consulta los sensores cada `SAMPLING_INTERVAL_MS` usando `millis()` (sin `delay()`).
  Si hay varios sensores, agrega una línea `sensorX.on(Event(...))` por cada uno, cada cual con su intervalo.
- **`on(Event)`**: identifica qué sensor avisó con `event.sourceId == sensorX.getPin()` y aplica la regla.
- **`applyState()`**: el único lugar que mueve las salidas según el estado (evita código repetido).
- **`printCompanyInformation()`**: empresa, producto y **tus nombres** (lo pide el constraint).

## 4–6. Ejemplos de Sensor y Actuator

- `AnalogSensor`: convierte el ADC a unidades reales y solo avisa si el valor cambió más que `changeTolerance`.
- `Led`: comandos ON, OFF y TOGGLE; el método `turn(bool)` hace el código más legible.
- `LcdDisplay`: buffer por línea; `setLine()` rellena con espacios y `refresh()` escribe todo de una vez.

Para otros componentes, copia la clase desde `../componentes/`.

## 7. `diagram.json`: tabla de conexiones

| Parte Wokwi (`type`) | Pines de la parte | ESP32 (sugerido) | Arduino Uno |
|---|---|---|---|
| `wokwi-hc-sr04` | VCC, TRIG, ECHO, GND | 5V, 5, 18, GND | 5V, 9, 10, GND |
| `wokwi-dht22` | VCC, SDA, GND | 3V3, 15 (o 4), GND | 5V, 7, GND |
| `wokwi-pir-motion-sensor` | VCC, OUT, GND | 5V, 19, GND | 5V, 2, GND |
| `wokwi-potentiometer` | VCC, SIG, GND | 3V3, 34 / 35, GND | 5V, A0, GND |
| `wokwi-hx711` (`type`: `50kg`) | VCC, DT, SCK, GND | 5V, 16, 17, GND | 5V, 3, 2, GND |
| `wokwi-led` + `wokwi-resistor` 220 Ω | A (con resistencia), C | 2, 25, 26, 27 → GND | 8–13 → GND |
| `wokwi-rgb-led` (`common`: `cathode`) | R, G, B, COM | 25, 26, 27, GND | 9, 10, 11, GND |
| `wokwi-servo` | V+, GND, PWM | 5V, GND, 13 | 5V, GND, 6 |
| `wokwi-buzzer` | 1, 2 | 23, GND | 8, GND |
| `wokwi-relay-module` | VCC, GND, IN | 5V, GND, 14 | 5V, GND, 4 |
| `wokwi-lcd1602` / `wokwi-lcd2004` (`pins`: `i2c`) | VCC, GND, SDA, SCL | 5V, GND, 21, 22 | 5V, GND, A4, A5 |

Agrega siempre las dos líneas del Serial Monitor (ESP32): `esp:TX → $serialMonitor:RX` y `esp:RX → $serialMonitor:TX`.

## 8. `libraries.txt`

| Si usas... | Agrega la línea |
|---|---|
| LCD I2C | `LiquidCrystal I2C` |
| DHT22 | `DHT sensor library` y `Adafruit Unified Sensor` |
| Servo en ESP32 | `ESP32Servo` |
| JSON | `ArduinoJson` |
| MQTT | `PubSubClient` |

Si el enunciado da la lista, **cópiala exacta** aunque no uses todas las librerías.

## Cómo adaptar el base en 5 minutos (ejemplo AquaSense)

1. Copia `UltrasonicSensor` y `ServoActuator` desde `componentes/`, y borra `AnalogSensor`, `Led` y `LcdDisplay` si no se usan.
2. Renombra `MyIoTDevice` a `SmartTankDevice`.
3. Constantes: `OPEN_BELOW_PERCENTAGE = 25`, `CLOSE_ABOVE_PERCENTAGE = 95`, `TANK_DEPTH_CM = 400`.
4. En `on()`: calcula el nivel % y abre o cierra la válvula.
5. Diagrama: HC-SR04 en 5/18 y servo en 13.

Compáralo con `../../ejemplos-resueltos/03-aquasense-smart-tank-esp32/`.
