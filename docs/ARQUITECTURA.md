# ARQUITECTURA — Visión resumida en español

Versión resumida de [ARCHITECTURE.md](ARCHITECTURE.md).

## Capas del proyecto

| Capa | Archivos | Responsabilidad |
|---|---|---|
| Aplicación | `main.cpp`, `src/core/Device_t.*` | CLI, config, flujo |
| Drivers | `drivers/IR_Transmitter.*`, `drivers/IR_Receiver.*` | Acceso GPIO / bcm2835 |
| Protocolo | `engine/IR_Engine.*` | Portadora 38 kHz + trama NEC |
| Presentación | `oled/*` | OLED SSD1306 por I2C (opcional) |
| Hardware | LED IR, VS1838B, OLED | Física conectada a la Pi |

## Flujo principal (`./bin/App --send POWER`)

```
main.cpp → Device_t(argc,argv)
   ├─ loadHardwareConfig()  → GPIO TX=22, RX=17, ADDR=0x04
   ├─ loadIrCodes()         → tabla de 36 teclas desde config/ir_codes.cfg
   └─ parseArgs()           → RunMode::SEND, sendKey=POWER
run()
   └─ IR_Transmitter::begin()  (bcm2835_init + GPIO22 salida)
       └─ cmdSend(POWER) → transmit(0x04, 0x08)
           └─ IR_Engine::sendNEC(0x04, 0x08, 0)
               ├─ start frame (9 ms + 4.5 ms)
               ├─ 4 bytes LSB-first (addr, ~addr, cmd, ~cmd)
               └─ stop bit (562 µs)
~Device_t() → IR_Transmitter::~IR_Transmitter → bcm2835_close()
```

## Reglas de diseño

- RAII: memoria gestionada con `unique_ptr`, sin `new`/`delete`.
- Config editable sin recompilar: pines, dirección NEC y códigos en `config/`.
- Versión definida en compilación: `-DVERSION="$(cat VERSION)"`.
- bcm2835 se abre/cierra en el ciclo de vida del transmisor.
- La OLED es opcional y no bloquea el arranque si falla.