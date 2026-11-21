# Componentes listos (copiar al proyecto)

Cada clase usa `ModestIoT.h` (incluido en esta carpeta). Copia el `.h` y el `.cpp` del componente que
necesites. Todas compilan; cambia solo el title block (`<Company> - <Your name>`).

## Qué componente usar según la palabra del enunciado

| Palabra en el enunciado | Clase | Evento / comando | Notas |
|---|---|---|---|
| HC-SR04, ultrasonic, distancia, nivel de agua, ocupación | `UltrasonicSensor` | `DATA_READ` → `getDistanceInCentimeters()` | Nivel % = (profundidad − distancia) / profundidad × 100 |
| DHT22, DHT11, temperatura, humedad | `DhtSensor` | `DATA_READ` → `getTemperatureCelsius()` | Llama a `begin()`; léelo cada 2 s o más |
| Potentiometer, gas (MQ2), luz (LDR), ritmo cardiaco simulado, válvula, flujo | `AnalogSensor` | `DATA_READ` → `getValue()` | ADC 4095 (ESP32) o 1023 (Uno) |
| PIR, motion, botón, pulsador, sensor de puerta | `DigitalInputSensor` | `ACTIVATED` (flanco de subida) | Botón: `activeLow = true` |
| "desde la app", "remoto", "forzar", comandos | `SerialCommandReceiver` | `COMMAND_RECEIVED` → `isCommand("OPEN")` | Se escribe en el Serial Monitor |
| LED | `Led` | `turn(bool)`, `TOGGLE` | `TOGGLE` sirve para parpadeo |
| RGB LED | `RgbLed` | `SHOW_RED / GREEN / BLUE`, `TURN_OFF` | Cátodo común |
| Servo, válvula, barrera, bolardo, puerta | `ServoActuator` | `moveTo(angle)` | Llama a `begin()`. En Uno usa `<Servo.h>` |
| LCD 16x2, LCD 20x4, display | `LcdDisplay` | `setLine()` + `refresh()` | `LcdDisplay(0x27, 16, 2, this)` |
| WiFi, REST, HTTP POST, JSON, servidor | `RestClient` | `connect()`, `postJson()` | Solo ESP32; `Wokwi-GUEST` sin contraseña |

## Cómo se conecta un componente al Device (siempre igual)

```cpp
// 1) .h del Device: miembro
UltrasonicSensor levelSensor;

// 2) constructor del Device: pines + this (para que avise al Device)
levelSensor(wiring.triggerPin, wiring.echoPin, this),

// 3) update(): pedir lectura
levelSensor.on(Event(Sensor::MEASURE_DATA_REQUESTED_EVENT_IDENTIFIER));

// 4) on(Event): identificar quién avisó y aplicar la regla
if (event.identifier == Sensor::DATA_READ_EVENT_IDENTIFIER && event.sourceId == levelSensor.getPin()) { ... }
```

## Cómo crear un componente que no está aquí (por ejemplo, un buzzer)

1. Copia `Led.h/.cpp` y renombra la clase a `Buzzer`.
2. Cambia los identificadores de comando a un rango libre (por ejemplo, 250, 251...).
3. En `executeCommand()` usa el hardware correspondiente (`tone(pin, frecuencia)` / `noTone(pin)`).
4. Si es un sensor nuevo, copia `AnalogSensor` o `DigitalInputSensor` y cambia `processEvent()`.

Rangos de identificadores usados: eventos 20–21 (Sensor), 30 (DigitalInput), 40 (comandos remotos);
comandos 100–102 (Led), 110–113 (RgbLed), 120 (Servo), 200 (LCD).
