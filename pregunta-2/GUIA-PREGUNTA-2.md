# Guía – Pregunta 2 (15 puntos)

## Qué piden

> Cree un nuevo proyecto en Wokwi y desarrolle un prototipo que satisfaga los requisitos planteados.

Se califica con 4 criterios:

| Criterio | Puntos | Qué revisan |
|---|---|---|
| C02 IoT Device Design | 5 | `diagram.json`: **todos** los componentes pedidos, bien conectados |
| C03 Software Features | 5 | Cada regla del enunciado funciona, con los valores correctos, **y se manejan las excepciones** |
| C04 Quality Attributes | 3 | Programación orientada a objetos con ModestIoT, title blocks / comentarios, constantes, sin redundancia, patrones, technical constraints |
| C05 Naming standards | 2 | Todo en inglés: clases en `PascalCase`, métodos y variables en `camelCase`, constantes en `UPPER_SNAKE_CASE` |

---

## Paso 1: subrayar estas palabras en el enunciado

| Busca en el texto... | Por qué | Se convierte en... |
|---|---|---|
| **"basado en ESP32" / "Arduino Uno"** | Cambia el ADC (4095 o 1023), la librería del servo, si hay WiFi y si `printf("%f")` funciona | El tipo de placa en `diagram.json` |
| **Nombres de sensores** (HC-SR04, DHT22, PIR, HX711, potentiometer, MQ2...) | Cada sensor es una clase que hereda de `Sensor` | Una clase en `componentes/` + un part en el diagrama |
| **Nombres de actuadores** (LED, RGB LED, servo, buzzer, relay, LCD 16x2 / 20x4) | Cada actuador es una clase que hereda de `Actuator` | Una clase en `componentes/` + un part en el diagrama |
| **Números con unidad** (≤ 50 cm, > 80 kg, < 25 %, 1–20 °C, 4–16 L/min, 96 min) | Son las reglas que se prueban en C03 y no pueden ser magic numbers (C04) | `static constexpr` en el Device |
| **Estados entre comillas o en mayúsculas** ("FREE", "Warm-Up", Bajo / Normal / Alto) | Se muestran en el display y en consola | `enum class` + constantes de texto |
| **"si ... entonces ..."**, "cuando", "hasta que" | Son las transiciones de estado | Métodos `handleX()` / `evaluateState()` |
| **"de forma remota", "desde la app", "forzar"** | Necesitas una entrada de comandos | `SerialCommandReceiver` (se simula con el Serial Monitor) |
| **"WiFi", "REST", "HTTP POST", "JSON", "nube"** | Conectividad (solo ESP32) | `RestClient` / WiFi `Wokwi-GUEST` |
| **"intermitente", "parpadear"** | Requiere tiempo sin usar `delay()` | `Led::TOGGLE` dentro de `update()` con `millis()` |
| **"Anexo" con fórmula** | El cálculo vale puntos en C03 | Un método de cálculo con el nombre de la fórmula |
| **Technical constraints**: Doxygen o title blocks, `libraries.txt` exacto, `loop` inhibido, nombre del zip | Cumplir cada uno vale en C04 | Checklist final |

## Paso 2: archivos que van SÍ O SÍ y por qué

| Archivo | ¿Obligatorio? | Por qué |
|---|---|---|
| `sketch.ino` | **Sí** | Punto de entrada de Wokwi. El constraint pide que **solo** cree el Device y llame `begin()` en `setup()` y `update()` en `loop()` |
| `diagram.json` | **Sí** | Es el circuito; sin él no hay nota en C02 |
| `libraries.txt` | **Sí** (si usas librerías) | Wokwi instala las librerías desde aquí. Si el enunciado da la lista, **cópiala exacta** |
| `wokwi-project.txt` | Recomendado | Lo crea la exportación de Wokwi; identifica el proyecto |
| `ModestIoT.h` / `ModestIoT.cpp` | **Sí** | El constraint dice "basado en ModestIoT": `Event`, `Command`, `Sensor`, `Actuator`, `Device` |
| `<Producto>Device.h/.cpp` | **Sí** | El Device del caso: es el **mediador** que tiene las reglas del negocio |
| Una clase `.h/.cpp` por cada tipo de sensor y actuador | **Sí** | "Crear clases y objetos para representar sensores y actuadores" |

