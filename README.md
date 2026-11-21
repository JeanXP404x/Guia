# Guía
Guía para resolver la. Todas las versiones del examen
(Libelium, AquaSense, Smart Lifemonitor, Moen...) siguen la misma plantilla:

| Pregunta | Qué piden | Puntos | Qué se entrega |
|---|---|---|---|
| 1 | *Definition of application service layer requirements*: interfaz de usuario por cada servicio | 5 | PPT (`upc-pre-<periodo>-1asi0572-pc2-file_v1.pptx`) |
| 2 | Prototipo en Wokwi con C++ orientado a objetos y basado en ModestIoT | 15 | `.zip` exportado de Wokwi |

## Estructura del repositorio

```
guia-pc2-iot/
├── README.md                      <- este archivo
├── pregunta-1/
│   ├── GUIA-PREGUNTA-1.md         <- cómo encontrar las palabras clave del caso y armar la respuesta
│   └── PLANTILLA-RESPUESTA.md     <- plantilla para copiar al PPT
├── pregunta-2/
│   ├── GUIA-PREGUNTA-2.md         <- cómo leer el enunciado, qué archivos van siempre, checklist
│   ├── codigo-base/               <- proyecto Wokwi mínimo que compila (punto de partida)
│   │   └── PASO-A-PASO.md         <- explicación archivo por archivo
│   └── componentes/               <- clases listas: sensores y actuadores para copiar
│       └── README.md              <- qué componente usar según la palabra del enunciado
└── ejemplos-resueltos/            <- los 4 exámenes resueltos
    └── README.md
```

## Orden recomendado en el examen (100 min)

1. **10 min**: leer el caso y subrayar palabras clave (ver las guías de cada pregunta).
2. **20 min**: Pregunta 1 (es la misma pregunta en todos los exámenes; solo cambia el caso).
3. **60 min**: Pregunta 2, partiendo de `codigo-base/` y copiando lo que falte de `componentes/`.
4. **10 min**: renombrar archivos, exportar el zip de Wokwi y revisar el checklist.
https://wokwi.com/projects/481259834282887169

