# API — Descripción de API/Interfaces

## Interfaz de línea de comandos

`./bin/App [opciones]`

| Opción | Descripción |
|---|---|
| `--version`, `-v` | Muestra la versión (macro `VERSION` en tiempo de compilación). |
| `--help`, `-h` | Muestra la ayuda. |
| `--send <TECLA>` | Envía el comando IR de la tecla. |
| `--interactive` | Consola interactiva REPL. |
| `--macro <nombre>` | Ejecuta una macro de `config/config.cfg`. |
| `--learn <TECLA>` | Aprende un código IR real (VS1838B) y lo guarda. |
| `--list` | Lista las teclas cargadas. |

Todos los modos de hardware requieren `sudo` (bcm2835 abre `/dev/mem`).

## Formato de `config/ir_codes.cfg`

```
# comentario
TECLA=0xNN
```

## Formato de `config/config.cfg` y `config/hardware.cfg`

```
CLAVE=valor
MACRO_<nombre>=<tecla1>,<tecla2>,...
```

## Clases principales (C++)

### `Device::Device_t` — `include/core/Device_t.hpp`

Orquesta la aplicación: parsea argumentos, carga configuración y ejecuta la
acción. Métodos: `run()`, `version()`.

### `IR::IR_Engine` — `include/engine/IR_Engine.hpp`

Motor NEC de bajo nivel. Métodos: `sendNEC(addr, cmd, repeat)`,
`carrierBurst(µs)`, `spaceBurst(µs)`, `sendStartFrame()`, `sendByte()`,
`sendStopBit()`, `sendRepeatFrame()`.

### `IR::IR_Transmitter` — `include/drivers/IR_Transmitter.hpp`

Emisor de alto nivel. `begin()`, `sendCommand(addr, cmd, repeat)`,
`end()`, `getGpioPin()`.

### `IR::IR_Receiver` — `include/drivers/IR_Receiver.hpp`

Receptor/decoder NEC. `begin()`, `learn(address&, command&)`,
`getGpioPin()`.

### `SSD1306` — `include/oled/SSD1306_OLED.hpp`

Display OLED por I2C (ioctl). `OLEDinit()`, `OLEDclearBuffer()`,
`OLEDupdate()`, `print()`, `drawRect()`, `setCursor()`, etc.

## Makefile

```
make            # compilación nativa
make crossover  # cruzada detectando arquitectura
make crossover32/64
make run        # sudo ./bin/App [ARGS]
make clean / distclean
make fetch      # descarga sysroot con bcm2835 de la Pi
make deploy     # scp del binario a la Pi
make remote     # git pull + make + run en la Pi
make info       # muestra configuración del build
```