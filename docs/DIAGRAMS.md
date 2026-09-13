# DIAGRAMS — Diagramas del proyecto

## Diagrama de capas

```
┌─────────────────────────────────────────────┐
│ Aplicación: main.cpp → Device_t             │
│   (CLI, config, macros, interacción)        │
├─────────────────────────────────────────────┤
│ Drivers: IR_Transmitter · IR_Receiver       │
│   (GPIO, bcm2835 init/cierre)               │
├─────────────────────────────────────────────┤
│ Protocolo: IR_Engine                        │
│   (portadora 38 kHz · trama NEC)            │
├─────────────────────────────────────────────┤
│ Presentación: OLED SSD1306 (I2C ioctl)      │
├─────────────────────────────────────────────┤
│ Hardware: LED IR · VS1838B · OLED           │
└─────────────────────────────────────────────┘
```

## Secuencia de envío de una tecla

```
Usuario        Device_t         IR_Transmitter   IR_Engine        GPIO 22
  │  --send POWER │                   │               │               │
  ├──────────────►│ make_unique       │               │               │
  │   run()       ├──► begin() ───────┼──►bcm2835_init│               │
  │               │                   │               ├─ fsel(OUTP)   │
  │               ├─ cmdSend(POWER) ──┼──► sendCommand(0x04,0x08)     │
  │               │                   │               ├─ sendNEC       │
  │               │                   │               │  ├ 9ms ON ─────►(((
  │               │                   │               │  ├ 4.5ms OFF ─►
  │               │                   │               │  ├ 32 bits     ►
  │               │                   │               │  └ stop 562µs  ►
  │     OK        │                   │               │               │
  └───────────────┘   ~Device_t ──────┼──► bcm2835_close
```

## Diagrama de estados del modo `--learn`

```
Esperando flanco bajo (señal IR)
   │  pulso inicial ≥ 8 ms ?
   ▼
Midiendo espacio inicial
   ├─ ~4.5 ms → decodificar 32 bits
   │      ├─ pulso ≈ 562 µs + espacio corto  → bit 0
   │      └─ pulso ≈ 562 µs + espacio largo  → bit 1
   │      └─ validar paridad → addr + cmd ✓
   └─ ~2.25 ms → repetición (descartar)
```

Véanse también:
- [INFRARED.md](INFRARED.md) — forma de onda NEC y tiempos.
- [ARCHITECTURE.md](ARCHITECTURE.md) — arquitectura de capas.