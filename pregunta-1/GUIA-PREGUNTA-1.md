# Guía – Pregunta 1 (5 puntos)

## Qué piden exactamente

> Bajo el marco de los 12 pasos del *IoT System Design Steps*, elabore el **Definition of application
> service layer requirements**. Considere a) **Definición de la interfaz de usuario para cada servicio
> prestado** (móvil, tablet, desktop o más de un tipo).

Es el **paso 6** de los 12 pasos. La redacción es **idéntica en todos los exámenes**; solo cambia el caso.

La rúbrica (C01) da el puntaje completo cuando:
- están **todos** los servicios del caso,
- cada servicio tiene **su interfaz de usuario** definida,
- y todo está **alineado con el caso** (usa sus datos, no algo genérico).

Por eso la respuesta se arma como una lista de: **Servicio → Usuario → Tipo de interfaz → Qué muestra o hace**.

---

## Paso 1: subrayar estas palabras en el caso

| Busca en el texto... | Por qué | Se convierte en... |
|---|---|---|
| **Usuarios / roles**: cliente, paciente, conductor, médico, operador, municipalidad, instalador, profesional, técnico, administrador, *Device Maker* | Cada servicio tiene un usuario; sin usuario no se sabe qué interfaz conviene | La columna **Usuario** |
| **Verbos de servicio**: *monitorear, observar, controlar, forzar, reservar, recibir notificaciones, configurar thresholds, registrar, hacer seguimiento, tracking* | Cada verbo es un servicio que la aplicación presta | Un **Servicio** (una fila) |
| **Canales**: *app móvil, Google Play, App Store, Web Console, portal, plataforma en la nube, dashboard* | El caso ya dice qué interfaz existe; usarla te alinea con el caso | La columna **Interfaz** |
| **Datos del dispositivo**: temperatura, nivel %, peso, PPM, distancia, flujo L/min, estado | La interfaz debe mostrar esos datos concretos | La columna **Qué muestra o hace** |
| **Números y umbrales**: 80 kg, 60–100 PPM, 25 % / 95 %, 96 min, 1–20 °C | Muestran que la respuesta está alineada con el caso (criterio C01) | Alertas y configuración |
| **Estados**: FREE / BUSY, Inactive / Warm-Up / Active, Bajo / Normal / Alto | La app debe mostrar el mismo estado que el dispositivo | Pantalla de monitoreo |
| **Acciones remotas**: "desde la app puede forzar...", "reservada de forma remota" | Es un servicio de **control** (no solo de lectura) | Servicio de control remoto |
| **Seguridad / datos sensibles**: salud, pagos, agua potable | Agregan requisitos de autenticación y cifrado | Requisitos generales |

## Paso 2: elegir el tipo de interfaz (y justificarlo)

| Interfaz | Úsala cuando... | Ejemplos |
|---|---|---|
| **Móvil** (Android / iOS) | El usuario está en movimiento, necesita notificaciones push o actúa en el momento | Conductor que reserva, paciente que ve su peso, cliente que abre una válvula |
| **Tablet** | Trabajo en campo, rondas o inspección con más pantalla que un celular | Inspector municipal, técnico instalador, médico en visitas |
| **Desktop (web)** | Gestión, muchos dispositivos a la vez, reportes, configuración masiva | Operador municipal, centro de monitoreo, administrador, Device Maker |
| **Más de una** | Un mismo servicio lo usan roles distintos | Historial: gráficos en móvil y reportes PDF en desktop |
| **Sin interfaz gráfica (API)** | Integración máquina a máquina | Integradores, endpoint REST |

## Paso 3: servicios que casi siempre aparecen

Revisa cuáles aplican a tu caso (lo normal es tener entre 5 y 7):

1. **Monitoreo en tiempo real** de los datos y del estado del dispositivo.
2. **Notificaciones / alertas** cuando se cruza un umbral.
3. **Control remoto** (si el caso lo menciona).
4. **Configuración** de umbrales, perfiles y horarios.
5. **Historial y reportes** (consumo, tendencias).
6. **Gestión de dispositivos**: alta, estado, firmware, mantenimiento (Device Maker, técnico).
7. **Portal para profesionales / integradores** (si el caso nombra un portal o una API).

Al final, agrega **requisitos generales**: autenticación por rol, HTTPS/TLS, latencia
(por ejemplo, menos de 5 s), disponibilidad e idioma.

## Paso 4: escribir cada servicio con esta fórmula

```
N. <Servicio> – Usuario: <rol> – Interfaz: <móvil / tablet / desktop> – <qué muestra o hace, con datos y umbrales del caso>.
```

Ejemplo (caso AquaSense):

> 3. Control remoto de la válvula – Usuario: cliente – Interfaz: app móvil – botones Abrir, Cerrar y
> Automático; el comando se envía sin importar la lectura del sensor y se muestra la confirmación.

## Errores que quitan puntos

- Servicios genéricos sin datos del caso (por ejemplo, "ver información").
- Poner la interfaz sin decir para qué usuario es.
- Olvidar servicios que el caso menciona de forma explícita (control remoto, tracking del Device Maker, portal Pro).
- Describir el hardware en lugar de la aplicación (eso es otra capa).
- Poner el PPT en otra hoja de la plantilla: va en la hoja de respuesta que indica el título.

## Ejemplos en los 4 casos

| Caso | Palabras clave encontradas | Servicios resultantes |
|---|---|---|
| Libelium Smart Parking | conductor, app, reservar de forma remota, municipalidad, Web Console, inspector (sobretiempo), Device Maker, portal de clientes | Disponibilidad, reserva, tiempo y alertas, fiscalización, gestión municipal, tracking, integración |
| Smart Lifemonitor | paciente, ritmo cardiaco, peso, enfermedades crónicas, alertas, médico | Mediciones, alertas, seguimiento clínico, centro 24/7, familiar, soporte |
| AquaSense | app móvil, nivel en tiempo real, notificaciones, forzar apertura o cierre, servidor REST | Monitoreo, alertas, control remoto, historial, gestión de tanques |
| Moen Nebia | app Smart Water Network, thresholds de temperatura y flujo, fugas, portal Moen Pro | Monitoreo, configuración, notificaciones, consumo, soporte a instaladores |

Las respuestas completas están en los PPT de `../ejemplos-resueltos/`.
