# ARCHITECTURE — Arquitectura del proyecto IR-LG-Remote

## Vista general

```
                ┌────────────────────────────────────────────┐
                │              main.cpp                      │
                │   std::make_unique<Device_t>(argc,argv)    │
                │        device->run()                       │
                └──────────────────┬─────────────────────────┘
                                   │
                ┌──────────────────▼─────────────────────────┐
                │            Device::Device_t                │
                │  - parseArgs (--send/--macro/--learn/...)  │
                │  - loadIrCodes / loadHardwareConfig        │
                │  - cmdSend / cmdMacro / cmdInteractive     │
                │  - cmdLearn / updateOled                   │
                └───┬──────────────┬──────────────┬──────────┘
                    │              │              │
     ┌──────────────▼───┐    ┌────▼───────────┐  │
     │ IR::IR_Transmitter│    │ IR::IR_Receiver │ (opcional)
     │  - bcm2835_init   │    │  - polling GPIO │
     │  - GPIO 22 salida │    │  - decodificar  │
     └──────┬────────────┘    └────────┬────────┘
            │                          │
     ┌──────▼────────────┐        (VS1838B GPIO 17)
     │    IR::IR_Engine   │
     │ - carrier 38 kHz  │
     │ - trama NEC       │
     └────────────────────┘
                    │
     ┌──────────────▼───────────┐
     │ OLED SSD1306 (opcional) │
     │  I2C via ioctl /dev/i2c │
     └──────────────────────────┘
```

## Capas

1. **Capa de aplicación** (`main.cpp`, `Device_t`): argumentos, config, flujo.
2. **Capa de drivers** (`IR_Transmitter`, `IR_Receiver`): encapsulan el acceso
   al GPIO y a bcm2835.
3. **Capa de protocolo** (`IR_Engine`): genera la portadora 38 kHz y las tramas
   NEC; no conoce el GPIO (lo recibe del driver).
4. **Capa de presentación** (`OLED SSD1306`): muestra estado; opcional y
   desacoplada (si falla, la app continúa sin pantalla).
5. **Capa de hardware**: LED IR + transistor (GPIO 22), VS1838B (GPIO 17),
   OLED I2C (bus 1, dirección 0x3C).

## Decisiones de diseño

- **RAII**: `Device_t` usa `unique_ptr` para `OLED`, `transmitter_`,
  `receiver_`. No hay `new`/`delete` explícitos.
- **bcm2835 de vida corta**: `IR_Transmitter::begin()` llama a `bcm2835_init()`
  y `end()` a `bcm2835_close()`. En modo `--learn` el transmisor inicia
  bcm2835 y el receptor lo asume inicializado.
- **Configuración por archivos**: pines, dirección NEC y códigos viven en
  `config/*.cfg`; editar no requiere recompilar.
- **Versión en tiempo de compilación**: el Makefile inyecta `-DVERSION="..."`
  desde `VERSION`; no se lee el archivo en tiempo de ejecución.

## Flujo de ejecución de una tecla

1. `Device_t::run()` crea `IR_Transmitter(gpioTx_)`.
2. `begin()` → `bcm2835_init()` + GPIO22 como salida.
3. `cmdSend("POWER")` busca el código en `irCodes_`.
4. `transmit()` → `IR_Transmitter::sendCommand(0x04, 0x08, 0)`.
5. `IR_Engine::sendNEC()` genera start frame + 32 bits + stop.
6. Al salir, el destructor de `IR_Transmitter` cierra bcm2835.