> Si el enunciado trae `libraries.txt` con `ModestIoT@wokwi:<hash>` (caso Libelium), usa esa librería
> en vez de `ModestIoT.h/.cpp` locales y **no repitas** nombres de clases que ya trae la librería.

## Paso 3: armar el proyecto (sobre `codigo-base/`)

1. Crea un proyecto nuevo en Wokwi (ESP32 o Arduino Uno, según el enunciado).
2. Sube los archivos de `codigo-base/`. Así ya compila y tienes la estructura.
3. Por cada sensor o actuador del enunciado, copia su clase desde `componentes/` (ver `componentes/README.md`).
4. Renombra `MyIoTDevice` con el nombre del producto (por ejemplo, `SmartTankDevice`).
5. En el Device:
   - agrega un campo por pin en el struct `...Wiring`,
   - declara los sensores y actuadores como **miembros** (composición) y pásales `this`,
   - pasa **todos los números** del enunciado a constantes,
   - escribe el `enum class` de estados,
   - en `update()` consulta cada sensor con su intervalo (DHT22: 2 s o más),
   - en `on(Event)` identifica el sensor por `event.sourceId` (su pin) y aplica la regla,
   - en `applyState()` / `refreshDisplay()` actualiza LEDs, servo y LCD.
6. Completa `printCompanyInformation()` con la empresa y **tus nombres** (lo pide el constraint).
7. Agrega las partes al `diagram.json` y conéctalas. La tabla de pines está en `codigo-base/PASO-A-PASO.md`.
8. Prueba **cada regla** y **cada excepción**: sensor sin lectura, comando inválido, valores en el límite exacto.

## Paso 4: excepciones que suman en C03

- Sensor sin lectura (DHT da NaN, HC-SR04 sin eco, HX711 no listo): conservar el último valor válido y avisar en consola.
- Comando remoto desconocido o que no aplica en el estado actual (por ejemplo, reservar una plaza ocupada).
- División entre cero (por ejemplo, flujo total 0 en una fórmula de promedio ponderado).
- Valores en el límite: decide `<` o `<=` según el texto ("50 cm **o menos**" es `<=`; "**supera** 96" es `>`).

## Diferencias ESP32 / Arduino Uno

| | ESP32 | Arduino Uno |
|---|---|---|
| ADC máximo | 4095 | 1023 |
| Servo | librería `ESP32Servo` | `#include <Servo.h>` (incluida) |
| `printf` / `%f` | `Serial.printf` funciona | No hay `Serial.printf`; usa `print()` y `dtostrf()` para floats |
| WiFi / REST | Sí (`Wokwi-GUEST`, sin contraseña) | No |
| Pines I2C (LCD) | SDA 21, SCL 22 | SDA A4, SCL A5 |
| Placa en el diagrama | `board-esp32-devkit-c-v4` | `wokwi-arduino-uno` |

## Checklist antes de entregar

- [ ] Todos los componentes del enunciado están en `diagram.json` y conectados.
- [ ] Cada regla y umbral del enunciado funciona (pruébalo moviendo los sensores en Wokwi).
- [ ] La consola muestra empresa, producto y **tus nombres y apellidos** al iniciar.
- [ ] `sketch.ino` solo crea el Device y llama `begin()` / `update()` (o inhibe `loop` si el enunciado lo pide).
- [ ] No hay magic numbers ni magic strings: todo está en constantes.
- [ ] Hay comentarios en inglés con el formato que pide el enunciado (title blocks o Doxygen) en **todos** los archivos.
- [ ] Todos los nombres están en inglés y siguen las convenciones.
- [ ] `libraries.txt` es exacto si el enunciado lo da.
- [ ] El zip exportado de Wokwi tiene el **nombre exacto** que pide el enunciado.
- [ ] El PPT tiene el nombre que se pide y la respuesta está en la hoja correcta.
